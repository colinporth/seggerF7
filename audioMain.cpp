// main.cpp - audio class usb
//{{{  includes
#include "../../../system.h"
#include "../../../cLcd.h"
#include "../../../cTouch.h"
#include "../../../usbd.h"

#include "../../../stm32746g_audio.h"
//}}}
std::string kVersion = "USB audio 22/2/18";
#define AUDIO_OUT_ENDPOINT  0x01

cLcd gLcd(16);

//{{{  device descriptors
#define STM_VID        0x0483
#define STM_AUDIO_PID  0x5730

__ALIGN_BEGIN const uint8_t kDeviceDescriptor[USB_LEN_DEV_DESC] __ALIGN_END = {
  0x12, USB_DESC_TYPE_DEVICE,
  0x01, 0x01,                 // bcdUSB 1.1
  0x00,                       // bDeviceClass
  0x00,                       // bDeviceSubClass
  0x00,                       // bDeviceProtocol
  USB_MAX_EP0_SIZE,           // bMaxPacketSize
  LOBYTE(STM_VID), HIBYTE(STM_VID),
  LOBYTE(STM_AUDIO_PID), HIBYTE(STM_AUDIO_PID),
  0x01, 0x01,                 // bcdDevice rel 1.01
  USBD_IDX_MFC_STR,           // Index of manufacturer string
  USBD_IDX_PRODUCT_STR,       // Index of product string
  USBD_IDX_SERIAL_STR,        // Index of serial number string
  USBD_MAX_NUM_CONFIGURATION  // bNumConfigurations
  };

uint8_t* deviceDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kDeviceDescriptor);
  return (uint8_t*)kDeviceDescriptor;
  }

// device qualifier descriptor
__ALIGN_BEGIN const uint8_t kDeviceQualifierDescriptor[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
  USB_LEN_DEV_QUALIFIER_DESC, USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00, 0x02,  // bcdUSb
  0x00,        // bDeviceClass
  0x00,        // bDeviceSubClass
  0x00,        // bDeviceProtocol
  0x40,        // bMaxPacketSize0
  0x01,        // bNumConfigurations
  0x00,        // bReserved
  };
