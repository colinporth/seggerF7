// mt9d111.cpp
//{{{  includes
#include "cCamera.h"

#include "cLcd.h"
//}}}

#define i2cAddress 0x90
//#define capture800x600
#define captureJpeg
//{{{  dcmi defines
#define DCMI_MODE_CONTINUOUS ((uint32_t)0x00000000U)
#define DCMI_MODE_SNAPSHOT   ((uint32_t)DCMI_CR_CM)

#define DCMI_PCKPOLARITY_FALLING ((uint32_t)0x00000000U)
#define DCMI_PCKPOLARITY_RISING  ((uint32_t)DCMI_CR_PCKPOL)

#define DCMI_VSPOLARITY_LOW  ((uint32_t)0x00000000U)
#define DCMI_VSPOLARITY_HIGH ((uint32_t)DCMI_CR_VSPOL)

#define DCMI_HSPOLARITY_LOW  ((uint32_t)0x00000000U)
#define DCMI_HSPOLARITY_HIGH ((uint32_t)DCMI_CR_HSPOL)

#define DCMI_JPEG_DISABLE    ((uint32_t)0x00000000U)
#define DCMI_JPEG_ENABLE     ((uint32_t)DCMI_CR_JPEG)

#define DCMI_CR_ALL_FRAME         ((uint32_t)0x00000000U)     // All frames are captured
#define DCMI_CR_ALTERNATE_2_FRAME ((uint32_t)DCMI_CR_FCRC_0)  // Every alternate frame captured
#define DCMI_CR_ALTERNATE_4_FRAME ((uint32_t)DCMI_CR_FCRC_1)  // One frame in 4 frames captured

#define DCMI_EXTEND_DATA_8B  ((uint32_t)0x00000000U)                      // Interface captures 8-bit data on every pixel clock
#define DCMI_EXTEND_DATA_10B ((uint32_t)DCMI_CR_EDM_0)                    // Interface captures 10-bit data on every pixel clock
#define DCMI_EXTEND_DATA_12B ((uint32_t)DCMI_CR_EDM_1)                    // Interface captures 12-bit data on every pixel clock
#define DCMI_EXTEND_DATA_14B ((uint32_t)(DCMI_CR_EDM_0 | DCMI_CR_EDM_1))  // Interface captures 14-bit data on every pixel clock

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
  CAMERA_IO_Write16 (i2cAddress, 0xF0, 0);
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "cameraId %x", CAMERA_IO_Read16 (i2cAddress, 0));

  // init camera registers
  mt9d111Init();

  // startup dcmi
  dcmiInit();
  }
//}}}

//{{{
char* cCamera::getString() {
  sprintf (mStr, "%d %dfps", mFrames, 1000/mTookTicks);
  return mStr;
  }
//}}}
//{{{
void cCamera::setFocus (int value) {

  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "setFocus %d", value);

  CAMERA_IO_Write16 (i2cAddress, 0xF0, 1);

  if (value <= 1) {
    CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x9071); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // SFR GPIO data b1:0 = 0 - disable GPIO1
    CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x9081); CAMERA_IO_Write16 (i2cAddress, 0xC8, 255);  // SFR GPIO wg_t00 = 255 initial off
    CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x9083); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);    // SFR GPIO wg_t10 = 0 no on
    }

  else {
    if (value > 254)
      value = 254;

    CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x9071); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02);        // SFR GPIO data b1:0 = enable GPIO1
    CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x9081); CAMERA_IO_Write16 (i2cAddress, 0xC8, 255 - value); // SFR GPIO wg_t00 pwm off
    CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x9083); CAMERA_IO_Write16 (i2cAddress, 0xC8, value);       // SFR GPIO wg_t10 pwm on
    }
  }
//}}}

//{{{
void cCamera::start (bool captureMode, uint32_t buffer) {

  mCapture = captureMode;

#ifdef captureJpeg
  mCapture ? jpeg() : preview();
#else
  mCapture ? capture() : preview();
#endif

  dcmiStart (buffer);
  }
