// camera.h
#pragma once
#include "stm32746g_discovery.h"
#include "cLcd.h"

class cCamera {
public:
  void init (cLcd* lcd, bool useCapture);

  uint32_t getXsize() { return mXsize; }
  uint32_t getYsize() { return mYsize; }

  void setFocus (int value);

  void start (uint32_t buffer);
  void preview();
  void capture();

private:
  void gpioInit();
  void mt9d111Init();
  void dcmiInit (DCMI_HandleTypeDef* dcmi);
  void dcmiStart (DCMI_HandleTypeDef* dcmi, uint32_t DCMI_Mode, uint32_t data, uint32_t length);

  DMA_HandleTypeDef dmaHandler;
  uint32_t cameraCurrentResolution = 0;
  uint32_t mXsize = 0;
  uint32_t mYsize = 0;
  };