//}}}
//{{{  configuration descriptor
__ALIGN_BEGIN const uint8_t kConfigurationDescriptor[109] __ALIGN_END = {
  // Configuration Descriptor
  9, USB_DESC_TYPE_CONFIGURATION,
  LOBYTE(109), HIBYTE(109), // wTotalLength
  2,    // bNumInterfaces
  1,    // bConfigurationValue
  0,    // iConfiguration
  0xC0, // bmAttributes - BUS Powred
  0x32, // bMaxPower = 100 mA

  // Control Interface Descriptor
  9, USB_DESC_TYPE_INTERFACE,
  0, // bInterfaceNumber
  0, // bAlternateSetting
  0, // bNumEndpoints
  1, // bInterfaceClass - audio device class 1
  1, // bInterfaceSubClass - control
  0, // bInterfaceProtocol
  0, // iInterface

  //{{{  audio control descriptors
  //  Audio Control Interface Descriptor
  9, 36, 1,
  0,1,       // bcdADC - 1.00
  39,0,      // wTotalLength
  1,         // bInCollection
  1,         // baInterfaceNr

  // Audio Control Input Terminal Descriptor
  12, 36, 2,
  1,         // bTerminalID = input 1
  1,1,       // wTerminalType AUDIO_TERMINAL_USB_STREAMING - 0x0101
  0x00,      // bAssocTerminal
  cPackets::CHANNELS,  // bNrChannels
  0x33,0x00, // wChannelConfig - 0x0033 - leftFront rightFront, leftSurround, rightSurround
  0,         // iChannelNames
  0,         // iTerminal

  // Audio Control Feature Unit Descriptor
  9, 36, 6,
  2,   // bUnitID = 2
  1,   // bSourceID
  1,   // bControlSize
  1,0, // bmaControls =  mute
  0,   // iTerminal

  // Audio Control Output Terminal Descriptor
  9, 36, 3,
  3,   // bTerminalID = 3
  1,3, // wTerminalType - speaker 0x0301
  0,   // bAssocTerminal
  2,   // bSourceID
  0,   // iTerminal
  //}}}
  //{{{  audio streaming descriptors
  // Audio Streaming Interface Descriptor - Zero Band - Interface 1 - Alternate Setting 0
  9, USB_DESC_TYPE_INTERFACE,
  1, // bInterfaceNumber
  0, // bAlternateSetting
  0, // bNumEndpoints
  1, // bInterfaceClass - audio device class
  2, // bInterfaceSubClass - streaming
  0, // bInterfaceProtocol
  0, // iInterface

  // Audio Streaming Interface Descriptor - Operational - Interface 1 - Alternate Setting 1
  9, USB_DESC_TYPE_INTERFACE,
  1, // bInterfaceNumber
  1, // bAlternateSetting
  1, // bNumEndpoints
  1, // bInterfaceClass - audio device class
  2, // bInterfaceSubClass - streaming
  0, // bInterfaceProtocol
  0, // iInterface

  // Audio Streaming Interface Descriptor
  7, 36, 1,
  1,   // bTerminalLink
  1,   // bDelay
  1,0, // wFormatTag AUDIO_FORMAT_PCM - 0x0001

  // Audio Streaming Descriptor Audio Type I Format
  11, 36, 2,
  1,        // bFormatType - type I
  cPackets::CHANNELS, // bNrChannels
  2,        // bSubFrameSize - 2bytes per frame (16bits)
  16,       // bBitResolution - 16bits per sample
  1,        // bSamFreqType - single frequency supported
  48000 & 0xFF, (48000 >> 8) & 0xFF, 48000 >> 16, // audio sampling frequency in 3 bytes
  //}}}

  // Standard AS Isochronous Synch Endpoint Descriptor - out endPoint 1
  9, USB_DESC_TYPE_ENDPOINT,
  AUDIO_OUT_ENDPOINT, // bEndpointAddress - out endpoint 1
  5,            // bmAttributes - isochronous,asynchronous
  cPackets::PACKET_SIZE & 0xFF, (cPackets::PACKET_SIZE >> 8) & 0xFF, // wMaxPacketSize bytes
  1,            // bInterval
  0,            // bRefresh
  0,            // bSynchAddress

  // Class-Specific AS Isochronous Audio Data Endpoint Descriptor
  7, 37, 1,
  1, // bmAttributes - sampling frequency control
  0, // bLockDelayUnits
  0, // wLockDelay
  };

__ALIGN_BEGIN uint8_t strDesc[256] __ALIGN_END;

uint8_t* configurationStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  #define USBD_CONFIGURATION_STRING "audio config"
  USBD_GetString ((uint8_t*)USBD_CONFIGURATION_STRING, strDesc, length);
  return strDesc;
  }
//}}}
//{{{  string descriptors
#define USBD_LANGID_STRING  0x409
//{{{
__ALIGN_BEGIN const uint8_t kLangIdDescriptor[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
  USB_LEN_LANGID_STR_DESC,
  USB_DESC_TYPE_STRING,
  LOBYTE(USBD_LANGID_STRING), HIBYTE(USBD_LANGID_STRING),
  };
//}}}
//{{{
uint8_t* langIdStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kLangIdDescriptor);
  return (uint8_t*)kLangIdDescriptor;
  }
//}}}

//{{{
uint8_t* manufacturerStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)"colin", strDesc, length);
  return strDesc;
  }
//}}}
//{{{
uint8_t* productStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)((speed == USBD_SPEED_HIGH) ? "Stm32 HS USB audio 1.0" : "Stm32 FS USB audio 1.0"), strDesc, length);
  return strDesc;
  }
//}}}
//{{{
uint8_t* interfaceStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)"audio Interface", strDesc, length);
  return strDesc;
  }
//}}}

#define USB_SIZ_STRING_SERIAL  0x1A
//{{{
__ALIGN_BEGIN uint8_t kStringSerial[USB_SIZ_STRING_SERIAL] __ALIGN_END = {
  USB_SIZ_STRING_SERIAL,
  USB_DESC_TYPE_STRING,
  };
