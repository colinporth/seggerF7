//{{{  includes
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_camera.h"
//}}}
//{{{  OV9655 Registers definition
#define  OV9655_ID    0x96

#define OV9655_SENSOR_PIDH              0x0A
#define OV9655_SENSOR_PIDL              0x0B
#define OV9655_SENSOR_COM7              0x12
#define OV9655_SENSOR_TSLB              0x3A
#define OV9655_SENSOR_MTX1              0x4F
#define OV9655_SENSOR_MTX2              0x50
#define OV9655_SENSOR_MTX3              0x51
#define OV9655_SENSOR_MTX4              0x52
#define OV9655_SENSOR_MTX5              0x53
#define OV9655_SENSOR_MTX6              0x54
#define OV9655_SENSOR_BRTN              0x55
#define OV9655_SENSOR_CNST1             0x56
#define OV9655_SENSOR_CNST2             0x57

#define OV9655_BRIGHTNESS_LEVEL0        0xB0     /* Brightness level -2         */
#define OV9655_BRIGHTNESS_LEVEL1        0x98     /* Brightness level -1         */
#define OV9655_BRIGHTNESS_LEVEL2        0x00     /* Brightness level 0          */
#define OV9655_BRIGHTNESS_LEVEL3        0x18     /* Brightness level +1         */
#define OV9655_BRIGHTNESS_LEVEL4        0x30     /* Brightness level +2         */

#define OV9655_BLACK_WHITE_BW           0xCC000000000000  /* Black and white effect      */
#define OV9655_BLACK_WHITE_NEGATIVE     0xEC808000008080  /* Negative effect             */
#define OV9655_BLACK_WHITE_BW_NEGATIVE  0xEC000000000000  /* BW and Negative effect      */
#define OV9655_BLACK_WHITE_NORMAL       0xCC808000008080  /* Normal effect               */

#define OV9655_CONTRAST_LEVEL0          0x30     /* Contrast level -2           */
#define OV9655_CONTRAST_LEVEL1          0x38     /* Contrast level -1           */
#define OV9655_CONTRAST_LEVEL2          0x40     /* Contrast level 0            */
#define OV9655_CONTRAST_LEVEL3          0x50     /* Contrast level +1           */
#define OV9655_CONTRAST_LEVEL4          0x60     /* Contrast level +2           */

#define OV9655_COLOR_EFFECT_NONE        0xCC808000008080  /* No color effect             */
#define OV9655_COLOR_EFFECT_ANTIQUE     0xCC000020F00000  /* Antique effect              */
#define OV9655_COLOR_EFFECT_BLUE        0xCC000000000060  /* Blue effect                 */
#define OV9655_COLOR_EFFECT_GREEN       0xCC000000008000  /* Green effect                */
#define OV9655_COLOR_EFFECT_RED         0xCC600000000000  /* Red effect                  */

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
static uint32_t CameraHwAddress;
static uint32_t CameraCurrentResolution;

void DCMI_IRQHandler() { HAL_DCMI_IRQHandler (&hDcmiHandler); }
void DMA2_Stream1_IRQHandler() { HAL_DMA_IRQHandler (hDcmiHandler.DMA_Handle); }

