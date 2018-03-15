#pragma once
//{{{  includes
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//}}}
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}
//{{{  usbd defines
#define  SWAPBYTE(addr)  (((uint16_t)(*((uint8_t *)(addr)))) + (((uint16_t)(*(((uint8_t *)(addr)) + 1))) << 8))

#define LOBYTE(x)  ((uint8_t)(x & 0x00FF))
#define HIBYTE(x)  ((uint8_t)((x & 0xFF00) >>8))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#define __weak   __attribute__((weak))
#define __packed  __attribute__((__packed__))
#define __ALIGN_END  __attribute__ ((aligned (4)))

#define DEVICE_ID1  (0x1FFF7A10)
#define DEVICE_ID2  (0x1FFF7A14)
#define DEVICE_ID3  (0x1FFF7A18)
#define USB_SIZ_STRING_SERIAL 0x1A

#define USBD_MAX_NUM_INTERFACES     1
#define USBD_MAX_NUM_CONFIGURATION  1
#define USBD_MAX_STR_DESC_SIZ       0x100
#define USBD_SUPPORT_USER_STRING    0
#define USBD_SELF_POWERED           1
#define USBD_DEBUG_LEVEL            0

#define USB_LEN_DEV_QUALIFIER_DESC  0x0A
#define USB_LEN_DEV_DESC            0x12
#define USB_LEN_CFG_DESC            0x09
#define USB_LEN_IF_DESC             0x09
#define USB_LEN_EP_DESC             0x07
#define USB_LEN_OTG_DESC            0x03
#define USB_LEN_LANGID_STR_DESC     0x04
#define USB_LEN_OTHER_SPEED_DESC_SIZ0x09

#define USBD_IDX_LANGID_STR         0x00
#define USBD_IDX_MFC_STR            0x01
#define USBD_IDX_PRODUCT_STR        0x02
#define USBD_IDX_SERIAL_STR         0x03
#define USBD_IDX_CONFIG_STR         0x04
#define USBD_IDX_INTERFACE_STR      0x05

#define USB_REQ_TYPE_STANDARD       0x00
#define USB_REQ_TYPE_CLASS          0x20
#define USB_REQ_TYPE_VENDOR         0x40
#define USB_REQ_TYPE_MASK           0x60

#define USB_REQ_RECIPIENT_DEVICE    0x00
#define USB_REQ_RECIPIENT_INTERFACE 0x01
#define USB_REQ_RECIPIENT_ENDPOINT  0x02
#define USB_REQ_RECIPIENT_MASK      0x03

#define USB_REQ_GET_STATUS          0x00
#define USB_REQ_CLEAR_FEATURE       0x01
#define USB_REQ_SET_FEATURE         0x03
#define USB_REQ_SET_ADDRESS         0x05
#define USB_REQ_GET_DESCRIPTOR      0x06
#define USB_REQ_SET_DESCRIPTOR      0x07
#define USB_REQ_GET_CONFIGURATION   0x08
#define USB_REQ_SET_CONFIGURATION   0x09
#define USB_REQ_GET_INTERFACE       0x0A
#define USB_REQ_SET_INTERFACE       0x0B
#define USB_REQ_SYNCH_FRAME         0x0C

#define USB_DESC_TYPE_DEVICE                      1
#define USB_DESC_TYPE_CONFIGURATION               2
#define USB_DESC_TYPE_STRING                      3
#define USB_DESC_TYPE_INTERFACE                   4
#define USB_DESC_TYPE_ENDPOINT                    5
#define USB_DESC_TYPE_DEVICE_QUALIFIER            6
#define USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION   7
#define USB_DESC_TYPE_BOS                         0x0F

#define USB_CONFIG_REMOTE_WAKEUP   2
#define USB_CONFIG_SELF_POWERED    1

#define USB_FEATURE_EP_HALT        0
#define USB_FEATURE_REMOTE_WAKEUP  1
#define USB_FEATURE_TEST_MODE      2

#define USB_DEVICE_CAPABITY_TYPE   0x10

#define USB_HS_MAX_PACKET_SIZE     512
#define USB_FS_MAX_PACKET_SIZE     64
#define USB_MAX_EP0_SIZE           64

