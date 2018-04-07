// camera.h
#pragma once
#include "stm32746g_discovery.h"
#include "../common/cLcd.h"

#define CAMERA_160x120    0x00
#define CAMERA_320x240    0x01
#define CAMERA_640x480    0x02
#define CAMERA_800x600    0x03
#define CAMERA_1600x1200  0x04

void cameraInit (cLcd* lcd, uint32_t resolution);

uint32_t cameraGetXSize();
uint32_t cameraGetYSize();

void cameraStart (uint32_t buffer, bool continuous);
void cameraPreview();
void cameraCapture();

void cameraSetFocus (int value);