//}}}
//{{{
void cCamera::preview() {

  mWidth = 800;
  mHeight = 600;
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "preview %dx%d", mWidth, mHeight);

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA120); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Sequencer.params.mode - none
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // Sequencer goto preview A
  }
//}}}
//{{{
void cCamera::capture() {

#ifdef capture800x600
  mWidth = 800;
  mHeight = 600;
#else
  mWidth = 1600;
  mHeight = 1200;
#endif

  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "capture %dx%d", mWidth, mHeight);

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA120); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02); // Sequencer.params.mode - capture video
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02); // Sequencer goto capture B
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

  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "jpeg %dx%d", mWidth, mHeight);

  //{{{  last data byte status ifp page2 0x02
  // b:0 = 1  transfer done
  // b:1 = 1  output fifo overflow
  // b:2 = 1  spoof oversize error
  // b:3 = 1  reorder buffer error
  // b:5:4    fifo watermark
  // b:7:6    quant table 0 to 2
  //}}}
  CAMERA_IO_Write16 (i2cAddress, 0xf0, 0x0001);

  // mode_config JPG bypass - shadow ifp page2 0x0a
  CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x270b); CAMERA_IO_Write16 (i2cAddress, 0xc8, 0x0010); // 0x0030 to disable B

  //{{{  jpeg.config id=9  0x07
  // b:0 =1  video
  // b:1 =1  handshake on error
  // b:2 =1  enable retry on error
  // b:3 =1  host indicates ready
  // ------
  // b:4 =1  scaled quant
  // b:5 =1  auto select quant
  // b:6:7   quant table id
  //}}}
  CAMERA_IO_Write16 (i2cAddress, 0xc6, 0xa907); CAMERA_IO_Write16 (i2cAddress, 0xc8, 0x0031);

  //{{{  mode fifo_config0_B - shadow ifp page2 0x0d
  //   output config  ifp page2  0x0d
  //   b:0 = 1  enable spoof frame
  //   b:1 = 1  enable pixclk between frames
  //   b:2 = 1  enable pixclk during invalid data
  //   b:3 = 1  enable soi/eoi
  //   -------
  //   b:4 = 1  enable soi/eoi during FV
  //   b:5 = 1  enable ignore spoof height
  //   b:6 = 1  enable variable pixclk
  //   b:7 = 1  enable byteswap
  //   -------
  //   b:8 = 1  enable FV on LV
  //   b:9 = 1  enable status inserted after data
  //   b:10 = 1  enable spoof codes
  //}}}
  CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2772); CAMERA_IO_Write16 (i2cAddress, 0xc8, 0x0067);

  //{{{  mode fifo_config1_B - shadow ifp page2 0x0e
  //   b:3:0   pclk1 divisor
  //   b:7:5   pclk1 slew
  //   -----
  //   b:11:8  pclk2 divisor
  //   b:15:13 pclk2 slew
  //}}}
  CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2774); CAMERA_IO_Write16 (i2cAddress, 0xc8, 0x0101);

  //{{{  mode fifo_config2_B - shadow ifp page2 0x0f
  //   b:3:0   pclk3 divisor
  //   b:7:5   pclk3 slew
  //}}}
  CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2776); CAMERA_IO_Write16 (i2cAddress, 0xc8, 0x0001);

  // mode OUTPUT WIDTH HEIGHT B
  CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2707); CAMERA_IO_Write16 (i2cAddress, 0xc8, mWidth);
  CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2709); CAMERA_IO_Write16 (i2cAddress, 0xc8, mHeight);

  // mode SPOOF WIDTH HEIGHT B
  CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2779); CAMERA_IO_Write16 (i2cAddress, 0xc8, mWidth);
  CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x277b); CAMERA_IO_Write16 (i2cAddress, 0xc8, mHeight);

  CAMERA_IO_Write16 (i2cAddress, 0x09, 0x000A); // factory bypass 10 bit sensor
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA120); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02); // Sequencer.params.mode - capture video
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02); // Sequencer goto capture B

  //{{{  readback jpeg params
  //CAMERA_IO_Write16 (i2cAddress, 0xc6, 0xa907);
  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "JPEG_CONFIG %x", CAMERA_IO_Read16 (i2cAddress, 0xc8));

  //CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2772);
  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "MODE_FIFO_CONF0_B %x", CAMERA_IO_Read16 (i2cAddress, 0xc8));

  //CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2774);
  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "MODE_FIFO_CONF1_B %x", CAMERA_IO_Read16 (i2cAddress, 0xc8));

  //CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2776);
  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "MODE_FIFO_CONF2_B %x", CAMERA_IO_Read16 (i2cAddress, 0xc8));

  //CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2707);
  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "MODE_OUTPUT_WIDTH_B %d", CAMERA_IO_Read16 (i2cAddress, 0xc8));

  //CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2709);
  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "MODE_OUTPUT_HEIGHT_B %d", CAMERA_IO_Read16 (i2cAddress, 0xc8));

  //CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2779);
  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "MODE_SPOOF_WIDTH_B %d", CAMERA_IO_Read16 (i2cAddress, 0xc8));

  //CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x277b);
  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "MODE_SPOOF_HEIGHT_B %d", CAMERA_IO_Read16 (i2cAddress, 0xc8));

  //CAMERA_IO_Write16 (i2cAddress, 0xc6, 0xa906);
  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "JPEG_FORMAT %d", CAMERA_IO_Read16 (i2cAddress, 0xc8));

  //CAMERA_IO_Write16 (i2cAddress, 0xc6, 0x2908);
  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "JPEG_RESTART_INT %d", CAMERA_IO_Read16 (i2cAddress, 0xc8));

  CAMERA_IO_Write16 (i2cAddress, 0xc6, 0xa90a);
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "JPEG_QSCALE_1 %d", CAMERA_IO_Read16 (i2cAddress, 0xc8));

  //CAMERA_IO_Write16 (i2cAddress, 0xc6, 0xa90b);
  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "JPEG_QSCALE_2 %d", CAMERA_IO_Read16 (i2cAddress, 0xc8));

  //CAMERA_IO_Write16 (i2cAddress, 0xc6, 0xa90c);
  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "JPEG_QSCALE_3 %d", CAMERA_IO_Read16 (i2cAddress, 0xc8));
  //}}}
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
      mCurPtr += 4 * mXferSize;

      if (mXferCount <= mXferMaxCount - 2) {
        // next dma chunk
        auto buf = mBuffPtr + ((mXferCount+1) * (4 * mXferSize));
        if (mXferCount & 1)
          DMA2_Stream1->M0AR = buf;
        else
          DMA2_Stream1->M1AR = buf;
        //cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "dma %d", mXferCount);
        }
      else if (mXferCount == mXferMaxCount - 1) {
        // penultimate chunk, reset M0AR for next frame
        DMA2_Stream1->M0AR = mBuffPtr;
        //cLcd::mLcd->debug (LCD_COLOR_CYAN, "dma %d", mXferCount);
        }
      else {
        // last chunk, reset M1AR, mXferCount for next frame
        DMA2_Stream1->M1AR = mBuffPtr + (4 * mXferSize);
        //cLcd::mLcd->debug (LCD_COLOR_GREEN, "dma %d done", mXferCount);
        mXferCount = 0;
        mCurPtr = mBuffPtr;
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

  uint32_t misr = READ_REG (DCMI->MISR);

  if ((misr & DCMI_FLAG_ERRRI) == DCMI_FLAG_ERRRI) {
    // synchronizationError interrupt
    DCMI->ICR = DCMI_FLAG_ERRRI;

    //__HAL_DMA_DISABLE (DMA_Handle);
    cLcd::mLcd->debug (LCD_COLOR_RED, "syncIrq");
    }

  if ((misr & DCMI_FLAG_OVRRI) == DCMI_FLAG_OVRRI) {
    // overflowError interrupt
    DCMI->ICR = DCMI_FLAG_OVRRI;
    //__HAL_DMA_DISABLE (DMA_Handle);
    cLcd::mLcd->debug (LCD_COLOR_RED, "overflowIrq");
    }

  if ((misr & DCMI_FLAG_VSYNCRI) == DCMI_FLAG_VSYNCRI) {
    DCMI->ICR = DCMI_FLAG_VSYNCRI;

    auto ticks = HAL_GetTick();
    mTookTicks = ticks - mTicks;
    mTicks = ticks;
    mFrames++;

    #ifdef captureJpeg
    if (mCapture) {
      uint32_t rx = (mXferSize - DMA2_Stream1->NDTR) * 4;
      uint8_t b1 = *((uint8_t*)(mCurPtr + rx - 4));
      uint8_t b2 = *((uint8_t*)(mCurPtr + rx - 3));
      uint8_t b3 = *((uint8_t*)(mCurPtr + rx - 2));
      uint8_t b4 = *((uint8_t*)(mCurPtr + rx - 1));
      cLcd::mLcd->debug (LCD_COLOR_WHITE, "v %2d %6d %7d %2x %d",
                                          mXferCount, rx, mCurPtr + rx - mLastXferCount, b4, (b3 << 16) + (b2 << 8) + b4);
      mLastXferCount = mCurPtr + rx;
      }
    #endif
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
void cCamera::mt9d111Init() {

  //{{{  soft reset
  CAMERA_IO_Write16 (i2cAddress, 0x65, 0xA000); // Bypass the PLL, R0x65:0 = 0xA000,

  CAMERA_IO_Write16 (i2cAddress, 0xF0, 1);      // page 1
  CAMERA_IO_Write16 (i2cAddress, 0xC3, 0x0501); // Perform MCU reset by setting R0xC3:1 = 0x0501.

  CAMERA_IO_Write16 (i2cAddress, 0xF0, 0);      // page 0
  CAMERA_IO_Write16 (i2cAddress, 0x0D, 0x0021); // Enable soft reset by setting R0x0D:0 = 0x0021. Bit 0 is used for the sensor core reset
  CAMERA_IO_Write16 (i2cAddress, 0x0D, 0x0000); // Disable soft reset by setting R0x0D:0 = 0x0000.
  HAL_Delay (100);
  //}}}

  // page 0
#ifdef capture800x600
  CAMERA_IO_Write16 (i2cAddress, 0x05, 0x0247); // capture B HBLANK
  CAMERA_IO_Write16 (i2cAddress, 0x06, 0x000B); // capture B VBLANK
  CAMERA_IO_Write16 (i2cAddress, 0x07, 0x0136); // preview A HBLANK
  CAMERA_IO_Write16 (i2cAddress, 0x08, 0x000B); // preview A VBLANK
  CAMERA_IO_Write16 (i2cAddress, 0x20, 0x8300); // capture B Read Mode
  CAMERA_IO_Write16 (i2cAddress, 0x21, 0x8400); // preview A Read Mode
#else
  CAMERA_IO_Write16 (i2cAddress, 0x05, 0x013e); // capture B HBLANK
  CAMERA_IO_Write16 (i2cAddress, 0x06, 0x000B); // capture B VBLANK
  CAMERA_IO_Write16 (i2cAddress, 0x07, 0x0153); // preview A HBLANK
  CAMERA_IO_Write16 (i2cAddress, 0x08, 0x000B); // preview A VBLANK
  CAMERA_IO_Write16 (i2cAddress, 0x20, 0x0300); // capture B Read Mode
  CAMERA_IO_Write16 (i2cAddress, 0x21, 0x8400); // preview A Read Mode
#endif

  // page 0 PLL - M=16,N=1,P=3 - (24mhz/(N+1))*M / 2*(P+1) = 24mhz
  CAMERA_IO_Write16 (i2cAddress, 0x66, 0x1001); // PLLControl1 -    M:N
  CAMERA_IO_Write16 (i2cAddress, 0x67, 0x0503); // PLLControl2 - 0x05:P
  CAMERA_IO_Write16 (i2cAddress, 0x65, 0xA000); // Clock CNTRL - pllOn
  CAMERA_IO_Write16 (i2cAddress, 0x65, 0x2000); // Clock CNTRL - usePll
  HAL_Delay (100);

  // page 1
  CAMERA_IO_Write16 (i2cAddress, 0xF0, 1);
  CAMERA_IO_Write16 (i2cAddress, 0x97, 0x22); // outputFormat - RGB565, swap odd even

#ifdef capture800x600
  //{{{  page 1 register wizard
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2703); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0320); // Output Width A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2705); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0258); // Output Height A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2707); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0320); // Output Width B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2709); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0258); // Output Height B

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x270B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0030); // mode_config = disable jpeg A,B

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x270F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x001C); // Row Start A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2711); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x003C); // Column Start A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2713); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x04b0); // Row Height A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2715); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0640); // Column Width A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2717); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0256); // Extra Delay A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2719); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0011); // Row Speed A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2727); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);      // Crop_X0 A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2729); CAMERA_IO_Write16 (i2cAddress, 0xC8, 800);    // Crop_X1 A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x272B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);      // Crop_Y0 A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x272D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 600);    // Crop_Y1 A

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA743); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02);   // Gamma and Contrast Settings A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA77D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x22);   // outputFormat A - RGB565, swap odd even

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x271B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x001C); // Row Start B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x271D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x003C); // Column Start B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x271F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x04b0); // Row Height B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2721); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0640); // Column Width B

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2723); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x023F); // Extra Delay B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2725); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0011); // Row Speed B

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2735); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);      // Crop_X0 B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2737); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0320); // Crop_X1 B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2739); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);      // Crop_Y0 B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x273B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0258); // Crop_Y1 B

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA744); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02);   // Gamma and Contrast Settings B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA77E); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x22);   // outputFormat B - RGB565, swap odd even

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA217); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x08);   // IndexTH23 = 8

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x276D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE0E2); // FIFO_Conf1 A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA76F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE1);   // FIFO_Conf2 A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2774); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE0E1); // FIFO_Conf1 B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA776); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE1);   // FIFO_Conf2 B

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x220B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0048); // Max R12 B (Shutter Delay)
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2228); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x022B); // RowTime (msclk per)/4

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x222F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x003B); // R9 Step
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA408); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x000D); // search_f1_50
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA409); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x000F); // search_f2_50
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA40A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x000A); // search_f1_60
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA40B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x000C); // search_f2_60
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2411); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x003B); // R9_Step_60
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2413); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0047); // R9_Step_50

  HAL_Delay (100);
  //}}}