/*  Device Status */
#define USBD_STATE_DEFAULT         1
#define USBD_STATE_ADDRESSED       2
#define USBD_STATE_CONFIGURED      3
#define USBD_STATE_SUSPENDED       4

/*  EP0 State */
#define USBD_EP0_IDLE              0
#define USBD_EP0_SETUP             1
#define USBD_EP0_DATA_IN           2
#define USBD_EP0_DATA_OUT          3
#define USBD_EP0_STATUS_IN         4
#define USBD_EP0_STATUS_OUT        5
#define USBD_EP0_STALL             6

#define USBD_EP_TYPE_CTRL          0
#define USBD_EP_TYPE_ISOC          1
#define USBD_EP_TYPE_BULK          2
#define USBD_EP_TYPE_INTR          3
//}}}
typedef enum { USBD_OK = 0, USBD_BUSY, USBD_FAIL } USBD_StatusTypeDef;
typedef enum { USBD_SPEED_HIGH = 0, USBD_SPEED_FULL  = 1, USBD_SPEED_LOW   = 2 } USBD_SpeedTypeDef;

//{{{  struct  USBD_SetupReqTypedef
typedef struct usb_setup_req {
  uint8_t   bmRequest;
  uint8_t   bRequest;
  uint16_t  wValue;
  uint16_t  wIndex;
  uint16_t  wLength;
  } USBD_SetupReqTypedef;
//}}}
struct _USBD_HandleTypeDef;
//{{{  struct USBD_ClassTypeDef
typedef struct _Device_cb {
  uint8_t (*Init)             (struct _USBD_HandleTypeDef* usbdHandle, uint8_t cfgidx);
  uint8_t (*DeInit)           (struct _USBD_HandleTypeDef* usbdHandle, uint8_t cfgidx);

  // Control Endpoints
  uint8_t (*Setup)            (struct _USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req);
  uint8_t (*EP0_TxSent)       (struct _USBD_HandleTypeDef* usbdHandle);
  uint8_t (*EP0_RxReady)      (struct _USBD_HandleTypeDef* usbdHandle);

  // Class Specific Endpoints
  uint8_t (*DataIn)           (struct _USBD_HandleTypeDef* usbdHandle, uint8_t endPointNum);
  uint8_t (*DataOut)          (struct _USBD_HandleTypeDef* usbdHandle, uint8_t endPointNum);
  uint8_t (*SOF)              (struct _USBD_HandleTypeDef* usbdHandle);
  uint8_t (*IsoINIncomplete)  (struct _USBD_HandleTypeDef* usbdHandle, uint8_t endPointNum);
  uint8_t (*IsoOUTIncomplete) (struct _USBD_HandleTypeDef* usbdHandle, uint8_t endPointNum);

  // descriptors
  uint8_t* (*GetHSConfigDescriptor)(uint16_t* length);
  uint8_t* (*GetFSConfigDescriptor)(uint16_t* length);
  uint8_t* (*GetOtherSpeedConfigDescriptor)(uint16_t* length);
  uint8_t* (*GetDeviceQualifierDescriptor)(uint16_t* length);
#if (USBD_SUPPORT_USER_STRING == 1)
  uint8_t* (*GetUsrStrDescriptor)(struct _USBD_HandleTypeDef* usbdHandle, uint8_t index, uint16_t* length);
#endif
  } USBD_ClassTypeDef;
//}}}
//{{{  struct USBD_DescriptorsTypeDef
typedef struct {
  uint8_t* (*GetDeviceDescriptor)( USBD_SpeedTypeDef speed , uint16_t *length);
  uint8_t* (*GetLangIDStrDescriptor)( USBD_SpeedTypeDef speed , uint16_t *length);
  uint8_t* (*GetManufacturerStrDescriptor)( USBD_SpeedTypeDef speed , uint16_t *length);
  uint8_t* (*GetProductStrDescriptor)( USBD_SpeedTypeDef speed , uint16_t *length);
  uint8_t* (*GetSerialStrDescriptor)( USBD_SpeedTypeDef speed , uint16_t *length);
  uint8_t* (*GetConfigurationStrDescriptor)( USBD_SpeedTypeDef speed , uint16_t *length);
  uint8_t* (*GetInterfaceStrDescriptor)( USBD_SpeedTypeDef speed , uint16_t *length);
#if (USBD_LPM_ENABLED == 1)
  uint8_t* (*GetBOSDescriptor)( USBD_SpeedTypeDef speed , uint16_t *length);
#endif
  } USBD_DescriptorsTypeDef;
