// mt9d111.cpp
//{{{  includes
#include "cCamera.h"

#include "cLcd.h"
//}}}

#define capture800x600
const bool kDebugIrq = false;
const uint8_t kI2cAddress = 0x90;

//{{{  dcmi defines
#define DCMI_IT_FRAME        ((uint32_t)DCMI_IER_FRAME_IE)    // Capture complete interrupt
#define DCMI_IT_OVR          ((uint32_t)DCMI_IER_OVR_IE)      // Overrun interrupt
#define DCMI_IT_ERR          ((uint32_t)DCMI_IER_ERR_IE)      // Synchronization error interrupt
#define DCMI_IT_VSYNC        ((uint32_t)DCMI_IER_VSYNC_IE)    // VSYNC interrupt
#define DCMI_IT_LINE         ((uint32_t)DCMI_IER_LINE_IE)     // Line interrupt

#define DCMI_FLAG_HSYNC      ((uint32_t)DCMI_SR_INDEX | DCMI_SR_HSYNC) // HSYNC pin state (active line / synchronization between lines)
#define DCMI_FLAG_VSYNC      ((uint32_t)DCMI_SR_INDEX | DCMI_SR_VSYNC) // VSYNC pin state (active frame / synchronization between frames)
#define DCMI_FLAG_FNE        ((uint32_t)DCMI_SR_INDEX | DCMI_SR_FNE)   // FIFO not empty flag

#define DCMI_FLAG_FRAMERI    ((uint32_t)DCMI_RIS_FRAME_RIS)  // Frame capture complete interrupt flag
#define DCMI_FLAG_OVRRI      ((uint32_t)DCMI_RIS_OVR_RIS)    // Overrun interrupt flag
#define DCMI_FLAG_ERRRI      ((uint32_t)DCMI_RIS_ERR_RIS)    // Synchronization error interrupt flag
#define DCMI_FLAG_VSYNCRI    ((uint32_t)DCMI_RIS_VSYNC_RIS)  // VSYNC interrupt flag
#define DCMI_FLAG_LINERI     ((uint32_t)DCMI_RIS_LINE_RIS)   // Line interrupt flag

#define DCMI_FLAG_FRAMEMI    ((uint32_t)DCMI_MIS_INDEX | DCMI_MIS_FRAME_MIS)  // DCMI Frame capture complete masked interrupt status
#define DCMI_FLAG_OVRMI      ((uint32_t)DCMI_MIS_INDEX | DCMI_MIS_OVR_MIS  )  // DCMI Overrun masked interrupt status
#define DCMI_FLAG_ERRMI      ((uint32_t)DCMI_MIS_INDEX | DCMI_MIS_ERR_MIS  )  // DCMI Synchronization error masked interrupt status
#define DCMI_FLAG_VSYNCMI    ((uint32_t)DCMI_MIS_INDEX | DCMI_MIS_VSYNC_MIS)  // DCMI VSYNC masked interrupt status
#define DCMI_FLAG_LINEMI     ((uint32_t)DCMI_MIS_INDEX | DCMI_MIS_LINE_MIS )  // DCMI Line masked interrupt status
//}}}

cCamera* gCamera = nullptr;

extern "C" {
  void DMA2_Stream1_IRQHandler() { gCamera->dmaIrqHandler(); }
  void DCMI_IRQHandler() { gCamera->dcmiIrqHandler(); }
  }