#else
  //{{{  page 1 register wizard
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2703); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0320); // Output Width A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2705); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0258); // Output Height A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2707); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0640); // Output Width B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2709); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x04b0); // Output Height B

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x270B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0030); // mode_config = disable jpeg A,B

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x270F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x001C); // Row Start A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2711); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x003C); // Column Start A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2713); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x04b0); // Row Height A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2715); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0640); // Column Width A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2717); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0042); // Extra Delay A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2719); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0011); // Row Speed A

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA743); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02);   // Gamma and Contrast Settings A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA77D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x22);   // outputFormat A - RGB565, swap odd even

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x271B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x001C); // Row Start B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x271D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x003C); // Column Start B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x271F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x04b0); // Row Height B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2721); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0640); // Column Width B

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2723); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x021e); // Extra Delay B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2725); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0011); // Row Speed B

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA744); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02);   // Gamma and Contrast Settings B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA77E); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x22);   // outputFormat B - RGB565, swap odd even

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2727); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);      // Crop_X0 A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2729); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0320); // Crop_X1 A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x272B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);      // Crop_Y0 A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x272D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0258); // Crop_Y1 A

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2735); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);      // Crop_X0 B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2737); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0640); // Crop_X1 B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2739); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);      // Crop_Y0 B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x273B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x04b0); // Crop_Y1 B

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x276D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE0E2); // FIFO_Conf1 A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA76F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE1);   // FIFO_Conf2 A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2774); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE0E1); // FIFO_Conf1 B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA776); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE1);   // FIFO_Conf2 B

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA217); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x08);   // IndexTH23 = 8

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x220B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x023c); // Max R12 B (Shutter Delay)
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2228); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0239); // RowTime (msclk per)/4

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x222F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x002B); // R9 Step
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA408); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0009); // search_f1_50
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA409); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0008); // search_f2_50
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA40A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0007); // search_f1_60
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA40B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0009); // search_f2_60
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2411); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x002B); // R9_Step_60
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2413); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0034); // R9_Step_50

  HAL_Delay (100);
  //}}}
