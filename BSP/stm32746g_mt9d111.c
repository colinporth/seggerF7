//{{{  includes
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_camera.h"
//}}}
//{{{  OV9655 Registers definition
#define OV9655_BRIGHTNESS_LEVEL0        0xB0     /* Brightness level -2         */
#define OV9655_BRIGHTNESS_LEVEL1        0x98     /* Brightness level -1         */
#define OV9655_BRIGHTNESS_LEVEL2        0x00     /* Brightness level 0          */
#define OV9655_BRIGHTNESS_LEVEL3        0x18     /* Brightness level +1         */
#define OV9655_BRIGHTNESS_LEVEL4        0x30     /* Brightness level +2         */

#define OV9655_CONTRAST_LEVEL0          0x30     /* Contrast level -2           */
#define OV9655_CONTRAST_LEVEL1          0x38     /* Contrast level -1           */
#define OV9655_CONTRAST_LEVEL2          0x40     /* Contrast level 0            */
#define OV9655_CONTRAST_LEVEL3          0x50     /* Contrast level +1           */
#define OV9655_CONTRAST_LEVEL4          0x60     /* Contrast level +2           */

#define CAMERA_480x272_RES_X 480
#define CAMERA_480x272_RES_Y 272
#define CAMERA_VGA_RES_X     640
#define CAMERA_VGA_RES_Y     480
#define CAMERA_QVGA_RES_X    320
#define CAMERA_QVGA_RES_Y    240
#define CAMERA_QQVGA_RES_X   160
#define CAMERA_QQVGA_RES_Y   120
//}}}

DCMI_HandleTypeDef hDcmiHandler;

static DMA_HandleTypeDef hdma_handler;
static uint32_t CameraCurrentResolution;

void DCMI_IRQHandler() { HAL_DCMI_IRQHandler (&hDcmiHandler); }
void DMA2_Stream1_IRQHandler() { HAL_DMA_IRQHandler (hDcmiHandler.DMA_Handle); }

