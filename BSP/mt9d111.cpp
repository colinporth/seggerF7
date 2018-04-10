// mt9d111.cpp
//{{{  includes
#include "stm32746g_discovery.h"
#include "cCamera.h"
//}}}

#define i2cAddress 0x90
static const uint8_t flagBitshiftOffset[8U] = {0U, 6U, 16U, 22U, 0U, 6U, 16U, 22U};
//{{{
struct tDcmiInfo {
  DCMI_TypeDef*      Instance;           // DCMI Register base address
  DCMI_InitTypeDef   Init;               // DCMI parameters
  __IO uint32_t      XferCount;          // DMA transfer counter
  __IO uint32_t      XferSize;           // DMA transfer size
  uint32_t           XferTransferNumber; // DMA transfer number
  uint32_t           pBuffPtr;           // Pointer to DMA output buffer
  DMA_HandleTypeDef* DMA_Handle;         // Pointer to the DMA handler
  };
//}}}
//{{{  struct dmaBaseRegisters
typedef struct {
  __IO uint32_t ISR;   /*!< DMA interrupt status register */
  __IO uint32_t Reserved0;
  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
  } tDmaBaseRegisters;
//}}}

DCMI_HandleTypeDef dcmiInfo;

//{{{
void dmaXferComplete (DMA_HandleTypeDef* dma) {

  if (dcmiInfo.XferCount > 2) {
    // next frameChunk
    if (((DMA2_Stream1->CR) & DMA_SxCR_CT != 0) && ((dcmiInfo.XferCount % 2) == 0))
      // Update M0AR for next frameChunk
      DMA2_Stream1->M0AR += 8 * dcmiInfo.XferSize;
    else if ((DMA2_Stream1->CR & DMA_SxCR_CT) == 0)
      // Update M1AR for next frameChunk
      DMA2_Stream1->M1AR += 8 * dcmiInfo.XferSize;
    dcmiInfo.XferCount--;
    //cLcd::mLcd->debug (LCD_COLOR_GREEN, "dmaXfer %d", dcmiInfo.XferCount);
    }

  else if (DMA2_Stream1->CR & DMA_SxCR_CT) {
    // last chunk but one, reset M0AR for next frame
    DMA2_Stream1->M0AR = dcmiInfo.pBuffPtr;
    dcmiInfo.XferCount--;
    //cLcd::mLcd->debug (LCD_COLOR_GREEN, "dmaXfer %d", dcmiInfo.XferCount);
    }

  else {
    // last chunk, reset M1AR,XferCount for next frame
    DMA2_Stream1->M1AR = dcmiInfo.pBuffPtr + (4 * dcmiInfo.XferSize);
    //__HAL_DCMI_ENABLE_IT (&dcmiInfo, DCMI_IT_FRAME);
    dcmiInfo.XferCount = dcmiInfo.XferTransferNumber;
    //cLcd::mLcd->debug (LCD_COLOR_GREEN, "dmaFrameDone");
    }
  }
//}}}
//{{{
void dmaError (DMA_HandleTypeDef* dma) {

  if (dcmiInfo.DMA_Handle->ErrorCode != HAL_DMA_ERROR_FE)
    cLcd::mLcd->debug (LCD_COLOR_RED, "DCMI DMAerror %x", dcmiInfo.DMA_Handle->ErrorCode);
  }
//}}}
//{{{
void dcmiError (DMA_HandleTypeDef* dma) {
  cLcd::mLcd->debug (LCD_COLOR_RED, "DCMIerror");
  }
//}}}

