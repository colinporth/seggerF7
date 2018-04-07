// camera.h
#pragma once
#include "stm32746g_discovery.h"
#include "../common/cLcd.h"

#define CAMERA_160x120    0x00
#define CAMERA_320x240    0x01
#define CAMERA_640x480    0x02
#define CAMERA_800x600    0x03
#define CAMERA_1600x1200  0x04

class cCamera {
public:
  void init (cLcd* lcd, uint32_t resolution);

  uint32_t getXSize();
  uint32_t getYSize();

  void setFocus (int value);

  void start (uint32_t buffer, bool continuous);
  void preview();
  void capture();

private:
  DMA_HandleTypeDef dmaHandler;
  uint32_t cameraCurrentResolution;

  void mt9d111Init (uint32_t resolution);
  void dcmiInit (DCMI_HandleTypeDef* dcmi);
  void dcmiStart (DCMI_HandleTypeDef* dcmi, uint32_t DCMI_Mode, uint32_t data, uint32_t length);
  };