// public
//{{{
void cCamera::init() {

  gCamera = this;

  gpioInit();

  // init camera i2c, readBack id
  CAMERA_IO_Init();
  write (0xF0, 0);
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "cameraId %x", CAMERA_IO_Read16 (kI2cAddress, 0));
  mt9d111Init();

  // init dcmi dma
  // - mDmaBaseRegisters, mStreamIndex
  const uint8_t kFlagBitshiftOffset[8U] = {0U, 6U, 16U, 22U, 0U, 6U, 16U, 22U};

  auto streamNum = (((uint32_t)DMA2_Stream1 & 0xFFU) - 16U) / 24U;
  mStreamIndex = kFlagBitshiftOffset[streamNum];
  if (streamNum > 3U) // return pointer to HISR and HIFCR
    mDmaBaseRegisters = (tDmaBaseRegisters*)(((uint32_t)DMA2_Stream1 & (uint32_t)(~0x3FFU)) + 4U);
  else // return pointer to LISR and LIFCR
    mDmaBaseRegisters = (tDmaBaseRegisters*)((uint32_t)DMA2_Stream1 & (uint32_t)(~0x3FFU));

  // clear all dma interrupt flags
  mDmaBaseRegisters->IFCR = 0x3FU << mStreamIndex;

  // NVIC configuration for DCMI transfer complete interrupt
  HAL_NVIC_SetPriority (DCMI_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (DCMI_IRQn);

  // NVIC configuration for DMA2D transfer complete interrupt
  HAL_NVIC_SetPriority (DMA2_Stream1_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (DMA2_Stream1_IRQn);
  }
//}}}

//{{{
void cCamera::setFocus (int value) {

  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "setFocus %d", value);

  write (0xF0, 1);

  if (value <= 1) {
    write1 (0x9071, 0x00); // SFR GPIO data b1:0 = 0 - disable GPIO1
    write1 (0x9081, 255);  // SFR GPIO wg_t00 = 255 initial off
    write1 (0x9083, 0);    // SFR GPIO wg_t10 = 0 no on
    }

  else {
    if (value > 254)
      value = 254;

    write1 (0x9071, 0x02);        // SFR GPIO data b1:0 = enable GPIO1
    write1 (0x9081, 255 - value); // SFR GPIO wg_t00 pwm off
    write1 (0x9083, value);       // SFR GPIO wg_t10 pwm on
    }

  mFocus = value;
  }
//}}}

//{{{
void cCamera::start (bool jpegMode, uint8_t* buffer) {

  jpegMode ? jpeg() : preview();
  mJpegMode = jpegMode;

  dcmiStart (buffer);
  }
//}}}

//{{{
void cCamera::dmaIrqHandler() {

  uint32_t isr = mDmaBaseRegisters->ISR;
  if (isr & (DMA_FLAG_TCIF0_4 << mStreamIndex)) {
    // transferComplete Interrupt, doubleBufferMode handling
    if (DMA2_Stream1->CR & DMA_IT_TC) {
      // clear transferComplete interrupt flag
      mDmaBaseRegisters->IFCR = DMA_FLAG_TCIF0_4 << mStreamIndex;

      mXferCount++;
      mFrameCur += 4 * mXferSize;

      if (mXferCount <= mXferMaxCount - 2) {
        // next dma chunk
        auto buf = mBufStart + ((mXferCount+1) * (4 * mXferSize));
        if (mXferCount & 1)
          DMA2_Stream1->M0AR = (uint32_t)buf;
        else
          DMA2_Stream1->M1AR = (uint32_t)buf;
        //cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "dma %d", mXferCount);
        }
      else if (mXferCount == mXferMaxCount - 1) {
        // penultimate chunk, reset M0AR for next frame
        DMA2_Stream1->M0AR = (uint32_t)mBufStart;
        //cLcd::mLcd->debug (LCD_COLOR_CYAN, "dma %d", mXferCount);
        }
      else {
        // last chunk, reset M1AR, mXferCount for next frame
        DMA2_Stream1->M1AR = (uint32_t)mBufStart + (4 * mXferSize);
        //cLcd::mLcd->debug (LCD_COLOR_GREEN, "dma %d done", mXferCount);
        mXferCount = 0;
        mFrameCur = mBufStart;
        }
      }
    }

  //{{{  fifoError Interrupt
  if (isr & (DMA_FLAG_FEIF0_4 << mStreamIndex))
    if (DMA2_Stream1->FCR & DMA_IT_FE) {
      mDmaBaseRegisters->IFCR = DMA_FLAG_FEIF0_4 << mStreamIndex;
      //cLcd::mLcd->debug (LCD_COLOR_RED, "dmaFifoError");
      }
  //}}}
  //{{{  transferError Interrupt
  if (isr & (DMA_FLAG_TEIF0_4 << mStreamIndex)) {
    if (DMA2_Stream1->CR & DMA_IT_TE) {
      DMA2_Stream1->CR  &= ~DMA_IT_TE;
      mDmaBaseRegisters->IFCR = DMA_FLAG_TEIF0_4 << mStreamIndex;

      cLcd::mLcd->debug (LCD_COLOR_RED, "dmaTransferError");
      }
    }
  //}}}
  //{{{  directMode Error Interrupt
  if (isr & (DMA_FLAG_DMEIF0_4 << mStreamIndex))
    if (DMA2_Stream1->CR & DMA_IT_DME) {
      mDmaBaseRegisters->IFCR = DMA_FLAG_DMEIF0_4 << mStreamIndex;
      cLcd::mLcd->debug (LCD_COLOR_RED, "dmaDirectModeError");
      }
  //}}}
  }
