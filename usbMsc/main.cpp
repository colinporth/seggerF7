// main.cpp - msc class usb
//{{{  includes
#include <deque>

#include "../common/system.h"
#include "cLcd.h"
#include "../common/cTouch.h"
#include "../common/cPs2.h"

#include "usbd_msc.h"
#include "../FatFs/ff.h"
#include "../FatFs/diskio.h"

#include "stm32746g_discovery_sd.h"
#include "cCamera.h"

#include "jpeglib.h"

#include "cmsis_os.h"
#include "cpuUsage.h"
#include "lwip/opt.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "sntp.h"

#include "ethernetif.h"
//}}}
//{{{  const
const bool kFreeRtos = true;

uint8_t*  kCamBuf    =  (uint8_t*)0xc0080000;
uint8_t*  kCamBufEnd =  (uint8_t*)0xc0700000;
uint16_t* kRgb565Buf = (uint16_t*)kCamBufEnd;

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
const char kJpegResponseHeader[] =
  "HTTP/1.0 200 OK\r\n"
  "Server: lwIP/1.3.1\r\n"
  "Content-type: image/jpeg\r\n\r\n"; // header + body follows
//}}}
//{{{
const char kBmpResponseHeader[] =
  "HTTP/1.0 200 OK\r\n"
  "Server: lwIP/1.3.1\r\n"
  "Content-type: image/bmp\r\n\r\n"; // header + body follows
//}}}
//{{{
const char kHtmlResponseHeader[] =
  "HTTP/1.0 200 OK\r\n"
  "Server: lwIP/1.3.1\r\n"
  "Content-type: text/html\r\n\r\n"; // header + body follows
//}}}
//{{{
const char kHtmlBody[] =
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
//}}}
const char kVersion[] = "WebCam 23/4/18";
const bool kWriteJpg  = false;
const bool kWriteMjpg = true;

//{{{
class cApp : public cTouch {
public:
  //{{{
  class cBox {
  public:
    //{{{
    cBox (const char* name, float width, float height)
        : mName(name), mLayoutWidth(width), mLayoutHeight(height) {
      //mWindow->changed();
      }
    //}}}
    virtual ~cBox() {}

    // gets
    const char* getName() const { return mName; }

    bool getEnable() { return mEnable; }
    bool getPick() { return mPick; }
    bool getShow() { return mEnable && (mPick || mPin); }
    bool getTimedOn() { return mTimedOn; }

    cPoint getSize() { return mRect.getSize(); }
    float getWidth() { return mRect.getWidth(); }
    float getHeight() { return mRect.getHeight(); }
    int getWidthInt() { return mRect.getWidthInt(); }
    int getHeightInt() { return mRect.getHeightInt(); }
    cPoint getTL() { return mRect.getTL(); }
    cPoint getTL (float offset) { return mRect.getTL (offset); }
    cPoint getTR() { return mRect.getTR(); }
    cPoint getBL() { return mRect.getBL(); }
    cPoint getBR() { return mRect.getBR(); }
    cPoint getCentre() { return mRect.getCentre(); }
    float getCentreX() { return mRect.getCentreX(); }
    float getCentreY() { return mRect.getCentreY(); }

    //{{{
    cBox* setPos (cPoint pos) {
      mLayoutX = pos.x;
      mLayoutY = pos.y;
      layout();
      return this;
      }
    //}}}
    //{{{
    cBox* setPos (float x, float y) {
      mLayoutX = x;
      mLayoutY = y;
      layout();
      return this;
      }
    //}}}
    cBox* setEnable (bool enable) { mEnable = enable; return this;  }
    cBox* setUnPick() { mPick = false;  return this; }
    cBox* setPin (bool pin) { mPin = pin; return this; }
    cBox* togglePin() { mPin = !mPin;  return this; }
    cBox* setTimedOn() { mTimedOn = true; return this;  }