//{{{
void dmaInit (DMA_HandleTypeDef *hdma) {

  // Disable the peripheral
  __HAL_DMA_DISABLE (hdma);

  // Check if the DMA Stream is effectively disabled
  while ((hdma->Instance->CR & DMA_SxCR_EN) != RESET);

  // Get the CR register value
  uint32_t tmp = hdma->Instance->CR;

  // Clear CHSEL, MBURST, PBURST, PL, MSIZE, PSIZE, MINC, PINC, CIRC, DIR, CT and DBM bits
  tmp &= ((uint32_t)~(DMA_SxCR_CHSEL | DMA_SxCR_MBURST | DMA_SxCR_PBURST | \
                      DMA_SxCR_PL    | DMA_SxCR_MSIZE  | DMA_SxCR_PSIZE  | \
                      DMA_SxCR_MINC  | DMA_SxCR_PINC   | DMA_SxCR_CIRC   | \
                      DMA_SxCR_DIR   | DMA_SxCR_CT     | DMA_SxCR_DBM));

  // Prepare the DMA Stream configuration
  tmp |=  hdma->Init.Channel             | hdma->Init.Direction        |
          hdma->Init.PeriphInc           | hdma->Init.MemInc           |
          hdma->Init.PeriphDataAlignment | hdma->Init.MemDataAlignment |
          hdma->Init.Mode                | hdma->Init.Priority;

  // the Memory burst and peripheral burst are not used when the FIFO is disabled
  if(hdma->Init.FIFOMode == DMA_FIFOMODE_ENABLE) // Get memory burst and peripheral burst
    tmp |=  hdma->Init.MemBurst | hdma->Init.PeriphBurst;

  // Write to DMA Stream CR register
  hdma->Instance->CR = tmp;

  // Get the FCR register value
  tmp = hdma->Instance->FCR;

  // Clear Direct mode and FIFO threshold bits
  tmp &= (uint32_t)~(DMA_SxFCR_DMDIS | DMA_SxFCR_FTH);

  // Prepare the DMA Stream FIFO configuration
  tmp |= hdma->Init.FIFOMode;

  // The FIFO threshold is not used when the FIFO mode is disabled
  if (hdma->Init.FIFOMode == DMA_FIFOMODE_ENABLE) //* Get the FIFO threshold
    tmp |= hdma->Init.FIFOThreshold;

  // Write to DMA Stream FCR
  hdma->Instance->FCR = tmp;

  // Initialize StreamBaseAddress and StreamIndex for  DMA streamBaseAddress used by HAL_DMA_IRQHandler()
  // lookup table for necessary bitshift of flags within status registers
  uint32_t stream_number = (((uint32_t)hdma->Instance & 0xFFU) - 16U) / 24U;
  hdma->StreamIndex = flagBitshiftOffset[stream_number];
  if (stream_number > 3U) // return pointer to HISR and HIFCR */
    hdma->StreamBaseAddress = (((uint32_t)hdma->Instance & (uint32_t)(~0x3FFU)) + 4U);
  else // return pointer to LISR and LIFCR */
    hdma->StreamBaseAddress = ((uint32_t)hdma->Instance & (uint32_t)(~0x3FFU));
  tDmaBaseRegisters* regs = (tDmaBaseRegisters*)hdma->StreamBaseAddress;

  // Clear all interrupt flags
  regs->IFCR = 0x3FU << hdma->StreamIndex;
  }
//}}}
//{{{
void dmaMultiBufferStart (DMA_HandleTypeDef* hdma, uint32_t src, uint32_t dst, uint32_t dst2, uint32_t length) {

  // Enable the Double buffer mode
  hdma->Instance->CR |= (uint32_t)DMA_SxCR_DBM;

  // Config src, dst address, length
  hdma->Instance->NDTR = length;
  hdma->Instance->PAR = src;
  hdma->Instance->M0AR = dst;
  hdma->Instance->M1AR = dst2;

  // Clear all flags
  __HAL_DMA_CLEAR_FLAG (hdma, __HAL_DMA_GET_TC_FLAG_INDEX (hdma));
  __HAL_DMA_CLEAR_FLAG (hdma, __HAL_DMA_GET_HT_FLAG_INDEX (hdma));
  __HAL_DMA_CLEAR_FLAG (hdma, __HAL_DMA_GET_TE_FLAG_INDEX (hdma));
  __HAL_DMA_CLEAR_FLAG (hdma, __HAL_DMA_GET_DME_FLAG_INDEX (hdma));
  __HAL_DMA_CLEAR_FLAG (hdma, __HAL_DMA_GET_FE_FLAG_INDEX (hdma));

  // enable interrupts
  hdma->Instance->CR  |= DMA_IT_TC | DMA_IT_TE | DMA_IT_DME;
  hdma->Instance->FCR |= DMA_IT_FE;

  // Enable peripheral
  __HAL_DMA_ENABLE (hdma);
  }
//}}}

