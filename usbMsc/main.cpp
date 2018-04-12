// main.cpp - msc class usb
//{{{  includes
#include "../common/system.h"
#include "cLcd.h"
#include "../common/cTouch.h"
#include "../common/cPs2.h"

#include "usbd_msc.h"
#include "../FatFs/ff.h"

#include "cLcd.h"
#include "stm32746g_discovery_sd.h"
#include "cCamera.h"

#include "jpeglib.h"
#include "jpegHeader.h"
//}}}
const char* kVersion = "Camera 12/4/18";

int focus = 0;
cCamera camera;

//{{{
class cApp : public cTouch {
public:
  cApp (int x, int y) : cTouch (x,y) {
    mBufferArray[0] = (uint8_t*)malloc (800 * 3);
    }
  cLcd* getLcd() { return mLcd; }
  cPs2* getPs2() { return mPs2; }

  void run();
  void onPs2Irq() { mPs2->onIrq(); }

  //{{{
  int getCountFiles (char* path) {
    mFiles = 0;
    countFiles (path);
    return mFiles;
    }
  //}}}

protected:
  virtual void onProx (int x, int y, int z);
  virtual void onPress (int x, int y);
  virtual void onMove (int x, int y, int z);
  virtual void onScroll (int x, int y, int z);
  virtual void onRelease (int x, int y);
  virtual void onKey (uint8_t ch, bool release);

private:
  void readDirectory (char* path);
  void countFiles (char* path);
  void reportFree();
  void reportLabel();
  void loadFile();
  void jpegDecode (uint8_t* jpegBuf, int jpegLen, uint16_t* rgb565buf);
  void jpegDecodeBody (uint8_t* jpegBuf, int jpegLen, uint16_t* rgb565buf, int scale, int width, int height);

  cLcd* mLcd = nullptr;
  cCamera* mCamera = nullptr;
  cPs2* mPs2 = nullptr;

  int mFiles = 0;
  DWORD mVsn = 0;
  char mLabel[40];
  uint8_t* mBufferArray[1];
  };
//}}}
cApp* gApp;
extern "C" { void EXTI9_5_IRQHandler() { gApp->onPs2Irq(); } }

// public
//{{{
void cApp::run() {

  mLcd = new cLcd (16);
  mLcd->init();
  //{{{  removed
  //mPs2 = new cPs2 (mLcd);
  //mPs2->initKeyboard();
  //}}}

  //mscInit (mLcd);
  //mscStart();
  //loadFile();

  mCamera = new cCamera();
  mCamera->init();
  mCamera->start (true, (uint8_t*)0xc0200000);

  int scale = (mCamera->getWidth() / mLcd->getWidth()) + 1;
  int lastCount = 0;
  bool lastButton = false;
  while (true) {
    pollTouch();
    //{{{  removed
    //while (mPs2->hasChar()) {
    //  auto ch = mPs2->getChar();
    //  onKey (ch & 0xFF, ch & 0x100);
    //  }
    //mLcd->startBgnd (kVersion, mscGetSectors());
    //}}}
    //mLcd->startBgnd ((uint16_t*)0xc0100000);
    if (mCamera->getCaptureMode()) {
      int jpegLen;
      auto jpegBuf = mCamera->getJpegFrame (jpegLen);
      if (jpegBuf) {
        if (jpegBuf + jpegLen > (uint8_t*)0xc0600000)
          memcpy ((void*)0xc0600000, (void*)0xc0200000, jpegBuf + jpegLen - (uint8_t*)0xc0600000);
        jpegDecodeBody (jpegBuf, jpegLen, (uint16_t*)0xc0100000, scale, mCamera->getWidth(), mCamera->getHeight());
        }
      mLcd->startBgnd ((uint16_t*)0xc0100000, mCamera->getWidth()/scale, mCamera->getHeight()/scale, BSP_PB_GetState (BUTTON_KEY));
      }
    else
      mLcd->startBgnd ((uint16_t*)0xc0200000, mCamera->getWidth(), mCamera->getHeight(), BSP_PB_GetState (BUTTON_KEY));

    mLcd->drawTitle (kVersion);
    mLcd->drawInfo (24, mCamera->getString());

    mLcd->drawDebug();
    mLcd->present();

    //bool button = BSP_PB_GetState (BUTTON_KEY);
    //if (!button && (button != lastButton))
    // mCamera->start (!mCamera->getCaptureMode(), (uint8_t*)0xc0200000);
    //lastButton = button;
    }
  }
//}}}

// protected
//{{{
void cApp::onProx (int x, int y, int z) {

  if (x || y) {
    //uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 0,(uint8_t)x,(uint8_t)y,0 };
    // hidSendReport (&gUsbDevice, HID_Buffer);
    mLcd->debug (LCD_COLOR_MAGENTA, "onProx %d %d %d", x, y, z);
    }
  }
//}}}
//{{{
void cApp::onPress (int x, int y) {

  //uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 1,0,0,0 };
  //hidSendReport (&gUsbDevice, HID_Buffer);
  mLcd->debug (LCD_COLOR_GREEN, "onPress %d %d", x, y);
  }
