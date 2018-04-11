// cCamera.h
#pragma once
#include "stm32746g_discovery.h"

class cCamera {
public:
  void init();
  void start (bool captureMode, uint32_t buffer);

  uint32_t getWidth() { return mWidth; }
  uint32_t getHeight() { return mHeight; }
  bool getCaptureMode() { return mCapture; }
  char* getString();

  void setFocus (int value);

  void dmaIrqHandler();
  void dcmiIrqHandler();

private:
  void gpioInit();
  void mt9d111Init();
  void dcmiInit();
  void dcmiStart (uint32_t data);

  void preview();
  void capture();
  void jpeg();

  //{{{  vars
  uint32_t mWidth = 0;
  uint32_t mHeight = 0;
  bool mCapture = false;

  int mFrames = 0;
  char mStr[40] = { 0 };
  uint32_t mTicks = 0;
  uint32_t mTookTicks = 0;
  uint32_t mLastXferCount = 0;

  // dcmi dma
  //{{{  struct dmaBaseRegisters
  struct tDmaBaseRegisters {
    __IO uint32_t ISR;   /*!< DMA interrupt status register */
    __IO uint32_t Reserved0;
    __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
    };
  //}}}
  tDmaBaseRegisters* mDmaBaseRegisters = nullptr;
  DMA_HandleTypeDef  mDmaHandler;

  __IO uint32_t     mXferCount;    // DMA transfer counter
  __IO uint32_t      mXferSize;     // DMA transfer size
  uint32_t           mXferMaxCount; // DMA transfer number
  uint32_t           mBuffPtr;      // Pointer to DMA output buffer
  uint32_t           mCurPtr;       // Pointer to DMA output buffer
  //}}}
  };
