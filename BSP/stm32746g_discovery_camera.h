#pragma once
#include "stm32746g_discovery.h"
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}

//{{{  defines
#define CAMERA_R160x120                 0x00   /* QQVGA Resolution                     */
#define CAMERA_R320x240                 0x01   /* QVGA Resolution                      */
#define CAMERA_R480x272                 0x02   /* 480x272 Resolution                   */
#define CAMERA_R640x480                 0x03   /* VGA Resolution                       */

#define CAMERA_CONTRAST_BRIGHTNESS      0x00   /* Camera contrast brightness features  */
#define CAMERA_BLACK_WHITE              0x01   /* Camera black white feature           */
#define CAMERA_COLOR_EFFECT             0x03   /* Camera color effect feature          */

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

#define CAMERA_BLACK_WHITE_BW           0x00   /* Black and white effect      */
#define CAMERA_BLACK_WHITE_NEGATIVE     0x01   /* Negative effect             */
#define CAMERA_BLACK_WHITE_BW_NEGATIVE  0x02   /* BW and Negative effect      */
#define CAMERA_BLACK_WHITE_NORMAL       0x03   /* Normal effect               */

#define CAMERA_COLOR_EFFECT_NONE        0x00   /* No effects                  */
#define CAMERA_COLOR_EFFECT_BLUE        0x01   /* Blue effect                 */
#define CAMERA_COLOR_EFFECT_GREEN       0x02   /* Green effect                */
#define CAMERA_COLOR_EFFECT_RED         0x03   /* Red effect                  */
#define CAMERA_COLOR_EFFECT_ANTIQUE     0x04   /* Antique effect              */

#define RESOLUTION_R160x120      CAMERA_R160x120      /* QQVGA Resolution     */
#define RESOLUTION_R320x240      CAMERA_R320x240      /* QVGA Resolution      */
#define RESOLUTION_R480x272      CAMERA_R480x272      /* 480x272 Resolution   */
#define RESOLUTION_R640x480      CAMERA_R640x480      /* VGA Resolution       */
//}}}

uint32_t BSP_CAMERA_Init (uint32_t Resolution);
void BSP_CAMERA_DeInit();

void BSP_CAMERA_ContinuousStart (uint8_t* buff);
void BSP_CAMERA_SnapshotStart (uint8_t* buff);

void BSP_CAMERA_Suspend();
void BSP_CAMERA_Resume();
void BSP_CAMERA_Stop();

void BSP_CAMERA_PwrUp();
void BSP_CAMERA_PwrDown();

void BSP_CAMERA_ContrastBrightnessConfig (uint32_t contrast_level, uint32_t brightness_level);
void BSP_CAMERA_BlackWhiteConfig (uint32_t Mode);
void BSP_CAMERA_ColorEffectConfig (uint32_t Effect);

void BSP_CAMERA_LineEventCallback();
void BSP_CAMERA_VsyncEventCallback();
void BSP_CAMERA_FrameEventCallback();
void BSP_CAMERA_ErrorCallback();

void BSP_CAMERA_MspInit (DCMI_HandleTypeDef* hdcmi, void* Params);
void BSP_CAMERA_MspDeInit (DCMI_HandleTypeDef* hdcmi, void* Params);

//{{{
#ifdef __cplusplus
}
#endif
//}}}