    // overrides
    //{{{
    virtual void layout() {

      mRect.left = (mLayoutX < 0) ? cLcd::getWidth() + mLayoutX : mLayoutX;
      if (mLayoutWidth > 0)
        mRect.right = mRect.left + mLayoutWidth;
      else if (mLayoutWidth == 0)
        mRect.right = getSize().x - mLayoutX;
      else // mLayoutWidth < 0
        mRect.right = cLcd::getHeight() + mLayoutWidth + mLayoutX;

      mRect.top = (mLayoutY < 0) ? cLcd::getHeight() + mLayoutY : mLayoutY;
      if (mLayoutHeight > 0)
        mRect.bottom = mRect.top + mLayoutHeight;
      else if (mLayoutHeight == 0)
        mRect.bottom = cLcd::getHeight() - mLayoutY;
      else // mLayoutHeight < 0
        mRect.bottom = cLcd::getHeight() + mLayoutHeight + mLayoutY;
      }
    //}}}
    //{{{
    virtual bool pick (cPoint pos, bool& change) {

      bool lastPick = mPick;

      mPick = mRect.inside (pos);
      if (!change && (mPick != lastPick))
        change = true;

      return mPick;
      }
    //}}}
    virtual bool onProx (cPoint pos) { return false; }
    virtual bool onProxExit() { return false; }
    virtual bool onDown (cPoint pos)  { return false; }
    virtual bool onMove (cPoint pos, cPoint inc)  { return false; }
    virtual bool onUp (bool mouseMoved, cPoint pos) { return false; }
    virtual void onDraw (cLcd* lcd) = 0;

  protected:
    const char* mName;

    bool mEnable = true;
    bool mPick = false;
    bool mPin = true;
    bool mTimedOn = false;

    float mLayoutWidth;
    float mLayoutHeight;
    float mLayoutX = 0;
    float mLayoutY = 0;

    cRect mRect;
    };
  //}}}

  //{{{
  cApp (int x, int y) : cTouch (x,y) {

    mCinfo.err = jpeg_std_error (&jerr);
    jpeg_create_decompress (&mCinfo);

    mBufArray[0] = (uint8_t*)malloc (1600 * 3);
    }
  //}}}
  //{{{
  ~cApp() {
    jpeg_destroy_decompress (&mCinfo);
    }
  //}}}

  cPs2* getPs2() { return mPs2; }
  cCamera* getCam() { return mCam; }

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

  cBox* add (cBox* box, cPoint pos);
  cBox* add (cBox* box, float x, float y);
  cBox* add (cBox* box);
  cBox* addBelow (cBox* box);
  cBox* addFront (cBox* box);
  cBox* addFront (cBox* box, float x, float y);
  void removeBox (cBox* box);

protected:
  virtual void onProx (int16_t x, int16_t y, uint8_t z);
  virtual void onPress (int16_t x, int16_t y);
  virtual void onMove (int16_t x, int16_t y, uint8_t z);
  virtual void onScroll (int16_t x, int16_t y, uint8_t z);
  virtual void onRelease (int16_t x, int16_t y);
  virtual void onKey (uint8_t ch, bool release);

private:
  void readDirectory (char* path);
  void countFiles (char* path);
  void reportFree();
  void reportLabel();

  int loadFile (const char* fileName, uint8_t* buf, uint16_t* rgb565Buf);

  void saveNumFile (const char* name, uint32_t num, const char* ext, uint8_t* buf, int bufLen);
  void saveNumFile (const char* name, uint32_t num, const char* ext, uint8_t* header, int headerLen, uint8_t* frame, int frameLen);

  void createNumFile (const char* name, uint32_t num, uint8_t* header, int headerLen, uint8_t* frame, int frameLen);
  void appendFile (int num, uint8_t* header, int headerLen, uint8_t* frame, int frameLen);
  void closeFile();

  //{{{  vars
  cLcd* mLcd = nullptr;
  cPs2* mPs2 = nullptr;
  cCamera* mCam = nullptr;

  std::deque <cBox*> mBoxes;
  cBox* mProxBox = nullptr;
  cBox* mPressedBox = nullptr;
  bool mDown = false;
  bool mMoved = false;
  cPoint mDownPos;
  cPoint mLastPos;

  char mLabel[40];
  DWORD mVsn = 0;
  int mFiles = 0;

