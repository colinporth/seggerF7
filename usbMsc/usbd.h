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
#define __ALIGN_BEGIN

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

#define  USB_LEN_DEV_QUALIFIER_DESC  0x0A
#define  USB_LEN_DEV_DESC            0x12
#define  USB_LEN_CFG_DESC            0x09
#define  USB_LEN_IF_DESC             0x09
#define  USB_LEN_EP_DESC             0x07
#define  USB_LEN_OTG_DESC            0x03
#define  USB_LEN_LANGID_STR_DESC     0x04
#define  USB_LEN_OTHER_SPEED_DESC_SIZ0x09

#define  USBD_IDX_LANGID_STR         0x00
#define  USBD_IDX_MFC_STR            0x01
#define  USBD_IDX_PRODUCT_STR        0x02
#define  USBD_IDX_SERIAL_STR         0x03
#define  USBD_IDX_CONFIG_STR         0x04
#define  USBD_IDX_INTERFACE_STR      0x05

#define  USB_REQ_TYPE_STANDARD       0x00
#define  USB_REQ_TYPE_CLASS          0x20
#define  USB_REQ_TYPE_VENDOR         0x40
#define  USB_REQ_TYPE_MASK           0x60

#define  USB_REQ_RECIPIENT_DEVICE    0x00
#define  USB_REQ_RECIPIENT_INTERFACE 0x01
#define  USB_REQ_RECIPIENT_ENDPOINT  0x02
#define  USB_REQ_RECIPIENT_MASK      0x03

#define  USB_REQ_GET_STATUS          0x00
#define  USB_REQ_CLEAR_FEATURE       0x01
#define  USB_REQ_SET_FEATURE         0x03
#define  USB_REQ_SET_ADDRESS         0x05
#define  USB_REQ_GET_DESCRIPTOR      0x06
#define  USB_REQ_SET_DESCRIPTOR      0x07
#define  USB_REQ_GET_CONFIGURATION   0x08
#define  USB_REQ_SET_CONFIGURATION   0x09
#define  USB_REQ_GET_INTERFACE       0x0A
#define  USB_REQ_SET_INTERFACE       0x0B
#define  USB_REQ_SYNCH_FRAME         0x0C

#define  USB_DESC_TYPE_DEVICE                      1
#define  USB_DESC_TYPE_CONFIGURATION               2
#define  USB_DESC_TYPE_STRING                      3
#define  USB_DESC_TYPE_INTERFACE                   4
#define  USB_DESC_TYPE_ENDPOINT                    5
#define  USB_DESC_TYPE_DEVICE_QUALIFIER            6
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION   7
#define  USB_DESC_TYPE_BOS                         0x0F

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
typedef enum { USBD_OK = 0, USBD_BUSY, USBD_FAIL, } USBD_StatusTypeDef;
typedef enum { USBD_SPEED_HIGH = 0, USBD_SPEED_FULL  = 1, USBD_SPEED_LOW   = 2, } USBD_SpeedTypeDef;
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
  uint8_t  (*Init)             (struct _USBD_HandleTypeDef *pdev , uint8_t cfgidx);
  uint8_t  (*DeInit)           (struct _USBD_HandleTypeDef *pdev , uint8_t cfgidx);
 /* Control Endpoints*/
  uint8_t  (*Setup)            (struct _USBD_HandleTypeDef *pdev , USBD_SetupReqTypedef  *req);
  uint8_t  (*EP0_TxSent)       (struct _USBD_HandleTypeDef *pdev );
  uint8_t  (*EP0_RxReady)      (struct _USBD_HandleTypeDef *pdev );
  /* Class Specific Endpoints*/
  uint8_t  (*DataIn)           (struct _USBD_HandleTypeDef *pdev , uint8_t epnum);
  uint8_t  (*DataOut)          (struct _USBD_HandleTypeDef *pdev , uint8_t epnum);
  uint8_t  (*SOF)              (struct _USBD_HandleTypeDef *pdev);
  uint8_t  (*IsoINIncomplete)  (struct _USBD_HandleTypeDef *pdev , uint8_t epnum);
  uint8_t  (*IsoOUTIncomplete) (struct _USBD_HandleTypeDef *pdev , uint8_t epnum);

  uint8_t  *(*GetHSConfigDescriptor)(uint16_t *length);
  uint8_t  *(*GetFSConfigDescriptor)(uint16_t *length);
  uint8_t  *(*GetOtherSpeedConfigDescriptor)(uint16_t *length);
  uint8_t  *(*GetDeviceQualifierDescriptor)(uint16_t *length);