//}}}
//{{{
void cApp::onMove (int x, int y, int z) {

  if (x || y) {
    //uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 1,(uint8_t)x,(uint8_t)y,0 };
    //hidSendReport (&gUsbDevice, HID_Buffer);
    focus += x;
    if (focus < 0)
      focus = 0;
    else if (focus > 254)
      focus = 254;
    camera.setFocus (focus);
    mLcd->debug (LCD_COLOR_GREEN, "onMove %d %d %d %d", x, y, z, focus);
    }
  }
//}}}
//{{{
void cApp::onScroll (int x, int y, int z) {
  mLcd->incScrollValue (y);
  }
//}}}
//{{{
void cApp::onRelease (int x, int y) {

  //uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 0,0,0,0 };
  //hidSendReport (&gUsbDevice, HID_Buffer);
  mLcd->debug (LCD_COLOR_GREEN, "onRelease %d %d", x, y);
  }
//}}}
//{{{
void cApp::onKey (uint8_t ch, bool release) {

  //mLcd->debug (LCD_COLOR_GREEN, "onKey %x %s", ch, release ? "release" : "press");
  if (ch == 0x51) // down arrow
    mLcd->incScrollIndex (-1);
  else if (ch == 0x52) // up arrow
    mLcd->incScrollIndex (1);
  else if (ch == 0x4e) // pagedown
    mLcd->incScrollIndex (-16);
  else if (ch == 0x4b) // pageup
    mLcd->incScrollIndex (16);
  }
//}}}

// private
//{{{
void cApp::readDirectory (char* path) {

  DIR dir;
  auto result = f_opendir (&dir, path);
  if (result == FR_OK) {
    int i;
    for (i = 0; path[i]; i++);
    path[i++] = '/';

    while (true) {
      FILINFO fno;
      auto result = f_readdir (&dir, &fno);
      if (result != FR_OK || !fno.fname[0])
        break;
      if (fno.fname[0] == '.')
        continue;

      int j = 0;
      do {
        path[i+j] = fno.fname[j];
        } while (fno.fname[j++]);

      if (fno.fattrib & AM_DIR) {
        mLcd->debug (LCD_COLOR_GREEN, "%s", path);
        readDirectory (path);
        }
      else
        mLcd->debug (LCD_COLOR_WHITE, "%s", path);
      }

    path[--i] = '\0';
    f_closedir (&dir);
    }
  }
//}}}
//{{{
void cApp::countFiles (char* path) {

  DIR dir;
  auto result = f_opendir (&dir, path);
  if (result == FR_OK) {
    int i;
    for (i = 0; path[i]; i++);
    path[i++] = '/';

    while (true) {
      FILINFO fno;
      auto result = f_readdir (&dir, &fno);
      if (result != FR_OK || !fno.fname[0])
        break;
      if (fno.fname[0] == '.')
        continue;

      int j = 0;
      do {
        path[i+j] = fno.fname[j];
        } while (fno.fname[j++]);

      if (fno.fattrib & AM_DIR)
        countFiles (path);
      else
        mFiles++;
      }

    path[--i] = '\0';
    f_closedir (&dir);
    }
  }
//}}}
//{{{
void cApp::reportFree() {

  DWORD freeClusters;
  FATFS* fatFs;
  if (f_getfree ("0:", &freeClusters, &fatFs) != FR_OK)
    mLcd->debug (LCD_COLOR_WHITE, "f_getfree failed");
  else {
    int freeSectors = freeClusters * fatFs->csize;
    int totalSectors = (fatFs->n_fatent - 2) * fatFs->csize;
    mLcd->debug (LCD_COLOR_WHITE, "%d free of %d total", freeSectors/2, totalSectors/2);
    }
  }
//}}}

//{{{
void cApp::loadFile() {

  auto fatFs = (FATFS*)malloc (sizeof (FATFS));
  if (!f_mount (fatFs, "", 0)) {
    f_getlabel ("", mLabel, &mVsn);
    mLcd->debug (LCD_COLOR_WHITE, "Label <%s> ", mLabel);

    //char pathName[256] = "/";
    //readDirectory (pathName);
    FILINFO filInfo;
    if (!f_stat ("image.jpg", &filInfo))
      mLcd->debug (LCD_COLOR_WHITE, "%d %u/%02u/%02u %02u:%02u %c%c%c%c%c",
                   (int)(filInfo.fsize),
                   (filInfo.fdate >> 9) + 1980, filInfo.fdate >> 5 & 15, filInfo.fdate & 31,
                    filInfo.ftime >> 11, filInfo.ftime >> 5 & 63,
                   (filInfo.fattrib & AM_DIR) ? 'D' : '-',
                   (filInfo.fattrib & AM_RDO) ? 'R' : '-',
                   (filInfo.fattrib & AM_HID) ? 'H' : '-',
                   (filInfo.fattrib & AM_SYS) ? 'S' : '-',
                   (filInfo.fattrib & AM_ARC) ? 'A' : '-');

    FIL file;
    if (!f_open (&file, "image.jpg", FA_READ)) {
      mLcd->debug (LCD_COLOR_WHITE, "image.jpg - found");
      UINT bytesRead;
      f_read (&file, (void*)0xc0200000, (UINT)filInfo.fsize, &bytesRead);
      mLcd->debug (LCD_COLOR_WHITE, "image.jpg bytes read %d", bytesRead);
      f_close (&file);
      jpegDecode ((uint8_t*)0xc0200000, bytesRead, (uint16_t*)0xc0100000);
      }
    else
      mLcd->debug (LCD_COLOR_RED, "image.jpg - not found");
    }
  else
    mLcd->debug (LCD_COLOR_RED, "not mounted");
  }