  struct jpeg_error_mgr jerr;
  struct jpeg_decompress_struct mCinfo;
  uint8_t* mBufArray[1];

  bool mValueChanged = false;
  bool mValue = false;

  bool mZoomChanged = false;
  bool mZoomValue = false;

  bool mDebugChanged = false;
  bool mDebugValue = true;
  //}}}
  };
//}}}
#include "../common/cToggleBox.h"

FATFS gFatFs;  // encourges allocation in lower DTCM SRAM
FIL   gFile;
cApp* gApp;

extern "C" { void EXTI9_5_IRQHandler() { gApp->onPs2Irq(); } }

// public
//{{{
void cApp::init() {

  mLcd = new cLcd (16);
  mLcd->init();

  add (new cToggleBox (60.f, 50.f, "jpeg", mValue, mValueChanged), 0.f, 272.f - 50.f);
  add (new cToggleBox (60.f, 50.f, "zoom", mZoomValue, mZoomChanged), 64.f, 272.f - 50.f);
  add (new cToggleBox (60.f, 50.f, "debug", mDebugValue, mDebugChanged), 128.f, 272.f - 50.f);

  // show title early
  mLcd->start();
  mLcd->drawInfo (LCD_COLOR_GREEN, 0, kVersion);
  mLcd->present();

  diskInit();

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

  uint32_t fileLen = 0;
  bool mounted = !f_mount (&gFatFs, "", 1);
  if (mounted) {
    //{{{  mounted, load splash piccy
    f_getlabel ("", mLabel, &mVsn);
    mLcd->debug (LCD_COLOR_WHITE, "sdCard ok - %s ", mLabel);

    fileLen = loadFile ("splash.jpg", kCamBuf, kRgb565Buf);

    mLcd->start (kRgb565Buf, mCinfo.output_width, mCinfo.output_height, true);
    mLcd->drawInfo (LCD_COLOR_WHITE, 0, kVersion);
    mLcd->drawDebug();
    mLcd->present();
    osDelay (1000);
    }
    //}}}
  else
    mLcd->debug (LCD_COLOR_GREEN, "sdCard not mounted");

  mCam = new cCamera();
  mCam->init (kCamBuf, kCamBufEnd);

  uint32_t fileNum = 1;
  uint32_t frameNum = 0;
  while (true) {
    pollTouch();
    //{{{  removed
    //while (mPs2->hasChar()) {
    //  auto ch = mPs2->getChar();
    //  onKey (ch & 0xFF, ch & 0x100);
    //  }
    //}}}

    uint32_t frameLen;
    bool jpeg;
    auto frame = mCam->getNextFrame (frameLen, jpeg);
    if (!frame) // no frame, clear
      mLcd->start();
    else if (jpeg) {
      uint32_t headerLen;
      if (mounted && kWriteJpg && (frameNum < 100)) {
        //{{{  save JFIF jpeg
        auto header = mCam->getHeader (true, 6, headerLen);
        saveNumFile ("save", frameNum++, "jpg", header, headerLen, frame, frameLen);
        mLcd->start();
        }
        //}}}
      else if (mounted && kWriteMjpg && !frameNum) {
        //{{{  save mjpeg first frame
        frameNum++;
        auto header = mCam->getHeader (true, 6, headerLen);
        createNumFile ("save", fileNum, header, headerLen, frame, frameLen);
        mLcd->start();
        }
        //}}}
      else if (mounted && kWriteMjpg && (frameNum < 500)) {
        //{{{  add mjpeg frame
        auto header = mCam->getHeader (false, 6, headerLen);
        appendFile (frameNum++, header, headerLen, frame, frameLen);
        mLcd->start();
        }
        //}}}
      else if (mounted && kWriteMjpg && frameNum == 500) {
        //{{{  close mjpeg
        frameNum++;
        closeFile();
        mLcd->start();
        }
        //}}}
      else {
        //{{{  jpegDecode
        auto header = mCam->getHeader (!frameNum++, 6, headerLen);
        jpeg_mem_src (&mCinfo, header, headerLen);
        jpeg_read_header (&mCinfo, TRUE);

        // jpegBody
        mCinfo.scale_num = 1;
        mCinfo.scale_denom = mZoomValue ? 1 : 2;
        mCinfo.dct_method = JDCT_FLOAT;
        mCinfo.out_color_space = JCS_RGB;

        jpeg_mem_src (&mCinfo, frame, frameLen);
        jpeg_start_decompress (&mCinfo);

        while (mCinfo.output_scanline < mCinfo.output_height) {
          jpeg_read_scanlines (&mCinfo, mBufArray, 1);
          //mLcd->rgb888to565 (mBufArray[0], kRgb565Buffer + mCinfo.output_scanline * mCinfo.output_width, mCinfo.output_width);
          mLcd->rgb888to565cpu (mBufArray[0], kRgb565Buf + mCinfo.output_scanline * mCinfo.output_width, mCinfo.output_width);
          }
        jpeg_finish_decompress (&mCinfo);

        mLcd->start (kRgb565Buf, mCinfo.output_width, mCinfo.output_height, true);
        }
        //}}}
      }
    else
      mLcd->start ((uint16_t*)frame, mCam->getWidth(), mCam->getHeight(), mZoomValue);

    mLcd->drawInfo (LCD_COLOR_WHITE, 0, kVersion);
    mLcd->drawInfo (LCD_COLOR_YELLOW, 15, "%d:%d:%dfps %d:%x:%d",
                                          osGetCPUUsage(), xPortGetFreeHeapSize(), mCam->getFps(),
                                          frameLen, mCam->getStatus(), mCam->getDmaCount());
    if (mDebugValue)
      mLcd->drawDebug();
    for (auto box : mBoxes) box->onDraw (mLcd);
    mLcd->present();

    if (mValueChanged) {
      mValueChanged = false;
      mValue ? mCam->capture() : mCam->preview();
      fileNum++;
      frameNum = 0;
      }
    }
  }