//{{{
static void mspInit (DCMI_HandleTypeDef* hdcmi, void* Params) {

  __HAL_RCC_DCMI_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  // Configure DCMI GPIO as alternate function
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

  hdma_handler.Init.Channel             = DMA_CHANNEL_1;
  hdma_handler.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_handler.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_handler.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_handler.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  hdma_handler.Init.Mode                = DMA_CIRCULAR;
  hdma_handler.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma_handler.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  hdma_handler.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_handler.Init.MemBurst            = DMA_MBURST_SINGLE;
  hdma_handler.Init.PeriphBurst         = DMA_PBURST_SINGLE;
  hdma_handler.Instance = DMA2_Stream1;

  // Associate the initialized DMA handle to the DCMI handle
  __HAL_LINKDMA (hdcmi, DMA_Handle, hdma_handler);

  // NVIC configuration for DCMI transfer complete interrupt
  HAL_NVIC_SetPriority (DCMI_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (DCMI_IRQn);

  // NVIC configuration for DMA2D transfer complete interrupt
  HAL_NVIC_SetPriority (DMA2_Stream1_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (DMA2_Stream1_IRQn);

  // Configure the DMA stream
  HAL_DMA_Init (hdcmi->DMA_Handle);
  }
//}}}
//{{{
static void mspDeInit (DCMI_HandleTypeDef* hdcmi, void* Params) {

  HAL_NVIC_DisableIRQ (DCMI_IRQn);
  HAL_NVIC_DisableIRQ (DMA2_Stream1_IRQn);

  HAL_DMA_DeInit (hdcmi->DMA_Handle);
  __HAL_RCC_DCMI_CLK_DISABLE();
  }
//}}}

//{{{
static uint32_t getPix (uint32_t resolution) {

  switch (resolution) {
    case CAMERA_R160x120: return  0x2580;
    case CAMERA_R320x240: return  0x9600;
    case CAMERA_R480x272: return  0xFF00;
    case CAMERA_R640x480: return 0x25800;
    case CAMERA_R800x600: return 0x3A980;
    default: return 0;
    }
  }
//}}}
//{{{
static uint64_t convertValue (uint32_t value) {

  switch (value) {
    case CAMERA_BRIGHTNESS_LEVEL0: return OV9655_BRIGHTNESS_LEVEL0;
    case CAMERA_BRIGHTNESS_LEVEL1: return OV9655_BRIGHTNESS_LEVEL1;
    case CAMERA_BRIGHTNESS_LEVEL2: return OV9655_BRIGHTNESS_LEVEL2;
    case CAMERA_BRIGHTNESS_LEVEL3: return OV9655_BRIGHTNESS_LEVEL3;
    case CAMERA_BRIGHTNESS_LEVEL4: return OV9655_BRIGHTNESS_LEVEL4;

    case CAMERA_CONTRAST_LEVEL0: return OV9655_CONTRAST_LEVEL0;
    case CAMERA_CONTRAST_LEVEL1: return OV9655_CONTRAST_LEVEL1;
    case CAMERA_CONTRAST_LEVEL2: return OV9655_CONTRAST_LEVEL2;
    case CAMERA_CONTRAST_LEVEL3: return OV9655_CONTRAST_LEVEL3;
    case CAMERA_CONTRAST_LEVEL4: return OV9655_CONTRAST_LEVEL4;

    default: return OV9655_CONTRAST_LEVEL0;
    }

  }
//}}}

//{{{
static void init (uint16_t DeviceAddr, uint32_t resolution) {

  HAL_Delay (200);

  //  soft reset
  CAMERA_IO_Write16 (DeviceAddr, 0x65, 0xA000); // Bypass the PLL, R0x65:0 = 0xA000,
  CAMERA_IO_Write16 (DeviceAddr, 0xF0, 1); // page 1
  CAMERA_IO_Write16 (DeviceAddr, 0xC3, 0x0501); // Perform MCU reset by setting R0xC3:1 = 0x0501.
  CAMERA_IO_Write16 (DeviceAddr, 0xF0, 0); // page 0
  CAMERA_IO_Write16 (DeviceAddr, 0x0D, 0x0021); // Enable soft reset by setting R0x0D:0 = 0x0021. Bit 0 is used for the sensor core reset
  CAMERA_IO_Write16 (DeviceAddr, 0x0D, 0x0000); // Disable soft reset by setting R0x0D:0 = 0x0000.
  HAL_Delay (100);

  CAMERA_IO_Write16 (DeviceAddr, 0x05, 0x0204); // HBLANK B = 516
  CAMERA_IO_Write16 (DeviceAddr, 0x06, 0x0014); // VBLANK B = 20
  CAMERA_IO_Write16 (DeviceAddr, 0x07, 0x00FE); // HBLANK A = 254
  CAMERA_IO_Write16 (DeviceAddr, 0x08, 0x000C); // VBLANK A = 12
  CAMERA_IO_Write16 (DeviceAddr, 0x20, 0x0300); // Read Mode B = 9:showBorder 8:overSized
  CAMERA_IO_Write16 (DeviceAddr, 0x21, 0x8400); // Read Mode A = 15:binning 10:bothADC

  //  PLL
  CAMERA_IO_Write16 (DeviceAddr, 0x66, 0x1001);  // PLL Control 1    M:15:8,N:7:0 - M=16, N=1  (24mhz/(N+1))*M / 2*(P+1) = 48mhz
  CAMERA_IO_Write16 (DeviceAddr, 0x67, 0x0501);  // PLL Control 2 0x05:15:8,P:7:0 - P=1
  //CAMERA_IO_Write16 (DeviceAddr, 0x66, 0x8011);  // PLL Control 1    M:15:8,N:7:0 - M=79, N=2 (24mhz/(N+1))*M / 2*(P+1) = 80mhz
  //CAMERA_IO_Write16 (DeviceAddr, 0x67, 0x0500);  // PLL Control 2 0x05:15:8,P:7:0 - P=1
  CAMERA_IO_Write16 (DeviceAddr, 0x65, 0xA000);  // Clock CNTRL - PLL ON
  CAMERA_IO_Write16 (DeviceAddr, 0x65, 0x2000);  // Clock CNTRL - USE PLL
  HAL_Delay (100);

  // page 1
  CAMERA_IO_Write16 (DeviceAddr, 0xF0, 1);
  CAMERA_IO_Write16 (DeviceAddr, 0x97, 0x0002); // output format configuration luma:chroma swap

  //{{{  sequencer
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA122); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x01); // Enter Preview: Auto Exposure = 1
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA123); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Enter Preview: Flicker Detection = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA124); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x01); // Enter Preview: Auto White Balance = 1
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA125); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Enter Preview: Auto Focus = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA126); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x01); // Enter Preview: Histogram = 1
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA127); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Enter Preview: Strobe Control  = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA128); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Enter Preview: Skip Control = 0

  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA129); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x03); // In Preview: Auto Exposure = 3
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA12A); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x02); // In Preview: Flicker Detection = 2
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA12B); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x03); // In Preview: Auto White Balance = 3
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA12C); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // In Preview: Auto Focus = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA12D); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x03); // In Preview: Histogram  = 3
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA12E); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // In Preview: Strobe Control = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA12F); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // In Preview: Skip Control = 0

  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA130); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x04); // Exit Preview: Auto Exposure = 4
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA131); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Exit Preview: Flicker Detection = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA132); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x01); // Exit Preview: Auto White Balance = 1
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA133); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Exit Preview: Auto Focus = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA134); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x01); // Exit Preview: Histogram = 1
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA135); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Exit Preview: Strobe Control = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA136); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Exit Preview: Skip Control = 0

  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA137); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Capture: Auto Exposure = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA138); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Capture: Flicker Detection = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA139); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Capture: Auto White Balance  = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA13A); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Capture: Auto Focus = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA13B); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Capture: Histogram = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA13C); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Capture: Strobe Control = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA13D); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Capture: Skip Control = 0
  //}}}
  //{{{  mode a,b params
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x270B); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x0030); // mode_config = disable jpeg A,B

  //{{{  [MODE A PARAMETERS]
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
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2703); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 800);    // Output Width A  = 800
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2705); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 600);    // Output Height A = 600
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x270F); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x001C); // Row Start A = 28
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2711); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x003C); // Column Start A = 60
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2713); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 1200);   // Row Height A = 1200
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2715); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 1600);   // Column Width A = 1600
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2717); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x0384); // Extra Delay A = 900
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2719); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x0011); // Row Speed A = 17
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2727); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0);      // Crop_X0 A = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2729); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 800);    // Crop_X1 A = 800
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x272B); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0);      // Crop_Y0 A = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x272D); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 600);    // Crop_Y1 A = 600
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA743); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x02);   // Gamma and Contrast Settings A
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA77D); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x02);   // output format config A = 0x02 swap luma:chroma

  //{{{  [MODE B PARAMETERS]
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
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2707); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 1600);   // Output Width B  = 1600
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2709); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 1200);   // Output Height B = 1200
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x271B); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x001C); // Row Start B = 28
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x271D); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x003C); // Column Start B = 60
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x271F); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 1200);   // Row Height B = 1200
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2721); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 1600);   // Column Width B = 1600
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2723); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x01A7); // Extra Delay B = 423
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2725); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x0011); // Row Speed B = 17
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2735); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0);      // Crop_X0 B = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2737); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 1600);   // Crop_X1 B = 1600
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2739); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0);      // Crop_Y0 B = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x273B); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 1200);   // Crop_Y1 B = 1200
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA744); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x02);   // Gamma and Contrast Settings B
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA77E); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x02);   // output format config B = 0x02 swap luma:chroma
  //}}}
  //{{{  Custom gamma tables...
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA745);    //Gamma Table 0 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00);  //      = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA746);    //Gamma Table 1 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x14);  //      = 20
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA747);    //Gamma Table 2 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x23);  //      = 35
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA748);    //Gamma Table 3 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x3A);  //      = 58
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA749);    //Gamma Table 4 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x5E);  //      = 94
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA74A);    //Gamma Table 5 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x76);  //      = 118
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA74B);    //Gamma Table 6 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x88);  //      = 136
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA74C);    //Gamma Table 7 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x96);  //      = 150
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA74D);    //Gamma Table 8 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xA3);  //      = 163
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA74E);    //Gamma Table 9 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xAF);  //      = 175
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA74F);    //Gamma Table 10 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xBA);  //      = 186
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA750);    //Gamma Table 11 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xC4);  //      = 196
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA751);    //Gamma Table 12 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xCE);  //      = 206
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA752);    //Gamma Table 13 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xD7);  //      = 215
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA753);    //Gamma Table 14 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xE0);  //      = 224
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA754);    //Gamma Table 15 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xE8);  //      = 232
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA755);    //Gamma Table 16 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xF0);  //      = 240
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA756);    //Gamma Table 17 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xF8);  //      = 248
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA757);    //Gamma Table 18 A
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xFF);  //      = 255
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA758);    //Gamma Table 0 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00);  //      = 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA759);    //Gamma Table 1 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x14);  //      = 20
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA75A);    //Gamma Table 2 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x23);  //      = 35
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA75B);    //Gamma Table 3 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x3A);  //      = 58
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA75C);    //Gamma Table 4 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x5E);  //      = 94
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA75D);    //Gamma Table 5 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x76);  //      = 118
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA75E);    //Gamma Table 6 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x88);  //      = 136
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA75F);    //Gamma Table 7 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x96);  //      = 150
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA760);    //Gamma Table 8 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xA3);  //      = 163
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA761);    //Gamma Table 9 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xAF);  //      = 175
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA762);    //Gamma Table 10 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xBA);  //      = 186
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA763);    //Gamma Table 11 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xC4);  //      = 196
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA764);    //Gamma Table 12 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xCE);  //      = 206
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA765);    //Gamma Table 13 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xD7);  //      = 215
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA766);    //Gamma Table 14 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xE0);  //      = 224
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA767);    //Gamma Table 15 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xE8);  //      = 232
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA768);    //Gamma Table 16 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xF0);  //      = 240
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA769);    //Gamma Table 17 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xF8);  //      = 248
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA76A);    //Gamma Table 18 B
  CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xFF);  //      = 255
  //}}}
  //{{{  other config
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x276D); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xE0E2); // FIFO_Conf1 A = 57570
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA76F); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xE1);   // FIFO_Conf2 A = 225
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2774); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xE0E1); // FIFO_Conf1 B = 57569
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA776); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xE1);   // FIFO_Conf2 B = 225

  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x220B); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x0192); // Max R12 B (Shutter Delay) = 402
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA217); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x08);   // IndexTH23 = 8
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2228); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x020F); // RowTime (msclk per)/4 = 527

  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x222F); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x009A); // R9 Step = 94
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA408); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x24);   // search_f1_50 = 21
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA409); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x26);   // search_f2_50 = 23
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA40A); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x1D);   // search_f1_60 = 17
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA40B); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x1F);   // search_f2_60 = 19
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2411); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x009A); // R9_Step_60 = 94
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x2413); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00B9); // R9_Step_50 = 112
  //}}}
  HAL_Delay (100);

  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA103); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x06); // Sequencer Refresh Mode
  HAL_Delay (200);

  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA103); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x05); // Sequencer Refresh
  HAL_Delay (200);

  //{{{  focus init
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x90B6); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x01); // SFR GPIO suspend

  // enable GPIO0,1 as output, initial value 0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x9079); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xFC); // SFR GPIO data direction
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x9071); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // SFR GPIO data b1:0 = 0 GPIO0,1 initial 0

  // use 8bit counter clkdiv 2^(1+2)=8 -> 48mhz -> 6mhz ->> 23.7khz
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x90B0); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x01); // SFR GPIO wg_config b0 = 1 8bit counter
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x90B2); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x02); // SFR GPIO wg_clkdiv b0 = 2

  // GPIO0
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x908B); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // SFR GPIO wg_n0 = 0 infinite
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x9081); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 255);  // SFR GPIO wg_t00 = 255 initial off
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x9083); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0);    // SFR GPIO wg_t10 = 0 no on

  // GPIO1
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x908A); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // SFR GPIO wg_n1 = 0 infinite
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x9080); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0xFF); // SFR GPIO wg_t01 = 255 max initial on
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x9082); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // SFR GPIO wg_t11 = 0 no off

  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x90B5); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // SFR GPIO reset
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0x90B6); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // SFR GPIO suspend
  //}}}
  //sensorScaling (600);
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA120); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x00); // Sequencer.params.mode - none
  CAMERA_IO_Write16 (DeviceAddr, 0xC6, 0xA103); CAMERA_IO_Write16 (DeviceAddr, 0xC8, 0x01); // Sequencer goto preview A - 800x600
  }
