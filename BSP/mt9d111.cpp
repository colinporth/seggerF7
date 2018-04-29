// mt9d111.cpp
//{{{  includes
#include "cCamera.h"

#include "cLcd.h"
//}}}
#define Capture800x600
#define I2cAddress 0x90
//{{{  const
//{{{
const uint8_t kJpegStdQuantTblY_ZZ[64] = {
   16,  11,  12,  14,  12,  10,  16,  14,
   13,  14,  18,  17,  16,  19,  24,  40,
   26,  24,  22,  22,  24,  49,  35,  37,
   29,  40,  58,  51,  61,  60,  57,  51,
   56,  55,  64,  72,  92,  78,  64,  68,
   87,  69,  55,  56,  80, 109,  81,  87,
   95,  98, 103, 104, 103,  62,  77, 113,
  121, 112, 100, 120,  92, 101, 103,  99 };
//}}}
//{{{
const uint8_t kJpegStdQuantTblC_ZZ[64] = {
  17, 18, 18, 24, 21, 24, 47, 26,
  26, 47, 99, 66, 56, 66, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99 };
//}}}
//{{{
const uint16_t kJpegStdHuffmanTbl[384] = {
  0x100, 0x101, 0x204, 0x30b, 0x41a, 0x678, 0x7f8, 0x9f6,
  0xf82, 0xf83, 0x30c, 0x41b, 0x679, 0x8f6, 0xaf6, 0xf84,
  0xf85, 0xf86, 0xf87, 0xf88, 0x41c, 0x7f9, 0x9f7, 0xbf4,
  0xf89, 0xf8a, 0xf8b, 0xf8c, 0xf8d, 0xf8e, 0x53a, 0x8f7,
  0xbf5, 0xf8f, 0xf90, 0xf91, 0xf92, 0xf93, 0xf94, 0xf95,
  0x53b, 0x9f8, 0xf96, 0xf97, 0xf98, 0xf99, 0xf9a, 0xf9b,
  0xf9c, 0xf9d, 0x67a, 0xaf7, 0xf9e, 0xf9f, 0xfa0, 0xfa1,
  0xfa2, 0xfa3, 0xfa4, 0xfa5, 0x67b, 0xbf6, 0xfa6, 0xfa7,
  0xfa8, 0xfa9, 0xfaa, 0xfab, 0xfac, 0xfad, 0x7fa, 0xbf7,
  0xfae, 0xfaf, 0xfb0, 0xfb1, 0xfb2, 0xfb3, 0xfb4, 0xfb5,
  0x8f8, 0xec0, 0xfb6, 0xfb7, 0xfb8, 0xfb9, 0xfba, 0xfbb,
  0xfbc, 0xfbd, 0x8f9, 0xfbe, 0xfbf, 0xfc0, 0xfc1, 0xfc2,
  0xfc3, 0xfc4, 0xfc5, 0xfc6, 0x8fa, 0xfc7, 0xfc8, 0xfc9,
  0xfca, 0xfcb, 0xfcc, 0xfcd, 0xfce, 0xfcf, 0x9f9, 0xfd0,
  0xfd1, 0xfd2, 0xfd3, 0xfd4, 0xfd5, 0xfd6, 0xfd7, 0xfd8,
  0x9fa, 0xfd9, 0xfda, 0xfdb, 0xfdc, 0xfdd, 0xfde, 0xfdf,
  0xfe0, 0xfe1, 0xaf8, 0xfe2, 0xfe3, 0xfe4, 0xfe5, 0xfe6,
  0xfe7, 0xfe8, 0xfe9, 0xfea, 0xfeb, 0xfec, 0xfed, 0xfee,
  0xfef, 0xff0, 0xff1, 0xff2, 0xff3, 0xff4, 0xff5, 0xff6,
  0xff7, 0xff8, 0xff9, 0xffa, 0xffb, 0xffc, 0xffd, 0xffe,
  0x30a, 0xaf9, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff,
  0xfd0, 0xfd1, 0xfd2, 0xfd3, 0xfd4, 0xfd5, 0xfd6, 0xfd7,
  0x101, 0x204, 0x30a, 0x418, 0x419, 0x538, 0x678, 0x8f4,
  0x9f6, 0xbf4, 0x30b, 0x539, 0x7f6, 0x8f5, 0xaf6, 0xbf5,
  0xf88, 0xf89, 0xf8a, 0xf8b, 0x41a, 0x7f7, 0x9f7, 0xbf6,
  0xec2, 0xf8c, 0xf8d, 0xf8e, 0xf8f, 0xf90, 0x41b, 0x7f8,
  0x9f8, 0xbf7, 0xf91, 0xf92, 0xf93, 0xf94, 0xf95, 0xf96,
  0x53a, 0x8f6, 0xf97, 0xf98, 0xf99, 0xf9a, 0xf9b, 0xf9c,
  0xf9d, 0xf9e, 0x53b, 0x9f9, 0xf9f, 0xfa0, 0xfa1, 0xfa2,
  0xfa3, 0xfa4, 0xfa5, 0xfa6, 0x679, 0xaf7, 0xfa7, 0xfa8,
  0xfa9, 0xfaa, 0xfab, 0xfac, 0xfad, 0xfae, 0x67a, 0xaf8,
  0xfaf, 0xfb0, 0xfb1, 0xfb2, 0xfb3, 0xfb4, 0xfb5, 0xfb6,
  0x7f9, 0xfb7, 0xfb8, 0xfb9, 0xfba, 0xfbb, 0xfbc, 0xfbd,
  0xfbe, 0xfbf, 0x8f7, 0xfc0, 0xfc1, 0xfc2, 0xfc3, 0xfc4,
  0xfc5, 0xfc6, 0xfc7, 0xfc8, 0x8f8, 0xfc9, 0xfca, 0xfcb,
  0xfcc, 0xfcd, 0xfce, 0xfcf, 0xfd0, 0xfd1, 0x8f9, 0xfd2,
  0xfd3, 0xfd4, 0xfd5, 0xfd6, 0xfd7, 0xfd8, 0xfd9, 0xfda,
  0x8fa, 0xfdb, 0xfdc, 0xfdd, 0xfde, 0xfdf, 0xfe0, 0xfe1,
  0xfe2, 0xfe3, 0xaf9, 0xfe4, 0xfe5, 0xfe6, 0xfe7, 0xfe8,
  0xfe9, 0xfea, 0xfeb, 0xfec, 0xde0, 0xfed, 0xfee, 0xfef,
  0xff0, 0xff1, 0xff2, 0xff3, 0xff4, 0xff5, 0xec3, 0xff6,
  0xff7, 0xff8, 0xff9, 0xffa, 0xffb, 0xffc, 0xffd, 0xffe,
  0x100, 0x9fa, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff,
  0xfd0, 0xfd1, 0xfd2, 0xfd3, 0xfd4, 0xfd5, 0xfd6, 0xfd7,
  0x100, 0x202, 0x203, 0x204, 0x205, 0x206, 0x30e, 0x41e,
  0x53e, 0x67e, 0x7fe, 0x8fe, 0xfff, 0xfff, 0xfff, 0xfff,
  0x100, 0x101, 0x102, 0x206, 0x30e, 0x41e, 0x53e, 0x67e,
  0x7fe, 0x8fe, 0x9fe, 0xafe, 0xfff, 0xfff, 0xfff, 0xfff };
