// main.cpp - hid class usb
//{{{  includes
#include "common/system.h"
#include "common/cLcd.h"
#include "common/cTouch.h"
#include "common/cPs2.h"
#include "common/usbd.h"

#include "stm32746g_audio.h"
//}}}

const char* kAudVersion = "USB Audio 14/3/18";
const char* kHidVersion = "USB keyboard 14/3/18";
#define HID_IN_ENDPOINT       0x81
#define HID_IN_ENDPOINT_SIZE  7
#define AUDIO_OUT_ENDPOINT    0x01

//{{{
class cPackets {
public:
  static const int CHANNELS = 4;
  static const int SAMPLE_RATE = 48000;
  static const int BYTES_PER_SAMPLE = 2;
  static const int PACKETS_PER_SECOND = 1000;
  static const int PACKETS = 8;
  static const int PACKET_SAMPLES = SAMPLE_RATE / PACKETS_PER_SECOND;
  static const int PACKET_SIZE = CHANNELS * PACKET_SAMPLES * BYTES_PER_SAMPLE;
  static const int SLOTS = 4;
  static const int SLOTS_PACKET_SIZE = SLOTS * BYTES_PER_SAMPLE * PACKET_SAMPLES;
  static const int SLOTS_PACKET_BUF_SIZE = PACKETS * SLOTS_PACKET_SIZE;

  //{{{
  bool getFaster() {
    return mFaster;
    }
  //}}}
  //{{{
  bool setFaster (bool faster) {
    if (faster != mFaster) {
      mFaster = faster;
      return true;
      }
    return false;
    }
  //}}}

  //{{{
  void add (int16_t* ptr) {

    for (int sample = 0; sample < PACKET_SAMPLES; sample++) {
      //  swap FR,RL and sum
      int16_t srcRL = *(ptr+2);
      *(ptr+2) = *(ptr+1);
      *(ptr+1) = srcRL;

      // sum
      mSumFL += abs (*ptr++);
      mSumFR += abs (*ptr++);
      mSumRL += abs (*ptr++);
      mSumRR += abs (*ptr++);
      }

    mNumPackets++;
    if (mNumPackets >= 20) {
      // save every 20ms
      mNumPackets = 0;
      mFL[mSample] = mSumFL >> 16;
      mFR[mSample] = mSumFR >> 16;
      mRL[mSample] = mSumRL >> 16;
      mRR[mSample] = mSumRR >> 16;
      mSample = (mSample+1) % (BSP_LCD_GetXSize()/2);
      mSumFR = 0;
      mSumFL = 0;
      mSumRL = 0;
      mSumRR = 0;
      }
    }
  //}}}
  //{{{
  void show() {

    auto centreX = BSP_LCD_GetXSize() / 2;
    auto centreY = BSP_LCD_GetYSize() / 2;

    BSP_LCD_SetTextColor (mFaster ? LCD_COLOR_GREEN : LCD_COLOR_MAGENTA);
    auto sample = mSample;
    for (auto i = 0u; i < centreX; i++) {
      sample = (sample + 1) % centreX;
      BSP_LCD_FillRect (i*2, centreY - mFL[sample], 2, mFL[sample] + mFR[sample]);
      }
    }
  //}}}

private:

  uint16_t mNumPackets = 0;
  uint16_t mSample = 0;

  unsigned mSumFL = 0;
  unsigned mSumFR = 0;
  unsigned mSumRL = 0;
  unsigned mSumRR = 0;

  uint16_t mFL[240];
  uint16_t mFR[240];
  uint16_t mRL[240];
  uint16_t mRR[240];

  bool mFaster = true;
  };
//}}}
cPackets gPackets;

//{{{
class cApp : public cTouch {
public:
  cApp (int x, int y) : cTouch (x,y) {}
  cLcd* getLcd() { return mLcd; }
  cPs2* getPs2() { return mPs2; }

  void run (bool keyboard);

protected:
  virtual void onProx (int x, int y, int z);
  virtual void onPress (int x, int y);
  virtual void onMove (int x, int y, int z);
  virtual void onScroll (int x, int y, int z);
  virtual void onRelease (int x, int y);

private:
  cLcd* mLcd = nullptr;
  cTouch* mTouch = nullptr;
  cPs2* mPs2 = nullptr;
  bool mAud = false;
  };
//}}}
cApp* gApp;
extern "C" { void EXTI9_5_IRQHandler() { gApp->getPs2()->onIrq(); } }