#endif

  //{{{  page 1 sequencer transitions
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA122); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // EnterPreview: Auto Exposure = 1
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA123); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // EnterPreview: Flicker Detection = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA124); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // EnterPreview: Auto White Balance = 1
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA125); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // EnterPreview: Auto Focus = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA126); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // EnterPreview: Histogram = 1
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA127); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // EnterPreview: Strobe Control  = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA128); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // EnterPreview: Skip Control = 0

  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA129); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x03); // InPreview: Auto Exposure = 3
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA12A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02); // InPreview: Flicker Detection = 2
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA12B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x03); // InPreview: Auto White Balance = 3
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA12C); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // InPreview: Auto Focus = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA12D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x03); // InPreview: Histogram  = 3
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA12E); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // InPreview: Strobe Control = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA12F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // InPreview: Skip Control = 0

  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA130); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x04); // ExitPreview: Auto Exposure = 4
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA131); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // ExitPreview: Flicker Detection = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA132); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // ExitPreview: Auto White Balance = 1
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA133); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // ExitPreview: Auto Focus = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA134); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // ExitPreview: Histogram = 1
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA135); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // ExitPreview: Strobe Control = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA136); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // ExitPreview: Skip Control = 0

  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA137); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Auto Exposure = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA138); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Flicker Detection = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA139); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Auto White Balance  = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA13A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Auto Focus = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA13B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Histogram = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA13C); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Strobe Control = 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA13D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Skip Control = 0
  //}}}
  //{{{  page 1 gamma tables
  //// gamma table A 0 to 18
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA745); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00);  // 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA746); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x14);  // 20
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA747); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x23);  // 35
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA748); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x3A);  // 58
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA749); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x5E);  // 94
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA74A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x76);  // 118
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA74B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x88);  // 136
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA74C); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x96);  // 150
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA74D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xA3);  // 163
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA74E); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xAF);  // 175
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA74F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xBA);  // 186
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA750); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xC4);  // 196
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA751); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xCE);  // 206
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA752); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xD7);  // 215
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA753); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE0);  // 224
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA754); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE8);  // 232
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA755); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xF0);  // 240
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA756); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xF8);  // 248
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA757); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xFF);  // 255

  //// gamma table B 0 to 18
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA758); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00);  // 0
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA759); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x14);  // 20
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA75A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x23);  // 35
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA75B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x3A);  // 58
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA75C); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x5E);  // 94
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA75D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x76);  // 118
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA75E); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x88);  // 136
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA75F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x96);  // 150
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA760); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xA3);  // 163
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA761); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xAF);  // 175
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA762); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xBA);  // 186
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA763); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xC4);  // 196
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA764); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xCE);  // 206
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA765); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xD7);  // 215
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA766); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE0);  // 224
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA767); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE8);  // 232
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA768); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xF0);  // 240
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA769); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xF8);  // 248
  //CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA76A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xFF);  // 255
  //}}}
  //{{{  page 1 focus init
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x90B6); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // SFR GPIO suspend

  // enable GPIO0,1 as output, initial value 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x9079); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xFC); // SFR GPIO data direction
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x9071); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // SFR GPIO data b1:0 = 0 GPIO0,1 initial 0

  // use 8bit counter clkdiv 2^(1+2)=8 -> 48mhz -> 6mhz ->> 23.7khz
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x90B0); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // SFR GPIO wg_config b0 = 1 8bit counter
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x90B2); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02); // SFR GPIO wg_clkdiv b0 = 2

  // GPIO0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x908B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // SFR GPIO wg_n0 = 0 infinite
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x9081); CAMERA_IO_Write16 (i2cAddress, 0xC8, 255);  // SFR GPIO wg_t00 = 255 initial off
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x9083); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);    // SFR GPIO wg_t10 = 0 no on

  // GPIO1
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x908A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // SFR GPIO wg_n1 = 0 infinite
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x9080); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xFF); // SFR GPIO wg_t01 = 255 max initial on
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x9082); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // SFR GPIO wg_t11 = 0 no off

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x90B5); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // SFR GPIO reset
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x90B6); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // SFR GPIO suspend
  //}}}

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x06); // Sequencer Refresh Mode
  HAL_Delay (200);
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x05); // Sequencer Refresh
  HAL_Delay (200);
  }