//}}}
//{{{
static void config (uint16_t DeviceAddr, uint32_t value, uint32_t brightness_value) {

  uint64_t value_tmp;
  uint32_t br_value;

  /* Convert the input value into ov9655 parameters */
  value_tmp = convertValue (value);
  br_value = (uint32_t)convertValue (brightness_value);
  //CAMERA_IO_Write16 (DeviceAddr, OV9655_SENSOR_BRTN, br_value);
  //CAMERA_IO_Write16 (DeviceAddr, OV9655_SENSOR_CNST1, value_tmp);
  }
//}}}
//{{{
static uint16_t readID (uint16_t DeviceAddr) {

  CAMERA_IO_Write16 (DeviceAddr, 0xF0, 0);
  return (CAMERA_IO_Read16 (DeviceAddr, 0));
  }
//}}}

// external
//{{{
uint32_t BSP_CAMERA_Init (uint32_t Resolution) {

  // Configures the DCMI to interface with the camera module
  DCMI_HandleTypeDef* hDcmi    = &hDcmiHandler;
  hDcmi->Instance              = DCMI;
  hDcmi->Init.CaptureRate      = DCMI_CR_ALL_FRAME;
  hDcmi->Init.HSPolarity       = DCMI_HSPOLARITY_LOW; // = DCMI_HSPOLARITY_LOW;
  hDcmi->Init.VSPolarity       = DCMI_HSPOLARITY_LOW;
  hDcmi->Init.SynchroMode      = DCMI_SYNCHRO_HARDWARE;
  hDcmi->Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  hDcmi->Init.PCKPolarity      = DCMI_PCKPOLARITY_RISING;

  BSP_CAMERA_PowerUp();

  CAMERA_IO_Init();
  uint32_t readBack = readID (CAMERA_I2C_ADDRESS_MT9D111);
  if (readBack == 0x1519) {
    mspInit (&hDcmiHandler, NULL);
    HAL_DCMI_Init (hDcmi);

    if (Resolution == CAMERA_R480x272) {
      // 480x272 uses cropped 800x600
      init (CAMERA_I2C_ADDRESS_MT9D111, CAMERA_R800x600);
      HAL_DCMI_ConfigCROP (hDcmi, (800 - 480)/2, (600 - 272)/2, (480 * 2) - 1, 272 - 1);
      HAL_DCMI_EnableCROP (hDcmi);
      }
    else {
      init (CAMERA_I2C_ADDRESS_MT9D111, Resolution);
      HAL_DCMI_DisableCROP (hDcmi);
      }

    CameraCurrentResolution = Resolution;
    }

  return readBack;
  }