//}}}
//{{{  struct USBD_EndpointTypeDef
typedef struct {
  uint32_t status;
  uint32_t total_length;
  uint32_t rem_length;
  uint32_t maxpacket;
  } USBD_EndpointTypeDef;
//}}}
//{{{  struct USBD_HandleTypeDef
typedef struct _USBD_HandleTypeDef {
  uint8_t  id;
  uint32_t dev_config;
  uint32_t dev_default_config;
  uint32_t dev_config_status;
  USBD_SpeedTypeDef dev_speed;

  USBD_EndpointTypeDef ep_in[15];
  USBD_EndpointTypeDef ep_out[15];

  uint32_t ep0_state;
  uint32_t ep0_data_len;
  uint8_t  dev_state;
  uint8_t  dev_old_state;
  uint8_t  dev_address;
  uint8_t  dev_connection_status;
  uint8_t  dev_test_mode;
  uint32_t dev_remote_wakeup;

  USBD_SetupReqTypedef request;
  USBD_DescriptorsTypeDef* pDesc;
  USBD_ClassTypeDef* pClass;

  void* pClassData;
  void* pUserData;
  void* pData;
  } USBD_HandleTypeDef;
//}}}

// USBD Low Level Driver
USBD_StatusTypeDef usbdLowLevelInit (USBD_HandleTypeDef* usbdHandle);
//{{{
inline uint8_t usbdLowLevelIsStallEP (USBD_HandleTypeDef* usbdHandle, uint8_t ep_addr) {
  auto pcdHandle = (PCD_HandleTypeDef*)usbdHandle->pData;
  if ((ep_addr & 0x80) == 0x80)
    return pcdHandle->IN_ep[ep_addr & 0xF].is_stall;
  else
    return pcdHandle->OUT_ep[ep_addr & 0xF].is_stall;
  }
//}}}
//{{{
inline uint32_t usbdLowLevelGetRxDataSize (USBD_HandleTypeDef* usbdHandle, uint8_t ep_addr) {
  return HAL_PCD_EP_GetRxCount ((PCD_HandleTypeDef*)usbdHandle->pData, ep_addr);
  }
//}}}
//{{{
inline USBD_StatusTypeDef usbdLowLevelOpenEP( USBD_HandleTypeDef* usbdHandle, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps) {
  HAL_PCD_EP_Open ((PCD_HandleTypeDef*)usbdHandle->pData, ep_addr, ep_mps, ep_type);
  return USBD_OK;
  }