//{{{  device descriptors
#define STM_VID        0x0483
#define STM_AUDIO_PID  0x5730
#define STM_HID_PID    0x5710

__ALIGN_BEGIN const uint8_t kAudDeviceDescriptor[USB_LEN_DEV_DESC] __ALIGN_END = {
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

uint8_t* audDeviceDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kAudDeviceDescriptor);
  return (uint8_t*)kAudDeviceDescriptor;
  }

// device qualifier descriptor
__ALIGN_BEGIN const uint8_t kAudDeviceQualifierDescriptor[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
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
__ALIGN_BEGIN const uint8_t kAudConfigurationDescriptor[109] __ALIGN_END = {
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

uint8_t* audConfigurationStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  #define USBD_CONFIGURATION_STRING "audio config"
  USBD_GetString ((uint8_t*)USBD_CONFIGURATION_STRING, strDesc, length);
  return strDesc;
  }
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
static uint8_t audUsbInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

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
static uint8_t audUsbDeInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

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
static uint8_t audUsbSetup (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  gApp->getLcd()->debug (LCD_COLOR_WHITE, "setup %02x:%02x %d:%d:%d",
                                req->bmRequest, req->bRequest, req->wLength, req->wValue, req->wIndex);

  auto audioData = (tAudioData*)device->pClassData;
  switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest) {
        //{{{
        case USB_REQ_GET_DESCRIPTOR:
          gApp->getLcd()->debug (LCD_COLOR_RED, "- USB_REQ_GET_DESCRIPTOR");
          break;
        //}}}
        //{{{
        case USB_REQ_GET_INTERFACE :
          USBD_CtlSendData (device, (uint8_t*)&audioData->mAltSetting, 1);
          gApp->getLcd()->debug (LCD_COLOR_YELLOW, "- usbReqGetInterface alt%x", (int)audioData->mAltSetting);
          break;
        //}}}
        //{{{
        case USB_REQ_SET_INTERFACE :
          if (req->wValue <= USBD_MAX_NUM_INTERFACES) {
            audioData->mAltSetting = (uint8_t)req->wValue;
            gApp->getLcd()->debug (LCD_COLOR_YELLOW, "%d - usbReqSetInterface alt:%x", req->wValue);
            }
          else {// NAK
            USBD_CtlError (device, req);
            gApp->getLcd()->debug (LCD_COLOR_RED, "%d - usbReqSetInterface");
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

          gApp->getLcd()->debug (LCD_COLOR_YELLOW, "- setCur %d:%x", req->wLength, audioData->mData[0]);
          break;
        //}}}
        //{{{
        case 0x02: // setMin
          gApp->getLcd()->debug (LCD_COLOR_YELLOW, "- setMin %d", req->wLength);
          break;
        //}}}
        //{{{
        case 0x03: // setMax
          gApp->getLcd()->debug (LCD_COLOR_YELLOW, "- setMax %d", req->wLength);
          break;
        //}}}
        //{{{
        case 0x04: // setRes
          gApp->getLcd()->debug (LCD_COLOR_YELLOW, "- setRes %d", req->wLength);
          break;
        //}}}
        //{{{
        case 0x81: // getCur
          if ((req->wValue >> 8) == 1) {
            USBD_CtlSendData (device, (uint8_t*)(&audioData->mMute), req->wLength);
            gApp->getLcd()->debug (LCD_COLOR_YELLOW, "- getCur mute %d:%x", req->wLength, audioData->mMute);
            }
          else if ((req->wValue >> 8) == 2) {
            USBD_CtlSendData (device, (uint8_t*)(&audioData->mCurVolume), req->wLength);
            gApp->getLcd()->debug (LCD_COLOR_YELLOW, "- getCur volume %d:%x", req->wLength, audioData->mCurVolume);
            }
          else
            gApp->getLcd()->debug (LCD_COLOR_RED, "- getCur %d", req->wLength);
          break;
        //}}}
        //{{{
        case 0x82: // getMin
          USBD_CtlSendData (device, (uint8_t*)(&audioData->mMinVolume), req->wLength);
          gApp->getLcd()->debug (LCD_COLOR_YELLOW, "- getMin %d:%x", req->wLength, audioData->mMinVolume);
          break;
        //}}}
        //{{{
        case 0x83: // getMax
          USBD_CtlSendData (device, (uint8_t*)(&audioData->mMaxVolume), req->wLength);
          gApp->getLcd()->debug (LCD_COLOR_YELLOW, "- getMax %d:%x", req->wLength, audioData->mMaxVolume);
          break;
        //}}}
        //{{{
        case 0x84: // getRes
          USBD_CtlSendData (device, (uint8_t*)(&audioData->mResVolume), req->wLength);
          gApp->getLcd()->debug (LCD_COLOR_YELLOW, "- getRes %d:%x", req->wLength, audioData->mResVolume);
          break;
        //}}}
        default:
          gApp->getLcd()->debug (LCD_COLOR_YELLOW, "- default %d", req->wLength);
          USBD_CtlError (device, req);
          return USBD_FAIL;
        }
    }
  return USBD_OK;
  }