//}}}
//}}}

cCamera* gCamera = nullptr;
SemaphoreHandle_t mFrameSem;

extern "C" {
  void DMA2_Stream1_IRQHandler() { gCamera->dmaIrqHandler(); }
  void DCMI_IRQHandler() { gCamera->dcmiIrqHandler(); }
  }

// public
//{{{
void cCamera::init (uint8_t* bufStart, uint8_t* bufEnd) {

  mBufStart = bufStart;
  mBufEnd = bufEnd;

  gCamera = this;
  vSemaphoreCreateBinary (mFrameSem);

  gpioInit();

  // init camera i2c, readBack id
  CAMERA_IO_Init();

  write (0xF0, 0);
  auto cameraId = CAMERA_IO_Read16 (I2cAddress, 0);
  if (cameraId == 0x1519) {
    mt9d111Init();

    // init dcmi dma
    // - mDmaBaseRegisters, mStreamIndex
    const uint8_t kFlagBitshiftOffset[8U] = {0U, 6U, 16U, 22U, 0U, 6U, 16U, 22U};

    auto streamNum = (((uint32_t)DMA2_Stream1 & 0xFFU) - 16U) / 24U;
    mStreamIndex = kFlagBitshiftOffset[streamNum];
    if (streamNum > 3U) // return pointer to HISR and HIFCR
      mDmaBaseRegisters = (tDmaBaseRegisters*)(((uint32_t)DMA2_Stream1 & (uint32_t)(~0x3FFU)) + 4U);
    else // return pointer to LISR and LIFCR
      mDmaBaseRegisters = (tDmaBaseRegisters*)((uint32_t)DMA2_Stream1 & (uint32_t)(~0x3FFU));

    // clear all dma interrupt flags
    mDmaBaseRegisters->IFCR = 0x3FU << mStreamIndex;

    // NVIC configuration for DCMI transfer complete interrupt
    HAL_NVIC_SetPriority (DCMI_IRQn, 0x08, 0);
    HAL_NVIC_EnableIRQ (DCMI_IRQn);

    // NVIC configuration for DMA2 transfer complete interrupt
    HAL_NVIC_SetPriority (DMA2_Stream1_IRQn, 0x08, 0);
    HAL_NVIC_EnableIRQ (DMA2_Stream1_IRQn);

    // select sequencer preview
    preview();

    // start dma, dcmi
    start();
    }
  else
    cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "cameraId problem %x", cameraId);
  }
//}}}

//{{{
uint8_t* cCamera::getBmpHeader (uint32_t& headerLen) {
//  set RGB565 16bpp bmp header, minimal for pic no greater than 1600x1200

  const int kHeaderLen = 0x0e;
  const int kDibLen =    0x28;
  const int kMaskLen =   0x0c;
  const int kPadLen =    0x02;
  //                     0x44 - padded out from 0x42

  int imageSize = mWidth*mHeight*2;
  headerLen = kHeaderLen + kDibLen + kMaskLen + kPadLen;
  int fileSize = headerLen + imageSize;

  memset (mHeader, 0, headerLen);

  mHeader[0] = 'B';  // magic
  mHeader[1] = 'M';

  mHeader[2] = fileSize;
  mHeader[3] = fileSize >> 8;
  mHeader[4] = fileSize >> 16;

  mHeader[0x0a] = headerLen;
  mHeader[0x0e] = kDibLen;

  mHeader[0x12] = mWidth;
  mHeader[0x13] = mWidth >> 8;

  mHeader[0x16] = -mHeight;
  mHeader[0x17] = -mHeight >> 8;
  mHeader[0x18] = -mHeight >> 16;
  mHeader[0x19] = -mHeight >> 24;

  mHeader[0x1A] = 1;  // 0x1A number color planes
  mHeader[0x1C] = 16; // 0x1C bits per pixel
  mHeader[0x1E] = 3;  // 0x1E compression

  mHeader[0x22] = imageSize;
  mHeader[0x23] = imageSize >> 8;
  mHeader[0x24] = imageSize >> 16;

  mHeader[0x26] = 0x13; // 0x26 horz resolution 72 dpi
  mHeader[0x27] = 0x0B;

  mHeader[0x2a] = 0x13; // 0x2a horz resolution 72 dpi
  mHeader[0x2b] = 0x0B;

  mHeader[0x37] = 0xF8;
  mHeader[0x3A] = 0xE0;
  mHeader[0x3B] = 0x07;
  mHeader[0x3E] = 0x1F;

  return mHeader;
  }
//}}}
//{{{
uint8_t* cCamera::getFullJpgHeader (int qscale, uint32_t& headerLen) {

  // set jpegHeader
  auto ptr = mHeader;

  *ptr++ = 0xFF; // SOI marker
  *ptr++ = 0xD8;
  headerLen = 2;

  headerLen += app0Marker (ptr);
  headerLen += quantTableMarker (mHeader + headerLen, qscale);
  headerLen += sofMarker (mHeader + headerLen, mWidth, mHeight);

  headerLen += huffTableMarkerAC (mHeader + headerLen, &kJpegStdHuffmanTbl[0], 0x10);
  headerLen += huffTableMarkerAC (mHeader + headerLen, &kJpegStdHuffmanTbl[176], 0x11);
  headerLen += huffTableMarkerDC (mHeader + headerLen, &kJpegStdHuffmanTbl[352], 0x00);
  headerLen += huffTableMarkerDC (mHeader + headerLen, &kJpegStdHuffmanTbl[368], 0x01);

  headerLen += sosMarker (mHeader + headerLen);

  return mHeader;
  }