//}}}
//{{{
inline USBD_StatusTypeDef usbdLowLevelCloseEP (USBD_HandleTypeDef* usbdHandle, uint8_t ep_addr) {
  HAL_PCD_EP_Close ((PCD_HandleTypeDef*)usbdHandle->pData, ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
inline USBD_StatusTypeDef usbdLowLevelFlushEP (USBD_HandleTypeDef* usbdHandle, uint8_t ep_addr) {
  HAL_PCD_EP_Flush ((PCD_HandleTypeDef*)usbdHandle->pData, ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
inline USBD_StatusTypeDef usbdLowLevelStallEP (USBD_HandleTypeDef* usbdHandle, uint8_t ep_addr) {
  HAL_PCD_EP_SetStall ((PCD_HandleTypeDef*)usbdHandle->pData, ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
inline USBD_StatusTypeDef usbdLowLevelClearStallEP (USBD_HandleTypeDef* usbdHandle, uint8_t ep_addr) {
  HAL_PCD_EP_ClrStall ((PCD_HandleTypeDef*)usbdHandle->pData, ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
inline USBD_StatusTypeDef usbdLowLevelSetUSBAddress (USBD_HandleTypeDef* usbdHandle, uint8_t dev_addr) {
  HAL_PCD_SetAddress ((PCD_HandleTypeDef*)usbdHandle->pData, dev_addr);
  return USBD_OK;
  }
//}}}
//{{{
inline USBD_StatusTypeDef usbdLowLevelTransmit (USBD_HandleTypeDef* usbdHandle, uint8_t ep_addr, uint8_t* pbuf, uint16_t size) {
  HAL_PCD_EP_Transmit ((PCD_HandleTypeDef*)usbdHandle->pData, ep_addr, pbuf, size);
  return USBD_OK;
  }
//}}}
//{{{
inline USBD_StatusTypeDef usbdLowLevelPrepareReceive (USBD_HandleTypeDef* usbdHandle, uint8_t ep_addr, uint8_t* pbuf, uint16_t size) {
  HAL_PCD_EP_Receive ((PCD_HandleTypeDef*)usbdHandle->pData, ep_addr, pbuf, size);
  return USBD_OK;
  }
//}}}

void USBD_CtlError (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req);
void USBD_GetString (uint8_t* desc, uint8_t* unicode, uint16_t* len);

USBD_StatusTypeDef USBD_CtlSendData (USBD_HandleTypeDef* usbdHandle, uint8_t* buf, uint16_t len);
USBD_StatusTypeDef USBD_CtlContinueSendData (USBD_HandleTypeDef* usbdHandle, uint8_t* pbuf, uint16_t len);
USBD_StatusTypeDef USBD_CtlPrepareRx (USBD_HandleTypeDef* usbdHandle, uint8_t* pbuf, uint16_t len);
USBD_StatusTypeDef USBD_CtlContinueRx (USBD_HandleTypeDef* usbdHandle, uint8_t* pbuf, uint16_t len);
USBD_StatusTypeDef USBD_CtlSendStatus (USBD_HandleTypeDef* usbdHandle);
USBD_StatusTypeDef USBD_CtlReceiveStatus (USBD_HandleTypeDef* usbdHandle);
uint16_t USBD_GetRxCount (USBD_HandleTypeDef* usbdHandle , uint8_t endPointNum);

USBD_StatusTypeDef usbdLowLevelSetupStage (USBD_HandleTypeDef* usbdHandle, uint8_t* psetup);
USBD_StatusTypeDef usbdLowLevelDataOutStage (USBD_HandleTypeDef* usbdHandle , uint8_t endPointNum, uint8_t* pdata);
USBD_StatusTypeDef usbdLowLevelDataInStage (USBD_HandleTypeDef* usbdHandle , uint8_t endPointNum, uint8_t* pdata);
USBD_StatusTypeDef usbdLowLevelReset (USBD_HandleTypeDef* usbdHandle);
USBD_StatusTypeDef usbdLowLevelSetSpeed (USBD_HandleTypeDef* usbdHandle, USBD_SpeedTypeDef speed);
USBD_StatusTypeDef usbdLowLevelSuspend (USBD_HandleTypeDef* usbdHandle);
USBD_StatusTypeDef usbdLowLevelResume (USBD_HandleTypeDef* usbdHandle);
USBD_StatusTypeDef usbdLowLevelSOF (USBD_HandleTypeDef* usbdHandle);
USBD_StatusTypeDef usbdLowLevelIsoINIncomplete (USBD_HandleTypeDef* usbdHandle, uint8_t endPointNum);
USBD_StatusTypeDef usbdLowLevelIsoOUTIncomplete (USBD_HandleTypeDef* usbdHandle, uint8_t endPointNum);
USBD_StatusTypeDef usbdLowLevelDevConnected (USBD_HandleTypeDef* usbdHandle);
USBD_StatusTypeDef usbdLowLevelDevDisconnected (USBD_HandleTypeDef* usbdHandle);

USBD_StatusTypeDef USBD_Init (USBD_HandleTypeDef* usbdHandle, USBD_DescriptorsTypeDef* pdesc, uint8_t id);
USBD_StatusTypeDef USBD_DeInit (USBD_HandleTypeDef* usbdHandle);
USBD_StatusTypeDef USBD_Start (USBD_HandleTypeDef* usbdHandle);
USBD_StatusTypeDef USBD_Stop (USBD_HandleTypeDef* usbdHandle);
USBD_StatusTypeDef USBD_RegisterClass (USBD_HandleTypeDef* usbdHandle, USBD_ClassTypeDef* pclass);
USBD_StatusTypeDef USBD_SetClassConfig (USBD_HandleTypeDef* usbdHandle, uint8_t cfgidx);
USBD_StatusTypeDef USBD_ClrClassConfig (USBD_HandleTypeDef* usbdHandle, uint8_t cfgidx);

//{{{
#ifdef __cplusplus
}
#endif
//}}}
