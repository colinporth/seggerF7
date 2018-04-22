// cCamera.h
#pragma once
#include "stm32746g_discovery.h"

class cCamera {
public:
  void init();

  uint32_t getWidth() { return mWidth; }
  uint32_t getHeight() { return mHeight; }

  bool getMode() { return mJpegMode; }
  uint8_t getStatus() { return mJpegStatus; }
  int getDmaCount() { return mXferCount; }
  int getFps() { return 1000/mTookTicks; }
  int getFocus() { return mFocus; }

  uint8_t* getHeader (bool full, int qscale, uint32_t& headerLen);
  uint8_t* getLastFrame (uint32_t& frameLen);
  uint8_t* getNextFrame (uint32_t& frameLen);

  void setFocus (int value);

  void start (bool jpegMode, uint8_t* buffer);

  void dmaIrqHandler();
  void dcmiIrqHandler();

private:
  void gpioInit();

  void write (uint8_t reg, uint16_t value);
  void write1 (uint16_t reg, uint16_t value);
  void mt9d111Init();

  void dcmiStart (uint8_t* buffer);

  int app0Marker (uint8_t* ptr);
  int quantTableMarker (uint8_t* ptr, uint8_t qscale);
  int sofMarker (uint8_t* ptr, uint16_t width, uint16_t height);
  int huffTableMarkerAC (uint8_t* ptr, const uint16_t* htable, int classId);
  int huffTableMarkerDC (uint8_t* ptr, const uint16_t* htable, int classId);
  int sosMarker (uint8_t* ptr);

  void preview();
  void jpeg();

  //{{{  vars
  uint8_t mHeader[620];

  uint32_t mWidth = 0;
  uint32_t mHeight = 0;
  bool mJpegMode = false;

  int mFocus = 0;

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

  uint32_t mRgb565FrameLen = 0;

  uint8_t* mBufStart = nullptr;
  uint8_t* mBufEnd = nullptr;

  uint8_t* mFrameStart = nullptr;
  uint8_t* mFrameCur = nullptr;

  uint8_t* mFrame = nullptr;
  uint32_t mFrameLen = 0;

  uint8_t mJpegStatus = 0;
  //}}}
  };