//}}}
//{{{
void cCamera::dcmiIrqHandler() {

  uint32_t misr = DCMI->MISR;

  if ((misr & DCMI_FLAG_ERRRI) == DCMI_FLAG_ERRRI) {
    // synchronizationError interrupt
    DCMI->ICR = DCMI_FLAG_ERRRI;
    cLcd::mLcd->debug (LCD_COLOR_RED, "syncIrq");
    }

  if ((misr & DCMI_FLAG_OVRRI) == DCMI_FLAG_OVRRI) {
    // overflowError interrupt
    DCMI->ICR = DCMI_FLAG_OVRRI;
    // dsiable dma
    DMA2_Stream1->CR &= ~DMA_SxCR_EN;
    cLcd::mLcd->debug (LCD_COLOR_RED, "overflowIrq");
    }

  if ((misr & DCMI_FLAG_VSYNCRI) == DCMI_FLAG_VSYNCRI) {
    DCMI->ICR = DCMI_FLAG_VSYNCRI;

    auto ticks = HAL_GetTick();
    mTookTicks = ticks - mTicks;
    mTicks = ticks;
    mFrames++;

    uint32_t dmaBytes = (mXferSize - DMA2_Stream1->NDTR) * 4;
    if (mJpegMode) {
      //{{{  status ifp page2 0x02
      // b:0 = 1  transfer done
      // b:1 = 1  output fifo overflow
      // b:2 = 1  spoof oversize error
      // b:3 = 1  reorder buffer error
      // b:5:4    fifo watermark
      // b:7:6    quant table 0 to 2
      //}}}
      uint8_t jpegStatus = mFrameCur[dmaBytes-1];
      if ((jpegStatus & 0x0f) == 0x01) {

        mFrameBuf = mFrameStart;
        mFrameBufLen = (mFrameCur[dmaBytes-2] << 16) + (mFrameCur[dmaBytes-3] << 8) + mFrameCur[dmaBytes-4];

        if (kDebugIrq)
          cLcd::mLcd->debug (LCD_COLOR_GREEN,
                             "v%2d:%6d:%8x %x:%d", mXferCount,dmaBytes,mFrameBuf, jpegStatus,mFrameBufLen);
        }
      else {
        mFrameBuf = nullptr;
        mFrameBufLen = 0;
        if (kDebugIrq)
          cLcd::mLcd->debug (LCD_COLOR_WHITE,
                             "v%d:%d %x %d", mXferCount,dmaBytes, jpegStatus, mFrameCur + dmaBytes - mFrameStart);
        }
      }
    else {
      mFrameBuf = mFrameStart;
      mFrameBufLen = mFixedFrameLen;
      if (kDebugIrq)
        cLcd::mLcd->debug (LCD_COLOR_CYAN, "v%2d:%6d:%8x %d", mXferCount,dmaBytes,mFrameBuf, mFrameBufLen);
      }

    // dodgy copy, should manage buffer better instead
    if (mFrameBuf + mFrameBufLen > mBufEnd) // wrap around to deliver contiguous buffer
      memcpy (mBufEnd, mBufStart, mFrameBuf + mFrameBufLen - mBufEnd);
    mFrameStart = mFrameCur + dmaBytes;
    }
  }
//}}}

// private
//{{{
void cCamera::gpioInit() {

  //  config clocks
  __HAL_RCC_DCMI_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  //  config gpio alternate function
  GPIO_InitTypeDef gpio_init_structure;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  gpio_init_structure.Alternate = GPIO_AF13_DCMI;

  gpio_init_structure.Pin = GPIO_PIN_4 | GPIO_PIN_6;
  HAL_GPIO_Init (GPIOA, &gpio_init_structure);

  gpio_init_structure.Pin = GPIO_PIN_3;
  HAL_GPIO_Init (GPIOD, &gpio_init_structure);

  gpio_init_structure.Pin = GPIO_PIN_5 | GPIO_PIN_6;
  HAL_GPIO_Init (GPIOE, &gpio_init_structure);

  gpio_init_structure.Pin = GPIO_PIN_9;
  HAL_GPIO_Init (GPIOG, &gpio_init_structure);

  gpio_init_structure.Pin = GPIO_PIN_9 | GPIO_PIN_10  | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14;
  HAL_GPIO_Init (GPIOH, &gpio_init_structure);
  }