//}}}

//{{{
static uint8_t audUsbEp0TxReady (USBD_HandleTypeDef* device) {
  return USBD_OK;
  }
//}}}
//{{{
static uint8_t audUsbEp0RxReady (USBD_HandleTypeDef* device) {
// only SET_CUR request is managed

  auto audioData = (tAudioData*)device->pClassData;
  if (audioData->mCommand == 0x01) {
    if (audioData->mUnit == 0) {
      audioData->mFrequency = audioData->mData[0] + (audioData->mData[1] << 8) + (audioData->mData[2] << 16);
      //BSP_AUDIO_OUT_SetFrequency (audioData->mFrequency);
      gApp->getLcd()->debug (LCD_COLOR_GREEN, "setFreq %d", audioData->mFrequency);
      }
    else if (audioData->mUnit == 2) {
      audioData->mMute = audioData->mData[0];
      BSP_AUDIO_OUT_SetMute (audioData->mMute);
      gApp->getLcd()->debug (LCD_COLOR_GREEN, "%s", audioData->mMute ? "muted" : "unmuted");
      }
    else
      gApp->getLcd()->debug (LCD_COLOR_RED, "epRx audReqSetCur %d cmd:%d unit:%d",
                                 audioData->mLength, audioData->mCommand, audioData->mUnit);
    }
  else
    gApp->getLcd()->debug (LCD_COLOR_YELLOW, "epRx:%d cmd:%d unit:%d data:%x",
                                  audioData->mLength, audioData->mCommand, audioData->mUnit, audioData->mData[0]);

  audioData->mCommand = 0;
  audioData->mLength = 0;

  return USBD_OK;
  }
//}}}

//{{{
static uint8_t audUsbDataIn (USBD_HandleTypeDef* device, uint8_t epNum) {

  gApp->getLcd()->debug (LCD_COLOR_RED, "usbDataIn");
  return USBD_OK;
  }
//}}}
//{{{
static uint8_t audUsbDataOut (USBD_HandleTypeDef* device, uint8_t epNum) {

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
static uint8_t audUsbSof (USBD_HandleTypeDef* device) {
  return USBD_OK;
  }
//}}}

//{{{
static uint8_t audUsbIsoInInComplete (USBD_HandleTypeDef* device, uint8_t epNum) {
  return USBD_OK;
  }
//}}}
//{{{
static uint8_t audUsbIsoOutInComplete (USBD_HandleTypeDef* device, uint8_t epNum) {
  return USBD_OK;
  }
//}}}

//{{{
static uint8_t* audUsbGetConfigDescriptor (uint16_t* length) {
  *length = sizeof (kAudConfigurationDescriptor);
  return (uint8_t*)kAudConfigurationDescriptor;
  }
//}}}
//{{{
static uint8_t* audUsbGetDeviceQualifierDescriptor (uint16_t *length) {
  *length = sizeof (kAudDeviceQualifierDescriptor);
  return (uint8_t*)kAudDeviceQualifierDescriptor;
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

//{{{  device descriptors
// device descriptor
__ALIGN_BEGIN const uint8_t kDeviceDescriptor[USB_LEN_DEV_DESC] __ALIGN_END = {
  sizeof(kDeviceDescriptor), USB_DESC_TYPE_DEVICE,
  0,2,                       // bcdUSB
  0,                         // bDeviceClass
  0,                         // bDeviceSubClass
  0,                         // bDeviceProtocol
  USB_MAX_EP0_SIZE,          // bMaxPacketSize
  LOBYTE(STM_VID), HIBYTE(STM_VID),
  LOBYTE(STM_HID_PID), HIBYTE(STM_HID_PID),
  0,2,                       // bcdDevice rel. 2.00
  USBD_IDX_MFC_STR,          // Index of manufacturer string
  USBD_IDX_PRODUCT_STR,      // Index of product string
  USBD_IDX_SERIAL_STR,       // Index of serial number string
  USBD_MAX_NUM_CONFIGURATION // bNumConfigurations
  };

uint8_t* deviceDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kDeviceDescriptor);
  return (uint8_t*)kDeviceDescriptor;
  }

