// main.cpp - msc class usb
//{{{  includes
#include "../common/system.h"
#include "cLcd.h"
#include "../common/cTouch.h"
#include "../common/cPs2.h"

#include "usbd_msc.h"
#include "../FatFs/ff.h"

#include "stm32746g_discovery_sd.h"
#include "cCamera.h"

#include "jpeglib.h"

#include "cmsis_os.h"
#include "lwip/opt.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/arch.h"
#include "lwip/api.h"

#include "ethernetif.h"
//}}}
#define USE_CAMERA

const char* kVersion = "WebCam 16/4/18";

uint16_t* kRgb565Buffer = (uint16_t*)0xc0100000;
uint8_t*  kJpegBuffer   =  (uint8_t*)0xc0200000;
uint8_t*  kJpegBuffer1  =  (uint8_t*)0xc0700000;

//{{{
class cApp : public cTouch {
public:
  //{{{
  cApp (int x, int y) : cTouch (x,y) {
    mBufferArray[0] = (uint8_t*)malloc (1600 * 3);
    mCinfo.err = jpeg_std_error (&jerr);
    jpeg_create_decompress (&mCinfo);
    }
  //}}}
  //{{{
  ~cApp() {
    jpeg_destroy_decompress (&mCinfo);
    }
  //}}}

  cPs2* getPs2() { return mPs2; }

  void init();
  void run();
  void onPs2Irq() { mPs2->onIrq(); }

  //{{{
  int getCountFiles (char* path) {
    mFiles = 0;
    countFiles (path);
    return mFiles;
    }
  //}}}

  cCamera* mCamera = nullptr;
  uint8_t mJpegHeader[1000];
  int mJpegHeaderLen = 0;

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

  int loadFile (uint8_t* jpegBuffer);
  void saveFile (uint8_t* jpegHeadBuf, int jpegHeadLen, uint8_t* jpegBuf, int jpegLen, int num);
  void jpegDecode (uint8_t* jpegBuf, int jpegLen, uint16_t* rgb565buf, int scale);

  int jfifApp0Marker (uint8_t* ptr);
  int sofMarker (uint8_t* ptr, int width, int height);
  int quantTableMarker (uint8_t* ptr, int qscale);
  int huffTableMarkerDC (uint8_t* ptr, const uint16_t* htable, int classId);
  int huffTableMarkerAC (uint8_t* ptr, const uint16_t* htable, int classId);
  int sosMarker (uint8_t* ptr);
  void setJpegHeader (int width, int height, int qscale);

  cLcd* mLcd = nullptr;
  cPs2* mPs2 = nullptr;

  int mFiles = 0;
  DWORD mVsn = 0;
  char mLabel[40];
  uint8_t* mBufferArray[1];

  struct jpeg_error_mgr jerr;
  struct jpeg_decompress_struct mCinfo;
  };
//}}}

cApp* gApp;
extern "C" { void EXTI9_5_IRQHandler() { gApp->onPs2Irq(); } }

//{{{
const char kHtmlResponse[] =
  "HTTP/1.0 200 OK\r\n"
  "Server: lwIP/1.3.1\r\n"
  "Content-type: text/html\r\n\r\n"; // header + body follows
//}}}
//{{{
const char k404Response[] =
  "HTTP/1.0 404 File not found\r\n"
  "Server: lwIP/1.3.1\r\n"
  "Content-type: text/html\r\n\r\n"

  "<html>"
    "<body>"
      "<h1>404 not found heading</h1>"
      "<p>404 not found paragraph</p>"
    "</body>"
  "</html>\r\n";
//}}}
//{{{
const char kJpgResponse[] =
  "HTTP/1.0 200 OK\r\n"
  "Server: lwIP/1.3.1\r\n"
  "Content-type: image/jpeg\r\n\r\n"; // header + body follows
//}}}
//{{{
const char kHtml[] =
  "<!DOCTYPE html>"
  "<html lang=en-GB>"
    "<body>"
      "<h1>Colin's webcam</h1>"
      "<p style=color:green title=tooltip>800x600</p>"
      "<img src=cam.jpg alt=missing width=800 height=600>"
      "<button>Click me</button>"
      "<a href=img1.jpg>This is a link</a>"
      "<svg width=100 height=100>"
        "<circle cx=50 cy=50 r=40 stroke=green stroke-width=8 fill=yellow>"
      "</svg>"
    "</body>"
  "</html>\r\n";