//{{{
/* Initialization sequence for VGA resolution (640x480)*/
const unsigned char OV9655_VGA[][2]=
{
  {0x00, 0x00},
  {0x01, 0x80},
  {0x02, 0x80},
  {0xb5, 0x00},
  {0x35, 0x00},
  {0xa8, 0xc1},
  {0x3a, 0xcc},
  {0x3d, 0x99},
  {0x77, 0x02},
  {0x13, 0xe7},
  {0x26, 0x72},
  {0x27, 0x08},
  {0x28, 0x08},
  {0x2c, 0x08},
  {0xab, 0x04},
  {0x6e, 0x00},
  {0x6d, 0x55},
  {0x00, 0x11},
  {0x10, 0x7b},
  {0xbb, 0xae},
  {0x11, 0x03},
  {0x72, 0x00},
  {0x3e, 0x0c},
  {0x74, 0x3a},
  {0x76, 0x01},
  {0x75, 0x35},
  {0x73, 0x00},
  {0xc7, 0x80},
  {0x62, 0x00},
  {0x63, 0x00},
  {0x64, 0x02},
  {0x65, 0x20},
  {0x66, 0x01},
  {0xc3, 0x4e},
  {0x33, 0x00},
  {0xa4, 0x50},
  {0xaa, 0x92},
  {0xc2, 0x01},
  {0xc1, 0xc8},
  {0x1e, 0x04},
  {0xa9, 0xef},
  {0x0e, 0x61},
  {0x39, 0x57},
  {0x0f, 0x48},
  {0x24, 0x3c},
  {0x25, 0x36},
  {0x12, 0x63},
  {0x03, 0x12},
  {0x32, 0xff},
  {0x17, 0x16},
  {0x18, 0x02},
  {0x19, 0x01},
  {0x1a, 0x3d},
  {0x36, 0xfa},
  {0x69, 0x0a},
  {0x8c, 0x8d},
  {0xc0, 0xaa},
  {0x40, 0xd0},
  {0x43, 0x14},
  {0x44, 0xf0},
  {0x45, 0x46},
  {0x46, 0x62},
  {0x47, 0x2a},
  {0x48, 0x3c},
  {0x59, 0x85},
  {0x5a, 0xa9},
  {0x5b, 0x64},
  {0x5c, 0x84},
  {0x5d, 0x53},
  {0x5e, 0x0e},
  {0x6c, 0x0c},
  {0xc6, 0x85},
  {0xcb, 0xf0},
  {0xcc, 0xd8},
  {0x71, 0x78},
  {0xa5, 0x68},
  {0x6f, 0x9e},
  {0x42, 0xc0},
  {0x3f, 0x82},
  {0x8a, 0x23},
  {0x14, 0x3a},
  {0x3b, 0xcc},
  {0x34, 0x3d},
  {0x41, 0x40},
  {0xc9, 0xe0},
  {0xca, 0xe8},
  {0xcd, 0x93},
  {0x7a, 0x20},
  {0x7b, 0x1c},
  {0x7c, 0x28},
  {0x7d, 0x3c},
  {0x7e, 0x5a},
  {0x7f, 0x68},
  {0x80, 0x76},
  {0x81, 0x80},
  {0x82, 0x88},
  {0x83, 0x8f},
  {0x84, 0x96},
  {0x85, 0xa3},
  {0x86, 0xaf},
  {0x87, 0xc4},
  {0x88, 0xd7},
  {0x89, 0xe8},
  {0x4f, 0x98},
  {0x50, 0x98},
  {0x51, 0x00},
  {0x52, 0x28},
  {0x53, 0x70},
  {0x54, 0x98},
  {0x58, 0x1a},
  {0x6b, 0x5a},
  {0x90, 0x92},
  {0x91, 0x92},
  {0x9f, 0x90},
  {0xa0, 0x90},
  {0x16, 0x24},
  {0x2a, 0x00},
  {0x2b, 0x00},
  {0xac, 0x80},
  {0xad, 0x80},
  {0xae, 0x80},
  {0xaf, 0x80},
  {0xb2, 0xf2},
  {0xb3, 0x20},
  {0xb4, 0x20},
  {0xb6, 0xaf},
  {0x29, 0x15},
  {0x9d, 0x02},
  {0x9e, 0x02},
  {0x9e, 0x02},
  {0x04, 0x03},
  {0x05, 0x2e},
  {0x06, 0x2e},
  {0x07, 0x2e},
  {0x08, 0x2e},
  {0x2f, 0x2e},
  {0x4a, 0xe9},
  {0x4b, 0xdd},
  {0x4c, 0xdd},
  {0x4d, 0xdd},
  {0x4e, 0xdd},
  {0x70, 0x06},
  {0xa6, 0x40},
  {0xbc, 0x02},
  {0xbd, 0x01},
  {0xbe, 0x02},
  {0xbf, 0x01},
  };