//}}}
//{{{
uint8_t* cCamera::getSmallJpgHeader (int qscale, uint32_t& headerLen) {

  // set jpegHeader
  auto ptr = mHeader;

  *ptr++ = 0xFF; // SOI marker
  *ptr++ = 0xD8;
  headerLen = 2;

  headerLen += app0Marker (ptr);
  headerLen += quantTableMarker (mHeader + headerLen, qscale);
  headerLen += sofMarker (mHeader + headerLen, mWidth, mHeight);
  headerLen += sosMarker (mHeader + headerLen);

  return mHeader;
  }
//}}}
//{{{
uint8_t* cCamera::getLastFrame (uint32_t& frameLen, bool& jpeg, uint32_t& frameId) {

  frameLen = mFrameLen;
  jpeg = mJpegFrame;
  frameId = mFrameId;
  return mFrame;
  }
//}}}
//{{{
uint8_t* cCamera::getNextFrame (uint32_t& frameLen, bool& jpeg) {

  xSemaphoreTake (mFrameSem, 500);

  frameLen = mFrameLen;
  jpeg = mJpegFrame;
  return mFrame;
  }
//}}}

//{{{
void cCamera::setFocus (int value) {

  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "setFocus %d", value);

  write (0xF0, 1);

  if (value <= 1) {
    write1 (0x9071, 0x00); // SFR GPIO data b1:0 = 0 - disable GPIO1
    write1 (0x9081, 255);  // SFR GPIO wg_t00 = 255 initial off
    write1 (0x9083, 0);    // SFR GPIO wg_t10 = 0 no on
    }

  else {
    if (value > 254)
      value = 254;

    write1 (0x9071, 0x02);        // SFR GPIO data b1:0 = enable GPIO1
    write1 (0x9081, 255 - value); // SFR GPIO wg_t00 pwm off
    write1 (0x9083, value);       // SFR GPIO wg_t10 pwm on
    }

  mFocus = value;
  }
//}}}

//{{{
void cCamera::preview() {

  mJpegMode = false;
  mCaptureMode = false;

  mFrame = nullptr;
  mFrameLen = 0;
  mWidth = 800;
  mHeight = 600;
  mRgb565FrameLen = getWidth() * getHeight() * 2;

  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "preview %dx%d", mWidth, mHeight);

  // switch sequencer to preview
  write (0xf0, 1);
  write1 (0xA120, 0x00);   // Sequencer.params.mode - none
  write1 (0xA103, 0x01);   // Sequencer transition to preview A
  }
//}}}
//{{{
void cCamera::capture() {

  mJpegMode = true;
  mCaptureMode = true;

  mFrame = nullptr;
  mFrameLen = 0;

#ifdef Capture800x600
  mWidth = 800;
  mHeight = 600;
#else
  mWidth = 1600;
  mWidth = 1200;
#endif

  mRgb565FrameLen = getWidth() * getHeight() * 2;

  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "capture jpeg %dx%d", mWidth, mHeight);

  // switch sequencer to captue jpeg
  write (0xf0, 1);
  write1 (0xA120, 0x02);   // Sequencer.params.mode - capture video
  write1 (0xA103, 0x02);   // Sequencer transition to capture B
  }
//}}}

//{{{
void cCamera::dmaIrqHandler() {

  uint32_t isr = mDmaBaseRegisters->ISR;

  //{{{  fifoError Interrupt
  if (isr & (DMA_FLAG_FEIF0_4 << mStreamIndex))
    if (DMA2_Stream1->FCR & DMA_IT_FE) {
      mDmaBaseRegisters->IFCR = DMA_FLAG_FEIF0_4 << mStreamIndex;
      // too many
      //cLcd::mLcd->debug (LCD_COLOR_RED, "dcmi dma Fifo Irq");
      }
  //}}}
  //{{{  transferError Interrupt
  if (isr & (DMA_FLAG_TEIF0_4 << mStreamIndex)) {
    if (DMA2_Stream1->CR & DMA_IT_TE) {
      mDmaBaseRegisters->IFCR = DMA_FLAG_TEIF0_4 << mStreamIndex;
      cLcd::mLcd->debug (LCD_COLOR_RED, "dcmi dma transferError Irq");
      }
    }
  //}}}
  //{{{  directMode Error Interrupt
  if (isr & (DMA_FLAG_DMEIF0_4 << mStreamIndex))
    if (DMA2_Stream1->CR & DMA_IT_DME) {
      mDmaBaseRegisters->IFCR = DMA_FLAG_DMEIF0_4 << mStreamIndex;
      cLcd::mLcd->debug (LCD_COLOR_RED, "dcmi dma directModeError IRQ");
      }
  //}}}

  if (isr & (DMA_FLAG_TCIF0_4 << mStreamIndex)) {
    // transferComplete Interrupt, doubleBufferMode handling
    if (DMA2_Stream1->CR & DMA_IT_TC) {
      // clear transferComplete interrupt flag
      mDmaBaseRegisters->IFCR = DMA_FLAG_TCIF0_4 << mStreamIndex;

      // set up address for next doubleBuffered dma chunk
      mXferCount++;
      auto buf = mBufStart + ((mXferCount+1) * (4 * mXferSize));
      if (buf >= mBufEnd)
        cLcd::mLcd->debug (LCD_COLOR_RED, "dma problem");

      if (mXferCount & 1)
        DMA2_Stream1->M0AR = (uint32_t)buf;
      else
        DMA2_Stream1->M1AR = (uint32_t)buf;

      mBufCur += 4 * mXferSize;
      }
    }

  }