//}}}
//{{{
void BSP_CAMERA_DeInit() {

  hDcmiHandler.Instance = DCMI;
  HAL_DCMI_DeInit (&hDcmiHandler);
  mspDeInit (&hDcmiHandler, NULL);
  }
//}}}

//{{{
uint32_t BSP_CAMERA_getXSize() {

  switch (CameraCurrentResolution) {
    case CAMERA_R160x120: return 160;
    case CAMERA_R320x240: return 320;
    case CAMERA_R480x272: return 480;
    case CAMERA_R640x480: return 640;
    case CAMERA_R800x600: return 800;
    default: return 0;
    }
  }
//}}}
//{{{
uint32_t BSP_CAMERA_getYSize() {

  switch (CameraCurrentResolution) {
    case CAMERA_R160x120: return 120;
    case CAMERA_R320x240: return 240;
    case CAMERA_R480x272: return 272;
    case CAMERA_R640x480: return 280;
    case CAMERA_R800x600: return 600;
    default: return 0;
    }
  }
//}}}

//{{{
void BSP_CAMERA_SnapshotStart (uint8_t* buff) {
  HAL_DCMI_Start_DMA (&hDcmiHandler, DCMI_MODE_SNAPSHOT, (uint32_t)buff, getPix (CameraCurrentResolution));
  }
