// cCamera.h
#pragma once
#include "stm32746g_discovery.h"

class cCamera {
public:
  void init();
  void start (bool captureMode, uint8_t* buffer);

  uint32_t getWidth() { return mWidth; }
  uint32_t getHeight() { return mHeight; }
  bool getCaptureMode() { return mCapture; }
  char* getString();

  uint8_t* getJpegFrame (int& jpegLen);

  void setFocus (int value);

  void dmaIrqHandler();
  void dcmiIrqHandler();

private:
  void gpioInit();
  void mt9d111Init();
  void dcmiInit();
  void dcmiStart (uint8_t* buffer);

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

  // dcmi dma
  //{{{  struct dmaBaseRegisters
  struct tDmaBaseRegisters {
    __IO uint32_t ISR;   // DMA interrupt status register
    __IO uint32_t Reserved0;
    __IO uint32_t IFCR;  // DMA interrupt flag clear register
    };
  //}}}
  tDmaBaseRegisters* mDmaBaseRegisters = nullptr;
  uint32_t mStreamIndex = 0;

  __IO uint32_t mXferCount;    // DMA transfer counter
  __IO uint32_t mXferSize;     // DMA transfer size
  uint32_t mXferMaxCount;      // DMA transfer number
  uint8_t* mBufPtr = nullptr;  // Pointer to DMA output buffer
  uint8_t* mCurPtr = nullptr;  // Pointer to DMA output buffer

  uint8_t* mStartFramePtr = nullptr;
  uint8_t* mJpegBuf = nullptr;
  int      mJpegLen = 0;
  //}}}
  };