//}}}

//{{{
cApp::cBox* cApp::add (cBox* box, cPoint pos) {

  mBoxes.push_back (box);
  box->setPos (pos);
  return box;
  }
//}}}
//{{{
cApp::cBox* cApp::add (cBox* box, float x, float y) {
  return add (box, cPoint(x,y));
  }
//}}}
//{{{
cApp::cBox* cApp::add (cBox* box) {
  return add (box, cPoint());
  }
//}}}
//{{{
cApp::cBox* cApp::addBelow (cBox* box) {

  mBoxes.push_back (box);
  auto lastBox = mBoxes.back();
  box->setPos (lastBox->getBL());
  return box;
  }
//}}}
//{{{
cApp::cBox* cApp::addFront (cBox* box) {

  mBoxes.push_front (box);
  box->setPos (cPoint());
  return box;
  }
//}}}
//{{{
cApp::cBox* cApp::addFront (cBox* box, float x, float y) {

  mBoxes.push_front (box);
  box->setPos (cPoint(x,y));
  return box;
  }
//}}}
//{{{
void cApp::removeBox (cBox* box) {

  for (auto boxIt = mBoxes.begin(); boxIt != mBoxes.end(); ++boxIt)
    if (*boxIt == box) {
      mBoxes.erase (boxIt);
      //changed();
      return;
      }
  }
//}}}

// protected
//{{{
void cApp::onProx (int16_t x, int16_t y, uint8_t z) {

  //uint8_t HID_Buf[HID_IN_ENDPOINT_SIZE] = { 0,(uint8_t)x,(uint8_t)y,0 };
  // hidSendReport (&gUsbDevice, HID_Buf);

  cPoint pos (x,y);

  bool change = false;
  auto lastProxBox = mProxBox;

  // search for prox in reverse draw order
  mProxBox = nullptr;
  for (auto boxIt = mBoxes.rbegin(); boxIt != mBoxes.rend(); ++boxIt) {
    bool wasPicked = (*boxIt)->getPick();
    if (!mProxBox && (*boxIt)->getEnable() && (*boxIt)->pick (pos, change)) {
      mProxBox = *boxIt;
      change |= mProxBox->onProx (pos - mProxBox->getTL());
      }
    else if (wasPicked) {
      (*boxIt)->setUnPick();
      change |= (*boxIt)->onProxExit();
      }
    }

  if (change || (mProxBox != lastProxBox)) {
    }
  }