//}}}

//{{{
void cCamera::write (uint8_t reg, uint16_t value) {

  CAMERA_IO_Write16 (kI2cAddress, reg, value);
  }
//}}}
//{{{
void cCamera::write1 (uint16_t reg, uint16_t value) {
  write (0xc6, reg);
  write (0xc8, value);
  }
//}}}
//{{{
void cCamera::mt9d111Init() {

  //{{{  soft reset
  write (0x65, 0xA000); // Bypass the PLL, R0x65:0 = 0xA000,

  write (0xF0, 1);      // page 1
  write (0xC3, 0x0501); // Perform MCU reset by setting R0xC3:1 = 0x0501.

  write (0xF0, 0);      // page 0
  write (0x0D, 0x0021); // Enable soft reset by setting R0x0D:0 = 0x0021. Bit 0 is used for the sensor core reset
  write (0x0D, 0x0000); // Disable soft reset by setting R0x0D:0 = 0x0000.
  HAL_Delay (100);
  //}}}

  // page 0
#ifdef capture800x600
  write (0x05, 0x0247); // capture B HBLANK
  write (0x06, 0x000B); // capture B VBLANK
  write (0x07, 0x0136); // preview A HBLANK
  write (0x08, 0x000B); // preview A VBLANK
  write (0x20, 0x8300); // capture B Read Mode
  write (0x21, 0x8400); // preview A Read Mode
#else
  write (0x05, 0x013e); // capture B HBLANK
  write (0x06, 0x000B); // capture B VBLANK
  write (0x07, 0x0153); // preview A HBLANK
  write (0x08, 0x000B); // preview A VBLANK
  write (0x20, 0x0300); // capture B Read Mode
  write (0x21, 0x8400); // preview A Read Mode
#endif

  // page 0 PLL - M=16,N=1,P=3 - (24mhz/(N+1))*M / 2*(P+1) = 24mhz
  write (0x66, 0x1001); // PLLControl1 -    M:N
  write (0x67, 0x0503); // PLLControl2 - 0x05:P
  write (0x65, 0xA000); // Clock CNTRL - pllOn
  write (0x65, 0x2000); // Clock CNTRL - usePll
  HAL_Delay (100);

  // page 1
  write (0xF0, 1);
  write (0x09, 0x000A); // factory bypass 10 bit sensor
  write (0x97, 0x22);   // outputFormat - RGB565, swap odd even

#ifdef capture800x600
  //{{{  register wizard
  write1 (0x2703, 0x0320); // Output Width A
  write1 (0x2705, 0x0258); // Output Height A
  write1 (0x2707, 0x0320); // Output Width B
  write1 (0x2709, 0x0258); // Output Height B

  write1 (0x270B, 0x0030); // mode_config = disable jpeg A,B

  write1 (0x270F, 0x001C); // Row Start A
  write1 (0x2711, 0x003C); // Column Start A
  write1 (0x2713, 0x04b0); // Row Height A
  write1 (0x2715, 0x0640); // Column Width A
  write1 (0x2717, 0x0256); // Extra Delay A
  write1 (0x2719, 0x0011); // Row Speed A
  write1 (0x2727, 0);      // Crop_X0 A
  write1 (0x2729, 800);    // Crop_X1 A
  write1 (0x272B, 0);      // Crop_Y0 A
  write1 (0x272D, 600);    // Crop_Y1 A

  write1 (0xA743, 0x02);   // Gamma and Contrast Settings A
  write1 (0xA77D, 0x22);   // outputFormat A - RGB565, swap odd even

  write1 (0x271B, 0x001C); // Row Start B
  write1 (0x271D, 0x003C); // Column Start B
  write1 (0x271F, 0x04b0); // Row Height B
  write1 (0x2721, 0x0640); // Column Width B

  write1 (0x2723, 0x023F); // Extra Delay B
  write1 (0x2725, 0x0011); // Row Speed B

  write1 (0x2735, 0);      // Crop_X0 B
  write1 (0x2737, 0x0320); // Crop_X1 B
  write1 (0x2739, 0);      // Crop_Y0 B
  write1 (0x273B, 0x0258); // Crop_Y1 B

  write1 (0xA744, 0x02);   // Gamma and Contrast Settings B
  write1 (0xA77E, 0x02);   // outputFormat B - YUV, swap odd even

  write1 (0xA217, 0x08);   // IndexTH23 = 8

  write1 (0x276D, 0xE0E2); // FIFO_Conf1 A
  write1 (0xA76F, 0xE1);   // FIFO_Conf2 A
  write1 (0x2774, 0xE0E1); // FIFO_Conf1 B
  write1 (0xA776, 0xE1);   // FIFO_Conf2 B

  write1 (0x220B, 0x0048); // Max R12 B (Shutter Delay)
  write1 (0x2228, 0x022B); // RowTime (msclk per)/4

  write1 (0x222F, 0x003B); // R9 Step
  write1 (0xA408, 0x000D); // search_f1_50
  write1 (0xA409, 0x000F); // search_f2_50
  write1 (0xA40A, 0x000A); // search_f1_60
  write1 (0xA40B, 0x000C); // search_f2_60
  write1 (0x2411, 0x003B); // R9_Step_60
  write1 (0x2413, 0x0047); // R9_Step_50

  HAL_Delay (100);
  //}}}
  //{{{  jpeg b config
  // jpeg.config id=9  0x07
  //   b:0  video
  //   b:1  handshake on error
  //   b:2  enable retry on error
  //   b:3  host indicates ready
  //   b:4  scaled quant
  //   b:5  auto select quant
  //   b:6:7  quant table id
  write1 (0xa907, 0x0031);

  // mode fifo_config0_B - shadow ifp page2 0x0d output config
  //   b:0   enable spoof frame
  //   b:1   enable pixclk between frames
  //   b:2   enable pixclk during invalid data
  //   b:3   enable soi/eoi
  //   b:4   enable soi/eoi during FV
  //   b:5   enable ignore spoof height
  //   b:6   enable variable pixclk
  //   b:7   enable byteswap
  //   b:8   enable FV on LV
  //   b:9   enable status inserted after data
  //   b:10  enable spoof codes
  write1 (0x2772, 0x0079);

  // mode fifo_config1_B - shadow ifp page2 0x0e
  //   b:3:0   pclk1 divisor
  //   b:7:5   pclk1 slew
  //   -----
  //   b:11:8  pclk2 divisor
  //   b:15:13 pclk2 slew
  write1 (0x2774, 0x0202);

  // mode fifo_config2_B - shadow ifp page2 0x0f
  //   b:3:0   pclk3 divisor
  //   b:7:5   pclk3 slew
  write1 (0x2776, 0x0002);

  write1 (0x2707, 800);  // mode OUTPUT WIDTH HEIGHT B
  write1 (0x2709, 600);
  //}}}
#else
  //{{{  register wizard
  write1 (0x2703, 0x0320); // Output Width A
  write1 (0x2705, 0x0258); // Output Height A
  write1 (0x2707, 0x0640); // Output Width B
  write1 (0x2709, 0x04b0); // Output Height B

  write1 (0x270B, 0x0030); // mode_config = disable jpeg A,B

  write1 (0x270F, 0x001C); // Row Start A
  write1 (0x2711, 0x003C); // Column Start A
  write1 (0x2713, 0x04b0); // Row Height A
  write1 (0x2715, 0x0640); // Column Width A
  write1 (0x2717, 0x0042); // Extra Delay A
  write1 (0x2719, 0x0011); // Row Speed A

  write1 (0xA743, 0x02);   // Gamma and Contrast Settings A
  write1 (0xA77D, 0x22);   // outputFormat A - RGB565, swap odd even

  write1 (0x271B, 0x001C); // Row Start B
  write1 (0x271D, 0x003C); // Column Start B
  write1 (0x271F, 0x04b0); // Row Height B
  write1 (0x2721, 0x0640); // Column Width B

  write1 (0x2723, 0x021e); // Extra Delay B
  write1 (0x2725, 0x0011); // Row Speed B

  write1 (0xA744, 0x02);   // Gamma and Contrast Settings B
  write1 (0xA77E, 0x02);   // outputFormat B - YUV, swap odd even

  write1 (0x2727, 0);      // Crop_X0 A
  write1 (0x2729, 0x0320); // Crop_X1 A
  write1 (0x272B, 0);      // Crop_Y0 A
  write1 (0x272D, 0x0258); // Crop_Y1 A

  write1 (0x2735, 0);      // Crop_X0 B
  write1 (0x2737, 0x0640); // Crop_X1 B
  write1 (0x2739, 0);      // Crop_Y0 B
  write1 (0x273B, 0x04b0); // Crop_Y1 B

  write1 (0x276D, 0xE0E2); // FIFO_Conf1 A
  write1 (0xA76F, 0xE1);   // FIFO_Conf2 A
  write1 (0x2774, 0xE0E1); // FIFO_Conf1 B
  write1 (0xA776, 0xE1);   // FIFO_Conf2 B

  write1 (0xA217, 0x08);   // IndexTH23 = 8

  write1 (0x220B, 0x023c); // Max R12 B (Shutter Delay)
  write1 (0x2228, 0x0239); // RowTime (msclk per)/4

  write1 (0x222F, 0x002B); // R9 Step
  write1 (0xA408, 0x0009); // search_f1_50
  write1 (0xA409, 0x0008); // search_f2_50
  write1 (0xA40A, 0x0007); // search_f1_60
  write1 (0xA40B, 0x0009); // search_f2_60
  write1 (0x2411, 0x002B); // R9_Step_60
  write1 (0x2413, 0x0034); // R9_Step_50

  HAL_Delay (100);
  //}}}
  //{{{  jpeg b config
  // jpeg.config id=9  0x07
  //   b:0  video
  //   b:1  handshake on error
  //   b:2  enable retry on error
  //   b:3  host indicates ready
  //   b:4  scaled quant
  //   b:5  auto select quant
  //   b:6:7  quant table id
  write1 (0xa907, 0x0031);

  // mode fifo_config0_B - shadow ifp page2 0x0d output config
  //   b:0   enable spoof frame
  //   b:1   enable pixclk between frames
  //   b:2   enable pixclk during invalid data
  //   b:3   enable soi/eoi
  //   b:4   enable soi/eoi during FV
  //   b:5   enable ignore spoof height
  //   b:6   enable variable pixclk
  //   b:7   enable byteswap
  //   b:8   enable FV on LV
  //   b:9   enable status inserted after data
  //   b:10  enable spoof codes
  write1 (0x2772, 0x0039);

  write1 (0x2707, 1600);  // mode OUTPUT WIDTH HEIGHT B
  write1 (0x2709, 800);
  //}}}
#endif

  //{{{  sequencer transitions
  //write1 (0xA122, 0x01); // previewEnter Auto Exposure = 1
  //write1 (0xA123, 0x00); // previewEnter Flicker Detection = 0
  //write1 (0xA124, 0x01); // previewEnter Auto White Balance = 1
  //write1 (0xA125, 0x00); // previewEnter Auto Focus = 0
  //write1 (0xA126, 0x01); // previewEnter Histogram = 1
  //write1 (0xA127, 0x00); // previewEnter Strobe Control  = 0
  //write1 (0xA128, 0x00); // previewEnter Skip Control = 0

  //write1 (0xA129, 0x03); // Preview Auto Exposure = 3
  //write1 (0xA12A, 0x02); // Preview Flicker Detection = 2
  //write1 (0xA12B, 0x03); // Preview Auto White Balance = 3
  //write1 (0xA12C, 0x00); // Preview Auto Focus = 0
  //write1 (0xA12D, 0x03); // Preview Histogram  = 3
  //write1 (0xA12E, 0x00); // Preview Strobe Control = 0
  //write1 (0xA12F, 0x00); // Preview Skip Control = 0

  //write1 (0xA130, 0x04); // previewLeave Auto Exposure = 4
  //write1 (0xA131, 0x00); // previewLeave Flicker Detection = 0
  //write1 (0xA132, 0x01); // previewLeave Auto White Balance = 1
  //write1 (0xA133, 0x00); // previewLeave Auto Focus = 0
  //write1 (0xA134, 0x01); // previewLeave Histogram = 1
  //write1 (0xA135, 0x00); // previewLeave Strobe Control = 0
  //write1 (0xA136, 0x00); // previewLeave Skip Control = 0

  //write1 (0xA137, 0x00); // CaptureEnter Auto Exposure = 4
  //write1 (0xA138, 0x00); // CaptureEnter Flicker Detection = 0
  //write1 (0xA139, 0x00); // CaptureEnter Auto White Balance = 3
  //write1 (0xA13A, 0x00); // CaptureEnter Auto Focus = 0
  //write1 (0xA13B, 0x00); // CaptureEnter Histogram = 3
  //write1 (0xA13C, 0x00); // CaptureEnter Strobe Control = 0
  //write1 (0xA13D, 0x00); // CaptureEnter Skip Control = 0
  //}}}
  //{{{  gamma tables
  //// gamma table A 0 to 18
  //write1 (0xA745, 0x00);  // 0
  //write1 (0xA746, 0x14);  // 20
  //write1 (0xA747, 0x23);  // 35
  //write1 (0xA748, 0x3A);  // 58
  //write1 (0xA749, 0x5E);  // 94
  //write1 (0xA74A, 0x76);  // 118
  //write1 (0xA74B, 0x88);  // 136
  //write1 (0xA74C, 0x96);  // 150
  //write1 (0xA74D, 0xA3);  // 163
  //write1 (0xA74E, 0xAF);  // 175
  //write1 (0xA74F, 0xBA);  // 186
  //write1 (0xA750, 0xC4);  // 196
  //write1 (0xA751, 0xCE);  // 206
  //write1 (0xA752, 0xD7);  // 215
  //write1 (0xA753, 0xE0);  // 224
  //write1 (0xA754, 0xE8);  // 232
  //write1 (0xA755, 0xF0);  // 240
  //write1 (0xA756, 0xF8);  // 248
  //write1 (0xA757, 0xFF);  // 255

  //// gamma table B 0 to 18
  //write1 (0xA758, 0x00);  // 0
  //write1 (0xA759, 0x14);  // 20
  //write1 (0xA75A, 0x23);  // 35
  //write1 (0xA75B, 0x3A);  // 58
  //write1 (0xA75C, 0x5E);  // 94
  //write1 (0xA75D, 0x76);  // 118
  //write1 (0xA75E, 0x88);  // 136
  //write1 (0xA75F, 0x96);  // 150
  //write1 (0xA760, 0xA3);  // 163
  //write1 (0xA761, 0xAF);  // 175
  //write1 (0xA762, 0xBA);  // 186
  //write1 (0xA763, 0xC4);  // 196
  //write1 (0xA764, 0xCE);  // 206
  //write1 (0xA765, 0xD7);  // 215
  //write1 (0xA766, 0xE0);  // 224
  //write1 (0xA767, 0xE8);  // 232
  //write1 (0xA768, 0xF0);  // 240
  //write1 (0xA769, 0xF8);  // 248
  //write1 (0xA76A, 0xFF);  // 255
  //}}}
  //{{{  focus init
  write1 (0x90B6, 0x01); // SFR GPIO suspend

  // enable GPIO0,1 as output, initial value 0
  write1 (0x9079, 0xFC); // SFR GPIO data direction
  write1 (0x9071, 0x00); // SFR GPIO data b1:0 = 0 GPIO0,1 initial 0

  // use 8bit counter clkdiv 2^(1+2)=8 -> 48mhz -> 6mhz ->> 23.7khz
  write1 (0x90B0, 0x01); // SFR GPIO wg_config b0 = 1 8bit counter
  write1 (0x90B2, 0x02); // SFR GPIO wg_clkdiv b0 = 2

  // GPIO0
  write1 (0x908B, 0x00); // SFR GPIO wg_n0 = 0 infinite
  write1 (0x9081, 255);  // SFR GPIO wg_t00 = 255 initial off
  write1 (0x9083, 0);    // SFR GPIO wg_t10 = 0 no on

  // GPIO1
  write1 (0x908A, 0x00); // SFR GPIO wg_n1 = 0 infinite
  write1 (0x9080, 0xFF); // SFR GPIO wg_t01 = 255 max initial on
  write1 (0x9082, 0x00); // SFR GPIO wg_t11 = 0 no off

  write1 (0x90B5, 0x00); // SFR GPIO reset
  write1 (0x90B6, 0x00); // SFR GPIO suspend
  //}}}

  write1 (0xA103, 0x06); // Sequencer Refresh Mode
  HAL_Delay (100);
  write1 (0xA103, 0x05); // Sequencer Refresh
  HAL_Delay (100);
  }