//}}}
//{{{
void intToUnicode (uint32_t value, uint8_t* pbuf, uint8_t len) {
  uint8_t idx = 0;
  for (idx = 0; idx < len; idx ++) {
    if (((value >> 28)) < 0xA )
      pbuf[2 * idx] = (value >> 28) + '0';
    else
      pbuf[2 * idx] = (value >> 28) + 'A' - 10;
    value = value << 4;
    pbuf[2 * idx + 1] = 0;
    }
  }
//}}}
//{{{
void getSerialNum() {
  auto deviceserial0 = *(uint32_t*)0x1FFF7A10;
  auto deviceserial1 = *(uint32_t*)0x1FFF7A14;
  auto deviceserial2 = *(uint32_t*)0x1FFF7A18;
  deviceserial0 += deviceserial2;
  if (deviceserial0 != 0) {
    intToUnicode (deviceserial0, &kStringSerial[2], 8);
    intToUnicode (deviceserial1, &kStringSerial[18], 4);
    }
  }
//}}}
//{{{
uint8_t* serialStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = USB_SIZ_STRING_SERIAL;
  getSerialNum();
  return (uint8_t*)kStringSerial;
  }
//}}}
//}}}
//{{{  audioClass handlers
typedef struct {
  uint8_t       mBuffer[cPackets::SLOTS_PACKET_BUF_SIZE];
  uint8_t       mPlayStarted;
  uint16_t      mWritePtr;
  __IO uint32_t mAltSetting;

  uint8_t       mCommand;
  uint8_t       mData[USB_MAX_EP0_SIZE];
  uint8_t       mLength;
  uint8_t       mUnit;

  uint16_t      mMute;
  uint16_t      mCurVolume;
  uint16_t      mMinVolume;
  uint16_t      mMaxVolume;
  uint16_t      mResVolume;
  uint16_t      mFrequency;
  } tAudioData;

//{{{
static uint8_t usbInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  // Open EP OUT
  USBD_LL_OpenEP (device, AUDIO_OUT_ENDPOINT, USBD_EP_TYPE_ISOC, cPackets::PACKET_SIZE);

  // allocate audioData
  tAudioData* audioData = (tAudioData*)malloc (sizeof (tAudioData));
  audioData->mPlayStarted = 0;
  audioData->mWritePtr = 0;
  audioData->mAltSetting = 0;

  audioData->mMute = 0;
  audioData->mCurVolume = 50;
  audioData->mMinVolume = 0;
  audioData->mMaxVolume = 100;
  audioData->mResVolume = 1;
  audioData->mFrequency = cPackets::SAMPLE_RATE;
  device->pClassData = audioData;

  BSP_AUDIO_OUT_Init (OUTPUT_DEVICE_BOTH, 100, cPackets::SAMPLE_RATE);
  BSP_AUDIO_OUT_SetAudioFrameSlot (SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1 | SAI_SLOTACTIVE_2 | SAI_SLOTACTIVE_3);

  // Prepare Out endpoint to receive 1st packet
  USBD_LL_PrepareReceive (device, AUDIO_OUT_ENDPOINT, audioData->mBuffer, cPackets::PACKET_SIZE);

  return USBD_OK;
  }
//}}}
//{{{
static uint8_t usbDeInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  // close out ep
  USBD_LL_CloseEP (device, AUDIO_OUT_ENDPOINT);

  // stop audio
  BSP_AUDIO_OUT_Stop (CODEC_PDWN_SW);
  free (device->pClassData);
  device->pClassData = NULL;

  return USBD_OK;
  }
//}}}