//}}}
//{{{
/* Initialization sequence for QVGA resolution (320x240) */
const unsigned char OV9655_QVGA[][2]= {
  {0x00, 0x00},
  {0x01, 0x80},
  {0x02, 0x80},
  {0x03, 0x02},
  {0x04, 0x03},
  {0x09, 0x01},
  {0x0b, 0x57},
  {0x0e, 0x61},
  {0x0f, 0x40},
  {0x11, 0x01},
  {0x12, 0x62},
  {0x13, 0xc7},
  {0x14, 0x3a},
  {0x16, 0x24},
  {0x17, 0x18},
  {0x18, 0x04},
  {0x19, 0x01},
  {0x1a, 0x81},
  {0x1e, 0x00},
  {0x24, 0x3c},
  {0x25, 0x36},
  {0x26, 0x72},
  {0x27, 0x08},
  {0x28, 0x08},
  {0x29, 0x15},
  {0x2a, 0x00},
  {0x2b, 0x00},
  {0x2c, 0x08},
  {0x32, 0x12},
  {0x33, 0x00},
  {0x34, 0x3f},
  {0x35, 0x00},
  {0x36, 0x3a},
  {0x38, 0x72},
  {0x39, 0x57},
  {0x3a, 0xcc},
  {0x3b, 0x04},
  {0x3d, 0x99},
  {0x3e, 0x02},
  {0x3f, 0xc1},
  {0x40, 0xc0},
  {0x41, 0x41},
  {0x42, 0xc0},
  {0x43, 0x0a},
  {0x44, 0xf0},
  {0x45, 0x46},
  {0x46, 0x62},
  {0x47, 0x2a},
  {0x48, 0x3c},
  {0x4a, 0xfc},
  {0x4b, 0xfc},
  {0x4c, 0x7f},
  {0x4d, 0x7f},
  {0x4e, 0x7f},
  {0x4f, 0x98},
  {0x50, 0x98},
  {0x51, 0x00},
  {0x52, 0x28},
  {0x53, 0x70},
  {0x54, 0x98},
  {0x58, 0x1a},
  {0x59, 0x85},
  {0x5a, 0xa9},
  {0x5b, 0x64},
  {0x5c, 0x84},
  {0x5d, 0x53},
  {0x5e, 0x0e},
  {0x5f, 0xf0},
  {0x60, 0xf0},
  {0x61, 0xf0},
  {0x62, 0x00},
  {0x63, 0x00},
  {0x64, 0x02},
  {0x65, 0x20},
  {0x66, 0x00},
  {0x69, 0x0a},
  {0x6b, 0x5a},
  {0x6c, 0x04},
  {0x6d, 0x55},
  {0x6e, 0x00},
  {0x6f, 0x9d},
  {0x70, 0x21},
  {0x71, 0x78},
  {0x72, 0x11},
  {0x73, 0x01},
  {0x74, 0x10},
  {0x75, 0x10},
  {0x76, 0x01},
  {0x77, 0x02},
  {0x7A, 0x12},
  {0x7B, 0x08},
  {0x7C, 0x16},
  {0x7D, 0x30},
  {0x7E, 0x5e},
  {0x7F, 0x72},
  {0x80, 0x82},
  {0x81, 0x8e},
  {0x82, 0x9a},
  {0x83, 0xa4},
  {0x84, 0xac},
  {0x85, 0xb8},
  {0x86, 0xc3},
  {0x87, 0xd6},
  {0x88, 0xe6},
  {0x89, 0xf2},
  {0x8a, 0x24},
  {0x8c, 0x80},
  {0x90, 0x7d},
  {0x91, 0x7b},
  {0x9d, 0x02},
  {0x9e, 0x02},
  {0x9f, 0x7a},
  {0xa0, 0x79},
  {0xa1, 0x40},
  {0xa4, 0x50},
  {0xa5, 0x68},
  {0xa6, 0x4a},
  {0xa8, 0xc1},
  {0xa9, 0xef},
  {0xaa, 0x92},
  {0xab, 0x04},
  {0xac, 0x80},
  {0xad, 0x80},
  {0xae, 0x80},
  {0xaf, 0x80},
  {0xb2, 0xf2},
  {0xb3, 0x20},
  {0xb4, 0x20},
  {0xb5, 0x00},
  {0xb6, 0xaf},
  {0xb6, 0xaf},
  {0xbb, 0xae},
  {0xbc, 0x7f},
  {0xbd, 0x7f},
  {0xbe, 0x7f},
  {0xbf, 0x7f},
  {0xbf, 0x7f},
  {0xc0, 0xaa},
  {0xc1, 0xc0},
  {0xc2, 0x01},
  {0xc3, 0x4e},
  {0xc6, 0x05},
  {0xc7, 0x81},
  {0xc9, 0xe0},
  {0xca, 0xe8},
  {0xcb, 0xf0},
  {0xcc, 0xd8},
  {0xcd, 0x93},
  {0x12, 0x63},
  {0x40, 0x10},
  };