//}}}

// public
//{{{
void cApp::init() {

  mLcd = new cLcd (16);
  mLcd->init();

  mLcd->start();
  mLcd->drawTitle (kVersion);
  mLcd->drawDebug();
  mLcd->present();

  //{{{  removed
  //mPs2 = new cPs2 (mLcd);
  //mPs2->initKeyboard();
  //}}}
  //mscInit (mLcd);
  //mscStart();
  }
//}}}
//{{{
void cApp::run() {

  uint8_t* jpegBuf = kJpegBuffer;
  int jpegLen = 0;
  //int jpegLen = loadFile (kJpegBuffer);

  //{{{
  #ifdef USE_CAMERA
    mCamera = new cCamera();
    mCamera->init();
    mCamera->start (false, kJpegBuffer);

    setJpegHeader (mCamera->getWidth(), mCamera->getHeight(), 6);
    mCinfo.scale_num = 1;
    mCinfo.scale_denom = (mCamera->getWidth() / mLcd->getWidth()) + 1;
    mCinfo.dct_method = JDCT_FLOAT;
    mCinfo.out_color_space = JCS_RGB;
  #endif
  //}}}

  int count = 0;
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

    if (!mCamera)
      mLcd->start();
    else if (mCamera->getJpegMode()) {
      if (!BSP_PB_GetState (BUTTON_KEY))
        mLcd->start();
      else {
        jpegBuf = mCamera->getFrameBuf (jpegLen);
        if (jpegBuf) {
          //{{{  jpeg decode buffer
          if (jpegBuf + jpegLen >  mCamera->getBufEnd())
            memcpy ((void*)0xc0600000, kJpegBuffer, jpegBuf + jpegLen -  mCamera->getBufEnd());

          // decode jpeg header
          jpeg_mem_src (&mCinfo, mJpegHeader, mJpegHeaderLen);
          jpeg_read_header (&mCinfo, TRUE);

          // decode jpeg body
          jpeg_mem_src (&mCinfo, jpegBuf, jpegLen);
          jpeg_start_decompress (&mCinfo);

          while (mCinfo.output_scanline < mCinfo.output_height) {
            jpeg_read_scanlines (&mCinfo, mBufferArray, 1);
            mLcd->convertRgb888toRgbB565 (mBufferArray[0], kRgb565Buffer + mCinfo.output_scanline * mCinfo.output_width, mCinfo.output_width);
            }
          jpeg_finish_decompress (&mCinfo);

          mLcd->startBgnd (kRgb565Buffer, mCinfo.output_width, mCinfo.output_height, true);
          }
          //}}}
        else
          mLcd->start();
        }
      }
    else {
      int rgb565Len = 0;
      auto rgb565Buf = mCamera->getFrameBuf (rgb565Len);
      if (rgb565Buf + rgb565Len > mCamera->getBufEnd())
        memcpy ((void*)0xc0600000, rgb565Buf, rgb565Buf + rgb565Len -  mCamera->getBufEnd());
      if (rgb565Buf)
        mLcd->startBgnd ((uint16_t*)rgb565Buf, mCamera->getWidth(), mCamera->getHeight(), BSP_PB_GetState (BUTTON_KEY));
      else
        mLcd->start();
      }

    mLcd->drawTitle (kVersion);
    if (mCamera) {
      // drawInfo
      char str[40] = {0};
      sprintf (str, "%dfps %d%c", mCamera->getFps(), mCamera->getFrameBufLen(), mCamera->getJpegMode() ? 'j':'p');
      mLcd->drawInfo (24, str);
      }
    mLcd->drawDebug();
    mLcd->present();

    bool button = BSP_PB_GetState (BUTTON_KEY);
    if (!button && (button != lastButton)) {
      if (mCamera) {
        auto jpegMode = !mCamera->getJpegMode();
        mCamera->start (jpegMode, jpegMode ? kJpegBuffer : (uint8_t*)kRgb565Buffer);
        }
      }
    lastButton = button;
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
    int focus = mCamera->getFocus() + x;
    if (focus < 0)
      focus = 0;
    else if (focus > 254)
      focus = 254;
    mCamera->setFocus (focus);
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
int cApp::loadFile (uint8_t* jpegBuffer) {

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
      f_read (&file, (void*)jpegBuffer, (UINT)filInfo.fsize, &bytesRead);
      mLcd->debug (LCD_COLOR_WHITE, "image.jpg bytes read %d", bytesRead);
      f_close (&file);
      if (bytesRead > 0)
        jpegDecode (kJpegBuffer, bytesRead, kRgb565Buffer, 4);
      return bytesRead;
      }
    else
      mLcd->debug (LCD_COLOR_RED, "image.jpg - not found");
    }
  else
    mLcd->debug (LCD_COLOR_RED, "not mounted");

  return 0;
  }