// device qualifier descriptor
__ALIGN_BEGIN const uint8_t kHidDeviceQualifierDescriptor[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
  sizeof(kHidDeviceQualifierDescriptor), USB_DESC_TYPE_DEVICE_QUALIFIER,
  0,2,   // bcdUSb
  0,     // bDeviceClass
  0,     // bDeviceSubClass
  0,     // bDeviceProtocol
  0x40,  // bMaxPacketSize0
  1,     // bNumConfigurations
  0,     // bReserved
  };

//}}}
//{{{  keyboard hid report descriptor
__ALIGN_BEGIN uint8_t kHidReportDescriptor[] __ALIGN_END = {
  0x05, 0x01,  // Usage Page (Generic Desktop Ctrls)
  0x09, 0x06,  // Usage (Keyboard)
  0xA1, 0x01,  // Collection (Application)
    //0x85, 0x01,  //  Report ID (1)  - extra byte in report with id = 1
    0x05, 0x07,  //  Usage Page (Keyboard/Keypad)
    0x75, 0x01,  //  Report Size - 1 bit
    0x95, 0x08,  //  Report Count - 8
    0x15, 0x00,  //  Logical Minimum - 0
    0x25, 0x01,  //  Logical Maximum - 1
    0x19, 0xE0,  //  Usage Minimum - 0xE0
    0x29, 0xE7,  //  Usage Maximum - 0xE7
    0x81, 0x02,  //  Input - Data, Var, Abs, No Wrap,Linear,Preferred State,No Null Position

    0x95, 0x05,  //  REPORT_COUNT - 5
    0x75, 0x01,  //  REPORT_SIZE - 1
    0x05, 0x08,  //  USAGE_PAGE - LEDs
    0x19, 0x01,  //  USAGE_MINIMUM - Num Lock
    0x29, 0x05,  //  USAGE_MAXIMUM  - Kana
    0x91, 0x02,  //  OUTPUT - Data,Var,Abs
    0x95, 0x01,  //  REPORT_COUNT  - 1
    0x75, 0x03,  //  REPORT_SIZE - 3
    0x91, 0x03,  //  OUTPUT - Cnst,Var,Abs

    0x95, 0x06,  //  Report Count - 6
    0x75, 0x08,  //  Report Size - 8 bits
    0x15, 0x00,  //  Logical Minimum - 0
    0x25, 0x65,  //  Logical Maximum - 101
    0x05, 0x07,  //  Usage Page (Keyboard/Keypad)
    0x19, 0x00,  //  Usage Minimum - 0x00
    0x29, 0x65,  //  Usage Maximum - 101
    0x81, 0x00,  //  Input - Data, Array, Abs, No Wrap,Linear,Preferred State,No Null Position
  0xC0,        // End Collection
  };
//}}}
//{{{  mouse hid report descriptor
//__ALIGN_BEGIN const uint8_t kHidReportDescriptor[] __ALIGN_END = {
  //0x05, 0x01, // Usage Page (Generic Desktop),
  //0x09, 0x02, // Usage (Mouse),
  //0xA1, 0x01, // Collection (Application),
    //0x09, 0x01, // Usage (Pointer),
    //0xA1, 0x00, // Collection (Physical),
      //0x05, 0x09, // Usage Page (Buttons),
      //0x19, 0x01, // Usage Minimum (01),
      //0x29, 0x03, // Usage Maximum (03),

      //0x15, 0x00, // Logical Minimum (0),
      //0x25, 0x01, // Logical Maximum (1),
      //0x95, 0x03, // Report Count (3),
      //0x75, 0x01, // Report Size (1),

      //0x81, 0x02, // Input (Data, Variable, Absolute), ;3 button bits

      //0x95, 0x01, // Report Count (1),
      //0x75, 0x05, // Report Size (5),
      //0x81, 0x01, // Input (Constant), ;5 bit padding

      //0x05, 0x01, // Usage Page (Generic Desktop),
      //0x09, 0x30, // Usage (X),
      //0x09, 0x31, // Usage (Y),
      //0x09, 0x38,

      //0x15, 0x81, // Logical Minimum (-127),
      //0x25, 0x7F, // Logical Maximum (127),
      //0x75, 0x08, // Report Size (8),
      //0x95, 0x03, // Report Count (3),

      //0x81, 0x06, // Input (Data, Variable, Relative), ;2 position bytes (X & Y)
    //0xC0,       // end collection - Physical

    //0x09, 0x3c, // usage
    //0x05, 0xff,
    //0x09, 0x01,
    //0x15, 0x00,
    //0x25, 0x01,
    //0x75, 0x01,
    //0x95, 0x02,
    //0xb1, 0x22,
    //0x75, 0x06,
    //0x95, 0x01,
    //0xb1, 0x01,
  //0xC0        // end collection - Application
  //};