//}}}
//{{{
/* Initialization sequence for QQVGA resolution (160x120) */
const char OV9655_QQVGA[][2]= {
  {0x00, 0x00},
  {0x01, 0x80},
  {0x02, 0x80},
  {0x03, 0x02},
  {0x04, 0x03},
  {0x09, 0x01},
  {0x0b, 0x57},
  {0x0e, 0x61},
  {0x0f, 0x40},
  {0x11, 0x01},
  {0x12, 0x62},
  {0x13, 0xc7},
  {0x14, 0x3a},
  {0x16, 0x24},
  {0x17, 0x18},
  {0x18, 0x04},
  {0x19, 0x01},
  {0x1a, 0x81},
  {0x1e, 0x00},
  {0x24, 0x3c},
  {0x25, 0x36},
  {0x26, 0x72},
  {0x27, 0x08},
  {0x28, 0x08},
  {0x29, 0x15},
  {0x2a, 0x00},
  {0x2b, 0x00},
  {0x2c, 0x08},
  {0x32, 0xa4},
  {0x33, 0x00},
  {0x34, 0x3f},
  {0x35, 0x00},
  {0x36, 0x3a},
  {0x38, 0x72},
  {0x39, 0x57},
  {0x3a, 0xcc},
  {0x3b, 0x04},
  {0x3d, 0x99},
  {0x3e, 0x0e},
  {0x3f, 0xc1},
  {0x40, 0xc0},
  {0x41, 0x41},
  {0x42, 0xc0},
  {0x43, 0x0a},
  {0x44, 0xf0},
  {0x45, 0x46},
  {0x46, 0x62},
  {0x47, 0x2a},
  {0x48, 0x3c},
  {0x4a, 0xfc},
  {0x4b, 0xfc},
  {0x4c, 0x7f},
  {0x4d, 0x7f},
  {0x4e, 0x7f},
  {0x4f, 0x98},
  {0x50, 0x98},
  {0x51, 0x00},
  {0x52, 0x28},
  {0x53, 0x70},
  {0x54, 0x98},
  {0x58, 0x1a},
  {0x59, 0x85},
  {0x5a, 0xa9},
  {0x5b, 0x64},
  {0x5c, 0x84},
  {0x5d, 0x53},
  {0x5e, 0x0e},
  {0x5f, 0xf0},
  {0x60, 0xf0},
  {0x61, 0xf0},
  {0x62, 0x00},
  {0x63, 0x00},
  {0x64, 0x02},
  {0x65, 0x20},
  {0x66, 0x00},
  {0x69, 0x0a},
  {0x6b, 0x5a},
  {0x6c, 0x04},
  {0x6d, 0x55},
  {0x6e, 0x00},
  {0x6f, 0x9d},
  {0x70, 0x21},
  {0x71, 0x78},
  {0x72, 0x22},
  {0x73, 0x02},
  {0x74, 0x10},
  {0x75, 0x10},
  {0x76, 0x01},
  {0x77, 0x02},
  {0x7A, 0x12},
  {0x7B, 0x08},
  {0x7C, 0x16},
  {0x7D, 0x30},
  {0x7E, 0x5e},
  {0x7F, 0x72},
  {0x80, 0x82},
  {0x81, 0x8e},
  {0x82, 0x9a},
  {0x83, 0xa4},
  {0x84, 0xac},
  {0x85, 0xb8},
  {0x86, 0xc3},
  {0x87, 0xd6},
  {0x88, 0xe6},
  {0x89, 0xf2},
  {0x8a, 0x24},
  {0x8c, 0x80},
  {0x90, 0x7d},
  {0x91, 0x7b},
  {0x9d, 0x02},
  {0x9e, 0x02},
  {0x9f, 0x7a},
  {0xa0, 0x79},
  {0xa1, 0x40},
  {0xa4, 0x50},
  {0xa5, 0x68},
  {0xa6, 0x4a},
  {0xa8, 0xc1},
  {0xa9, 0xef},
  {0xaa, 0x92},
  {0xab, 0x04},
  {0xac, 0x80},
  {0xad, 0x80},
  {0xae, 0x80},
  {0xaf, 0x80},
  {0xb2, 0xf2},
  {0xb3, 0x20},
  {0xb4, 0x20},
  {0xb5, 0x00},
  {0xb6, 0xaf},
  {0xb6, 0xaf},
  {0xbb, 0xae},
  {0xbc, 0x7f},
  {0xbd, 0x7f},
  {0xbe, 0x7f},
  {0xbf, 0x7f},
  {0xbf, 0x7f},
  {0xc0, 0xaa},
  {0xc1, 0xc0},
  {0xc2, 0x01},
  {0xc3, 0x4e},
  {0xc6, 0x05},
  {0xc7, 0x82},
  {0xc9, 0xe0},
  {0xca, 0xe8},
  {0xcb, 0xf0},
  {0xcc, 0xd8},
  {0xcd, 0x93},
  {0x12, 0x63},
  {0x40, 0x10},
  };
