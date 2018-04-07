// camera.h
#pragma once
#include "stm32746g_discovery.h"
#include "../common/cLcd.h"

#define CAMERA_R160x120                 0x00   /* QQVGA Resolution                     */
#define CAMERA_R320x240                 0x01   /* QVGA Resolution                      */
#define CAMERA_R480x272                 0x02   /* 480x272 Resolution                   */
#define CAMERA_R640x480                 0x03   /* VGA Resolution                       */
#define CAMERA_R800x600                 0x04   /* VGA Resolution                       */
#define CAMERA_R1600x1200               0x05   /* VGA Resolution                       */

void cameraInit (cLcd* lcd, uint32_t resolution);

uint32_t cameraGetXSize();
uint32_t cameraGetYSize();

void cameraStart (uint32_t buffer, bool continuous);
void cameraPreview();
void cameraCapture();

void cameraSetFocus (int value);
