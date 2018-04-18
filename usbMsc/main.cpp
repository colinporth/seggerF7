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
#include "cpuUsage.h"
#include "lwip/opt.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/arch.h"
#include "lwip/api.h"

#include "ethernetif.h"
//}}}
#define freeRtos

const char kVersion[] = "WebCam 18/4/18";

uint16_t* kRgb565Buffer = (uint16_t*)0xc0100000;
uint8_t*  kCamBuffer    =  (uint8_t*)0xc0200000;
uint8_t*  kCamBufferEnd =  (uint8_t*)0xc0600000; // plus a bit for wraparound

//{{{
class cApp : public cTouch {
public:
  //{{{
  cApp (int x, int y) : cTouch (x,y) {

    mCinfo.err = jpeg_std_error (&jerr);
    jpeg_create_decompress (&mCinfo);

    mBufferArray[0] = (uint8_t*)malloc (1600 * 3);
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

  cCamera* mCam = nullptr;

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

// public
//{{{
void cApp::init() {

  mLcd = new cLcd (16);
  mLcd->init();

  // show title early
  mLcd->start();
  mLcd->drawInfo (LCD_COLOR_WHITE, 0, kVersion);
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

  mCam = new cCamera();
  mCam->init();
  mCam->start (false, kCamBuffer);

  bool lastButton = false;
  while (true) {
    pollTouch();
    //{{{  removed
    //while (mPs2->hasChar()) {
    //  auto ch = mPs2->getChar();
    //  onKey (ch & 0xFF, ch & 0x100);
    //  }
    //}}}

    if (!mCam->getFrame()) // no frame, clear
      mLcd->start();
    else if (!mCam->getMode())  // rgb565
      mLcd->start ((uint16_t*)mCam->getFrame(), mCam->getWidth(), mCam->getHeight(), BSP_PB_GetState (BUTTON_KEY));
    else {
      // jpegDecode
      auto frameLen = mCam->getFrameLen();
      jpeg_mem_src (&mCinfo, mCam->getHeader(), mCam->getHeaderLen());
      jpeg_read_header (&mCinfo, TRUE);
      mCinfo.scale_num = 1;
      mCinfo.scale_denom = BSP_PB_GetState (BUTTON_KEY) ? 1 : 2;
      mCinfo.dct_method = JDCT_FLOAT;
      mCinfo.out_color_space = JCS_RGB;

      // jpegBody
      jpeg_mem_src (&mCinfo, mCam->getFrame(), mCam->getFrameLen());
      jpeg_start_decompress (&mCinfo);
      while (mCinfo.output_scanline < mCinfo.output_height) {
        jpeg_read_scanlines (&mCinfo, mBufferArray, 1);
        //mLcd->rgb888to565 (mBufferArray[0], kRgb565Buffer + mCinfo.output_scanline * mCinfo.output_width, mCinfo.output_width);
        mLcd->rgb888to565cpu (mBufferArray[0], kRgb565Buffer + mCinfo.output_scanline * mCinfo.output_width, mCinfo.output_width);
        }
      jpeg_finish_decompress (&mCinfo);
      mLcd->start (kRgb565Buffer, mCinfo.output_width, mCinfo.output_height, true);
      }

    mLcd->drawInfo (LCD_COLOR_WHITE, 0, kVersion);
    mLcd->drawInfo (LCD_COLOR_YELLOW, 16, "%d %d %dfps %d:%x:%s:%d",
                    osGetCPUUsage(), xPortGetFreeHeapSize(),
                    mCam->getFps(), mCam->getFrameLen(), mCam->getStatus(),
                    mCam->getMode() ? "j":"p", mCam->getDmaCount());
    mLcd->drawDebug();
    mLcd->present();

    bool button = BSP_PB_GetState (BUTTON_KEY);
    if (!button && (button != lastButton))
      mCam->start (!mCam->getMode(), kCamBuffer);
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
    int focus = mCam->getFocus() + x;
    if (focus < 0)
      focus = 0;
    else if (focus > 254)
      focus = 254;
    mCam->setFocus (focus);
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
        jpegDecode (kCamBuffer, bytesRead, kRgb565Buffer, 4);
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
    mLcd->rgb888to565 (mBufferArray[0], rgb565buf + (mCinfo.output_scanline * mCinfo.output_width), mCinfo.output_width);
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
                  if (gApp->mCam) {
                    auto frame = gApp->mCam->getFrame();
                    if (frame) {
                      int frameLen = gApp->mCam->getFrameLen();

                      // send http response header
                      if (gApp->mCam->getMode())
                        netconn_write (request, kJpegResponseHeader, sizeof(kJpegResponseHeader)-1, NETCONN_NOCOPY);
                      else
                        netconn_write (request, kBmpResponseHeader, sizeof(kBmpResponseHeader)-1, NETCONN_NOCOPY);

                      // send imageFile format header
                      netconn_write (request, gApp->mCam->getHeader(), gApp->mCam->getHeaderLen(), NETCONN_NOCOPY);

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
void appThread (void* arg) {

  gApp->run();
  }
//}}}
//{{{
void netThread (void* arg) {

  // Static IP ADDRESS
  #define IP_ADDR0   192
  #define IP_ADDR1   168
  #define IP_ADDR2   1
  #define IP_ADDR3   100

  // NETMASK
  #define NETMASK_ADDR0   255
  #define NETMASK_ADDR1   255
  #define NETMASK_ADDR2   255
  #define NETMASK_ADDR3   0

  // Gateway Address
  #define GW_ADDR0   192
  #define GW_ADDR1   168
  #define GW_ADDR2   0
  #define GW_ADDR3   1

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
    cLcd::mLcd->debug (LCD_COLOR_YELLOW, "ethernet up");
    sys_thread_new ("dhcp", dhcpThread, &netIf, 2048, osPriorityBelowNormal);
    sys_thread_new ("server", serverThread, NULL, 2048, osPriorityAboveNormal);
    }
  else {
    cLcd::mLcd->debug (LCD_COLOR_YELLOW, "ethernet down");
    netif_set_down (&netIf);
    }

  while (true) {
    //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "netThread tick");
    osDelay (5000);
    }
  //osThreadTerminate (NULL);
  }
//}}}
//{{{
void startThread (void* arg) {

  sys_thread_new ("app", appThread, NULL, 10000, osPriorityNormal);
  sys_thread_new ("net", netThread, NULL, 2048, osPriorityNormal);

  while (true)
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

#ifdef freeRtos
  sys_thread_new ("start", startThread, NULL, 2048, osPriorityNormal);
  osKernelStart();
  while (true);
#else
  gApp->run();
#endif
}
//}}}
