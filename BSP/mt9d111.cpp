// mt9d111.cpp
//{{{  includes
#include "stm32746g_discovery.h"
#include "camera.h"
//}}}

#define i2cAddress 0x90

DCMI_HandleTypeDef dcmiHandler;
DMA_HandleTypeDef dmaHandler;
uint32_t cameraCurrentResolution;
cLcd* lcdPtr = nullptr;

extern "C" {
  //{{{
  void dcmiDmaXferComplete (DMA_HandleTypeDef* dma) {

    DCMI_HandleTypeDef* dcmi = (DCMI_HandleTypeDef*)((DMA_HandleTypeDef*)dma)->Parent;

    if (dcmi->XferCount != 0) {
      uint32_t tmp = (DMA2_Stream1->CR) & DMA_SxCR_CT;
      if (((dcmi->XferCount % 2) == 0) && (tmp != 0)) {
        // Update memory 0 address location
        uint32_t tmp = DMA2_Stream1->M0AR;
        HAL_DMAEx_ChangeMemory (dcmi->DMA_Handle, (tmp + (8*dcmi->XferSize)), MEMORY0);
        dcmi->XferCount--;
        }

      else if ((DMA2_Stream1->CR & DMA_SxCR_CT) == 0) {
        // Update memory 1 address location
        uint32_t tmp = DMA2_Stream1->M1AR;
        HAL_DMAEx_ChangeMemory (dcmi->DMA_Handle, (tmp + (8*dcmi->XferSize)), MEMORY1);
        dcmi->XferCount--;
        }
      }

    else if ((DMA2_Stream1->CR & DMA_SxCR_CT) != 0)
      // Update memory 0 address location
      DMA2_Stream1->M0AR = dcmi->pBuffPtr;

    else if ((DMA2_Stream1->CR & DMA_SxCR_CT) == 0) {
      // Update memory 1 address location
      uint32_t tmp = dcmi->pBuffPtr;
      DMA2_Stream1->M1AR = tmp + (4*dcmi->XferSize);
      dcmi->XferCount = dcmi->XferTransferNumber;
      }

    // Check if the frame is transferred
    if (dcmi->XferCount == dcmi->XferTransferNumber)
      // Enable the Frame interrupt
      __HAL_DCMI_ENABLE_IT (dcmi, DCMI_IT_FRAME);
    }
  //}}}
  //{{{
  void dcmiDmaError (DMA_HandleTypeDef* dma) {

    DCMI_HandleTypeDef* dcmi = (DCMI_HandleTypeDef*)((DMA_HandleTypeDef*)dma)->Parent;
    if (dcmi->DMA_Handle->ErrorCode != HAL_DMA_ERROR_FE)
      lcdPtr->debug (LCD_COLOR_RED, "DCMI DMAerror %x", dcmi->DMA_Handle->ErrorCode);
    }
  //}}}
  //{{{
  void dcmiError (DMA_HandleTypeDef* dma) {
    lcdPtr->debug (LCD_COLOR_RED, "DCMIerror");
    }
  //}}}

  //{{{
  void DCMI_IRQHandler() {

    uint32_t isr_value = READ_REG (DCMI->MISR);
    if ((isr_value & DCMI_FLAG_ERRRI) == DCMI_FLAG_ERRRI) {
      // Synchronization error interrupt
      __HAL_DCMI_CLEAR_FLAG (&dcmiHandler, DCMI_FLAG_ERRRI);
      dcmiHandler.DMA_Handle->XferAbortCallback = dcmiError;
      HAL_DMA_Abort_IT (dcmiHandler.DMA_Handle);
      }

    if ((isr_value & DCMI_FLAG_OVRRI) == DCMI_FLAG_OVRRI) {
      // Overflow interrupt
      __HAL_DCMI_CLEAR_FLAG (&dcmiHandler, DCMI_FLAG_OVRRI);
      dcmiHandler.DMA_Handle->XferAbortCallback = dcmiError;
      HAL_DMA_Abort_IT (dcmiHandler.DMA_Handle);
      }

    if ((isr_value & DCMI_FLAG_FRAMERI) == DCMI_FLAG_FRAMERI) {
      // frame interrupt, in snapshot mode, disable Vsync, Error and Overrun interrupts
      if ((DCMI->CR & DCMI_CR_CM) == DCMI_MODE_SNAPSHOT)
        __HAL_DCMI_DISABLE_IT (&dcmiHandler, DCMI_IT_LINE | DCMI_IT_VSYNC | DCMI_IT_ERR | DCMI_IT_OVR);
      __HAL_DCMI_DISABLE_IT (&dcmiHandler, DCMI_IT_FRAME);
      __HAL_DCMI_CLEAR_FLAG (&dcmiHandler, DCMI_FLAG_FRAMERI);
      lcdPtr->debug (LCD_COLOR_GREEN, "frameIrq");
      }
    }
  //}}}
  void DMA2_Stream1_IRQHandler() { HAL_DMA_IRQHandler (dcmiHandler.DMA_Handle); }
  }