//}}}
//{{{
void cCamera::dcmiInit() {

  // config dma CR - CHSEL, MBURST, PBURST, PL, MSIZE, PSIZE, MINC, PINC, CIRC, DIR, CT and DBM bits
  uint32_t tmp = DMA2_Stream1->CR;
  tmp &= ((uint32_t)~(DMA_SxCR_CHSEL | DMA_SxCR_MBURST | DMA_SxCR_PBURST |
                      DMA_SxCR_PL     | DMA_SxCR_MSIZE  | DMA_SxCR_PSIZE  |
                      DMA_SxCR_MINC   | DMA_SxCR_PINC   | DMA_SxCR_CIRC   |
                      DMA_SxCR_DIR    | DMA_SxCR_CT     | DMA_SxCR_DBM));
  tmp |=  DMA_CHANNEL_1       | DMA_PERIPH_TO_MEMORY | DMA_PINC_DISABLE | DMA_MINC_ENABLE |
          DMA_PDATAALIGN_WORD | DMA_MDATAALIGN_WORD  | DMA_CIRCULAR     | DMA_PRIORITY_VERY_HIGH |
          DMA_MBURST_INC4     | DMA_PBURST_SINGLE;
  DMA2_Stream1->CR = tmp;

  // config dma FCR - clear directMode and fifoThreshold
  tmp = DMA2_Stream1->FCR;
  tmp &= (uint32_t)~(DMA_SxFCR_DMDIS | DMA_SxFCR_FTH);
  tmp |= DMA_FIFOMODE_ENABLE | DMA_FIFO_THRESHOLD_FULL;
  DMA2_Stream1->FCR = tmp;

  // config mDmaBaseRegisters, mStreamIndex
  const uint8_t kFlagBitshiftOffset[8U] = {0U, 6U, 16U, 22U, 0U, 6U, 16U, 22U};
  auto streamNum = (((uint32_t)DMA2_Stream1 & 0xFFU) - 16U) / 24U;
  mStreamIndex = kFlagBitshiftOffset[streamNum];
  if (streamNum > 3U) // return pointer to HISR and HIFCR
    mDmaBaseRegisters = (tDmaBaseRegisters*)(((uint32_t)DMA2_Stream1 & (uint32_t)(~0x3FFU)) + 4U);
  else // return pointer to LISR and LIFCR
    mDmaBaseRegisters = (tDmaBaseRegisters*)((uint32_t)DMA2_Stream1 & (uint32_t)(~0x3FFU));

  // clear all dma interrupt flags
  mDmaBaseRegisters->IFCR = 0x3FU << mStreamIndex;

  // config DCMI HS, VS, DE and PC polarity
  DCMI->CR &= ~(DCMI_CR_PCKPOL | DCMI_CR_HSPOL  | DCMI_CR_VSPOL  | DCMI_CR_EDM_0  | DCMI_CR_EDM_1  | DCMI_CR_FCRC_0 |
                DCMI_CR_FCRC_1 | DCMI_CR_JPEG   | DCMI_CR_ESS    | DCMI_CR_BSM_0  | DCMI_CR_BSM_1  | DCMI_CR_OEBS |
                DCMI_CR_LSM    | DCMI_CR_OELS);
  DCMI->CR |= DCMI_CR_ALL_FRAME       | DCMI_HSPOLARITY_LOW | DCMI_HSPOLARITY_LOW  |
              DCMI_PCKPOLARITY_RISING | DCMI_EXTEND_DATA_8B | DCMI_JPEG_ENABLE;

  // NVIC configuration for DCMI transfer complete interrupt
  HAL_NVIC_SetPriority (DCMI_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (DCMI_IRQn);

  // NVIC configuration for DMA2D transfer complete interrupt
  HAL_NVIC_SetPriority (DMA2_Stream1_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (DMA2_Stream1_IRQn);

  // enable Error, overrun, vsync interrupts
  DCMI->IER |= DCMI_IT_ERR | DCMI_IT_OVR | DCMI_IT_VSYNC;
  }
//}}}

//{{{
void cCamera::dcmiStart (uint32_t data) {

  // disable DCMI by resetting DCMIEN bit
  DCMI->CR &= ~DCMI_CR_ENABLE;

  // disable dma
  DMA2_Stream1->CR &= ~DMA_SxCR_EN;

  // enable DCMI by setting DCMIEN bit
  DCMI->CR |= DCMI_CR_ENABLE;

  // config the DCMI Mode
  DCMI->CR = (DCMI->CR & ~(DCMI_CR_CM)) | DCMI_MODE_CONTINUOUS;

  // calc the number of xfers with xferSize <= 64k
  mBuffPtr = data;
  mXferCount = 0;
  mXferMaxCount = 1;
  mXferSize = getWidth()*getHeight()/2;
  while (mXferSize > 0xFFFF) {
    mXferSize = mXferSize / 2;
    mXferMaxCount = mXferMaxCount * 2;
    }

  // enable dma doubleBufferMode,  config src, dst addresses, length
  DMA2_Stream1->CR |= (uint32_t)DMA_SxCR_DBM;
  DMA2_Stream1->PAR = (uint32_t)&DCMI->DR;
  DMA2_Stream1->M0AR = data;
  DMA2_Stream1->M1AR = data + (4*mXferSize);
  DMA2_Stream1->NDTR = mXferSize;

  // clear all dma flags
  DMA2->LIFCR = DMA_FLAG_TCIF1_5;
  DMA2->LIFCR = DMA_FLAG_HTIF1_5;
  DMA2->LIFCR = DMA_FLAG_TEIF1_5;
  DMA2->LIFCR = DMA_FLAG_FEIF1_5;
  DMA2->LIFCR = DMA_FLAG_DMEIF1_5;

  // enable dma interrupts
  DMA2_Stream1->CR  |= DMA_IT_TC | DMA_IT_TE | DMA_IT_DME;
  DMA2_Stream1->FCR |= DMA_IT_FE;

  // enable dma
  DMA2_Stream1->CR |= DMA_SxCR_EN;

  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "dcmiStart %d:%d:%d", mXferCount, mXferSize, getWidth()*getHeight()/2);

  // enable dcmi capture
  DCMI->CR |= DCMI_CR_CAPTURE;
  }
//}}}

//{{{  jpeg
//int jpegLen = framePtr[frameLen-4] | (framePtr[frameLen-3] << 8) | (framePtr[frameLen-2] << 16);
//auto jpegStatus = framePtr[frameLen-1];
// make jpeg header
//uint8_t jpegHeader[1000];
//int jpegHeaderLen = setJpegHeader (jpegHeader, mWidth, mHeight, 0, mJpegQscale1);
// append EOI marker
//framePtr[jpegLen] = 0xff;
//framePtr[jpegLen+1] = 0xd9;
//}}}
