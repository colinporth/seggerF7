// cCamera.h
#pragma once
#include "stm32746g_discovery.h"

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
  void dmaInit();
  void dcmiInit();
  void dcmiStart (uint32_t data, uint32_t length);

  void preview();
  void capture();
  void jpeg();

  //{{{
  struct tDcmiInfo {
     __IO uint32_t     mXferCount;    // DMA transfer counter
    __IO uint32_t      mXferSize;     // DMA transfer size
    uint32_t           mXferMaxCount; // DMA transfer number
    uint32_t           mBuffPtr;      // Pointer to DMA output buffer
    DMA_HandleTypeDef* DMA_Handle;    // Pointer to the DMA handler

    // init
    uint32_t SynchroMode;
    uint32_t PCKPolarity;
    uint32_t VSPolarity;
    uint32_t HSPolarity;
    uint32_t CaptureRate;
    uint32_t ExtendedDataMode;
    uint32_t JPEGMode;
    uint32_t ByteSelectMode;
    uint32_t ByteSelectStart;
    uint32_t LineSelectMode;
    uint32_t LineSelectStart;
    };
  //}}}
  tDcmiInfo dcmiInfo;
  DMA_HandleTypeDef dmaHandler;

  uint32_t mWidth = 0;
  uint32_t mHeight = 0;
  bool mCaptureMode = false;

  int mFrames = 0;
  char mStr[40] = { 0 };
  uint32_t mTicks = 0;
  uint32_t mTookTicks = 0;
  };