//}}}
//{{{  hid configuration descriptor
__ALIGN_BEGIN const uint8_t kHidConfigurationDescriptor[34] __ALIGN_END = {
  9, USB_DESC_TYPE_CONFIGURATION,
  sizeof(kHidConfigurationDescriptor),0,  // wTotalLength - bytes returned
  1,     // bNumInterfaces: 1 interface
  1,     // bConfigurationValue - configuration value
  0,     // iConfiguration - index of string descriptor describing the configuration
  0xE0,  // bmAttributes: bus powered and Support Remote Wake-up
  0x32,  // MaxPower 100 mA: this current is used for detecting Vbus

  // hid interface descriptor
  9, USB_DESC_TYPE_INTERFACE,
  0,  // bInterfaceNumber - number of Interface
  0,  // bAlternateSetting - alternate setting
  1,  // bNumEndpoints
  3,  // bInterfaceClass - HID
  1,  // bInterfaceSubClass - boot = 1, boot = 0
  1,  // nInterfaceProtocol - keyboard -  none = 0, keyboard = 1, mouse = 2
  0,  // iInterface - index of string descriptor

  // HID descriptor
  9, 0x21,
  0x11,1,  // bcdHID: HID Class Spec release number
  0,       // bCountryCode: Hardware target country
  1,       // bNumDescriptors: number HID class descriptors to follow
  0x22,    // bDescriptorType
  sizeof(kHidReportDescriptor),0, // wItemLength - total length of report descriptor

  // hid endpoint descriptor
  7, USB_DESC_TYPE_ENDPOINT,
  HID_IN_ENDPOINT,       // bEndpointAddress - endpoint address (IN)
  3,                     // bmAttributes - interrupt endpoint
  HID_IN_ENDPOINT_SIZE,  // wMaxPacketSize
  0,
  10,                    // bInterval - polling interval (10 ms)
  };
//}}}
//{{{  string descriptors
#define LANGID_STRING            0x409
__ALIGN_BEGIN const uint8_t kLangIdDescriptor[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
  USB_LEN_LANGID_STR_DESC,
  USB_DESC_TYPE_STRING,
  LOBYTE(LANGID_STRING), HIBYTE(LANGID_STRING),
  };

uint8_t* langIdStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kLangIdDescriptor);
  return (uint8_t*)kLangIdDescriptor;
  }

__ALIGN_BEGIN uint8_t stringDescriptor[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

uint8_t* productStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)"product", stringDescriptor, length);
  return stringDescriptor;
  }

uint8_t* manufacturerStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)"Colin", stringDescriptor, length);
  return stringDescriptor;
  }

uint8_t* configurationStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)"config", stringDescriptor, length);
  return stringDescriptor;
  }

uint8_t* interfaceStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)"interface", stringDescriptor, length);
  return stringDescriptor;
  }

#define USB_SIZ_STRING_SERIAL    0x1A
__ALIGN_BEGIN uint8_t stringSerial[USB_SIZ_STRING_SERIAL] __ALIGN_END = {
  USB_SIZ_STRING_SERIAL,
  USB_DESC_TYPE_STRING,
  };

//{{{
void intToUnicode (uint32_t value , uint8_t* pbuf , uint8_t len) {
  uint8_t idx = 0;
  for( idx = 0 ; idx < len ; idx ++) {
    if( ((value >> 28)) < 0xA )
      pbuf[ 2* idx] = (value >> 28) + '0';
    else
      pbuf[2* idx] = (value >> 28) + 'A' - 10;
    value = value << 4;
    pbuf[ 2* idx + 1] = 0;
    }
  }