//}}}
//{{{
void cCamera::dcmiIrqHandler() {

  uint32_t misr = DCMI->MISR;

  if ((misr & DCMI_RIS_VSYNC_RIS) == DCMI_RIS_VSYNC_RIS) {
    DCMI->ICR = DCMI_RIS_VSYNC_RIS;

    uint32_t dmaLen = (mXferSize - DMA2_Stream1->NDTR) * 4;
    uint8_t* nextFrameStart = mBufCur + dmaLen;
    uint32_t frameLen = nextFrameStart - mLastFrameStart;

    if (!frameLen) {
      //{{{  zero frameLen
      mFrame = nullptr;
      mFrameLen = 0;
      //cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "zero frameLen");
      }
      //}}}
    else if (frameLen == mRgb565FrameLen) {
      //{{{  good rgb565Frame
      mFrame = mLastFrameStart;
      mFrameLen = frameLen;
      mJpegFrame = false;
      mFrameId++;

      portBASE_TYPE taskWoken = pdFALSE;
      if (xSemaphoreGiveFromISR (mFrameSem, &taskWoken) == pdTRUE)
        portEND_SWITCHING_ISR (taskWoken);

      //cLcd::mLcd->debug (LCD_COLOR_GREEN, "r%2d:%6d:%8x %d", mXferCount,dmaBytes,mFrame, mFrameLen);
      }
      //}}}
    else if (mJpegMode && (frameLen < mRgb565FrameLen)) {
      // get last byte jpegStatus
      mJpegStatus = mBufCur[dmaLen-1];
      if ((mJpegStatus & 0x0f) == 0x01) {
        // get last-1,2,3 bytes jpegLen
        uint32_t jpegLen = (mBufCur[dmaLen-2] << 16) + (mBufCur[dmaLen-3] << 8) + mBufCur[dmaLen-4];
        if (jpegLen > frameLen) {
          //{{{  bad jpegLen > frameLen
          mFrame = nullptr;
          mFrameLen = 0;

          cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "jpegLen > frameLen %d %d", jpegLen, frameLen);
          }
          //}}}
        else {
          //{{{  good jpegFrame
          mLastFrameStart[jpegLen++] = 0xFF;
          mLastFrameStart[jpegLen++] = 0xD9;

          mFrame = mLastFrameStart;
          mFrameLen = jpegLen;
          mJpegFrame = true;
          mFrameId++;

          portBASE_TYPE taskWoken = pdFALSE;
          if (xSemaphoreGiveFromISR (mFrameSem, &taskWoken) == pdTRUE)
            portEND_SWITCHING_ISR (taskWoken);

          //cLcd::mLcd->debug (LCD_COLOR_GREEN, "j%2d:%6d %x:%d", mXferCount,dmaBytes, mJpegStatus,mFrameLen);
          }
          //}}}
        }
      else {
        //{{{  bad jpegStatus
        mFrame = nullptr;
        mFrameLen = 0;

        cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "jpegStatus:%x framelen:%d", mJpegStatus, frameLen);
        }
        //}}}
      }
    else {
      //{{{  bad frameLen
      mFrame = nullptr;
      mFrameLen = 0;

      cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "len %x %x %d %d", mBufCur, mLastFrameStart, dmaLen, frameLen);
      }
      //}}}

    // enough room for largest frame
    if (nextFrameStart + mRgb565FrameLen <= mBufEnd)
      mLastFrameStart = nextFrameStart;
    else {
      //{{{  set dma to bufStart
      // disable dma
      DMA2_Stream1->CR &= ~DMA_SxCR_EN;

      // reset pointers
      mLastFrameStart = mBufStart;
      mBufCur = mBufStart;
      mXferCount = 0;

      // clear all dma interrupt flags
      DMA2->LIFCR = DMA_FLAG_TCIF1_5;
      DMA2->LIFCR = DMA_FLAG_TEIF1_5;
      DMA2->LIFCR = DMA_FLAG_DMEIF1_5;
      DMA2->LIFCR = DMA_FLAG_HTIF1_5;
      DMA2->LIFCR = DMA_FLAG_FEIF1_5;

      // reset doubleBuffer addresses and xferCount
      DMA2_Stream1->M0AR = (uint32_t)mBufStart;
      DMA2_Stream1->M1AR = (uint32_t)mBufStart + (4 * mXferSize);

      // re-enable dma
      DMA2_Stream1->CR = DMA_CHANNEL_1 |
                         DMA_PERIPH_TO_MEMORY | DMA_CIRCULAR | DMA_SxCR_DBM | DMA_PRIORITY_HIGH |
                         DMA_PDATAALIGN_WORD | DMA_MDATAALIGN_WORD |
                         DMA_PBURST_SINGLE | DMA_MBURST_INC4 |
                         DMA_PINC_DISABLE | DMA_MINC_ENABLE |
                         DMA_IT_TC | DMA_IT_TE | DMA_IT_DME |
                         DMA_SxCR_EN;

      //cLcd::mLcd->debug (LCD_COLOR_CYAN, "dmaWrap %x", nextFrameStart);
      }
      //}}}

    auto ticks = HAL_GetTick();
    mTookTicks = ticks - mTicks;
    mTicks = ticks;
    }

  if ((misr & DCMI_RIS_ERR_RIS) == DCMI_RIS_ERR_RIS) {
    //{{{  synchronizationError interrupt
    DCMI->ICR = DCMI_RIS_ERR_RIS;
    cLcd::mLcd->debug (LCD_COLOR_RED, "DCMI sync Irq");
    }
    //}}}

  if ((misr & DCMI_RIS_OVR_RIS) == DCMI_RIS_OVR_RIS) {
    //{{{  overflowError interrupt
    DCMI->ICR = DCMI_RIS_OVR_RIS;
    // disible dma
    DMA2_Stream1->CR &= ~DMA_SxCR_EN;
    cLcd::mLcd->debug (LCD_COLOR_RED, "DCMI overflow Irq");
    }
    //}}}
  }
//}}}

// private
//{{{
void cCamera::gpioInit() {

  //  config clocks
  __HAL_RCC_DCMI_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  //  config gpio alternate function
  GPIO_InitTypeDef gpio_init_structure;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  gpio_init_structure.Alternate = GPIO_AF13_DCMI;

  gpio_init_structure.Pin = GPIO_PIN_4 | GPIO_PIN_6;
  HAL_GPIO_Init (GPIOA, &gpio_init_structure);

  gpio_init_structure.Pin = GPIO_PIN_3;
  HAL_GPIO_Init (GPIOD, &gpio_init_structure);

  gpio_init_structure.Pin = GPIO_PIN_5 | GPIO_PIN_6;
  HAL_GPIO_Init (GPIOE, &gpio_init_structure);

  gpio_init_structure.Pin = GPIO_PIN_9;
  HAL_GPIO_Init (GPIOG, &gpio_init_structure);

  gpio_init_structure.Pin = GPIO_PIN_9 | GPIO_PIN_10  | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14;
  HAL_GPIO_Init (GPIOH, &gpio_init_structure);
  }