//{{{
void mt9d111Init (uint32_t resolution) {

  //  soft reset
  CAMERA_IO_Write16 (i2cAddress, 0x65, 0xA000); // Bypass the PLL, R0x65:0 = 0xA000,
  CAMERA_IO_Write16 (i2cAddress, 0xF0, 1); // page 1
  CAMERA_IO_Write16 (i2cAddress, 0xC3, 0x0501); // Perform MCU reset by setting R0xC3:1 = 0x0501.
  CAMERA_IO_Write16 (i2cAddress, 0xF0, 0); // page 0
  CAMERA_IO_Write16 (i2cAddress, 0x0D, 0x0021); // Enable soft reset by setting R0x0D:0 = 0x0021. Bit 0 is used for the sensor core reset
  CAMERA_IO_Write16 (i2cAddress, 0x0D, 0x0000); // Disable soft reset by setting R0x0D:0 = 0x0000.
  HAL_Delay (100);

  CAMERA_IO_Write16 (i2cAddress, 0x07, 0x00FE); // HBLANK A = 254
  CAMERA_IO_Write16 (i2cAddress, 0x08, 0x000C); // VBLANK A = 12
  CAMERA_IO_Write16 (i2cAddress, 0x21, 0x8400); // Read Mode A = 15:binning 10:bothADC

  CAMERA_IO_Write16 (i2cAddress, 0x05, 0x0204); // HBLANK B = 516
  CAMERA_IO_Write16 (i2cAddress, 0x06, 0x0014); // VBLANK B = 20
  CAMERA_IO_Write16 (i2cAddress, 0x20, 0x0300); // Read Mode B = 9:showBorder 8:overSized

  // PLL - M=18,N=1,P=2 - (24mhz/(N+1))*M / 2*(P+1) = 36mhz
  CAMERA_IO_Write16 (i2cAddress, 0x66, 0x1201); // PLLControl1 -    M:N
  CAMERA_IO_Write16 (i2cAddress, 0x67, 0x0502); // PLLControl2 - 0x05:P
  CAMERA_IO_Write16 (i2cAddress, 0x65, 0xA000); // Clock CNTRL - PLL ON
  CAMERA_IO_Write16 (i2cAddress, 0x65, 0x2000); // Clock CNTRL - USE PLL
  HAL_Delay (100);

  // page 1
  CAMERA_IO_Write16 (i2cAddress, 0xF0, 1);
  CAMERA_IO_Write16 (i2cAddress, 0x97, 0x22); // outputFormat - RGB565, swap odd even

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x270B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0030); // mode_config = disable jpeg A,B
  //{{{  preview A, capture B params
  //{{{  preview A
  /*
  ; Max Frame Time: 33.3333 msec
  ; Max Frame Clocks: 1316666.6 clocks (39.500 MHz)
  ; No. of ADCs: 1
  ; Skip Mode: 1x cols, 1x rows, Bin Mode: Yes
  ; Active Sensor Columns: 808 pixels / 1616 clocks
  ; Active Sensor Rows: 608 rows
  ; Horiz Blanking: 254 pixels / 508 clocks
  ; Vert Blanking: 11 rows
  ; Extra Delay: 955 clocks
  ;
  ; Actual Frame Clocks: 1316666 clocks
  ; Row Time: 53.772 usec / 2124 clocks
  ; Frame time: 33.333316 msec
  ; Frames per Sec: 30 fps
  ;
  ; Max Shutter Delay: 402
  ; 50Hz Flicker Period: 185.97 lines
  ; 60Hz Flicker Period: 154.97 lines
  */
  //}}}
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2703); CAMERA_IO_Write16 (i2cAddress, 0xC8, 800);    // Output Width A  = 800
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2705); CAMERA_IO_Write16 (i2cAddress, 0xC8, 600);    // Output Height A = 600
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x270F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x001C); // Row Start A = 28
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2711); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x003C); // Column Start A = 60
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2713); CAMERA_IO_Write16 (i2cAddress, 0xC8, 1200);   // Row Height A = 1200
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2715); CAMERA_IO_Write16 (i2cAddress, 0xC8, 1600);   // Column Width A = 1600
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2717); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0384); // Extra Delay A = 900
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2719); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0011); // Row Speed A = 17
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2727); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);      // Crop_X0 A = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2729); CAMERA_IO_Write16 (i2cAddress, 0xC8, 800);    // Crop_X1 A = 800
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x272B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);      // Crop_Y0 A = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x272D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 600);    // Crop_Y1 A = 600
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA743); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02);   // Gamma and Contrast Settings A
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA77D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x22);   // outputFormat A - RGB565, swap odd even

  //{{{  capture
  /*
  ; Max Frame Time: 66.6667 msec
  ; Max Frame Clocks: 2633333.3 clocks (39.500 MHz)
  ; No. of ADCs: 2
  ; Skip Mode: 1x cols, 1x rows, Bin Mode: No
  ; Active Sensor Columns: 1608 pixels
  ; Active Sensor Rows: 1208 rows
  ; Horiz Blanking: 516 pixels
  ; Vert Blanking: 31 rows
  ; Extra Delay: 1697 clocks
  ;
  ; Actual Frame Clocks: 2633333 clocks
  ; Row Time: 53.772 usec / 2124 clocks
  ; Frame time: 66.666658 msec
  ; Frames per Sec: 15 fps
  ;
  ; Max Shutter Delay: 1663
  ; 50Hz Flicker Period: 185.97 lines
  ; 60Hz Flicker Period: 154.97 lines
  */
  //}}}
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2707); CAMERA_IO_Write16 (i2cAddress, 0xC8, 1600);   // Output Width B  = 1600
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2709); CAMERA_IO_Write16 (i2cAddress, 0xC8, 1200);   // Output Height B = 1200
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x271B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x001C); // Row Start B = 28
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x271D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x003C); // Column Start B = 60
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x271F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 1200);   // Row Height B = 1200
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2721); CAMERA_IO_Write16 (i2cAddress, 0xC8, 1600);   // Column Width B = 1600
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2723); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01A7); // Extra Delay B = 423
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2725); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0011); // Row Speed B = 17
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2735); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);      // Crop_X0 B = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2737); CAMERA_IO_Write16 (i2cAddress, 0xC8, 1600);   // Crop_X1 B = 1600
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2739); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0);      // Crop_Y0 B = 0
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x273B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 1200);   // Crop_Y1 B = 1200
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA744); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02);   // Gamma and Contrast Settings B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA77E); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x22);   // outputFormat B - RGB565, swap odd even
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
  //{{{  other config
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x276D); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE0E2); // FIFO_Conf1 A = 57570
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA76F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE1);   // FIFO_Conf2 A = 225
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2774); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE0E1); // FIFO_Conf1 B = 57569
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA776); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0xE1);   // FIFO_Conf2 B = 225

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x220B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x0192); // Max R12 B (Shutter Delay) = 402
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA217); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x08);   // IndexTH23 = 8
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2228); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x020F); // RowTime (msclk per)/4 = 527

  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x222F); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x009A); // R9 Step = 94
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA408); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x24);   // search_f1_50 = 21
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA409); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x26);   // search_f2_50 = 23
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA40A); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x1D);   // search_f1_60 = 17
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA40B); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x1F);   // search_f2_60 = 19
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2411); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x009A); // R9_Step_60 = 94
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0x2413); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00B9); // R9_Step_50 = 112
  HAL_Delay (100);
  //}}}

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
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x06); // Sequencer Refresh Mode
  HAL_Delay (100);
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x05); // Sequencer Refresh
  HAL_Delay (100);

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
void dcmiInit (DCMI_HandleTypeDef* dcmi) {

  // config DCMI
  dcmi->Instance = DCMI;
  dcmi->Init.CaptureRate = DCMI_CR_ALL_FRAME;
  dcmi->Init.HSPolarity = DCMI_HSPOLARITY_LOW;
  dcmi->Init.VSPolarity = DCMI_HSPOLARITY_LOW;
  dcmi->Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
  dcmi->Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  dcmi->Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;

  //{{{  cinfig clocks
  __HAL_RCC_DCMI_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  //}}}
  //{{{  config gpio alternate function
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
  //}}}

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
  HAL_DMA_Init (dcmi->DMA_Handle);

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
void dcmiStart (DCMI_HandleTypeDef* dcmi, uint32_t DCMI_Mode, uint32_t data, uint32_t length) {

  // enable DCMI by setting DCMIEN bit
  __HAL_DCMI_ENABLE (dcmi);

  // config the DCMI Mode
  DCMI->CR = (DCMI->CR & ~(DCMI_CR_CM)) | DCMI_Mode;

  dcmi->DMA_Handle->XferCpltCallback = dcmiDmaXferComplete;
  dcmi->DMA_Handle->XferErrorCallback = dcmiDmaError;
  dcmi->DMA_Handle->XferAbortCallback = NULL;

  // reset transfer counters value
  dcmi->XferCount = 0;
  dcmi->XferTransferNumber = 0;

  if (length <= 0xFFFF)
    // enable the DMA Stream
    HAL_DMA_Start_IT (dcmi->DMA_Handle, (uint32_t)&dcmi->Instance->DR, data, length);
  else {
    // DCMI_DOUBLE_BUFFER Mode, set the DMA memory1 conversion complete callback
    dcmi->DMA_Handle->XferM1CpltCallback = dcmiDmaXferComplete;

    // Initialize transfer parameters
    dcmi->XferCount = 1;
    dcmi->XferSize = length;
    dcmi->pBuffPtr = data;

    // Get the number of buffer
    while (dcmi->XferSize > 0xFFFF) {
      dcmi->XferSize = dcmi->XferSize / 2;
      dcmi->XferCount = dcmi->XferCount * 2;
      }

    dcmi->XferCount = dcmi->XferCount - 2;
    dcmi->XferTransferNumber = dcmi->XferCount;

    // start DMA multiBuffer transfer
    HAL_DMAEx_MultiBufferStart_IT (dcmi->DMA_Handle, (uint32_t)&dcmi->Instance->DR,
                                   data, data + (4*dcmi->XferSize), dcmi->XferSize);
    }

  // enable capture
  DCMI->CR |= DCMI_CR_CAPTURE;
  }
//}}}

// external
//{{{
void cameraInit (cLcd* lcd, uint32_t resolution) {

  lcdPtr = lcd;

  // init camera i2c, readBack id
  CAMERA_IO_Init();
  CAMERA_IO_Write16 (i2cAddress, 0xF0, 0);
  lcdPtr->debug (LCD_COLOR_YELLOW, "cameraId %x", CAMERA_IO_Read16 (i2cAddress, 0));

  dcmiInit (&dcmiHandler);
  mt9d111Init (resolution);
  cameraCurrentResolution = resolution;

  cameraPreview();
  }
//}}}

//{{{
uint32_t cameraGetXSize() {

  switch (cameraCurrentResolution) {
    case CAMERA_R160x120: return 160;
    case CAMERA_R320x240: return 320;
    case CAMERA_R480x272: return 480;
    case CAMERA_R640x480: return 640;
    case CAMERA_R800x600: return 800;
    case CAMERA_R1600x1200: return 1600;
    default: return 0;
    }
  }
//}}}
//{{{
uint32_t cameraGetYSize() {

  switch (cameraCurrentResolution) {
    case CAMERA_R160x120: return 120;
    case CAMERA_R320x240: return 240;
    case CAMERA_R480x272: return 272;
    case CAMERA_R640x480: return 280;
    case CAMERA_R800x600: return 600;
    case CAMERA_R1600x1200: return 1200;
    default: return 0;
    }
  }
//}}}

//{{{
void cameraStart (uint32_t buffer, bool continuous) {

  dcmiStart (&dcmiHandler, continuous ? DCMI_MODE_CONTINUOUS : DCMI_MODE_SNAPSHOT,
             buffer, cameraGetXSize() * cameraGetYSize() / 2);
  }
//}}}
//{{{
void cameraPreview() {
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA120); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x00); // Sequencer.params.mode - none
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x01); // Sequencer goto preview A - 800x600
  }
//}}}
//{{{
void cameraCapture() {
  // use capture B
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA120); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02); // Sequencer.params.mode - capture video
  CAMERA_IO_Write16 (i2cAddress, 0xC6, 0xA103); CAMERA_IO_Write16 (i2cAddress, 0xC8, 0x02); // Sequencer goto capture B  - 1600x1200
  }
//}}}

//{{{
void cameraSetFocus (int value) {

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