//}}}
//{{{
void cApp::jpegDecode (uint8_t* jpegBuf, int jpegLen, uint16_t* rgb565buf) {

  struct jpeg_error_mgr jerr;
  struct jpeg_decompress_struct cinfo;

  //uint8_t jpegHeader[1000];
  //int jpegHeaderLen = setJpegHeader (jpegHeader, width, height, 0, 6);
  cinfo.err = jpeg_std_error (&jerr);
  jpeg_create_decompress (&cinfo);
  jpeg_mem_src (&cinfo, jpegBuf, jpegLen);
  jpeg_read_header (&cinfo, TRUE);

  mLcd->debug (LCD_COLOR_WHITE, "jpeg image:%dx%d", cinfo.image_width, cinfo.image_height);

  cinfo.dct_method = JDCT_FLOAT;
  cinfo.out_color_space = JCS_RGB;
  cinfo.scale_num = 1;
  cinfo.scale_denom = 4;

  //jpeg_mem_src (&mCinfo, buff, buffLen);
  jpeg_start_decompress (&cinfo);

  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines (&cinfo, mBufferArray, 1);

    auto src = mBufferArray[0];
    auto dst = rgb565buf + cinfo.output_scanline * 480;
    for (int x = 0; x < cinfo.output_width; x++) {
      uint8_t r = (*src++) & 0xF8;
      uint8_t g = (*src++) & 0xFC;
      uint8_t b = (*src++) & 0xF8;
      *dst++ = (r << 8) | (g << 3) | (b >> 3);
      }
    }
  mLcd->debug (LCD_COLOR_WHITE, "jpeg out:%dx%d", cinfo.output_width, cinfo.output_height);

  jpeg_finish_decompress (&cinfo);
  jpeg_destroy_decompress (&cinfo);
  }
//}}}
//{{{
void cApp::jpegDecodeBody (uint8_t* jpegBuf, int jpegLen, uint16_t* rgb565buf, int scale, int width, int height) {

  struct jpeg_error_mgr jerr;
  struct jpeg_decompress_struct cinfo;

  uint8_t jpegHeader[1000];
  int jpegHeaderLen = setJpegHeader (jpegHeader, width, height, 0, 6);

  cinfo.err = jpeg_std_error (&jerr);
  jpeg_create_decompress (&cinfo);
  jpeg_mem_src (&cinfo, jpegHeader, jpegHeaderLen);
  jpeg_read_header (&cinfo, TRUE);

  cinfo.dct_method = JDCT_FLOAT;
  cinfo.out_color_space = JCS_RGB;
  cinfo.scale_num = 1;
  cinfo.scale_denom = scale;

  jpeg_mem_src (&cinfo, jpegBuf, jpegLen);
  jpeg_start_decompress (&cinfo);

  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines (&cinfo, mBufferArray, 1);

    auto src = mBufferArray[0];
    auto dst = rgb565buf + cinfo.output_scanline * cinfo.output_width;
    for (int x = 0; x < cinfo.output_width; x++) {
      uint8_t r = (*src++) & 0xF8;
      uint8_t g = (*src++) & 0xFC;
      uint8_t b = (*src++) & 0xF8;
      *dst++ = (r << 8) | (g << 3) | (b >> 3);
      }
    }

  jpeg_finish_decompress (&cinfo);
  jpeg_destroy_decompress (&cinfo);
  }
//}}}

//{{{
int main() {

  SCB_EnableICache();
  SCB_EnableDCache();
  HAL_Init();
  //{{{  config system clock
  // System Clock source            = PLL (HSE)
  // SYSCLK(Hz)                     = 216000000
  // HCLK(Hz)                       = 216000000
  // AHB Prescaler                  = 1
  // APB1 Prescaler                 = 4
  // APB2 Prescaler                 = 2
  // HSE Frequency(Hz)              = 25000000
  // PLL_M                          = 25
  // PLL_N                          = 432
  // PLL_P                          = 2
  // PLLSAI_N                       = 384
  // PLLSAI_P                       = 8
  // VDD(V)                         = 3.3
  // Main regulator output voltage  = Scale1 mode
  // Flash Latency(WS)              = 7

  // Enable HSE Oscillator and activate PLL with HSE as source
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK)
    while (true) {}

  // Activate the OverDrive to reach the 216 Mhz Frequency
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    while (true) {}

  // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    while (true) {}
  //}}}
  BSP_PB_Init (BUTTON_KEY, BUTTON_MODE_GPIO);

  gApp = new cApp (cLcd::getWidth(), cLcd::getHeight());
  gApp->run();
  }
//}}}