//}}}
//{{{
void getSerialNum() {
  #define DEVICE_ID1 (0x1FFF7A10)
  #define DEVICE_ID2 (0x1FFF7A14)
  #define DEVICE_ID3 (0x1FFF7A18)
  uint32_t deviceserial0 = *(uint32_t*)DEVICE_ID1;
  uint32_t deviceserial1 = *(uint32_t*)DEVICE_ID2;
  uint32_t deviceserial2 = *(uint32_t*)DEVICE_ID3;
  deviceserial0 += deviceserial2;
  if (deviceserial0 != 0) {
    intToUnicode (deviceserial0, &stringSerial[2] ,8);
    intToUnicode (deviceserial1, &stringSerial[18] ,4);
    }
  }
//}}}
uint8_t* serialStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = USB_SIZ_STRING_SERIAL;
  getSerialNum();
  return (uint8_t*)stringSerial;
  }
//}}}

//{{{  hidClass handlers
// tHidData
typedef struct {
  uint32_t mProtocol;
  uint32_t mIdleState;
  uint32_t mAltSetting;
  uint32_t mConfiguration;
  bool     mBusy;
  uint8_t  mData[USB_MAX_EP0_SIZE];
  } tHidData;

//{{{
uint8_t usbInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  // Open EP IN
  USBD_LL_OpenEP (device, HID_IN_ENDPOINT, USBD_EP_TYPE_INTR, HID_IN_ENDPOINT_SIZE);
  device->pClassData = malloc (sizeof(tHidData));
  ((tHidData*)device->pClassData)->mBusy = false;

  return 0;
  }
//}}}
//{{{
uint8_t usbDeInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  // Close HID EPs
  USBD_LL_CloseEP (device, HID_IN_ENDPOINT);
  free (device->pClassData);
  device->pClassData = NULL;

  return USBD_OK;
  }
//}}}

//{{{
uint8_t usbSetup (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  auto hidData = (tHidData*)device->pClassData;
  gApp->getLcd()->debug (LCD_COLOR_YELLOW, "usbSetup bmRq:%x bRq:%x wV:%x wL:%d",
                                            req->bmRequest, req->bRequest, req->wValue, req->wLength);

  switch (req->bmRequest & USB_REQ_TYPE_MASK) { // 0x60
    case USB_REQ_TYPE_STANDARD: // 0X00
      switch (req->bRequest) {
        case USB_REQ_GET_DESCRIPTOR: // 0x06
          if (req->wValue >> 8 == 0x21) {
            gApp->getLcd()->debug (LCD_COLOR_RED, "-getDescriptor hid - offset ok?");
            USBD_CtlSendData (device, (uint8_t*)kHidReportDescriptor+18, 9);
            }
          else if (req->wValue >> 8 == 0x22) {
            gApp->getLcd()->debug (LCD_COLOR_GREEN, "-getDescriptor report len:%d", req->wLength);
            USBD_CtlSendData (device, (uint8_t*)kHidReportDescriptor, sizeof(kHidReportDescriptor));
            }
          break;
        case USB_REQ_SET_DESCRIPTOR: // 0x07
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-setDescriptor");
          break;
        case USB_REQ_GET_CONFIGURATION : // 0x08
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-getConfiguration?");
          USBD_CtlSendData (device, (uint8_t*)&hidData->mConfiguration, 1);
          break;
        case USB_REQ_SET_CONFIGURATION:  // 0x09
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-setConfiguration?");
          hidData->mConfiguration = (uint8_t)(req->wValue);
          break;
        case USB_REQ_GET_INTERFACE: // 0x0a
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-getInterface");
          USBD_CtlSendData (device, (uint8_t*)&hidData->mAltSetting, 1);
          break;
        case USB_REQ_SET_INTERFACE: // 0x0b
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-setInterface");
          hidData->mAltSetting = (uint8_t)(req->wValue);
          break;
        }
      break;

    case USB_REQ_TYPE_CLASS: // 0x20
      switch (req->bRequest) {
        case 0x01: // getReport
          gApp->getLcd()->debug (LCD_COLOR_RED, "-getReport");
          break;
        case 0x02: // getIdle
          USBD_CtlSendData (device, (uint8_t*)&hidData->mIdleState, 1);
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-getIdle %d", hidData->mIdleState);
          break;
        case 0x03: // getProtocol
          USBD_CtlSendData (device, (uint8_t*)&hidData->mProtocol, 1);
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-getProtocol %d", hidData->mProtocol);
          break;
        case 0x09: // setReport
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-setReport v:%d wI:%x wLen:%d",
                                 req->wValue, req->wIndex, req->wLength);
          USBD_CtlPrepareRx (device, hidData->mData, req->wLength);
          break;
        case 0x0A: // setIdle
          hidData->mIdleState = (uint8_t)(req->wValue >> 8);
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-setIdle %d", req->wValue);
          break;
        case 0x0B: // setProtocol
          hidData->mProtocol = (uint8_t)(req->wValue);
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-setProtocol %d", req->wValue);
          break;
        default:
          USBD_CtlError (device, req);
          return USBD_FAIL;
        }
      break;
      }

  return USBD_OK;
  }