//}}}
//{{{
void BSP_CAMERA_ContinuousStart (uint8_t* buff) {
  HAL_DCMI_Start_DMA (&hDcmiHandler, DCMI_MODE_CONTINUOUS, (uint32_t)buff, getPix (CameraCurrentResolution));
  }
//}}}
//{{{
void BSP_CAMERA_ContrastBrightness (uint32_t contrast_level, uint32_t brightness_level) {
  }
//}}}

//{{{
void BSP_CAMERA_Stop() {

  HAL_DCMI_Stop (&hDcmiHandler);
  BSP_CAMERA_PowerDown();
  }
//}}}
//{{{
void BSP_CAMERA_PowerUp() {

  __HAL_RCC_GPIOH_CLK_ENABLE();

  // Configure DCMI GPIO as alternate function
  GPIO_InitTypeDef gpio_init_structure;
  gpio_init_structure.Pin = GPIO_PIN_13;
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init (GPIOH, &gpio_init_structure);

  // De-assert the camera POWER_DOWN pin (active high)
  HAL_GPIO_WritePin (GPIOH, GPIO_PIN_13, GPIO_PIN_RESET);

  HAL_Delay (3);     // POWER_DOWN de-asserted during 3ms
  }
//}}}
//{{{
void BSP_CAMERA_PowerDown() {

  __HAL_RCC_GPIOH_CLK_ENABLE();

  // Configure DCMI GPIO as alternate function
  GPIO_InitTypeDef gpio_init_structure;
  gpio_init_structure.Pin = GPIO_PIN_13;
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init (GPIOH, &gpio_init_structure);

  // Assert the camera POWER_DOWN pin (active high)
  HAL_GPIO_WritePin (GPIOH, GPIO_PIN_13, GPIO_PIN_SET);
  }
//}}}
void BSP_CAMERA_Suspend() { HAL_DCMI_Suspend (&hDcmiHandler); }
void BSP_CAMERA_Resume() { HAL_DCMI_Resume (&hDcmiHandler); }

void HAL_DCMI_LineEventCallback (DCMI_HandleTypeDef* hdcmi) {}
void HAL_DCMI_VsyncEventCallback (DCMI_HandleTypeDef* hdcmi) {}
void HAL_DCMI_FrameEventCallback (DCMI_HandleTypeDef* hdcmi) {}
void HAL_DCMI_ErrorCallback (DCMI_HandleTypeDef* hdcmi) {}