//}}}

//{{{
void cCamera::write (uint8_t reg, uint16_t value) {

  CAMERA_IO_Write16 (I2cAddress, reg, value);
  }
//}}}
//{{{
void cCamera::write1 (uint16_t reg, uint16_t value) {

  write (0xc6, reg);
  write (0xc8, value);
  }
//}}}
//{{{
void cCamera::mt9d111Init() {

  //{{{  soft reset
  write (0x65, 0xA000); // Clock(R/W) - bypass PLL, R0x65:0 = 0xA000,

  write (0xF0, 1);      // page 1
  write (0xC3, 0x0501); // MicrocontrollerBootMode - reset MCU by setting R0xC3:1 = 0x0501.

  write (0xF0, 0);      // page 0
  write (0x0D, 0x0021); // Enable soft reset by setting R0x0D:0 = 0x0021. Bit 0 is used for the sensor core reset
  write (0x0D, 0x0000); // Disable soft reset by setting R0x0D:0 = 0x0000.
  HAL_Delay (100);
  //}}}

  // page 0
#ifdef Capture800x600
  write (0x05, 0x03a8); // capture B HBLANK
  write (0x06, 0x000B); // capture B VBLANK
  write (0x07, 0x0285); // preview A HBLANK
  write (0x08, 0x000B); // preview A VBLANK
  write (0x20, 0x8300); // capture B Read Mode
  write (0x21, 0x8400); // preview A Read Mode
#else
  write (0x05, 0x013e); // capture B HBLANK
  write (0x06, 0x000B); // capture B VBLANK
  write (0x07, 0x0153); // preview A HBLANK
  write (0x08, 0x000B); // preview A VBLANK
  write (0x20, 0x0300); // capture B Read Mode
  write (0x21, 0x8400); // preview A Read Mode
#endif

  // PLL M=18, N=1, P=1    (24mhz / (N+1)) * M / 2 * (P+1) = 54mhz
  write (0x66, 0x1201); // PLLControl1 - M:N
  write (0x67, 0x0501); // PLLControl2 - 0x05:P
  write (0x65, 0xA000); // Clock(R/W)  - pllOn
  write (0x65, 0x2000); // Clock(R/W)  - usePll
  HAL_Delay (100);

  // page 1
  write (0xF0, 1);
  write (0x09, 0x000A); // FactoryBypass             - 10 bit sensor
  write (0x0A, 0x0002); // PadSlew                   - pixclk slew
  write (0x97, 0x22);   // OutputFormatConfiguration - RGB565, swap odd even

#ifdef Capture800x600
  //{{{  register wizard
  write1 (0x2703, 0x0320); // Output Width A
  write1 (0x2705, 0x0258); // Output Height A
  write1 (0x2707, 0x0320); // Output Width B
  write1 (0x2709, 0x0258); // Output Height B

  write1 (0x270F, 0x001C); // Row Start A
  write1 (0x2711, 0x003C); // Column Start A
  write1 (0x2713, 0x04b0); // Row Height A
  write1 (0x2715, 0x0640); // Column Width A
  write1 (0x2717, 0x0251); // Extra Delay A
  write1 (0x2719, 0x0011); // Row Speed A
  write1 (0x2727, 0);      // Crop_X0 A
  write1 (0x2729, 800);    // Crop_X1 A
  write1 (0x272B, 0);      // Crop_Y0 A
  write1 (0x272D, 600);    // Crop_Y1 A

  write1 (0xA743, 0x02);   // Gamma and Contrast Settings A
  write1 (0xA77D, 0x22);   // outputFormat A - RGB565, swap odd even

  write1 (0x271B, 0x001C); // Row Start B
  write1 (0x271D, 0x003C); // Column Start B
  write1 (0x271F, 0x04b0); // Row Height B
  write1 (0x2721, 0x0640); // Column Width B

  write1 (0x2723, 0x01d0); // Extra Delay B
  write1 (0x2725, 0x0011); // Row Speed B

  write1 (0x2735, 0);      // Crop_X0 B
  write1 (0x2737, 0x0320); // Crop_X1 B
  write1 (0x2739, 0);      // Crop_Y0 B
  write1 (0x273B, 0x0258); // Crop_Y1 B

  write1 (0xA744, 0x02);   // Gamma and Contrast Settings B
  write1 (0xA77E, 0x02);   // outputFormat B - YUV, swap odd even

  write1 (0xA217, 0x08);   // IndexTH23 = 8

  write1 (0x276D, 0xE0E2); // FIFO_Conf1 A
  write1 (0xA76F, 0xE1);   // FIFO_Conf2 A
  write1 (0x2774, 0xE0E1); // FIFO_Conf1 B
  write1 (0xA776, 0xE1);   // FIFO_Conf2 B

  write1 (0x220B, 0x01a9); // Max R12 B (Shutter Delay)
  write1 (0x2228, 0x02d2); // RowTime (msclk per)/4

  write1 (0x222F, 0x004d); // R9 Step
  write1 (0xA408, 0x0011); // search_f1_50
  write1 (0xA409, 0x0013); // search_f2_50
  write1 (0xA40A, 0x000e); // search_f1_60
  write1 (0xA40B, 0x0010); // search_f2_60
  write1 (0x2411, 0x004d); // R9_Step_60
  write1 (0x2413, 0x005c); // R9_Step_50
  //}}}
#else
  //{{{  register wizard
  write1 (0x2703, 0x0320); // Output Width A
  write1 (0x2705, 0x0258); // Output Height A
  write1 (0x2707, 0x0640); // Output Width B
  write1 (0x2709, 0x04b0); // Output Height B

  write1 (0x270F, 0x001C); // Row Start A
  write1 (0x2711, 0x003C); // Column Start A
  write1 (0x2713, 0x04b0); // Row Height A
  write1 (0x2715, 0x0640); // Column Width A
  write1 (0x2717, 0x0042); // Extra Delay A
  write1 (0x2719, 0x0011); // Row Speed A

  write1 (0xA743, 0x02);   // Gamma and Contrast Settings A
  write1 (0xA77D, 0x22);   // outputFormat A - RGB565, swap odd even

  write1 (0x271B, 0x001C); // Row Start B
  write1 (0x271D, 0x003C); // Column Start B
  write1 (0x271F, 0x04b0); // Row Height B
  write1 (0x2721, 0x0640); // Column Width B

  write1 (0x2723, 0x021e); // Extra Delay B
  write1 (0x2725, 0x0011); // Row Speed B

  write1 (0xA744, 0x02);   // Gamma and Contrast Settings B
  write1 (0xA77E, 0x02);   // outputFormat B - YUV, swap odd even

  write1 (0x2727, 0);      // Crop_X0 A
  write1 (0x2729, 0x0320); // Crop_X1 A
  write1 (0x272B, 0);      // Crop_Y0 A
  write1 (0x272D, 0x0258); // Crop_Y1 A

  write1 (0x2735, 0);      // Crop_X0 B
  write1 (0x2737, 0x0640); // Crop_X1 B
  write1 (0x2739, 0);      // Crop_Y0 B
  write1 (0x273B, 0x04b0); // Crop_Y1 B

  write1 (0x276D, 0xE0E2); // FIFO_Conf1 A
  write1 (0xA76F, 0xE1);   // FIFO_Conf2 A
  write1 (0x2774, 0xE0E1); // FIFO_Conf1 B
  write1 (0xA776, 0xE1);   // FIFO_Conf2 B

  write1 (0xA217, 0x08);   // IndexTH23 = 8

  write1 (0x220B, 0x023c); // Max R12 B (Shutter Delay)
  write1 (0x2228, 0x0239); // RowTime (msclk per)/4

  write1 (0x222F, 0x002B); // R9 Step
  write1 (0xA408, 0x0009); // search_f1_50
  write1 (0xA409, 0x0008); // search_f2_50
  write1 (0xA40A, 0x0007); // search_f1_60
  write1 (0xA40B, 0x0009); // search_f2_60
  write1 (0x2411, 0x002B); // R9_Step_60
  write1 (0x2413, 0x0034); // R9_Step_50
  //}}}
#endif
  //{{{  jpeg b config
  // jpeg.config id=9  0x07
  //                           b:3  host indicates ready
  //   b:6:7  quant table id   b:2  enable retry on error
  //   b:5  auto select quant  b:1  handshake on error
  //   b:4  scaled quant       b:0  video
  write1 (0xa907, 0x0031);

  // mode fifo_config0_B - shadow ifp page2 0x0d - Output Configuration Register
  //   b:11 freezeUpdate         b:7 enable byteswap              b:3 enable soi/eoi
  //   b:10 enable spoof codes  xb:6 enable variable pixclk       b:2 enable pixclk during invalid data
  //  xb:9  enable status byte  xb:5 enable ignore spoof height   b:1 enable pixclk between frames
  //   b:8  enable FV on LV      b:4 enable soi/eoi during FV    xb:0 enable spoof frame
  write1 (0x2772, 0x0261);

  // mode fifo_config1_B - shadow ifp page2 0x0e - Output PCLK1 & PCLK2 Configuration Register
  //   15:13 pclk2slew  11:8 pclk2divisor  7:5 pclk1slew  3:0 pclk1divisor
  write1 (0x2774, 0x4344);

  // mode fifo_config2_B - shadow ifp page2 0x0f - Output PCLK3 Configuration Register
  //   7:5 pclk3slew  3:0 pclk3divisor
  write1 (0x2776, 0x0042);

  write1 (0x270b, 0x0010); // mode_config - disable jpeg A, enable jpeg B
  //write1 (0x270B, 0x0030); // mode_config = disable jpeg A,B
  //}}}

  //{{{  focus init
  write1 (0x90B6, 0x01); // SFR GPIO suspend

  // enable GPIO0,1 as output, initial value 0
  write1 (0x9079, 0xFC); // SFR GPIO data direction
  write1 (0x9071, 0x00); // SFR GPIO data b1:0 = 0 GPIO0,1 initial 0

  // use 8bit counter clkdiv 2^(1+2)=8 -> 48mhz -> 6mhz ->> 23.7khz
  write1 (0x90B0, 0x01); // SFR GPIO wg_config b0 = 1 8bit counter
  write1 (0x90B2, 0x02); // SFR GPIO wg_clkdiv b0 = 2

  // GPIO0
  write1 (0x908B, 0x00); // SFR GPIO wg_n0 = 0 infinite
  write1 (0x9081, 255);  // SFR GPIO wg_t00 = 255 initial off
  write1 (0x9083, 0);    // SFR GPIO wg_t10 = 0 no on

  // GPIO1
  write1 (0x908A, 0x00); // SFR GPIO wg_n1 = 0 infinite
  write1 (0x9080, 0xFF); // SFR GPIO wg_t01 = 255 max initial on
  write1 (0x9082, 0x00); // SFR GPIO wg_t11 = 0 no off

  write1 (0x90B5, 0x00); // SFR GPIO reset
  write1 (0x90B6, 0x00); // SFR GPIO suspend
  //}}}
  //{{{  sequencer transitions
  //write1 (0xA122, 0x01); // previewEnter Auto Exposure = 1
  //write1 (0xA123, 0x00); // previewEnter Flicker Detection = 0
  //write1 (0xA124, 0x01); // previewEnter Auto White Balance = 1
  //write1 (0xA125, 0x00); // previewEnter Auto Focus = 0
  //write1 (0xA126, 0x01); // previewEnter Histogram = 1
  //write1 (0xA127, 0x00); // previewEnter Strobe Control  = 0
  //write1 (0xA128, 0x00); // previewEnter Skip Control = 0

  //write1 (0xA129, 0x03); // Preview Auto Exposure = 3
  //write1 (0xA12A, 0x02); // Preview Flicker Detection = 2
  //write1 (0xA12B, 0x03); // Preview Auto White Balance = 3
  //write1 (0xA12C, 0x00); // Preview Auto Focus = 0
  //write1 (0xA12D, 0x03); // Preview Histogram  = 3
  //write1 (0xA12E, 0x00); // Preview Strobe Control = 0
  //write1 (0xA12F, 0x00); // Preview Skip Control = 0

  //write1 (0xA130, 0x04); // previewLeave Auto Exposure = 4
  //write1 (0xA131, 0x00); // previewLeave Flicker Detection = 0
  //write1 (0xA132, 0x01); // previewLeave Auto White Balance = 1
  //write1 (0xA133, 0x00); // previewLeave Auto Focus = 0
  //write1 (0xA134, 0x01); // previewLeave Histogram = 1
  //write1 (0xA135, 0x00); // previewLeave Strobe Control = 0
  //write1 (0xA136, 0x00); // previewLeave Skip Control = 0

  //write1 (0xA137, 0x00); // CaptureEnter Auto Exposure = 4
  //write1 (0xA138, 0x00); // CaptureEnter Flicker Detection = 0
  //write1 (0xA139, 0x00); // CaptureEnter Auto White Balance = 3
  //write1 (0xA13A, 0x00); // CaptureEnter Auto Focus = 0
  //write1 (0xA13B, 0x00); // CaptureEnter Histogram = 3
  //write1 (0xA13C, 0x00); // CaptureEnter Strobe Control = 0
  //write1 (0xA13D, 0x00); // CaptureEnter Skip Control = 0
  //}}}
  //{{{  gamma tables
  //// gamma table A 0 to 18
  //write1 (0xA745, 0x00);  // 0
  //write1 (0xA746, 0x14);  // 20
  //write1 (0xA747, 0x23);  // 35
  //write1 (0xA748, 0x3A);  // 58
  //write1 (0xA749, 0x5E);  // 94
  //write1 (0xA74A, 0x76);  // 118
  //write1 (0xA74B, 0x88);  // 136
  //write1 (0xA74C, 0x96);  // 150
  //write1 (0xA74D, 0xA3);  // 163
  //write1 (0xA74E, 0xAF);  // 175
  //write1 (0xA74F, 0xBA);  // 186
  //write1 (0xA750, 0xC4);  // 196
  //write1 (0xA751, 0xCE);  // 206
  //write1 (0xA752, 0xD7);  // 215
  //write1 (0xA753, 0xE0);  // 224
  //write1 (0xA754, 0xE8);  // 232
  //write1 (0xA755, 0xF0);  // 240
  //write1 (0xA756, 0xF8);  // 248
  //write1 (0xA757, 0xFF);  // 255

  //// gamma table B 0 to 18
  //write1 (0xA758, 0x00);  // 0
  //write1 (0xA759, 0x14);  // 20
  //write1 (0xA75A, 0x23);  // 35
  //write1 (0xA75B, 0x3A);  // 58
  //write1 (0xA75C, 0x5E);  // 94
  //write1 (0xA75D, 0x76);  // 118
  //write1 (0xA75E, 0x88);  // 136
  //write1 (0xA75F, 0x96);  // 150
  //write1 (0xA760, 0xA3);  // 163
  //write1 (0xA761, 0xAF);  // 175
  //write1 (0xA762, 0xBA);  // 186
  //write1 (0xA763, 0xC4);  // 196
  //write1 (0xA764, 0xCE);  // 206
  //write1 (0xA765, 0xD7);  // 215
  //write1 (0xA766, 0xE0);  // 224
  //write1 (0xA767, 0xE8);  // 232
  //write1 (0xA768, 0xF0);  // 240
  //write1 (0xA769, 0xF8);  // 248
  //write1 (0xA76A, 0xFF);  // 255
  //}}}

  write1 (0xA103, 0x06); // Sequencer Refresh Mode
  write1 (0xA103, 0x05); // Sequencer Refresh
  }