//{{{
static uint8_t usbSetup (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  gLcd.debug (LCD_COLOR_WHITE, "setup %02x:%02x %d:%d:%d",
                                req->bmRequest, req->bRequest, req->wLength, req->wValue, req->wIndex);

  auto audioData = (tAudioData*)device->pClassData;
  switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest) {
        //{{{
        case USB_REQ_GET_DESCRIPTOR:
          gLcd.debug (LCD_COLOR_RED, "- USB_REQ_GET_DESCRIPTOR");
          break;
        //}}}
        //{{{
        case USB_REQ_GET_INTERFACE :
          USBD_CtlSendData (device, (uint8_t*)&audioData->mAltSetting, 1);
          gLcd.debug (LCD_COLOR_YELLOW, "- usbReqGetInterface alt%x", (int)audioData->mAltSetting);
          break;
        //}}}
        //{{{
        case USB_REQ_SET_INTERFACE :
          if (req->wValue <= USBD_MAX_NUM_INTERFACES) {
            audioData->mAltSetting = (uint8_t)req->wValue;
            gLcd.debug (LCD_COLOR_YELLOW, "%d - usbReqSetInterface alt:%x", req->wValue);
            }
          else {// NAK
            USBD_CtlError (device, req);
            gLcd.debug (LCD_COLOR_RED, "%d - usbReqSetInterface");
            }
          break;
        //}}}
        //{{{
        default:
          USBD_CtlError (device, req);
          return USBD_FAIL;
        //}}}
        }
      break;

    case USB_REQ_TYPE_CLASS :
      switch (req->bRequest) {
        //{{{
        case 0x01: // setCur
          if ((req->wValue >> 8) == 1) {
            if (req->wLength) {
              // rx buffer from ep0
              USBD_CtlPrepareRx (device, audioData->mData, req->wLength);
              audioData->mCommand = 0x01;             // set request value
              audioData->mLength = req->wLength;      // set request data length
              audioData->mUnit = HIBYTE(req->wIndex); // set request target unit
              }
            }

          gLcd.debug (LCD_COLOR_YELLOW, "- setCur %d:%x", req->wLength, audioData->mData[0]);
          break;
        //}}}
        //{{{
        case 0x02: // setMin
          gLcd.debug (LCD_COLOR_YELLOW, "- setMin %d", req->wLength);
          break;
        //}}}
        //{{{
        case 0x03: // setMax
          gLcd.debug (LCD_COLOR_YELLOW, "- setMax %d", req->wLength);
          break;
        //}}}
        //{{{
        case 0x04: // setRes
          gLcd.debug (LCD_COLOR_YELLOW, "- setRes %d", req->wLength);
          break;
        //}}}
        //{{{
        case 0x81: // getCur
          if ((req->wValue >> 8) == 1) {
            USBD_CtlSendData (device, (uint8_t*)(&audioData->mMute), req->wLength);
            gLcd.debug (LCD_COLOR_YELLOW, "- getCur mute %d:%x", req->wLength, audioData->mMute);
            }
          else if ((req->wValue >> 8) == 2) {
            USBD_CtlSendData (device, (uint8_t*)(&audioData->mCurVolume), req->wLength);
            gLcd.debug (LCD_COLOR_YELLOW, "- getCur volume %d:%x", req->wLength, audioData->mCurVolume);
            }
          else
            gLcd.debug (LCD_COLOR_RED, "- getCur %d", req->wLength);
          break;
        //}}}
        //{{{
        case 0x82: // getMin
          USBD_CtlSendData (device, (uint8_t*)(&audioData->mMinVolume), req->wLength);
          gLcd.debug (LCD_COLOR_YELLOW, "- getMin %d:%x", req->wLength, audioData->mMinVolume);
          break;
        //}}}
        //{{{
        case 0x83: // getMax
          USBD_CtlSendData (device, (uint8_t*)(&audioData->mMaxVolume), req->wLength);
          gLcd.debug (LCD_COLOR_YELLOW, "- getMax %d:%x", req->wLength, audioData->mMaxVolume);
          break;
        //}}}
        //{{{
        case 0x84: // getRes
          USBD_CtlSendData (device, (uint8_t*)(&audioData->mResVolume), req->wLength);
          gLcd.debug (LCD_COLOR_YELLOW, "- getRes %d:%x", req->wLength, audioData->mResVolume);
          break;
        //}}}
        default:
          gLcd.debug (LCD_COLOR_YELLOW, "- default %d", req->wLength);
          USBD_CtlError (device, req);
          return USBD_FAIL;
        }
    }
  return USBD_OK;
  }
//}}}

//{{{
static uint8_t usbEp0TxReady (USBD_HandleTypeDef* device) {
  return USBD_OK;
  }
