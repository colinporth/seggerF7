// main.cpp - msc class usb
//{{{  includes
#include "../common/system.h"
#include "../common/cLcd.h"
#include "../common/cTouch.h"
#include "../common/cPs2.h"

#include "usbd_msc.h"
#include "../FatFs/ff.h"

#include "../common/cLcd.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_camera.h"
//}}}
const char* kVersion = "USB Msc/Cam 6/4/18";
const char kSdPath[40] = "0:/";
int focus = 0;
int line = 0;
bool grabbing = false;
//{{{
class cApp : public cTouch {
public:
  cApp (int x, int y) : cTouch (x,y) {}
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

  cLcd* mLcd = nullptr;
  cPs2* mPs2 = nullptr;
  bool mButton = false;
  int mFiles = 0;

  DWORD mVsn = 0;
  char mLabel[40];
  };
//}}}
cApp* gApp;

extern "C" {
  void EXTI9_5_IRQHandler() { gApp->onPs2Irq(); }
  //{{{
  void DCMI_DMAXferCplt (DMA_HandleTypeDef* hdma) {

    DCMI_HandleTypeDef* hdcmi = (DCMI_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

    if (hdcmi->XferCount != 0) {
      uint32_t tmp = ((hdcmi->DMA_Handle->Instance->CR) & DMA_SxCR_CT);
      if (((hdcmi->XferCount % 2) == 0) && (tmp != 0)) {
        // Update memory 0 address location
        uint32_t tmp = hdcmi->DMA_Handle->Instance->M0AR;
        HAL_DMAEx_ChangeMemory (hdcmi->DMA_Handle, (tmp + (8*hdcmi->XferSize)), MEMORY0);
        hdcmi->XferCount--;
        }

      else if ((hdcmi->DMA_Handle->Instance->CR & DMA_SxCR_CT) == 0) {
        // Update memory 1 address location
        uint32_t tmp = hdcmi->DMA_Handle->Instance->M1AR;
        HAL_DMAEx_ChangeMemory (hdcmi->DMA_Handle, (tmp + (8*hdcmi->XferSize)), MEMORY1);
        hdcmi->XferCount--;
        }
      }

    else if ((hdcmi->DMA_Handle->Instance->CR & DMA_SxCR_CT) != 0)
      // Update memory 0 address location
      hdcmi->DMA_Handle->Instance->M0AR = hdcmi->pBuffPtr;

    else if ((hdcmi->DMA_Handle->Instance->CR & DMA_SxCR_CT) == 0) {
      // Update memory 1 address location
      uint32_t tmp = hdcmi->pBuffPtr;
      hdcmi->DMA_Handle->Instance->M1AR = (tmp + (4*hdcmi->XferSize));
      hdcmi->XferCount = hdcmi->XferTransferNumber;
      }

    // Check if the frame is transferred
    if (hdcmi->XferCount == hdcmi->XferTransferNumber) {
      // Enable the Frame interrupt */
      __HAL_DCMI_ENABLE_IT (hdcmi, DCMI_IT_FRAME);

      // When snapshot mode, set dcmi state to ready
      if ((hdcmi->Instance->CR & DCMI_CR_CM) == DCMI_MODE_SNAPSHOT)
        hdcmi->State = HAL_DCMI_STATE_READY;
      }
    }
  //}}}
  //{{{
  void DCMI_DMAError (DMA_HandleTypeDef* hdma) {

    DCMI_HandleTypeDef* hdcmi = (DCMI_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
    if (hdcmi->DMA_Handle->ErrorCode != HAL_DMA_ERROR_FE) 
      gApp->getLcd()->debug (LCD_COLOR_RED, "DCMI DMAerror %x", hdcmi->DMA_Handle->ErrorCode);
    }
  //}}}
  //{{{
  void DCMI_Error (DMA_HandleTypeDef* hdma) {
    gApp->getLcd()->debug (LCD_COLOR_RED, "DCMIerror");
    }
  //}}}

  //{{{
  HAL_StatusTypeDef HAL_DCMI_Init (DCMI_HandleTypeDef* hdcmi) {

    hdcmi->State = HAL_DCMI_STATE_BUSY;

    // Configures the HS, VS, DE and PC polarity
    hdcmi->Instance->CR &= ~(DCMI_CR_PCKPOL | DCMI_CR_HSPOL  | DCMI_CR_VSPOL  | DCMI_CR_EDM_0 |
                             DCMI_CR_EDM_1  | DCMI_CR_FCRC_0 | DCMI_CR_FCRC_1 | DCMI_CR_JPEG  |
                             DCMI_CR_ESS | DCMI_CR_BSM_0 | DCMI_CR_BSM_1 | DCMI_CR_OEBS |
                             DCMI_CR_LSM | DCMI_CR_OELS);
    hdcmi->Instance->CR |=  (uint32_t)(hdcmi->Init.SynchroMode | hdcmi->Init.CaptureRate |
                                       hdcmi->Init.VSPolarity  | hdcmi->Init.HSPolarity  |
                                       hdcmi->Init.PCKPolarity | hdcmi->Init.ExtendedDataMode |
                                       hdcmi->Init.JPEGMode | hdcmi->Init.ByteSelectMode |
                                       hdcmi->Init.ByteSelectStart | hdcmi->Init.LineSelectMode |
                                       hdcmi->Init.LineSelectStart);

    // Enable Error and Overrun interrupts
    __HAL_DCMI_ENABLE_IT (hdcmi, DCMI_IT_ERR | DCMI_IT_OVR);

    // Update error code
    hdcmi->ErrorCode = HAL_DCMI_ERROR_NONE;

    // Initialize the DCMI state
    hdcmi->State = HAL_DCMI_STATE_READY;

    return HAL_OK;
    }
  //}}}
  //{{{
  HAL_StatusTypeDef HAL_DCMI_Start_DMA (DCMI_HandleTypeDef* hdcmi, uint32_t DCMI_Mode, uint32_t pData, uint32_t Length) {

    // Initialize the second memory address
    uint32_t SecondMemAddress = 0;

    // Lock the DCMI peripheral state
    hdcmi->State = HAL_DCMI_STATE_BUSY;

    // Enable DCMI by setting DCMIEN bit
    __HAL_DCMI_ENABLE (hdcmi);

    // Configure the DCMI Mode
    hdcmi->Instance->CR &= ~(DCMI_CR_CM);
    hdcmi->Instance->CR |= (uint32_t)(DCMI_Mode);

    hdcmi->DMA_Handle->XferCpltCallback = DCMI_DMAXferCplt;
    hdcmi->DMA_Handle->XferErrorCallback = DCMI_DMAError;
    hdcmi->DMA_Handle->XferAbortCallback = NULL;

    // Reset transfer counters value
    hdcmi->XferCount = 0;
    hdcmi->XferTransferNumber = 0;

    if (Length <= 0xFFFF)
      // Enable the DMA Stream
      HAL_DMA_Start_IT (hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, (uint32_t)pData, Length);
    else {
      // DCMI_DOUBLE_BUFFER Mode, Set the DMA memory1 conversion complete callback
      hdcmi->DMA_Handle->XferM1CpltCallback = DCMI_DMAXferCplt;

      // Initialize transfer parameters
      hdcmi->XferCount = 1;
      hdcmi->XferSize = Length;
      hdcmi->pBuffPtr = pData;

      // Get the number of buffer
      while (hdcmi->XferSize > 0xFFFF) {
        hdcmi->XferSize = hdcmi->XferSize / 2;
        hdcmi->XferCount = hdcmi->XferCount * 2;
        }

      hdcmi->XferCount = hdcmi->XferCount - 2;
      hdcmi->XferTransferNumber = hdcmi->XferCount;
      SecondMemAddress = (uint32_t)(pData + (4*hdcmi->XferSize));

      // Start DMA multi buffer transfer
      HAL_DMAEx_MultiBufferStart_IT (hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, (uint32_t)pData, SecondMemAddress, hdcmi->XferSize);
      }

    // Enable Capture 
    hdcmi->Instance->CR |= DCMI_CR_CAPTURE;

    return HAL_OK;
    }
  //}}}
  //{{{
  void HAL_DCMI_IRQHandler (DCMI_HandleTypeDef* hdcmi) {

    uint32_t isr_value = READ_REG (hdcmi->Instance->MISR);
    if ((isr_value & DCMI_FLAG_ERRRI) == DCMI_FLAG_ERRRI) {
      // Synchronization error interrupt, Clear the Synchronization error flag
      __HAL_DCMI_CLEAR_FLAG(hdcmi, DCMI_FLAG_ERRRI);
      hdcmi->ErrorCode |= HAL_DCMI_ERROR_SYNC;
      hdcmi->State = HAL_DCMI_STATE_ERROR;
      hdcmi->DMA_Handle->XferAbortCallback = DCMI_Error;
      HAL_DMA_Abort_IT (hdcmi->DMA_Handle);
      }

    if ((isr_value & DCMI_FLAG_OVRRI) == DCMI_FLAG_OVRRI) {
      // Overflow interrupt
      __HAL_DCMI_CLEAR_FLAG(hdcmi, DCMI_FLAG_OVRRI);
      hdcmi->ErrorCode |= HAL_DCMI_ERROR_OVR;
      hdcmi->State = HAL_DCMI_STATE_ERROR;
      hdcmi->DMA_Handle->XferAbortCallback = DCMI_Error;
      HAL_DMA_Abort_IT (hdcmi->DMA_Handle);
      }

    if ((isr_value & DCMI_FLAG_FRAMERI) == DCMI_FLAG_FRAMERI) {
      // frame interrupt, in snapshot mode, disable Vsync, Error and Overrun interrupts */
      if ((hdcmi->Instance->CR & DCMI_CR_CM) == DCMI_MODE_SNAPSHOT)
        __HAL_DCMI_DISABLE_IT(hdcmi, DCMI_IT_LINE | DCMI_IT_VSYNC | DCMI_IT_ERR | DCMI_IT_OVR);
      __HAL_DCMI_DISABLE_IT(hdcmi, DCMI_IT_FRAME);
      __HAL_DCMI_CLEAR_FLAG(hdcmi, DCMI_FLAG_FRAMERI);
      line = 0;
      gApp->getLcd()->debug (LCD_COLOR_GREEN, "frameIrq");
      grabbing = false;
      }
    }
  //}}}

  //{{{
  HAL_StatusTypeDef HAL_DCMI_ConfigCrop (DCMI_HandleTypeDef* hdcmi, uint32_t X0, uint32_t Y0, uint32_t XSize, uint32_t YSize) {

    hdcmi->Instance->CWSIZER = (XSize | (YSize << DCMI_CWSIZE_VLINE_Pos));
    hdcmi->Instance->CWSTRTR = (X0 | (Y0 << DCMI_CWSTRT_VST_Pos));
    hdcmi->Instance->CR |= (uint32_t)DCMI_CR_CROP;
    hdcmi->State = HAL_DCMI_STATE_READY;

    return HAL_OK;
    }
  //}}}
  //{{{
  HAL_StatusTypeDef HAL_DCMI_DisableCrop (DCMI_HandleTypeDef* hdcmi) {

    hdcmi->Instance->CR &= ~(uint32_t)DCMI_CR_CROP;
    hdcmi->State = HAL_DCMI_STATE_READY;
    return HAL_OK;
    }
  //}}}
  }

// public
//{{{
void cApp::run() {

  mButton = BSP_PB_GetState (BUTTON_KEY);

  // init lcd
  mLcd = new cLcd (16);
  mLcd->init();

  //mPs2 = new cPs2 (mLcd);
  //mPs2->initKeyboard();

  mscInit (mLcd);
  mscStart();

  //if (f_mount ((FATFS*)malloc (sizeof (FATFS)), kSdPath, 0) == FR_OK) {
  //  char pathName[256] = "/";
  //  readDirectory (pathName);
  //  reportLabel();
  //  }
  //else
  //  mLcd->debug (LCD_COLOR_RED, "not mounted");
  //mLcd->debug (LCD_COLOR_YELLOW, "cameraId %x", BSP_CAMERA_Init (CAMERA_R800x600));
  mLcd->debug (LCD_COLOR_YELLOW, "cameraId %x", BSP_CAMERA_Init (CAMERA_R480x272));  // CAMERA_R320x240
  HAL_Delay (100);

  grabbing = true;
  BSP_CAMERA_Start ((uint8_t*)SDRAM_USER, true);

  int lastCount = 0;
  while (true) {
    pollTouch();
    //while (mPs2->hasChar()) {
    //  auto ch = mPs2->getChar();
    //  onKey (ch & 0xFF, ch & 0x100);
    //  }

    //mLcd->startBgnd (kVersion, mscGetSectors());
    mLcd->startCam ((uint16_t*)SDRAM_USER);
    mLcd->drawTitle (kVersion);
    mLcd->drawDebug();
    mLcd->present();

    if (BSP_PB_GetState (BUTTON_KEY) && !grabbing) {
      BSP_CAMERA_Start ((uint8_t*)SDRAM_USER, false);
      grabbing = true;
      gApp->getLcd()->debug (LCD_COLOR_CYAN, "grab");
      }

    if (false) {
    //if (hasSdChanged()) {
      //{{{  check num files
      char pathName[256] = "/";
      auto count = getCountFiles (pathName);
      if (count != lastCount) {
        f_getlabel (kSdPath, mLabel, &mVsn);
        mLcd->debug (LCD_COLOR_WHITE, "Label <%s> - %d files", mLabel, count);
        lastCount = count;
        reportFree();
        }
      }
      //}}}
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
    BSP_CAMERA_setFocus (focus);
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
void cApp::reportLabel() {

  f_getlabel (kSdPath, mLabel, &mVsn);
  mLcd->debug (LCD_COLOR_WHITE, "Label <%s> ", mLabel);
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

  gApp = new cApp (BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  gApp->run();
  }
//}}}