//}}}

//{{{
void cCamera::start() {

  mBufCur = mBufStart;
  mLastFrameStart = mBufStart;

  mFrame = nullptr;
  mFrameLen = 0;

  mXferSize = 0xFFFF;  // max dmaLen in 4byte words
  mXferCount = 0;

  // enable dma doubleBufferMode,  config src, dst addresses, length
  DMA2_Stream1->PAR = (uint32_t)&(DCMI->DR);
  DMA2_Stream1->M0AR = (uint32_t)mBufStart;
  DMA2_Stream1->M1AR = (uint32_t)(mBufStart + (4*mXferSize));
  DMA2_Stream1->NDTR = mXferSize;

  // clear all dma interrupt flags
  DMA2->LIFCR = DMA_FLAG_TCIF1_5;
  DMA2->LIFCR = DMA_FLAG_TEIF1_5;
  DMA2->LIFCR = DMA_FLAG_DMEIF1_5;
  DMA2->LIFCR = DMA_FLAG_HTIF1_5;
  DMA2->LIFCR = DMA_FLAG_FEIF1_5;

  // enable dma - fifo, interrupt
  DMA2_Stream1->FCR = DMA_IT_FE | DMA_FIFOMODE_ENABLE | DMA_FIFO_THRESHOLD_FULL;

  // enable dma
  DMA2_Stream1->CR = DMA_CHANNEL_1 |
                     DMA_PERIPH_TO_MEMORY | DMA_CIRCULAR | DMA_SxCR_DBM | DMA_PRIORITY_HIGH |
                     DMA_PDATAALIGN_WORD | DMA_MDATAALIGN_WORD |
                     DMA_PBURST_SINGLE | DMA_MBURST_INC4 |
                     DMA_PINC_DISABLE | DMA_MINC_ENABLE |
                     DMA_IT_TC | DMA_IT_TE | DMA_IT_DME |
                     DMA_SxCR_EN;

  // enable dcmi - error,overrun,vsync interrupts
  DCMI->IER |= DCMI_IER_ERR_IE | DCMI_IER_OVR_IE | DCMI_IER_VSYNC_IE;
  // enable dcmi - continuous,capture,enable
  DCMI->CR = DCMI_CR_PCKPOL | DCMI_CR_JPEG | DCMI_CR_CAPTURE | DCMI_CR_ENABLE;
  }
