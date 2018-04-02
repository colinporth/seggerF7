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

//{{{
// OV9655_GAIN       0x00
// OV9655_BLUE       0x01
// OV9655_RED        0x02
// OV9655_VREF       0x03
// OV9655_COM1       0x04
// OV9655_BAVE       0x05
// OV9655_GbAVE      0x06
// OV9655_GrAVE      0x07
// OV9655_RAVE       0x08
// OV9655_COM2       0x09
// OV9655_PID        0x0A
// OV9655_VER        0x0B
// OV9655_COM3       0x0C
// OV9655_COM4       0x0D
// OV9655_COM5       0x0E
// OV9655_COM6       0x0F
// OV9655_AEC        0x10
// OV9655_CLKRC      0x11
// OV9655_COM7       0x12
// OV9655_COM8       0x13
// OV9655_COM9       0x14
// OV9655_COM10      0x15
// OV9655_REG16      0x16
// OV9655_HSTART     0x17
// OV9655_HSTOP      0x18
// OV9655_VSTART     0x19
// OV9655_VSTOP      0x1A
// OV9655_PSHFT      0x1B
// OV9655_MIDH       0x1C
// OV9655_MIDL       0x1D
// OV9655_MVFP       0x1E
// OV9655_BOS        0x20
// OV9655_GBOS       0x21
// OV9655_GROS       0x22
// OV9655_ROS        0x23
// OV9655_AEW        0x24
// OV9655_AEB        0x25
// OV9655_VPT        0x26
// OV9655_BBIAS      0x27
// OV9655_GbBIAS     0x28
// OV9655_PREGAIN    0x29
// OV9655_EXHCH      0x2A
// OV9655_EXHCL      0x2B
// OV9655_RBIAS      0x2C
// OV9655_ADVFL      0x2D
// OV9655_ADVFH      0x2E
// OV9655_YAVE       0x2F
// OV9655_HSYST      0x30
// OV9655_HSYEN      0x31
// OV9655_HREF       0x32
// OV9655_CHLF       0x33
// OV9655_AREF1      0x34
// OV9655_AREF2      0x35
// OV9655_AREF3      0x36
// OV9655_ADC1       0x37
// OV9655_ADC2       0x38
// OV9655_AREF4      0x39
// OV9655_TSLB       0x3A
// OV9655_COM11      0x3B
// OV9655_COM12      0x3C
// OV9655_COM13      0x3D
// OV9655_COM14      0x3E
// OV9655_EDGE       0x3F
// OV9655_COM15      0x40
// OV9655_COM16      0x41
// OV9655_COM17      0x42
// OV9655_MTX1       0x4F
// OV9655_MTX2       0x50
// OV9655_MTX3       0x51
// OV9655_MTX4       0x52
// OV9655_MTX5       0x53
// OV9655_MTX6       0x54
// OV9655_BRTN       0x55
// OV9655_CNST1      0x56
// OV9655_CNST2      0x57
// OV9655_MTXS       0x58
// OV9655_AWBOP1     0x59
// OV9655_AWBOP2     0x5A
// OV9655_AWBOP3     0x5B
// OV9655_AWBOP4     0x5C
// OV9655_AWBOP5     0x5D
// OV9655_AWBOP6     0x5E
// OV9655_BLMT       0x5F
// OV9655_RLMT       0x60
// OV9655_GLMT       0x61
// OV9655_LCC1       0x62
// OV9655_LCC2       0x63
// OV9655_LCC3       0x64
// OV9655_LCC4       0x65
// OV9655_MANU       0x66
// OV9655_MANV       0x67
// OV9655_MANY       0x68
// OV9655_VARO       0x69
// OV9655_BD50MAX    0x6A
// OV9655_DBLV       0x6B
// OV9655_DNSTH      0x70
// OV9655_POIDX      0x72
// OV9655_PCKDV      0x73
// OV9655_XINDX      0x74
// OV9655_YINDX      0x75
// OV9655_SLOP       0x7A
// OV9655_GAM1       0x7B
// OV9655_GAM2       0x7C
// OV9655_GAM3       0x7D
// OV9655_GAM4       0x7E
// OV9655_GAM5       0x7F
// OV9655_GAM6       0x80
// OV9655_GAM7       0x81
// OV9655_GAM8       0x82
// OV9655_GAM9       0x83
// OV9655_GAM10      0x84
// OV9655_GAM11      0x85
// OV9655_GAM12      0x86
// OV9655_GAM13      0x87
// OV9655_GAM14      0x88
// OV9655_GAM15      0x89
// OV9655_COM18      0x8B
// OV9655_COM19      0x8C
// OV9655_COM20      0x8D
// OV9655_DMLNL      0x92
// OV9655_DMLNH      0x93
// OV9655_LCC6       0x9D
// OV9655_LCC7       0x9E
// OV9655_AECH       0xA1
// OV9655_BD50       0xA2
// OV9655_BD60       0xA3
// OV9655_COM21      0xA4
// OV9655_GREEN      0xA6
// OV9655_VZST       0xA7
// OV9655_REFA8      0xA8
// OV9655_REFA9      0xA9
// OV9655_BLC1       0xAC
// OV9655_BLC2       0xAD
// OV9655_BLC3       0xAE
// OV9655_BLC4       0xAF
// OV9655_BLC5       0xB0
// OV9655_BLC6       0xB1
// OV9655_BLC7       0xB2
// OV9655_BLC8       0xB3
// OV9655_CTRLB4     0xB4
// OV9655_FRSTL      0xB7
// OV9655_FRSTH      0xB8
// OV9655_ADBOFF     0xBC
// OV9655_ADROFF     0xBD
// OV9655_ADGbOFF    0xBE
// OV9655_ADGrOFF    0xBF
// OV9655_COM23      0xC4
// OV9655_BD60MAX    0xC5
// OV9655_COM24      0xC7
//}}}
//{{{
const uint8_t OV9655_640x480[][2] = {
  {0x00, 0x00}, // OV9655_GAIN       0x00
  {0x01, 0x80}, // OV9655_BLUE       0x01
  {0x02, 0x80}, // OV9655_RED        0x02
  {0xb5, 0x00},
  {0x35, 0x00}, // OV9655_AREF2      0x35
  {0xa8, 0xc1}, // OV9655_REFA8      0xA8
  {0x3a, 0xcc}, // OV9655_TSLB       0x3A
  {0x3d, 0x99}, // OV9655_COM13      0x3D
  {0x77, 0x02},
  {0x13, 0xe7}, // OV9655_COM8       0x13
  {0x26, 0x72}, // OV9655_VPT        0x26
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

  {0x7b, 0x1c}, // OV9655_GAM1      0x7B
  {0x7c, 0x28}, // OV9655_GAM2      0x7C
  {0x7d, 0x3c}, // OV9655_GAM3      0x7D
  {0x7e, 0x5a}, // OV9655_GAM4      0x7E
  {0x7f, 0x68}, // OV9655_GAM5      0x7F
  {0x80, 0x76}, // OV9655_GAM6      0x80
  {0x81, 0x80}, // OV9655_GAM7      0x81
  {0x82, 0x88}, // OV9655_GAM8      0x82
  {0x83, 0x8f}, // OV9655_GAM9      0x83
  {0x84, 0x96}, // OV9655_GAM10     0x84
  {0x85, 0xa3}, // OV9655_GAM11     0x85
  {0x86, 0xaf}, // OV9655_GAM12     0x86
  {0x87, 0xc4}, // OV9655_GAM13     0x87
  {0x88, 0xd7}, // OV9655_GAM14     0x88
  {0x89, 0xe8}, // OV9655_GAM15     0x89

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
const uint8_t OV9655_320x240[][2] = {
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
const uint8_t OV9655_160x120[][2] = {
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

DCMI_HandleTypeDef hDcmiHandler;

static DMA_HandleTypeDef hdma_handler;
static uint32_t CameraCurrentResolution;

void DCMI_IRQHandler() { HAL_DCMI_IRQHandler (&hDcmiHandler); }
void DMA2_Stream1_IRQHandler() { HAL_DMA_IRQHandler (hDcmiHandler.DMA_Handle); }

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

  uint32_t index;

  CAMERA_IO_Init();

  // Prepare the camera to be configured by resetting all its registers CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_COM7, 0x80);
  CAMERA_Delay (200);

  switch (resolution) {
    case CAMERA_R160x120: {
      for (index = 0; index<(sizeof(OV9655_160x120)/2); index++) {
        CAMERA_IO_Write (DeviceAddr, OV9655_160x120[index][0], OV9655_160x120[index][1]);
        CAMERA_Delay (2);
        }
      break;
      }

    case CAMERA_R320x240: {
      for (index = 0; index < (sizeof(OV9655_320x240)/2); index++) {
        CAMERA_IO_Write (DeviceAddr, OV9655_320x240[index][0], OV9655_320x240[index][1]);
        CAMERA_Delay (2);
        }
      break;
      }

    case CAMERA_R480x272: {
      /* Not supported resolution */
      break;
      }

    case CAMERA_R640x480: {
      for (index = 0; index < (sizeof(OV9655_640x480)/2); index++) {
        CAMERA_IO_Write (DeviceAddr, OV9655_640x480[index][0], OV9655_640x480[index][1]);
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
static void config (uint16_t DeviceAddr, uint32_t value, uint32_t brightness_value) {

  uint64_t value_tmp;
  uint32_t br_value;

  /* Convert the input value into ov9655 parameters */
  value_tmp = convertValue (value);
  br_value = (uint32_t)convertValue (brightness_value);
  CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_BRTN, br_value);
  CAMERA_IO_Write (DeviceAddr, OV9655_SENSOR_CNST1, value_tmp);
  }
//}}}
//{{{
static uint16_t readID (uint16_t DeviceAddr) {

  CAMERA_IO_Init();
  return (CAMERA_IO_Read (DeviceAddr, OV9655_SENSOR_PIDH));
  }
//}}}

//{{{
static uint32_t getSize (uint32_t resolution) {

  switch (resolution) {
    case CAMERA_R160x120: return  0x2580;
    case CAMERA_R320x240: return  0x9600;
    case CAMERA_R480x272: return  0xFF00;
    case CAMERA_R640x480: return 0x25800;
    default: return 0;
    }
  }
//}}}

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

// external
//{{{
uint32_t BSP_CAMERA_Init (uint32_t Resolution) {

  // Configures the DCMI to interface with the camera module ***/
  DCMI_HandleTypeDef* hDcmi    = &hDcmiHandler;
  hDcmi->Instance              = DCMI;
  hDcmi->Init.CaptureRate      = DCMI_CR_ALL_FRAME;
  hDcmi->Init.HSPolarity       = DCMI_HSPOLARITY_LOW;
  hDcmi->Init.SynchroMode      = DCMI_SYNCHRO_HARDWARE;
  hDcmi->Init.VSPolarity       = DCMI_VSPOLARITY_HIGH;
  hDcmi->Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  hDcmi->Init.PCKPolarity      = DCMI_PCKPOLARITY_RISING;

  BSP_CAMERA_PowerUp();

  uint32_t readBack = readID (CAMERA_I2C_ADDRESS);
  if (readBack == OV9655_ID) {
    mspInit (&hDcmiHandler, NULL);
    HAL_DCMI_Init (hDcmi);

    if (Resolution == CAMERA_R480x272) {
      // For 480x272, use cropped 640x480
      init (CAMERA_I2C_ADDRESS, CAMERA_R640x480);
      HAL_DCMI_ConfigCROP (hDcmi,           /* Crop in the middle of the VGA picture */
                           (CAMERA_VGA_RES_X - CAMERA_480x272_RES_X)/2,
                           (CAMERA_VGA_RES_Y - CAMERA_480x272_RES_Y)/2,
                           (CAMERA_480x272_RES_X * 2) - 1,
                           CAMERA_480x272_RES_Y - 1);
      HAL_DCMI_EnableCROP (hDcmi);
      }
    else {
      init (CAMERA_I2C_ADDRESS, Resolution);
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
void BSP_CAMERA_SnapshotStart (uint8_t* buff) {
  HAL_DCMI_Start_DMA (&hDcmiHandler, DCMI_MODE_SNAPSHOT, (uint32_t)buff, getSize (CameraCurrentResolution));
  }
//}}}
//{{{
void BSP_CAMERA_ContinuousStart (uint8_t* buff) {
  HAL_DCMI_Start_DMA (&hDcmiHandler, DCMI_MODE_CONTINUOUS, (uint32_t)buff, getSize (CameraCurrentResolution));
  }
//}}}
//{{{
void BSP_CAMERA_ContrastBrightness (uint32_t contrast_level, uint32_t brightness_level) {
  config (CAMERA_I2C_ADDRESS, contrast_level, brightness_level);
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

void HAL_DCMI_LineEventCallback (DCMI_HandleTypeDef* hdcmi) { BSP_CAMERA_LineEventCallback(); }
__weak void BSP_CAMERA_LineEventCallback() {}

void HAL_DCMI_VsyncEventCallback (DCMI_HandleTypeDef* hdcmi) { BSP_CAMERA_VsyncEventCallback(); }
__weak void BSP_CAMERA_VsyncEventCallback() {}

void HAL_DCMI_FrameEventCallback (DCMI_HandleTypeDef* hdcmi) { BSP_CAMERA_FrameEventCallback(); }
__weak void BSP_CAMERA_FrameEventCallback() {}

void HAL_DCMI_ErrorCallback (DCMI_HandleTypeDef* hdcmi) { BSP_CAMERA_ErrorCallback(); }
__weak void BSP_CAMERA_ErrorCallback() {}