//}}}

//{{{
static uint64_t ov9655_ConvertValue (uint32_t feature, uint32_t value) {

  uint64_t ret = 0;

  switch (feature) {
    case CAMERA_BLACK_WHITE: {
      switch (value) {
        case CAMERA_BLACK_WHITE_BW: ret =  OV9655_BLACK_WHITE_BW; break;
        case CAMERA_BLACK_WHITE_NEGATIVE: ret =  OV9655_BLACK_WHITE_NEGATIVE; break;
        case CAMERA_BLACK_WHITE_BW_NEGATIVE: ret =  OV9655_BLACK_WHITE_BW_NEGATIVE; break;
        case CAMERA_BLACK_WHITE_NORMAL: ret =  OV9655_BLACK_WHITE_NORMAL; break;
        default: ret =  OV9655_BLACK_WHITE_NORMAL; break;
        }
      break;
      }

    case CAMERA_CONTRAST_BRIGHTNESS: {
      switch (value) {
        case CAMERA_BRIGHTNESS_LEVEL0: ret = OV9655_BRIGHTNESS_LEVEL0; break;
        case CAMERA_BRIGHTNESS_LEVEL1: ret =  OV9655_BRIGHTNESS_LEVEL1; break;
        case CAMERA_BRIGHTNESS_LEVEL2: ret =  OV9655_BRIGHTNESS_LEVEL2; break;
        case CAMERA_BRIGHTNESS_LEVEL3: ret =  OV9655_BRIGHTNESS_LEVEL3; break;
        case CAMERA_BRIGHTNESS_LEVEL4: ret =  OV9655_BRIGHTNESS_LEVEL4; break;
        case CAMERA_CONTRAST_LEVEL0: ret =  OV9655_CONTRAST_LEVEL0; break;
        case CAMERA_CONTRAST_LEVEL1: ret =  OV9655_CONTRAST_LEVEL1; break;
        case CAMERA_CONTRAST_LEVEL2: ret =  OV9655_CONTRAST_LEVEL2; break;
        case CAMERA_CONTRAST_LEVEL3: ret =  OV9655_CONTRAST_LEVEL3; break;
        case CAMERA_CONTRAST_LEVEL4: ret =  OV9655_CONTRAST_LEVEL4; break;
        default: ret =  OV9655_CONTRAST_LEVEL0; break;
        }
      break;
      }

    case CAMERA_COLOR_EFFECT: {
      switch (value) {
        case CAMERA_COLOR_EFFECT_ANTIQUE: ret =  OV9655_COLOR_EFFECT_ANTIQUE; break;
        case CAMERA_COLOR_EFFECT_BLUE: ret =  OV9655_COLOR_EFFECT_BLUE; break;
        case CAMERA_COLOR_EFFECT_GREEN: ret =  OV9655_COLOR_EFFECT_GREEN; break;
        case CAMERA_COLOR_EFFECT_RED: ret = OV9655_COLOR_EFFECT_RED; break;
        case CAMERA_COLOR_EFFECT_NONE:
        default: ret =  OV9655_COLOR_EFFECT_NONE; break;
        }
      break;
      }

    default: {
      ret = 0;
      break;
      }
    }

  return ret;
  }