//}}}
//{{{
uint8_t usbEp0TxReady (USBD_HandleTypeDef* device) {

  gApp->getLcd()->debug (LCD_COLOR_YELLOW, "usbEp0TxReady");
  return USBD_OK;
  }
//}}}
//{{{
uint8_t usbEp0RxReady (USBD_HandleTypeDef* device) {

  auto hidData = (tHidData*)device->pClassData;
  gApp->getLcd()->debug (LCD_COLOR_YELLOW, "usbEp0RxReady data:%x", hidData->mData[0]);

  // D0: NUM lock
  // D1: CAPS lock
  // D2: SCROLL lock
  // D3: Compose
  // D4: Kana
  uint8_t leds = 0;
  if (hidData->mData[0] & 0x01)
    leds |= 0x02;
  if (hidData->mData[0] & 0x02)
    leds |= 0x04;
  if (hidData->mData[0] & 0x04)
    leds |= 0x01;
  gApp->getPs2()->sendLeds (leds);

  return USBD_OK;
  }
//}}}

//{{{
uint8_t* usbGetConfigurationDescriptor (uint16_t* length) {

  *length = sizeof(kHidConfigurationDescriptor);
  return (uint8_t*)kHidConfigurationDescriptor;
  }
//}}}
//{{{
uint8_t usbDataIn (USBD_HandleTypeDef* device, uint8_t epnum) {
// Ensure that the FIFO is empty before a new transfer, this condition could
// be caused by  a new transfer before the end of the previous transfer

  ((tHidData*)device->pClassData)->mBusy = false;
  gApp->getLcd()->debug (LCD_COLOR_YELLOW, "usbDataIn");
  return USBD_OK;
  }
//}}}
//{{{
uint8_t usbDataOut (USBD_HandleTypeDef* device, uint8_t epNum) {

  gApp->getLcd()->debug (LCD_COLOR_YELLOW, "usbDataOut");
  return USBD_OK;
  }
//}}}

//{{{
uint8_t* usbGetDeviceQualifierDescriptor (uint16_t* length) {

  gApp->getLcd()->debug (LCD_COLOR_YELLOW, "usbGetDeviceQualifierDescriptor");
  *length = sizeof (kHidDeviceQualifierDescriptor);
  return (uint8_t*)kHidDeviceQualifierDescriptor;
  }
//}}}
//}}}
//{{{
void hidSendKeyboard (uint8_t modifier, uint8_t code) {

  class cKeyboardHID {
  public:
    cKeyboardHID (uint8_t modifier, uint8_t code) : mModifier(modifier), mKey1(code) {}
    //uint8_t mId = 1;
    uint8_t mModifier = 0;
    uint8_t mKey1 = 0;
    uint8_t mKey2 = 0;
    uint8_t mKey3 = 0;
    uint8_t mKey4 = 0;
    uint8_t mKey5 = 0;
    uint8_t mKey6 = 0;
    };

  auto hidData = (tHidData*)gUsbDevice.pClassData;
  if (gUsbDevice.dev_state == USBD_STATE_CONFIGURED) {
    if (!hidData->mBusy) {
      hidData->mBusy = true;
      cKeyboardHID keyboardHID (modifier, code);
      USBD_LL_Transmit (&gUsbDevice, HID_IN_ENDPOINT, (uint8_t*)(&keyboardHID), HID_IN_ENDPOINT_SIZE);
      HAL_Delay (10);
      }
    else
      gApp->getLcd()->debug (LCD_COLOR_RED, "missed char %x", code);
    }
  else
    gApp->getLcd()->debug (LCD_COLOR_RED, "unconfigured");
  }
//}}}
//{{{
//void hidSendMouse (USBD_HandleTypeDef* device, uint8_t* report) {

  //auto hidData = (tHidData*)device->pClassData;
  //if (device->dev_state == USBD_STATE_CONFIGURED) {
    //if (!hidData->mBusy) {
      //hidData->mBusy = true;
      //USBD_LL_Transmit (device, HID_IN_ENDPOINT, report, HID_IN_ENDPOINT_SIZE);
      //}
    //}
  //}