extern "C" {
  //{{{
  void DMA2_Stream1_IRQHandler() {

    // calculate DMA base and stream number
    DMA_HandleTypeDef* hdma = dcmiInfo.DMA_Handle;
    tDmaBaseRegisters* regs = (tDmaBaseRegisters*)hdma->StreamBaseAddress;
    uint32_t tmpisr = regs->ISR;

    // transferError Interrupt
    if ((tmpisr & (DMA_FLAG_TEIF0_4 << hdma->StreamIndex)) != RESET) {
      if (__HAL_DMA_GET_IT_SOURCE (hdma, DMA_IT_TE) != RESET) {
        // Disable the transfer error interrupt
        hdma->Instance->CR  &= ~(DMA_IT_TE);
        // Clear the transferError flag
        regs->IFCR = DMA_FLAG_TEIF0_4 << hdma->StreamIndex;
        cLcd::mLcd->debug (LCD_COLOR_RED, "dmaTransferError");
        }
      }

    // fifoError Interrupt
    if ((tmpisr & (DMA_FLAG_FEIF0_4 << hdma->StreamIndex)) != RESET)
      if (__HAL_DMA_GET_IT_SOURCE (hdma, DMA_IT_FE) != RESET) {
        // Clear the fifoError flag
        regs->IFCR = DMA_FLAG_FEIF0_4 << hdma->StreamIndex;
        //cLcd::mLcd->debug (LCD_COLOR_RED, "dmaFifoError");
        }

    // directMode Error Interrupt
    if ((tmpisr & (DMA_FLAG_DMEIF0_4 << hdma->StreamIndex)) != RESET)
      if (__HAL_DMA_GET_IT_SOURCE (hdma, DMA_IT_DME) != RESET) {
        // Clear the directMode error flag
        regs->IFCR = DMA_FLAG_DMEIF0_4 << hdma->StreamIndex;
        cLcd::mLcd->debug (LCD_COLOR_RED, "dmaDirectModeError");
        }

    // transferComplete Interrupt
    if ((tmpisr & (DMA_FLAG_TCIF0_4 << hdma->StreamIndex)) != RESET) {
      if(__HAL_DMA_GET_IT_SOURCE (hdma, DMA_IT_TC) != RESET) {
        // Clear the transferComplete flag
        regs->IFCR = DMA_FLAG_TCIF0_4 << hdma->StreamIndex;

        if (((hdma->Instance->CR) & (uint32_t)(DMA_SxCR_DBM)) != RESET)
          dmaXferComplete (hdma);
        else {
          // Disable the transfer complete interrupt if the DMA mode is not CIRCULAR
          if ((hdma->Instance->CR & DMA_SxCR_CIRC) == RESET) // Disable the transfer complete interrupt
            hdma->Instance->CR  &= ~(DMA_IT_TC);
          dmaXferComplete (hdma);
          }
        }
      }
    }
  //}}}
  //{{{
  void DCMI_IRQHandler() {

    uint32_t misr = READ_REG (DCMI->MISR);

    if ((misr & DCMI_FLAG_ERRRI) == DCMI_FLAG_ERRRI) {
      // synchronizationError interrupt
      __HAL_DCMI_CLEAR_FLAG (&dcmiInfo, DCMI_FLAG_ERRRI);
      __HAL_DMA_DISABLE (dcmiInfo.DMA_Handle);
      cLcd::mLcd->debug (LCD_COLOR_RED, "syncIrq");
      }

    if ((misr & DCMI_FLAG_OVRRI) == DCMI_FLAG_OVRRI) {
      // overflowError interrupt
      __HAL_DCMI_CLEAR_FLAG (&dcmiInfo, DCMI_FLAG_OVRRI);
      __HAL_DMA_DISABLE (dcmiInfo.DMA_Handle);
      cLcd::mLcd->debug (LCD_COLOR_RED, "overflowIrq");
      }

    //if ((misr & DCMI_FLAG_FRAMERI) == DCMI_FLAG_FRAMERI) {
    //  __HAL_DCMI_DISABLE_IT (&dcmiInfo, DCMI_IT_FRAME);
    //  __HAL_DCMI_CLEAR_FLAG (&dcmiInfo, DCMI_FLAG_FRAMERI);
    //  cLcd::mLcd->debug (LCD_COLOR_GREEN, "frameIrq");
    //  }
    }
  //}}}
  }

// public
//{{{
void cCamera::init (bool useCapture) {

  gpioInit();

  // init camera i2c, readBack id
  CAMERA_IO_Init();
  CAMERA_IO_Write16 (i2cAddress, 0xF0, 0);
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "cameraId %x", CAMERA_IO_Read16 (i2cAddress, 0));

  // init camera registers
  mt9d111Init();

  // startup dcmi
  dcmiInit (&dcmiInfo);

  preview();
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
void cCamera::start (uint32_t buffer) {

  dcmiStart (&dcmiInfo, DCMI_MODE_CONTINUOUS, buffer, getWidth()*getHeight()/2);
  }
//}}}
//{{{
void cCamera::preview() {
  mWidth = 800;
  mHeight = 600;
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "preview");
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA120); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Sequencer.params.mode - none
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // Sequencer goto preview A - 800x600
  }
