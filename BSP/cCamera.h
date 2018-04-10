// camera.h
#pragma once
#include "stm32746g_discovery.h"
#include "cLcd.h"

class cCamera {
public:
  void init();
  void start (bool captureMode, uint32_t buffer);

  uint32_t getWidth() { return mWidth; }
  uint32_t getHeight() { return mHeight; }
  bool getCaptureMode() { return mCaptureMode; }

  char* getString();

  void setFocus (int value);

  void dmaIrqHandler();
  void dcmiIrqHandler();

private:
  void gpioInit();
  void mt9d111Init();
  void dmaInit (DMA_HandleTypeDef* hdma);
  void dcmiInit (DCMI_HandleTypeDef* dcmi);
  void dcmiStart (DCMI_HandleTypeDef* dcmi, uint32_t DCMI_Mode, uint32_t data, uint32_t length);

  void preview();
  void capture();
  void jpeg();

  DMA_HandleTypeDef dmaHandler;
  uint32_t mWidth = 0;
  uint32_t mHeight = 0;
  bool mCaptureMode = false;

  int mFrames = 0;
  char mStr[40] = { 0 };
  uint32_t mTicks = 0;
  uint32_t mTookTicks = 0;
  };
