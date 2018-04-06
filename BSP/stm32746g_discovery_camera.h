#pragma once
#include "stm32746g_discovery.h"
//{{{  defines
#define CAMERA_R160x120                 0x00   /* QQVGA Resolution                     */
#define CAMERA_R320x240                 0x01   /* QVGA Resolution                      */
#define CAMERA_R480x272                 0x02   /* 480x272 Resolution                   */
#define CAMERA_R640x480                 0x03   /* VGA Resolution                       */
#define CAMERA_R800x600                 0x04   /* VGA Resolution                       */
#define CAMERA_R1600x1200               0x05   /* VGA Resolution                       */

#define CAMERA_BRIGHTNESS_LEVEL0        0x00   /* Brightness level -2         */
#define CAMERA_BRIGHTNESS_LEVEL1        0x01   /* Brightness level -1         */
#define CAMERA_BRIGHTNESS_LEVEL2        0x02   /* Brightness level 0          */
#define CAMERA_BRIGHTNESS_LEVEL3        0x03   /* Brightness level +1         */
#define CAMERA_BRIGHTNESS_LEVEL4        0x04   /* Brightness level +2         */

#define CAMERA_CONTRAST_LEVEL0          0x05   /* Contrast level -2           */
#define CAMERA_CONTRAST_LEVEL1          0x06   /* Contrast level -1           */
#define CAMERA_CONTRAST_LEVEL2          0x07   /* Contrast level  0           */
#define CAMERA_CONTRAST_LEVEL3          0x08   /* Contrast level +1           */
#define CAMERA_CONTRAST_LEVEL4          0x09   /* Contrast level +2           */
//}}}

//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}
uint32_t BSP_CAMERA_Init (uint32_t Resolution);

uint32_t BSP_CAMERA_getXSize();
uint32_t BSP_CAMERA_getYSize();

void BSP_CAMERA_ContinuousStart (uint8_t* buff);

void BSP_CAMERA_setFocus (int value);
void BSP_CAMERA_Preview();

void BSP_CAMERA_Stop();

void BSP_CAMERA_LineEventCallback();
void BSP_CAMERA_VsyncEventCallback();
void BSP_CAMERA_FrameEventCallback();
void BSP_CAMERA_ErrorCallback();

//{{{
#ifdef __cplusplus
}
#endif
//}}}