//}}}

//{{{
void ov9655_Init (uint16_t DeviceAddr, uint32_t resolution) {

  uint32_t index;

  /* Initialize I2C */
  CAMERA_IO_Init();

  /* Prepare the camera to be configured by resetting all its registers */
  CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_COM7, 0x80);
  CAMERA_Delay (200);

  /* Initialize OV9655 */
  switch (resolution) {
    case CAMERA_R160x120: {
      for (index=0; index<(sizeof(OV9655_QQVGA)/2); index++) {
        CAMERA_IO_Write (DeviceAddr, OV9655_QQVGA[index][0], OV9655_QQVGA[index][1]);
        CAMERA_Delay (2);
        }
      break;
      }

    case CAMERA_R320x240: {
      for (index=0; index < (sizeof(OV9655_QVGA)/2); index++) {
        CAMERA_IO_Write (DeviceAddr, OV9655_QVGA[index][0], OV9655_QVGA[index][1]);
        CAMERA_Delay (2);
        }
      break;
      }

    case CAMERA_R480x272: {
      /* Not supported resolution */
      break;
      }

    case CAMERA_R640x480: {
      for (index=0; index < (sizeof(OV9655_VGA)/2); index++) {
        CAMERA_IO_Write (DeviceAddr, OV9655_VGA[index][0], OV9655_VGA[index][1]);
        CAMERA_Delay (2);
        }
      break;
      }

    default: {
      break;
      }
    }
  }
//}}}
//{{{
void ov9655_Config (uint16_t DeviceAddr, uint32_t feature, uint32_t value, uint32_t brightness_value) {

  uint8_t tslb, mtx1, mtx2, mtx3, mtx4, mtx5, mtx6;
  uint64_t value_tmp;
  uint32_t br_value;

  /* Convert the input value into ov9655 parameters */
  value_tmp = ov9655_ConvertValue (feature, value);
  br_value = (uint32_t)ov9655_ConvertValue (CAMERA_CONTRAST_BRIGHTNESS, brightness_value);

  switch (feature) {
    case CAMERA_CONTRAST_BRIGHTNESS: {
      CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_BRTN, br_value);
      CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_CNST1, value_tmp);
      break;
      }

    case CAMERA_BLACK_WHITE:
    case CAMERA_COLOR_EFFECT: {
      tslb = (uint8_t)(value_tmp >> 48);
      mtx1 = (uint8_t)(value_tmp >> 40);
      mtx2 = (uint8_t)(value_tmp >> 32);
      mtx3 = (uint8_t)(value_tmp >> 24);
      mtx4 = (uint8_t)(value_tmp >> 16);
      mtx5 = (uint8_t)(value_tmp >> 8);
      mtx6 = (uint8_t)(value_tmp);
      CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_TSLB, tslb);
      CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_MTX1, mtx1);
      CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_MTX2, mtx2);
      CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_MTX3, mtx3);
      CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_MTX4, mtx4);
      CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_MTX5, mtx5);
      CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_MTX6, mtx6);
      break;
      }

    default:
      break;
    }
  }
//}}}
//{{{
uint16_t ov9655_ReadID (uint16_t DeviceAddr) {

  /* Initialize I2C */
  CAMERA_IO_Init();

  /* Get the camera ID */
  return (CAMERA_IO_Read (DeviceAddr, OV9655_SENSOR_PIDH));
  }
//}}}

//{{{
static uint32_t GetSize (uint32_t resolution) {

  uint32_t size = 0;

  switch (resolution) {
    case CAMERA_R160x120: size = 0x2580; break;
    case CAMERA_R320x240: size = 0x9600; break;
    case CAMERA_R480x272: size = 0xFF00; break;
    case CAMERA_R640x480: size = 0x25800; break;
    default: break;
    }

  return size;
  }
//}}}