//}}}
//{{{
void cApp::onPress (int16_t x, int16_t y) {

  //uint8_t HID_Buf[HID_IN_ENDPOINT_SIZE] = { 1,0,0,0 };
  //hidSendReport (&gUsbDevice, HID_Buf);

  mDown = true;
  mMoved = false;

  cPoint pos (x,y);

  mPressedBox = nullptr;

  // search for pressed in reverse draw order
  for (auto boxIt = mBoxes.rbegin(); boxIt != mBoxes.rend(); ++boxIt) {
    bool change = false;
    if ((*boxIt)->getEnable() && (*boxIt)->pick (pos, change)) {
      mPressedBox = *boxIt;
      mPressedBox->onDown (pos - mPressedBox->getTL());
      break;
      }
    }
  }
//}}}
//{{{
void cApp::onMove (int16_t x, int16_t y, uint8_t z) {

  //uint8_t HID_Buf[HID_IN_ENDPOINT_SIZE] = { 1,(uint8_t)x,(uint8_t)y,0 };
  //hidSendReport (&gUsbDevice, HID_Buf);

  cPoint pos (x,y);

  if (mDown) {
    mMoved = true;
    if (mPressedBox)
      mPressedBox->onMove (pos - mPressedBox->getTL(), pos - mLastPos);
    mLastPos = pos;
    }
  }
//}}}
//{{{
void cApp::onScroll (int16_t x, int16_t y, uint8_t z) {
  mLcd->incScrollValue (y);
  }