//}}}
//{{{
//int32_t hidGetPollingInterval (USBD_HandleTypeDef* device) {

  //if (device->dev_speed == USBD_SPEED_HIGH)
    //// high speed endpoints, values between 1..16 are allowed. Values correspond to interval/ of 2 ^ (bInterval-1).
    //return (uint32_t)(((1 << (7 - 1)))/8);
  //else
    //// low, full speed endpoints
    //return (uint32_t)10;
  //}
//}}}

// init usbDevice library

//{{{
void cApp::run (bool keyboard) {

  mAud = BSP_PB_GetState (BUTTON_KEY);

  // init lcd
  mLcd = new cLcd (12);
  mLcd->init();

  if (!mAud) {
    //{{{  init ps2 keyboard
    mPs2 = new cPs2 (mLcd);
    if (keyboard)
      mPs2->initKeyboard();
    else
      mPs2->initTouchpad();
    }
    //}}}

  // init usb
  //{{{  audClass
  USBD_ClassTypeDef audClass = {
    audUsbInit,
    audUsbDeInit,
    audUsbSetup,
    audUsbEp0TxReady,
    audUsbEp0RxReady,
    audUsbDataIn,
    audUsbDataOut,
    audUsbSof,
    audUsbIsoInInComplete,
    audUsbIsoOutInComplete,
    audUsbGetConfigDescriptor,
    audUsbGetConfigDescriptor,
    audUsbGetConfigDescriptor,
    audUsbGetDeviceQualifierDescriptor,
    };
  //}}}
  //{{{  audDescriptor
  USBD_DescriptorsTypeDef audDescriptor = {
    audDeviceDescriptor,
    langIdStringDescriptor,
    manufacturerStringDescriptor,
    productStringDescriptor,
    serialStringDescriptor,
    audConfigurationStringDescriptor,
    interfaceStringDescriptor,
    };
  //}}}
  //{{{  hidClass
  USBD_ClassTypeDef hidClass = {
    usbInit,
    usbDeInit,
    usbSetup,
    usbEp0TxReady,
    usbEp0RxReady,
    usbDataIn,
    usbDataOut,
    NULL, // usbSof
    NULL, // usbIsoInInComplete
    NULL, // usbIsoOutInComplete
    usbGetConfigurationDescriptor,
    usbGetConfigurationDescriptor,
    usbGetConfigurationDescriptor,
    usbGetDeviceQualifierDescriptor,
    };
  //}}}
  //{{{  hidDescriptor
  USBD_DescriptorsTypeDef hidDescriptor = {
    deviceDescriptor,
    langIdStringDescriptor,
    manufacturerStringDescriptor,
    productStringDescriptor,
    serialStringDescriptor,
    configurationStringDescriptor,
    interfaceStringDescriptor,
    };
  //}}}

  gUsbDevice.pClassData = NULL;
  if (mAud) {
    USBD_Init (&gUsbDevice, &audDescriptor, 0);
    USBD_RegisterClass (&gUsbDevice, &audClass);
    }
  else {
    USBD_Init (&gUsbDevice, &hidDescriptor, 0);
    USBD_RegisterClass (&gUsbDevice, &hidClass);
    }
  USBD_Start (&gUsbDevice);

  while (true) {
    //printf ("hello colin %d\n", HAL_GetTick());

    pollTouch();

    mLcd->start (mAud ? kAudVersion : kHidVersion);

    if (mAud) {
      gPackets.show();
      show();
      }
    else {
      if (keyboard)
        mPs2->showChars();
      else
        mPs2->showTouch();
      mPs2->showCodes();
      mPs2->showWave();
      }
    mLcd->present();

    if (keyboard) {
      while (mPs2->hasRawChar())
        mPs2->getRawChar();
      while (mPs2->hasChar()) {
        auto ch = mPs2->getChar();
        hidSendKeyboard (ch >> 8, ch & 0xFF);
        mLcd->debug (ch & 0x100 ? LCD_COLOR_GREEN : LCD_COLOR_YELLOW, "sendHid %02x:%02x", ch >> 8, ch & 0xFF);
        }
      }
    }
  }
//}}}
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
    mLcd->debug (LCD_COLOR_GREEN, "onMove %d %d %d", x, y, z);
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
  gApp->run (true);
  }
//}}}