//}}}

//{{{
int cCamera::app0Marker (uint8_t* ptr) {

  *ptr++ = 0xFF; // APP0 marker
  *ptr++ = 0xE0;

  int length = 17;
  *ptr++ = length >> 8;// length field
  *ptr++ = length & 0xFF;

  *ptr++ = 0x4A; // JFIF identifier
  *ptr++ = 0x46;
  *ptr++ = 0x49;
  *ptr++ = 0x46;
  *ptr++ = 0x00;

  *ptr++ = 0x01; // version
  *ptr++ = 0x02;

  *ptr++ = 0x00; // units - none
  *ptr++ = 0x00; // X density
  *ptr++ = 0x01;
  *ptr++ = 0x00; // Y density
  *ptr++ = 0x01;

  *ptr++ = 0x00; // X thumbnail
  *ptr++ = 0x00; // Y thumbnail

  *ptr++ = 0x00; // dummy pad for 4 byte alignment

  return length+2;
  }
//}}}
//{{{
int cCamera::quantTableMarker (uint8_t* ptr, uint8_t qscale) {

  *ptr++ = 0xFF;// quantization table marker
  *ptr++ = 0xDB;

  int length = 132;
  *ptr++ = length >> 8;// length field
  *ptr++ = length & 0xFF;

  *ptr++ = 0;// quantization table precision | identifier
  for (int i = 0; i < 64; i++) {
    int q = (kJpegStdQuantTblY_ZZ[i] * qscale + 16) >> 5;
    *ptr++ = q;// quantization table element
    }

  *ptr++ = 1;// quantization table precision | identifier
  for (int i = 0; i < 64; i++) {
    int q = (kJpegStdQuantTblC_ZZ[i] * qscale + 16) >> 5;
    *ptr++ = q;// quantization table element
    }

  return length+2;
  }