//}}}
//{{{
void cCamera::capture() {
  mWidth = 800;
  mHeight = 600;
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "capture");
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA120); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02); // Sequencer.params.mode - capture video
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02); // Sequencer goto capture B  - 1600x1200
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

  CAMERA_IO_Write16 (i2cAddress, 0x05, 0x0247); // capture B HBLANK
  CAMERA_IO_Write16 (i2cAddress, 0x06, 0x000B); // capture B VBLANK
  CAMERA_IO_Write16 (i2cAddress, 0x07, 0x0136); // preview A HBLANK
  CAMERA_IO_Write16 (i2cAddress, 0x08, 0x000B); // preview A VBLANK
  CAMERA_IO_Write16 (i2cAddress, 0x20, 0x8300); // capture B Read Mode
  CAMERA_IO_Write16 (i2cAddress, 0x21, 0x8400); // preview A Read Mode

  // PLL - M=16,N=1,P=3 - (24mhz/(N+1))*M / 2*(P+1) = 24mhz
  CAMERA_IO_Write16 (i2cAddress, 0x66, 0x1001); // PLLControl1 -    M:N
  CAMERA_IO_Write16 (i2cAddress, 0x67, 0x0503); // PLLControl2 - 0x05:P
  CAMERA_IO_Write16 (i2cAddress, 0x65, 0xA000); // Clock CNTRL - PLL ON
  CAMERA_IO_Write16 (i2cAddress, 0x65, 0x2000); // Clock CNTRL - USE PLL
  HAL_Delay (100);

  // page 1
  CAMERA_IO_Write16 (i2cAddress, 0xF0, 1);
  CAMERA_IO_Write16 (i2cAddress, 0x97, 0x22);   // outputFormat - RGB565, swap odd even
  //{{{  sequencer
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA122); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // EnterPreview: Auto Exposure = 1
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA123); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // EnterPreview: Flicker Detection = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA124); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // EnterPreview: Auto White Balance = 1
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA125); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // EnterPreview: Auto Focus = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA126); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // EnterPreview: Histogram = 1
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA127); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // EnterPreview: Strobe Control  = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA128); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // EnterPreview: Skip Control = 0

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA129); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x03); // InPreview: Auto Exposure = 3
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA12A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02); // InPreview: Flicker Detection = 2
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA12B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x03); // InPreview: Auto White Balance = 3
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA12C); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // InPreview: Auto Focus = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA12D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x03); // InPreview: Histogram  = 3
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA12E); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // InPreview: Strobe Control = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA12F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // InPreview: Skip Control = 0

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA130); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x04); // ExitPreview: Auto Exposure = 4
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA131); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // ExitPreview: Flicker Detection = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA132); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // ExitPreview: Auto White Balance = 1
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA133); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // ExitPreview: Auto Focus = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA134); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // ExitPreview: Histogram = 1
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA135); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // ExitPreview: Strobe Control = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA136); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // ExitPreview: Skip Control = 0

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA137); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Auto Exposure = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA138); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Flicker Detection = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA139); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Auto White Balance  = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA13A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Auto Focus = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA13B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Histogram = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA13C); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Strobe Control = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA13D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Capture: Skip Control = 0
  //}}}
  //{{{  register wizard
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
  //{{{  gamma tables
  // gamma table A 0 to 18
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA745); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00);  // 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA746); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x14);  // 20
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA747); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x23);  // 35
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA748); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x3A);  // 58
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA749); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x5E);  // 94
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA74A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x76);  // 118
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA74B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x88);  // 136
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA74C); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x96);  // 150
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA74D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xA3);  // 163
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA74E); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xAF);  // 175
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA74F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xBA);  // 186
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA750); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xC4);  // 196
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA751); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xCE);  // 206
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA752); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xD7);  // 215
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA753); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE0);  // 224
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA754); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE8);  // 232
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA755); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xF0);  // 240
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA756); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xF8);  // 248
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA757); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xFF);  // 255

  // gamma table B 0 to 18
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA758); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00);  // 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA759); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x14);  // 20
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA75A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x23);  // 35
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA75B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x3A);  // 58
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA75C); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x5E);  // 94
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA75D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x76);  // 118
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA75E); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x88);  // 136
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA75F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x96);  // 150
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA760); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xA3);  // 163
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA761); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xAF);  // 175
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA762); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xBA);  // 186
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA763); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xC4);  // 196
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA764); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xCE);  // 206
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA765); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xD7);  // 215
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA766); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE0);  // 224
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA767); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE8);  // 232
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA768); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xF0);  // 240
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA769); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xF8);  // 248
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA76A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xFF);  // 255
  //}}}

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x06); // Sequencer Refresh Mode
  HAL_Delay (200);
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x05); // Sequencer Refresh
  HAL_Delay (200);

  //{{{  focus init
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
  }