#if (USBD_SUPPORT_USER_STRING == 1)
  uint8_t  *(*GetUsrStrDescriptor)(struct _USBD_HandleTypeDef *pdev ,uint8_t index,  uint16_t *length);
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
USBD_StatusTypeDef USBD_LL_Init (USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_LL_DeInit (USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_LL_Stop (USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_LL_OpenEP  (USBD_HandleTypeDef* pdev, uint8_t  ep_addr, uint8_t  ep_type, uint16_t ep_mps);
USBD_StatusTypeDef USBD_LL_CloseEP (USBD_HandleTypeDef* pdev, uint8_t ep_addr);
USBD_StatusTypeDef USBD_LL_FlushEP (USBD_HandleTypeDef* pdev, uint8_t ep_addr);
USBD_StatusTypeDef USBD_LL_StallEP (USBD_HandleTypeDef* pdev, uint8_t ep_addr);
USBD_StatusTypeDef USBD_LL_ClearStallEP (USBD_HandleTypeDef* pdev, uint8_t ep_addr);
uint8_t            USBD_LL_IsStallEP (USBD_HandleTypeDef* pdev, uint8_t ep_addr);
USBD_StatusTypeDef USBD_LL_SetUSBAddress (USBD_HandleTypeDef* pdev, uint8_t dev_addr);
USBD_StatusTypeDef USBD_LL_Transmit (USBD_HandleTypeDef* pdev, uint8_t  ep_addr, uint8_t * pbuf, uint16_t  size);
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef* pdev, uint8_t  ep_addr, uint8_t * pbuf, uint16_t  size);
uint32_t USBD_LL_GetRxDataSize  (USBD_HandleTypeDef* pdev, uint8_t  ep_addr);
void USBD_LL_Delay (uint32_t Delay);

//
void USBD_CtlError (USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req);
void USBD_GetString (uint8_t* desc, uint8_t* unicode, uint16_t* len);

USBD_StatusTypeDef USBD_CtlSendData (USBD_HandleTypeDef* pdev, uint8_t* buf, uint16_t len);
USBD_StatusTypeDef USBD_CtlContinueSendData (USBD_HandleTypeDef* pdev, uint8_t* pbuf, uint16_t len);
USBD_StatusTypeDef USBD_CtlPrepareRx (USBD_HandleTypeDef* pdev, uint8_t* pbuf, uint16_t len);
USBD_StatusTypeDef USBD_CtlContinueRx (USBD_HandleTypeDef* pdev, uint8_t* pbuf, uint16_t len);
USBD_StatusTypeDef USBD_CtlSendStatus (USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_CtlReceiveStatus (USBD_HandleTypeDef* pdev);
uint16_t USBD_GetRxCount (USBD_HandleTypeDef* pdev , uint8_t epnum);

USBD_StatusTypeDef USBD_LL_SetupStage (USBD_HandleTypeDef* pdev, uint8_t* psetup);
USBD_StatusTypeDef USBD_LL_DataOutStage (USBD_HandleTypeDef* pdev , uint8_t epnum, uint8_t* pdata);
USBD_StatusTypeDef USBD_LL_DataInStage (USBD_HandleTypeDef* pdev , uint8_t epnum, uint8_t* pdata);
USBD_StatusTypeDef USBD_LL_Reset (USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_LL_SetSpeed (USBD_HandleTypeDef* pdev, USBD_SpeedTypeDef speed);
USBD_StatusTypeDef USBD_LL_Suspend (USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_LL_Resume (USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_LL_SOF (USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_LL_IsoINIncomplete (USBD_HandleTypeDef* pdev, uint8_t epnum);
USBD_StatusTypeDef USBD_LL_IsoOUTIncomplete (USBD_HandleTypeDef* pdev, uint8_t epnum);
USBD_StatusTypeDef USBD_LL_DevConnected (USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_LL_DevDisconnected (USBD_HandleTypeDef* pdev);

USBD_StatusTypeDef USBD_Init (USBD_HandleTypeDef* pdev, USBD_DescriptorsTypeDef* pdesc, uint8_t id);
USBD_StatusTypeDef USBD_DeInit(USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_Start (USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_Stop (USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_RegisterClass (USBD_HandleTypeDef* pdev, USBD_ClassTypeDef* pclass);
USBD_StatusTypeDef USBD_RunTestMode (USBD_HandleTypeDef* pdev);
USBD_StatusTypeDef USBD_SetClassConfig (USBD_HandleTypeDef* pdev, uint8_t cfgidx);
USBD_StatusTypeDef USBD_ClrClassConfig (USBD_HandleTypeDef* pdev, uint8_t cfgidx);

//{{{
#ifdef __cplusplus
}
#endif
//}}}