//{{{
uint32_t BSP_CAMERA_Init (uint32_t Resolution) {

  // Configures the DCMI to interface with the camera module ***/
  DCMI_HandleTypeDef* phdcmi    = &hDcmiHandler;
  phdcmi->Init.CaptureRate      = DCMI_CR_ALL_FRAME;
  phdcmi->Init.HSPolarity       = DCMI_HSPOLARITY_LOW;
  phdcmi->Init.SynchroMode      = DCMI_SYNCHRO_HARDWARE;
  phdcmi->Init.VSPolarity       = DCMI_VSPOLARITY_HIGH;
  phdcmi->Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  phdcmi->Init.PCKPolarity      = DCMI_PCKPOLARITY_RISING;
  phdcmi->Instance              = DCMI;

  /* Power up camera */
  BSP_CAMERA_PwrUp();

  /* Read ID of Camera module via I2C */
  uint32_t readBack = ov9655_ReadID (CAMERA_I2C_ADDRESS);
  if (readBack == OV9655_ID) {
    /* Initialize the camera driver structure */
    CameraHwAddress = CAMERA_I2C_ADDRESS;

    /* DCMI Initialization */
    BSP_CAMERA_MspInit (&hDcmiHandler, NULL);
    HAL_DCMI_Init (phdcmi);

    /* Camera Module Initialization via I2C to the wanted 'Resolution' */
    if (Resolution == CAMERA_R480x272) {
      // For 480x272 resolution, the OV9655 sensor is set to VGA resolution
      // as OV9655 doesn't supports 480x272 resolution DCMI is configured to output a 480x272 cropped window
      ov9655_Init (CameraHwAddress, CAMERA_R640x480);
      HAL_DCMI_ConfigCROP (phdcmi,           /* Crop in the middle of the VGA picture */
                           (CAMERA_VGA_RES_X - CAMERA_480x272_RES_X)/2,
                           (CAMERA_VGA_RES_Y - CAMERA_480x272_RES_Y)/2,
                           (CAMERA_480x272_RES_X * 2) - 1,
                           CAMERA_480x272_RES_Y - 1);
      HAL_DCMI_EnableCROP (phdcmi);
      }
    else {
      ov9655_Init (CameraHwAddress, Resolution);
      HAL_DCMI_DisableCROP (phdcmi);
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
  BSP_CAMERA_MspDeInit (&hDcmiHandler, NULL);
  }
//}}}

//{{{
void BSP_CAMERA_ContinuousStart (uint8_t* buff) {
  HAL_DCMI_Start_DMA (&hDcmiHandler, DCMI_MODE_CONTINUOUS, (uint32_t)buff, GetSize (CameraCurrentResolution));
  }
//}}}
//{{{
void BSP_CAMERA_SnapshotStart (uint8_t* buff) {
  HAL_DCMI_Start_DMA (&hDcmiHandler, DCMI_MODE_SNAPSHOT, (uint32_t)buff, GetSize(CameraCurrentResolution));
  }
//}}}

void BSP_CAMERA_Suspend() { HAL_DCMI_Suspend (&hDcmiHandler); }
void BSP_CAMERA_Resume() { HAL_DCMI_Resume(&hDcmiHandler); }

//{{{
void BSP_CAMERA_Stop() {

  HAL_DCMI_Stop (&hDcmiHandler);
  BSP_CAMERA_PwrDown();
  }
//}}}
//{{{
void BSP_CAMERA_PwrUp() {


  /* Enable GPIO clock */
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /* Configure DCMI GPIO as alternate function */
  GPIO_InitTypeDef gpio_init_structure;
  gpio_init_structure.Pin = GPIO_PIN_13;
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init (GPIOH, &gpio_init_structure);

  /* De-assert the camera POWER_DOWN pin (active high) */
  HAL_GPIO_WritePin (GPIOH, GPIO_PIN_13, GPIO_PIN_RESET);

  HAL_Delay (3);     /* POWER_DOWN de-asserted during 3ms */
  }
//}}}
//{{{
void BSP_CAMERA_PwrDown() {


  /* Enable GPIO clock */
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /* Configure DCMI GPIO as alternate function */
  GPIO_InitTypeDef gpio_init_structure;
  gpio_init_structure.Pin       = GPIO_PIN_13;
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
  HAL_GPIO_Init (GPIOH, &gpio_init_structure);

  /* Assert the camera POWER_DOWN pin (active high) */
  HAL_GPIO_WritePin (GPIOH, GPIO_PIN_13, GPIO_PIN_SET);
  }
//}}}

//{{{
/**
  * @brief  Configures the camera contrast and brightness.
  * @param  contrast_level: Contrast level
  *          This parameter can be one of the following values:
  *            @arg  CAMERA_CONTRAST_LEVEL4: for contrast +2
  *            @arg  CAMERA_CONTRAST_LEVEL3: for contrast +1
  *            @arg  CAMERA_CONTRAST_LEVEL2: for contrast  0
  *            @arg  CAMERA_CONTRAST_LEVEL1: for contrast -1
  *            @arg  CAMERA_CONTRAST_LEVEL0: for contrast -2
  * @param  brightness_level: Contrast level
  *          This parameter can be one of the following values:
  *            @arg  CAMERA_BRIGHTNESS_LEVEL4: for brightness +2
  *            @arg  CAMERA_BRIGHTNESS_LEVEL3: for brightness +1
  *            @arg  CAMERA_BRIGHTNESS_LEVEL2: for brightness  0
  *            @arg  CAMERA_BRIGHTNESS_LEVEL1: for brightness -1
  *            @arg  CAMERA_BRIGHTNESS_LEVEL0: for brightness -2
  * @retval None
  */
void BSP_CAMERA_ContrastBrightnessConfig (uint32_t contrast_level, uint32_t brightness_level) {
  ov9655_Config (CameraHwAddress, CAMERA_CONTRAST_BRIGHTNESS, contrast_level, brightness_level);
  }
//}}}
//{{{
void BSP_CAMERA_BlackWhiteConfig (uint32_t Mode) {
  ov9655_Config (CameraHwAddress, CAMERA_BLACK_WHITE, Mode, 0);
  }
//}}}
//{{{
void BSP_CAMERA_ColorEffectConfig( uint32_t Effect) {
  ov9655_Config (CameraHwAddress, CAMERA_COLOR_EFFECT, Effect, 0);
  }
//}}}

//{{{
__weak void BSP_CAMERA_MspInit (DCMI_HandleTypeDef* hdcmi, void* Params) {

  __HAL_RCC_DCMI_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /* Configure DCMI GPIO as alternate function */
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

  /* Associate the initialized DMA handle to the DCMI handle */
  __HAL_LINKDMA (hdcmi, DMA_Handle, hdma_handler);

  /*** Configure the NVIC for DCMI and DMA ***/
  /* NVIC configuration for DCMI transfer complete interrupt */
  HAL_NVIC_SetPriority (DCMI_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (DCMI_IRQn);

  /* NVIC configuration for DMA2D transfer complete interrupt */
  HAL_NVIC_SetPriority (DMA2_Stream1_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (DMA2_Stream1_IRQn);

  /* Configure the DMA stream */
  HAL_DMA_Init (hdcmi->DMA_Handle);
  }
//}}}
//{{{
__weak void BSP_CAMERA_MspDeInit (DCMI_HandleTypeDef* hdcmi, void* Params) {

  HAL_NVIC_DisableIRQ (DCMI_IRQn);
  HAL_NVIC_DisableIRQ (DMA2_Stream1_IRQn);

  HAL_DMA_DeInit (hdcmi->DMA_Handle);
  __HAL_RCC_DCMI_CLK_DISABLE();
  }
//}}}

void HAL_DCMI_LineEventCallback (DCMI_HandleTypeDef* hdcmi) { BSP_CAMERA_LineEventCallback(); }
__weak void BSP_CAMERA_LineEventCallback() {}

void HAL_DCMI_VsyncEventCallback (DCMI_HandleTypeDef* hdcmi) { BSP_CAMERA_VsyncEventCallback(); }
__weak void BSP_CAMERA_VsyncEventCallback() {}

void HAL_DCMI_FrameEventCallback (DCMI_HandleTypeDef* hdcmi) { BSP_CAMERA_FrameEventCallback(); }
__weak void BSP_CAMERA_FrameEventCallback() {}

void HAL_DCMI_ErrorCallback (DCMI_HandleTypeDef* hdcmi) { BSP_CAMERA_ErrorCallback(); }
__weak void BSP_CAMERA_ErrorCallback() {}