//}}}
//{{{
void cApp::jpegDecode (uint8_t* jpegBuf, int jpegLen, uint16_t* rgb565buf, int scale) {

  jpeg_mem_src (&mCinfo, jpegBuf, jpegLen);
  jpeg_read_header (&mCinfo, TRUE);

  mLcd->debug (LCD_COLOR_WHITE, "jpegDecode in:%dx%d", mCinfo.image_width, mCinfo.image_height);

  mCinfo.dct_method = JDCT_FLOAT;
  mCinfo.out_color_space = JCS_RGB;
  mCinfo.scale_num = 1;
  mCinfo.scale_denom = scale;

  jpeg_start_decompress (&mCinfo);
  while (mCinfo.output_scanline < mCinfo.output_height) {
    jpeg_read_scanlines (&mCinfo, mBufferArray, 1);
    mLcd->convertRgb888toRgbB565 (mBufferArray[0], rgb565buf + (mCinfo.output_scanline * mCinfo.output_width), mCinfo.output_width);
    }
  jpeg_finish_decompress (&mCinfo);

  mLcd->debug (LCD_COLOR_WHITE, "jpegDecode out:%dx%d %d", mCinfo.output_width, mCinfo.output_height, scale);
  }
//}}}

//{{{
void cApp::saveFile (uint8_t* jpegHeadBuf, int jpegHeadLen, uint8_t* jpegBuf, int jpegLen, int num) {

  char saveName[40];
  sprintf (saveName, "saveImage%03d.jpg", num);
  mLcd->debug (LCD_COLOR_GREEN, saveName);

  FIL file;
  if (!f_open (&file, saveName, FA_WRITE | FA_CREATE_ALWAYS)) {
    UINT bytesWritten;
    if (jpegHeadBuf && jpegHeadLen)
      f_write (&file, jpegHeadBuf, jpegHeadLen, &bytesWritten);
    f_write (&file, jpegBuf, jpegLen, &bytesWritten);
    f_close (&file);
    }
  else
    mLcd->debug (LCD_COLOR_RED, saveName);
  }
//}}}

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
//{{{
int cApp::jfifApp0Marker (uint8_t* ptr) {

  *ptr++ = 0xFF; // APP0 marker
  *ptr++ = 0xE0;

  int length = 16;
  *ptr++ = length >> 8;// length field
  *ptr++ = length & 0xFF;

  *ptr++ = 0x4A; // JFIF identifier
  *ptr++ = 0x46;
  *ptr++ = 0x49;
  *ptr++ = 0x46;
  *ptr++ = 0x00;

  *ptr++ = 0x01; // version
  *ptr++ = 0x02;

  *ptr++ = 0x00; // units
  *ptr++ = 0x00; // X density
  *ptr++ = 0x01;
  *ptr++ = 0x00; // Y density
  *ptr++ = 0x01;

  *ptr++ = 0x00; // X thumbnail
  *ptr++ = 0x00; // Y thumbnail

  return length+2;
  }