//}}}
//{{{
static uint8_t usbEp0RxReady (USBD_HandleTypeDef* device) {
// only SET_CUR request is managed

  auto audioData = (tAudioData*)device->pClassData;
  if (audioData->mCommand == 0x01) {
    if (audioData->mUnit == 0) {
      audioData->mFrequency = audioData->mData[0] + (audioData->mData[1] << 8) + (audioData->mData[2] << 16);
      //BSP_AUDIO_OUT_SetFrequency (audioData->mFrequency);
      gLcd.debug (LCD_COLOR_GREEN, "setFreq %d", audioData->mFrequency);
      }
    else if (audioData->mUnit == 2) {
      audioData->mMute = audioData->mData[0];
      BSP_AUDIO_OUT_SetMute (audioData->mMute);
      gLcd.debug (LCD_COLOR_GREEN, "%s", audioData->mMute ? "muted" : "unmuted");
      }
    else
      gLcd.debug (LCD_COLOR_RED, "epRx audReqSetCur %d cmd:%d unit:%d",
                                 audioData->mLength, audioData->mCommand, audioData->mUnit);
    }
  else
    gLcd.debug (LCD_COLOR_YELLOW, "epRx:%d cmd:%d unit:%d data:%x",
                                  audioData->mLength, audioData->mCommand, audioData->mUnit, audioData->mData[0]);

  audioData->mCommand = 0;
  audioData->mLength = 0;

  return USBD_OK;
  }
//}}}

//{{{
static uint8_t usbDataIn (USBD_HandleTypeDef* device, uint8_t epNum) {

  gLcd.debug (LCD_COLOR_RED, "usbDataIn");
  return USBD_OK;
  }
//}}}
//{{{
static uint8_t usbDataOut (USBD_HandleTypeDef* device, uint8_t epNum) {

  if (epNum == AUDIO_OUT_ENDPOINT) {
    auto audioData = (tAudioData*)device->pClassData;
    if (!audioData->mPlayStarted && (audioData->mWritePtr >= cPackets::SLOTS_PACKET_BUF_SIZE/2)) {
      //{{{  start playing
      BSP_AUDIO_OUT_Play ((uint16_t*)audioData->mBuffer, cPackets::SLOTS_PACKET_BUF_SIZE);
      audioData->mPlayStarted = 1;
      }
      //}}}
    gPackets.add ((int16_t*)(audioData->mBuffer + audioData->mWritePtr));

    // prepare outEndpoint to rx next audio packet
    audioData->mWritePtr += cPackets::SLOTS_PACKET_SIZE;
    if (audioData->mWritePtr >= cPackets::SLOTS_PACKET_BUF_SIZE)
      audioData->mWritePtr = 0;
    USBD_LL_PrepareReceive (device, AUDIO_OUT_ENDPOINT, &audioData->mBuffer[audioData->mWritePtr], cPackets::PACKET_SIZE);
    }

  return USBD_OK;
  }
//}}}

//{{{
static uint8_t usbSof (USBD_HandleTypeDef* device) {
  return USBD_OK;
  }
//}}}

//{{{
static uint8_t usbIsoInInComplete (USBD_HandleTypeDef* device, uint8_t epNum) {
  return USBD_OK;
  }
//}}}
//{{{
static uint8_t usbIsoOutInComplete (USBD_HandleTypeDef* device, uint8_t epNum) {
  return USBD_OK;
  }
//}}}

//{{{
static uint8_t* usbGetConfigDescriptor (uint16_t* length) {
  *length = sizeof (kConfigurationDescriptor);
  return (uint8_t*)kConfigurationDescriptor;
  }
//}}}
//{{{
static uint8_t* usbGetDeviceQualifierDescriptor (uint16_t *length) {
  *length = sizeof (kDeviceQualifierDescriptor);
  return (uint8_t*)kDeviceQualifierDescriptor;
  }
//}}}
//}}}