//}}}
//{{{
void cApp::onRelease (int16_t x, int16_t y) {

  //uint8_t HID_Buf[HID_IN_ENDPOINT_SIZE] = { 0,0,0,0 };
  //hidSendReport (&gUsbDevice, HID_Buf);

  cPoint pos (x,y);
  mLastPos = pos;

  bool changed = mPressedBox && mPressedBox->onUp (mMoved, pos - mPressedBox->getTL());
  mPressedBox = nullptr;

  mDown = false;
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
int cApp::loadFile (const char* fileName, uint8_t* buf, uint16_t* rgb565Buf) {

  //char pathName[256] = "/";
  //readDirectory (pathName);
  FILINFO filInfo;
  if (!f_stat (fileName, &filInfo))
    mLcd->debug (LCD_COLOR_WHITE, "%d %u/%02u/%02u %02u:%02u %c%c%c%c%c",
                                  (int)(filInfo.fsize),
                                  (filInfo.fdate >> 9) + 1980, filInfo.fdate >> 5 & 15, filInfo.fdate & 31,
                                   filInfo.ftime >> 11, filInfo.ftime >> 5 & 63,
                                  (filInfo.fattrib & AM_DIR) ? 'D' : '-',
                                  (filInfo.fattrib & AM_RDO) ? 'R' : '-',
                                  (filInfo.fattrib & AM_HID) ? 'H' : '-',
                                  (filInfo.fattrib & AM_SYS) ? 'S' : '-',
                                  (filInfo.fattrib & AM_ARC) ? 'A' : '-');

  if (!f_open (&gFile, fileName, FA_READ)) {
    mLcd->debug (LCD_COLOR_WHITE, "image.jpg - found");

    UINT bytesRead;
    f_read (&gFile, (void*)buf, (UINT)filInfo.fsize, &bytesRead);
    mLcd->debug (LCD_COLOR_WHITE, "image.jpg bytes read %d", bytesRead);
    f_close (&gFile);

    if (bytesRead > 0) {
      jpeg_mem_src (&mCinfo, buf, bytesRead);
      jpeg_read_header (&mCinfo, TRUE);
      mLcd->debug (LCD_COLOR_WHITE, "jpegDecode in:%dx%d", mCinfo.image_width, mCinfo.image_height);

      mCinfo.dct_method = JDCT_FLOAT;
      mCinfo.out_color_space = JCS_RGB;
      mCinfo.scale_num = 1;
      mCinfo.scale_denom = 4;
      jpeg_start_decompress (&mCinfo);
      while (mCinfo.output_scanline < mCinfo.output_height) {
        jpeg_read_scanlines (&mCinfo, mBufArray, 1);
        mLcd->rgb888to565 (mBufArray[0], rgb565Buf + (mCinfo.output_scanline * mCinfo.output_width), mCinfo.output_width);
        }
      jpeg_finish_decompress (&mCinfo);

      mLcd->debug (LCD_COLOR_WHITE, "jpegDecode out:%dx%d %d", mCinfo.output_width, mCinfo.output_height, 4);
      return bytesRead;
      }
    }
  else
    mLcd->debug (LCD_COLOR_RED, "image.jpg - not found");

  return 0;
  }
//}}}

//{{{
void cApp::saveNumFile (const char* name, uint32_t num, const char* ext, uint8_t* buf, int bufLen) {

  char fileName[40];
  sprintf (fileName, "%s%03d.%s", name, num, ext);

  if (f_open (&gFile, fileName, FA_WRITE | FA_CREATE_ALWAYS))
    mLcd->debug (LCD_COLOR_RED, "saveNumFile %s fail", fileName);

  else {
    UINT bytesWritten;
    f_write (&gFile, buf, (bufLen + 3) & 0xFFFFFFFC, &bytesWritten);
    f_close (&gFile);
    mLcd->debug (LCD_COLOR_YELLOW, "saveNumFile %s %d:%d", fileName, bufLen, bytesWritten);
    }
   }
//}}}
//{{{
void cApp::saveNumFile (const char* name, uint32_t num, const char* ext, uint8_t* header, int headerLen, uint8_t* frame, int frameLen) {

  char fileName[40];
  sprintf (fileName, "%s%03d.%s", name, num, ext);

  if (f_open (&gFile, fileName, FA_WRITE | FA_CREATE_ALWAYS))
    mLcd->debug (LCD_COLOR_RED, "saveNumFile %s fail", fileName);

  else {
    if (headerLen & 0x03)
      mLcd->debug (LCD_COLOR_RED, "saveNumFile align %s %d", name, headerLen);

    UINT bytesWritten;
    f_write (&gFile, header, headerLen, &bytesWritten);
    f_write (&gFile, frame, (frameLen + 3) & 0xFFFFFFFC, &bytesWritten);
    f_close (&gFile);

    mLcd->debug (LCD_COLOR_YELLOW, "%s %d:%d:%d ok", fileName,  headerLen,frameLen, bytesWritten);
    }
  }
//}}}

//{{{
void cApp::createNumFile (const char* name, uint32_t num, uint8_t* header, int headerLen, uint8_t* frame, int frameLen) {

  char fileName[40];
  sprintf (fileName, "%s%d.mjpg", name, num);

  if (f_open (&gFile, fileName, FA_WRITE | FA_CREATE_ALWAYS))
    mLcd->debug (LCD_COLOR_RED, "createFile %s fail", name);

  else {
    if (headerLen & 0x03)
      mLcd->debug (LCD_COLOR_RED, "createFile align %s %d", fileName, headerLen);

    UINT bytesWritten;
    f_write (&gFile, header, headerLen, &bytesWritten);
    f_write (&gFile, frame, (frameLen + 3) & 0xFFFFFFFC, &bytesWritten);

    mLcd->debug (LCD_COLOR_YELLOW, "%s %d:%d:%d ok", fileName, headerLen,frameLen, bytesWritten);
    }
   }
//}}}
//{{{
void cApp::appendFile (int num, uint8_t* header, int headerLen, uint8_t* frame, int frameLen) {

  UINT bytesWritten;
  f_write (&gFile, header, headerLen, &bytesWritten);
  f_write (&gFile, frame, (frameLen + 3) & 0xFFFFFFFC, &bytesWritten);

  mLcd->debug (LCD_COLOR_YELLOW, "append %d %d:%d:%d ok", num, headerLen,frameLen, bytesWritten);
  }
//}}}
//{{{
void cApp::closeFile() {

  f_close (&gFile);
  }
//}}}

//{{{
void appThread (void* arg) {

  gApp->run();
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
                  netconn_write (request, kHtmlResponseHeader, sizeof(kHtmlResponseHeader)-1, NETCONN_NOCOPY);
                  netconn_write (request, kHtmlBody, sizeof(kHtmlBody)-1, NETCONN_NOCOPY);
                  ok = true;
                  }
                  //}}}
                else if (!strncmp (buf, "GET /cam.jpg", 12)) {
                  //{{{  cam.jpg
                  if (gApp->getCam()) {
                    uint32_t frameLen;
                    bool jpeg;
                    auto frame = gApp->getCam()->getLastFrame (frameLen, jpeg);
                    if (frame) {
                      // send http response header
                      netconn_write (request,
                                     jpeg ? kJpegResponseHeader : kBmpResponseHeader,
                                     jpeg ? sizeof(kJpegResponseHeader)-1 : sizeof(kBmpResponseHeader)-1,
                                     NETCONN_NOCOPY);

                      // send imageFile format header
                      uint32_t headerLen;
                      auto header = gApp->getCam()->getHeader (true, 6, headerLen);
                      netconn_write (request, header, headerLen, NETCONN_NOCOPY);

                      // send imageFile body
                      netconn_write (request, frame, frameLen, NETCONN_NOCOPY);
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

  enum eDhcpState { DHCP_OFF, DHCP_START, DHCP_WAIT_ADDRESS, DHCP_ADDRESS_ASSIGNED, DHCP_TIMEOUT, DHCP_LINK_DOWN };

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
          cLcd::mLcd->debug (LCD_COLOR_GREEN, "dhcp %s", ip4addr_ntoa ((const ip4_addr_t*)&netif->ip_addr));
          sntpSetServerName (0, "pool.ntp.org");
          sntpInit();
          }
        else {
          auto dhcp = (struct dhcp*)netif_get_client_data (netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
          if (dhcp->tries > 4) {
            dhcpState = DHCP_TIMEOUT;
            dhcp_stop (netif);
            cLcd::mLcd->debug (LCD_COLOR_RED, "dhcp timeout");
            }
          }
        break;

      case DHCP_LINK_DOWN:
        cLcd::mLcd->debug (LCD_COLOR_RED, "dhcp link down");
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

  //{{{  Static IP ADDRESS
  #define IP_ADDR0   192
  #define IP_ADDR1   168
  #define IP_ADDR2   1
  #define IP_ADDR3   100
  //}}}
  //{{{  NETMASK
  #define NETMASK_ADDR0   255
  #define NETMASK_ADDR1   255
  #define NETMASK_ADDR2   255
  #define NETMASK_ADDR3   0
  //}}}
  //{{{  Gateway Address
  #define GW_ADDR0   192
  #define GW_ADDR1   168
  #define GW_ADDR2   0
  #define GW_ADDR3   1
  //}}}

  struct netif netIf;
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

  tcpip_init (NULL, NULL);

  //IP_ADDR4(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
  //IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
  //IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
  ip_addr_set_zero_ip4 (&ipaddr);
  ip_addr_set_zero_ip4 (&netmask);
  ip_addr_set_zero_ip4 (&gw);
  netif_add (&netIf, &ipaddr, &netmask, &gw, NULL, &ethernetIfInit, &tcpip_input);

  netif_set_default (&netIf);
  if (netif_is_link_up (&netIf)) {
    netif_set_up (&netIf);
    cLcd::mLcd->debug (LCD_COLOR_GREEN, "ethernet ok");
    sys_thread_new ("dhcp", dhcpThread, &netIf, 2048, osPriorityBelowNormal);
    sys_thread_new ("server", serverThread, NULL, 2048, osPriorityAboveNormal);
    }
  else {
    cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "ethernet not connected");
    netif_set_down (&netIf);
    }

  osThreadTerminate (NULL);
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

  if (kFreeRtos) {
    sys_thread_new ("app", appThread, NULL, 10000, osPriorityNormal);
    sys_thread_new ("net", netThread, NULL, 2048, osPriorityNormal);
    osKernelStart();
    while (true) {};
    }
  else
    gApp->run();
  }
//}}}