//}}}
//{{{
int cApp::sofMarker (uint8_t* ptr, int width, int height) {

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
int cApp::quantTableMarker (uint8_t* ptr, int qscale) {

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
int cApp::huffTableMarkerAC (uint8_t* ptr, const uint16_t* htable, int classId) {

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
int cApp::huffTableMarkerDC (uint8_t* ptr, const uint16_t* htable, int classId) {

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
int cApp::sosMarker (uint8_t* ptr) {

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
//{{{
void cApp::setJpegHeader (int width, int height, int qscale) {

  auto ptr = mJpegHeader;

  *ptr++ = 0xFF; // SOI marker
  *ptr++ = 0xD8;
  mJpegHeaderLen = 2;

  mJpegHeaderLen += jfifApp0Marker (ptr);
  mJpegHeaderLen += quantTableMarker (mJpegHeader + mJpegHeaderLen, qscale);
  mJpegHeaderLen += sofMarker (mJpegHeader + mJpegHeaderLen, width, height);
  mJpegHeaderLen += huffTableMarkerAC (mJpegHeader + mJpegHeaderLen, &kJpegStdHuffmanTbl[0], 0x10);
  mJpegHeaderLen += huffTableMarkerAC (mJpegHeader + mJpegHeaderLen, &kJpegStdHuffmanTbl[176], 0x11);
  mJpegHeaderLen += huffTableMarkerDC (mJpegHeader + mJpegHeaderLen, &kJpegStdHuffmanTbl[352], 0x00);
  mJpegHeaderLen += huffTableMarkerDC (mJpegHeader + mJpegHeaderLen, &kJpegStdHuffmanTbl[368], 0x01);
  mJpegHeaderLen += sosMarker (mJpegHeader + mJpegHeaderLen);
  }
//}}}

//{{{
void appThread (void* arg) {

  gApp->run();
  //while (true)
  //  osThreadTerminate(NULL);
  }
//}}}
//{{{
void serverThread (void* arg) {
// minimal http server

  // create a new TCP connection handle
  struct netconn* connection = netconn_new (NETCONN_TCP);
  if (connection != NULL) {
    // bind to port 80 (HTTP) with default IP address
    if (netconn_bind (connection, NULL, 80) == ERR_OK) {
      netconn_listen (connection);
      while (true) {
        struct netconn* request;
        if (netconn_accept (connection, &request) == ERR_OK) {
          struct netbuf* requestNetBuf;
          if (netconn_recv (request, &requestNetBuf) == ERR_OK) {
            if (netconn_err (request) == ERR_OK) {
              char* buf;
              u16_t bufLen;
              netbuf_data (requestNetBuf, (void**)&buf, &bufLen);

              //{{{  debug request buf
              char str[40];
              int src = 0;
              int dst = 0;

              // copy till return
              while ((src < bufLen) && (buf[src] != 0x0d)) {
                if (buf[src] == 0x0a) // skip lineFeed
                  src++;
                else if (dst < 39)
                  str[dst++] = buf[src++];
                }

              // terminate str
              str[dst] = 0;

              cLcd::mLcd->debug (LCD_COLOR_YELLOW, str);
              //}}}

              // simple HTTP GET command parser
              bool ok = false;
              if ((bufLen >= 5) && !strncmp (buf, "GET /", 5)) {
                if (!strncmp (buf, "GET / ", 6)) {
                  //{{{  html
                  netconn_write (request, kHtmlResponse, sizeof(kHtmlResponse)-1, NETCONN_NOCOPY);
                  netconn_write (request, kHtml, sizeof(kHtml)-1, NETCONN_NOCOPY);
                  ok = true;
                  }
                  //}}}
                else if (!strncmp (buf, "GET /cam.jpg", 12)) {
                  //{{{  cam.jpg
                  if (gApp->mCamera && gApp->mCamera->getJpegMode()) {
                    int jpegLen;
                    auto jpegBuf = gApp->mCamera->getFrameBuf (jpegLen);
                    if (jpegBuf) {
                      if (jpegBuf + jpegLen <= (uint8_t*)0xc0600000)
                        memcpy (kJpegBuffer1, jpegBuf, jpegLen);
                      else {
                        // wrap around case
                        uint32_t firstChunkLen = (uint8_t*)0xc0600000 - jpegBuf;
                        uint32_t secondChunkLen = jpegLen - firstChunkLen;
                        memcpy (kJpegBuffer1, jpegBuf, firstChunkLen);
                        memcpy (kJpegBuffer1 + firstChunkLen, kJpegBuffer, secondChunkLen);
                        }

                      netconn_write (request, kJpgResponse, sizeof(kJpgResponse)-1, NETCONN_NOCOPY);
                      netconn_write (request, gApp->mJpegHeader, gApp->mJpegHeaderLen, NETCONN_NOCOPY);
                      netconn_write (request, kJpegBuffer1, jpegLen, NETCONN_NOCOPY);
                      ok = true;
                      }
                    }
                  }
                  //}}}
                if (!ok)
                  netconn_write (request, k404Response, sizeof(k404Response)-1, NETCONN_NOCOPY);
                }
              }
            }

          netconn_close (request);
          netbuf_delete (requestNetBuf);
          netconn_delete (request);
          }
        }
      }
    }
  }
//}}}
//{{{
void dhcpThread (void* arg) {

  #define DHCP_OFF               0
  #define DHCP_START             1
  #define DHCP_WAIT_ADDRESS      2
  #define DHCP_ADDRESS_ASSIGNED  3
  #define DHCP_TIMEOUT           4
  #define DHCP_LINK_DOWN         5

  auto netif = (struct netif*)arg;

  uint8_t dhcpState = netif_is_up (netif) ? DHCP_START : DHCP_LINK_DOWN;
  while (true) {
    switch (dhcpState) {
      case DHCP_START:
        ip_addr_set_zero_ip4 (&netif->ip_addr);
        ip_addr_set_zero_ip4 (&netif->netmask);
        ip_addr_set_zero_ip4 (&netif->gw);
        dhcp_start (netif);
        dhcpState = DHCP_WAIT_ADDRESS;
        break;

      case DHCP_WAIT_ADDRESS:
        if (dhcp_supplied_address (netif)) {
          dhcpState = DHCP_ADDRESS_ASSIGNED;
          cLcd::mLcd->debug (LCD_COLOR_GREEN, "DHCP %s", ip4addr_ntoa ((const ip4_addr_t*)&netif->ip_addr));
          }
        else {
          auto dhcp = (struct dhcp*)netif_get_client_data (netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
          if (dhcp->tries > 4) {
            dhcpState = DHCP_TIMEOUT;
            dhcp_stop (netif);
            cLcd::mLcd->debug (LCD_COLOR_RED, "DHCP timeout");
            }
          }
        break;

      case DHCP_LINK_DOWN:
        cLcd::mLcd->debug (LCD_COLOR_RED, "DHCP link down");
        dhcp_stop (netif);
        dhcpState = DHCP_OFF;
        break;

      default:
        break;
      }

    osDelay (250);
    }
  }
//}}}
//{{{
void netThread (void* arg) {

  tcpip_init (NULL, NULL);

  struct netif netIf;
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

  ip_addr_set_zero_ip4 (&ipaddr);
  ip_addr_set_zero_ip4 (&netmask);
  ip_addr_set_zero_ip4 (&gw);
  netif_add (&netIf, &ipaddr, &netmask, &gw, NULL, &ethernetIfInit, &tcpip_input);

  netif_set_default (&netIf);
  if (netif_is_link_up (&netIf))
    netif_set_up (&netIf);
  else
    netif_set_down (&netIf);

  sys_thread_new ("dhcp", dhcpThread, &netIf, configMINIMAL_STACK_SIZE * 2, osPriorityBelowNormal);
  sys_thread_new ("server", serverThread, NULL, DEFAULT_THREAD_STACKSIZE, osPriorityAboveNormal);

  while (true)
    osThreadTerminate(NULL);
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
  gApp->init();

  sys_thread_new ("app", appThread, NULL, configMINIMAL_STACK_SIZE * 5, osPriorityNormal);
  sys_thread_new ("net", netThread, NULL, configMINIMAL_STACK_SIZE * 5, osPriorityNormal);
  osKernelStart();

  while (true);
  }
//}}}