//}}}

//{{{
void cCamera::dcmiStart (uint8_t* buf) {

  // disable dcmi
  DCMI->CR &= ~DCMI_CR_ENABLE;
  // disable dma
  DMA2_Stream1->CR &= ~DMA_SxCR_EN;

  // bufEnd, may use more to provide continuos last buffer
  mBufStart = buf;
  uint32_t dmaLen = mJpegMode ? 0x00100000 : getWidth()*getHeight()*2;
  mBufEnd = buf + (4 * dmaLen);

  // calc the number of xfers with xferSize <= 64k
  mFrameStart = mBufStart;
  mFrameCur = mBufStart;
  mXferCount = 0;
  mXferMaxCount = 1;
  mXferSize = dmaLen;
  while (mXferSize > 0xFFFF) {
    mXferSize = mXferSize / 2;
    mXferMaxCount = mXferMaxCount * 2;
    }
  mFrameBuf = nullptr;
  mFrameBufLen = 0;
  mFixedFrameLen = getWidth() * getHeight() * 2;
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "dcmiStart %d:%d:%x", mXferMaxCount, mXferSize, dmaLen);

  // enable dma doubleBufferMode,  config src, dst addresses, length
  DMA2_Stream1->PAR = (uint32_t)&(DCMI->DR);
  DMA2_Stream1->M0AR = (uint32_t)mBufStart;
  DMA2_Stream1->M1AR = (uint32_t)(mBufStart + (4*mXferSize));
  DMA2_Stream1->NDTR = mXferSize;

  // clear all dma interrupt flags
  DMA2->LIFCR = DMA_FLAG_TCIF1_5;
  DMA2->LIFCR = DMA_FLAG_TEIF1_5;
  DMA2->LIFCR = DMA_FLAG_DMEIF1_5;
  DMA2->LIFCR = DMA_FLAG_HTIF1_5;
  DMA2->LIFCR = DMA_FLAG_FEIF1_5;

  // enable dma - fifo, interrupt
  DMA2_Stream1->FCR = DMA_IT_FE | DMA_FIFOMODE_ENABLE | DMA_FIFO_THRESHOLD_FULL;

  // enable dma
  DMA2_Stream1->CR = DMA_CHANNEL_1 |
                     DMA_PERIPH_TO_MEMORY | DMA_CIRCULAR | DMA_SxCR_DBM | DMA_PRIORITY_HIGH |
                     DMA_PDATAALIGN_WORD | DMA_MDATAALIGN_WORD |
                     DMA_PBURST_SINGLE | DMA_MBURST_INC4 |
                     DMA_PINC_DISABLE | DMA_MINC_ENABLE |
                     DMA_IT_TC | DMA_IT_TE | DMA_IT_DME |
                     DMA_SxCR_EN;

  // enable dcmi - error,overrun,vsync interrupts
  DCMI->IER |= DCMI_IT_ERR | DCMI_IT_OVR | DCMI_IT_VSYNC;
  // enable dcmi - continuous,capture,enable
  DCMI->CR = DCMI_CR_PCKPOL | DCMI_CR_JPEG | DCMI_CR_CAPTURE | DCMI_CR_ENABLE;
  }
//}}}

//{{{
void cCamera::preview() {

  mWidth = 800;
  mHeight = 600;
  mJpegMode = false;

  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "preview %dx%d", mWidth, mHeight);

  // switch to preview
  write (0xf0, 1);
  write1 (0x270b, 0x0030); // mode_config = disable jpeg A,B
  write1 (0xA120, 0x00);   // Sequencer.params.mode - none
  write1 (0xA103, 0x01);   // Sequencer transition to preview A
  }
//}}}
//{{{
void cCamera::jpeg() {

#ifdef capture800x600
  mWidth = 800;
  mHeight = 600;
#else
  mWidth = 1600;
  mHeight = 1200;
#endif
  mJpegMode = true;

  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "jpeg %dx%d", mWidth, mHeight);

  write (0xf0, 1);
  write1 (0x270b, 0x0010); // mode_config - disable jpeg A, enable jpeg B
  write1 (0xA120, 0x02);   // Sequencer.params.mode - capture video
  write1 (0xA103, 0x02);   // Sequencer transition to capture B
  }
//}}}