//{{{
void audioClock (bool faster) {
// Set the PLL configuration according to the audio frequency
// target = 48000*2*2 * 256 = 49.152Mhz

// slower
// - PLLI2S_VCO: VCO_344 N
// - I2S_CLK = PLLI2S_VCO / PLLI2SQ = 344/7 = 49.142 Mhz
// - I2S_CLK1 = I2S_CLK / PLLI2SDIVQ = 49.142/1 = 49.142 Mhz

// faster
// - PLLI2S_VCO: VCO_295 N
// - I2S_CLK = PLLI2S_VCO / PLLI2SQ = 295/6 = 49.166 Mhz
// - I2S_CLK1 = I2S_CLK / PLLI2SDIVQ = 49.1666/1 = 49.142 Mhz

  if (gPackets.setFaster (faster)) {
    RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;
    HAL_RCCEx_GetPeriphCLKConfig (&RCC_ExCLKInitStruct);
    RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
    RCC_ExCLKInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SP = 8;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = faster ? 295 : 344;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = faster ? 6 : 7;
    RCC_ExCLKInitStruct.PLLI2SDivQ = 1;
    HAL_RCCEx_PeriphCLKConfig (&RCC_ExCLKInitStruct);
    }
  }
//}}}
extern "C" {
  //{{{
  void BSP_AUDIO_OUT_ClockConfig (SAI_HandleTypeDef* hsai, uint32_t freq, void* params) {
    audioClock (false);
    }
  //}}}
  //{{{
  void BSP_AUDIO_OUT_TransferComplete_CallBack() {

    auto writePtrOnRead = ((tAudioData*)gUsbDevice.pClassData)->mWritePtr / cPackets::SLOTS_PACKET_SIZE;

    if (writePtrOnRead > cPackets::PACKETS/2) {
      if (!gPackets.getFaster())
        audioClock (true);
      }
    else if (writePtrOnRead < cPackets::PACKETS/2) {
      if (gPackets.getFaster())
        audioClock (false);
      }
    }
  //}}}
  }

class cAppTouch : public cTouch {
public:
  cAppTouch (int x, int y) : cTouch(x,y) {}

protected:
  //{{{
  void onProx (int x, int y, int z) {
    if (x || y)
      gLcd.debug (LCD_COLOR_MAGENTA, "onProx %d %d %d", x, y, z);
    }
  //}}}
  //{{{
  void onPress (int x, int y) {
    gLcd.debug (LCD_COLOR_GREEN, "onPress %d %d", x, y);
    }
  //}}}
  //{{{
  void onMove (int x, int y, int z) {
    if (x || y) {
      gLcd.incScrollValue (y);
      gLcd.debug (LCD_COLOR_GREEN, "onMove %d %d %d", x, y, z);
      }
    }
  //}}}
  //{{{
  void onScroll (int x, int y, int z) {
    gLcd.incScrollValue (y);
    }
  //}}}
  //{{{
  void onRelease (int x, int y) {
    gLcd.debug (LCD_COLOR_GREEN, "onRelease %d %d", x, y);
    }
  //}}}
  };

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
    while (1) {}

  // Activate the OverDrive to reach the 216 Mhz Frequency
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    while (1) {}

  // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    while (1) {}
  //}}}

  gLcd.init();
  cAppTouch touch (BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  BSP_PB_Init (BUTTON_KEY, BUTTON_MODE_GPIO);

  // init usbDevice library
  //{{{
  USBD_DescriptorsTypeDef audioDescriptor = {
    deviceDescriptor,
    langIdStringDescriptor,
    manufacturerStringDescriptor,
    productStringDescriptor,
    serialStringDescriptor,
    configurationStringDescriptor,
    interfaceStringDescriptor,
    };
  //}}}
  //{{{
  USBD_ClassTypeDef audioClass = {
    usbInit,
    usbDeInit,
    usbSetup,
    usbEp0TxReady,
    usbEp0RxReady,
    usbDataIn,
    usbDataOut,
    usbSof,
    usbIsoInInComplete,
    usbIsoOutInComplete,
    usbGetConfigDescriptor,
    usbGetConfigDescriptor,
    usbGetConfigDescriptor,
    usbGetDeviceQualifierDescriptor,
    };
  //}}}
  gUsbDevice.pClassData = NULL;
  USBD_Init (&gUsbDevice, &audioDescriptor, 0);
  USBD_RegisterClass (&gUsbDevice, &audioClass);
  USBD_Start (&gUsbDevice);

  while (true) {
    touch.poll();
    gLcd.show (kVersion);
    gPackets.show();
    touch.show();
    gLcd.flip();
    }
  }
//}}}