//}}}
//{{{
int cCamera::sofMarker (uint8_t* ptr, uint16_t width, uint16_t height) {

  *ptr++ = 0xFF; // startOfFrame: baseline DCT
  *ptr++ = 0xC0;

  int length = 17;
  *ptr++ = length >> 8;// length field
  *ptr++ = length & 0xFF;

  *ptr++ = 0x08; // sample precision

  *ptr++ = height >> 8; // number of lines
  *ptr++ = height & 0xFF;

  *ptr++ = width >> 8; // number of samples per line
  *ptr++ = width & 0xFF;

  *ptr++ = 0x03; // number of image components in frame

  *ptr++ = 0x00; // component identifier: Y
  *ptr++ = 0x21; // horizontal | vertical sampling factor: Y
  *ptr++ = 0x00; // quantization table selector: Y

  *ptr++ = 0x01; // component identifier: Cb
  *ptr++ = 0x11; // horizontal | vertical sampling factor: Cb
  *ptr++ = 0x01; // quantization table selector: Cb

  *ptr++ = 0x02; // component identifier: Cr
  *ptr++ = 0x11; // horizontal | vertical sampling factor: Cr
  *ptr++ = 0x01; // quantization table selector: Cr

  return length+2;
  }
//}}}
//{{{
int cCamera::huffTableMarkerAC (uint8_t* ptr, const uint16_t* htable, int classId) {

  *ptr++ = 0xFF; // huffman table marker
  *ptr++ = 0xC4;

  int length = 19;
  uint8_t* plength = ptr; // place holder for length field
  *ptr++;
  *ptr++;

  *ptr++ = classId;// huffman table class identifier
  for (int l = 0; l < 16; l++) {
    int count = 0;
    for (int i = 0; i < 162; i++) {
      if ((htable[i] >> 8) == l)
        count++;
      }
    *ptr++ = count; // number of huffman codes of length l+1
    }

  for (int l = 0; l < 16; l++) {
    // check EOB: 0|0
    if ((htable[160] >> 8) == l) {
      *ptr++ = 0; // HUFFVAL with huffman codes of length l+1
      length++;
      }

    // check HUFFVAL: 0|1 to E|A
    for (int i = 0; i < 150; i++) {
      if ((htable[i] >> 8) == l) {
        int a = i/10;
        int b = i%10;
        *ptr++ = (a<<4)|(b+1); // HUFFVAL with huffman codes of length l+1
        length++;
        }
      }

    // check ZRL: F|0
    if ((htable[161] >> 8) == l) {
      *ptr++ = 0xF0; // HUFFVAL with huffman codes of length l+1
      length++;
      }

    // check HUFFVAL: F|1 to F|A
    for (int i = 150; i < 160; i++) {
      if ((htable[i] >> 8) == l) {
        int a = i/10;
        int b = i%10;
        *ptr++ = (a<<4)|(b+1); // HUFFVAL with huffman codes of length l+1
        length++;
        }
      }
    }

  *plength++ = length >> 8; // length field
  *plength = length & 0xFF;

  return length + 2;
  }
//}}}
//{{{
int cCamera::huffTableMarkerDC (uint8_t* ptr, const uint16_t* htable, int classId) {

  *ptr++ = 0xFF; // huffman table marker
  *ptr++ = 0xC4;

  int length = 19;
  uint8_t* plength = ptr; // place holder for length field
  *ptr++;
  *ptr++;

  *ptr++ = classId; // huffman table class identifier
  for (int l = 0; l < 16; l++) {
    int count = 0;
    for (int i = 0; i < 12; i++) {
      if ((htable[i] >> 8) == l)
        count++;
      }
    *ptr++ = count; // number of huffman codes of length l+1
    }

  for (int l = 0; l < 16; l++) {
    for (int i = 0; i < 12; i++) {
      if ((htable[i] >> 8) == l) {
        *ptr++ = i; // HUFFVAL with huffman codes of length l+1
        length++;
        }
      }
    }

  *plength++ = length >> 8;// length field
  *plength = length & 0xFF;

  return length + 2;
  }
//}}}
//{{{
int cCamera::sosMarker (uint8_t* ptr) {

  *ptr++ = 0xFF;// startOfScan marker
  *ptr++ = 0xDA;

  int length = 12;
  *ptr++ = length >> 8;// length field
  *ptr++ = length & 0xFF;

  *ptr++ = 0x03;// number of image components in scan
  *ptr++ = 0x00;// scan component selector: Y
  *ptr++ = 0x00;// DC | AC huffman table selector: Y
  *ptr++ = 0x01;// scan component selector: Cb
  *ptr++ = 0x11;// DC | AC huffman table selector: Cb
  *ptr++ = 0x02;// scan component selector: Cr
  *ptr++ = 0x11;// DC | AC huffman table selector: Cr

  *ptr++ = 0x00;// Ss: start of predictor selector
  *ptr++ = 0x3F;// Se: end of spectral selector
  *ptr++ = 0x00;// Ah | Al: successive approximation bit position

  return length+2;
  }
//}}}

//line2 ("cam AE 6500");
//writeReg111 (0xC6, 0xA102); writeReg111 (0xC8, 0x21);
//writeReg111 (0xC6, 0xA102); writeReg111 (0xC8, 0);