//}}}
//{{{
void cCamera::dcmiInit (DCMI_HandleTypeDef* dcmi) {

  // config DCMI
  dcmi->Instance = DCMI;
  dcmi->Init.CaptureRate = DCMI_CR_ALL_FRAME;
  dcmi->Init.HSPolarity = DCMI_HSPOLARITY_LOW;
  dcmi->Init.VSPolarity = DCMI_HSPOLARITY_LOW;
  dcmi->Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
  dcmi->Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  dcmi->Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;

  // Associate the initialized DMA handle to the DCMI handle
  dmaHandler.Init.Channel             = DMA_CHANNEL_1;
  dmaHandler.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  dmaHandler.Init.PeriphInc           = DMA_PINC_DISABLE;
  dmaHandler.Init.MemInc              = DMA_MINC_ENABLE;
  dmaHandler.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dmaHandler.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  dmaHandler.Init.Mode                = DMA_CIRCULAR;
  dmaHandler.Init.Priority            = DMA_PRIORITY_HIGH;
  dmaHandler.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  dmaHandler.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  dmaHandler.Init.MemBurst            = DMA_MBURST_SINGLE;
  dmaHandler.Init.PeriphBurst         = DMA_PBURST_SINGLE;
  dmaHandler.Instance = DMA2_Stream1;
  __HAL_LINKDMA (dcmi, DMA_Handle, dmaHandler);

  // NVIC configuration for DCMI transfer complete interrupt
  HAL_NVIC_SetPriority (DCMI_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (DCMI_IRQn);

  // NVIC configuration for DMA2D transfer complete interrupt
  HAL_NVIC_SetPriority (DMA2_Stream1_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (DMA2_Stream1_IRQn);

  // Configure the DMA stream
  dmaInit (dcmi->DMA_Handle);

  // Configures the HS, VS, DE and PC polarity
  DCMI->CR &= ~(DCMI_CR_PCKPOL | DCMI_CR_HSPOL  | DCMI_CR_VSPOL  |
                DCMI_CR_EDM_0  | DCMI_CR_EDM_1  | DCMI_CR_FCRC_0 |
                DCMI_CR_FCRC_1 | DCMI_CR_JPEG   | DCMI_CR_ESS |
                DCMI_CR_BSM_0  | DCMI_CR_BSM_1  | DCMI_CR_OEBS |
                DCMI_CR_LSM | DCMI_CR_OELS);
  DCMI->CR |=  (uint32_t)(dcmi->Init.SynchroMode     | dcmi->Init.CaptureRate |
                          dcmi->Init.VSPolarity      | dcmi->Init.HSPolarity  |
                          dcmi->Init.PCKPolarity     | dcmi->Init.ExtendedDataMode |
                          dcmi->Init.JPEGMode        | dcmi->Init.ByteSelectMode |
                          dcmi->Init.ByteSelectStart | dcmi->Init.LineSelectMode |
                          dcmi->Init.LineSelectStart);

  // Enable Error and Overrun interrupts
  __HAL_DCMI_ENABLE_IT (dcmi, DCMI_IT_ERR | DCMI_IT_OVR);
  }
//}}}
//{{{
void cCamera::dcmiStart (DCMI_HandleTypeDef* dcmi, uint32_t DCMI_Mode, uint32_t data, uint32_t length) {

  // enable DCMI by setting DCMIEN bit
  __HAL_DCMI_ENABLE (dcmi);

  // config the DCMI Mode
  DCMI->CR = (DCMI->CR & ~(DCMI_CR_CM)) | DCMI_Mode;

  // calc the number of xfers with xferSize <= 64k
  dcmi->pBuffPtr = data;
  dcmi->XferTransferNumber = 1;
  dcmi->XferSize = length;
  while (dcmi->XferSize > 0xFFFF) {
    dcmi->XferSize = dcmi->XferSize / 2;
    dcmi->XferTransferNumber = dcmi->XferTransferNumber * 2;
    }
  dcmi->XferCount = dcmi->XferTransferNumber;

  dmaMultiBufferStart (dcmi->DMA_Handle, (uint32_t)&dcmi->Instance->DR,
                       data, data + (4*dcmi->XferSize), dcmi->XferSize);
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "dmaStart len:%d xferCount:%d xferSize:%d", length, dcmi->XferCount, dcmi->XferSize);

  // enable capture
  DCMI->CR |= DCMI_CR_CAPTURE;
  }
//}}}
