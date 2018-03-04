#pragma once
//{{{  includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//}}}
//{{{  defines
//{{{
#ifndef __weak
  #define __weak   __attribute__((weak))
#endif /* __weak */
//}}}
//{{{
#ifndef __packed
  #define __packed __attribute__((__packed__))
#endif /* __packed */
//}}}
#define __ALIGN_END __attribute__ ((aligned (4)))
#define __ALIGN_BEGIN

#define  SWAPBYTE(addr) (((uint16_t)(*((uint8_t *)(addr)))) + \
                        (((uint16_t)(*(((uint8_t *)(addr)) + 1))) << 8))

#define LOBYTE(x) ((uint8_t)(x & 0x00FF))
#define HIBYTE(x) ((uint8_t)((x & 0xFF00) >>8))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define USBD_MAX_NUM_INTERFACES     1
#define USBD_MAX_NUM_CONFIGURATION  1
#define USBD_MAX_STR_DESC_SIZ       0x100
#define USBD_SELF_POWERED           1
#define USBD_DEBUG_LEVEL            0

//{{{  len
#define USB_LEN_DEV_QUALIFIER_DESC     0x0A
#define USB_LEN_DEV_DESC               0x12
#define USB_LEN_CFG_DESC               0x09
#define USB_LEN_IF_DESC                0x09
#define USB_LEN_EP_DESC                0x07
#define USB_LEN_OTG_DESC               0x03
#define USB_LEN_LANGID_STR_DESC        0x04
#define USB_LEN_OTHER_SPEED_DESC_SIZ   0x09
//}}}
//{{{  idx
#define  USBD_IDX_LANGID_STR            0x00
#define  USBD_IDX_MFC_STR               0x01
#define  USBD_IDX_PRODUCT_STR           0x02
#define  USBD_IDX_SERIAL_STR            0x03
#define  USBD_IDX_CONFIG_STR            0x04
#define  USBD_IDX_INTERFACE_STR         0x05
//}}}
//{{{  req
#define  USB_REQ_TYPE_STANDARD          0x00
#define  USB_REQ_TYPE_CLASS             0x20
#define  USB_REQ_TYPE_VENDOR            0x40
#define  USB_REQ_TYPE_MASK              0x60

#define  USB_REQ_RECIPIENT_DEVICE       0x00
#define  USB_REQ_RECIPIENT_INTERFACE    0x01
#define  USB_REQ_RECIPIENT_ENDPOINT     0x02
#define  USB_REQ_RECIPIENT_MASK         0x03

#define  USB_REQ_GET_STATUS             0x00
#define  USB_REQ_CLEAR_FEATURE          0x01
#define  USB_REQ_SET_FEATURE            0x03
#define  USB_REQ_SET_ADDRESS            0x05
#define  USB_REQ_GET_DESCRIPTOR         0x06
#define  USB_REQ_SET_DESCRIPTOR         0x07
#define  USB_REQ_GET_CONFIGURATION      0x08
#define  USB_REQ_SET_CONFIGURATION      0x09
#define  USB_REQ_GET_INTERFACE          0x0A
#define  USB_REQ_SET_INTERFACE          0x0B
#define  USB_REQ_SYNCH_FRAME            0x0C
//}}}
//{{{  descriptor type
#define  USB_DESC_TYPE_DEVICE              1
#define  USB_DESC_TYPE_CONFIGURATION       2
#define  USB_DESC_TYPE_STRING              3
#define  USB_DESC_TYPE_INTERFACE           4
#define  USB_DESC_TYPE_ENDPOINT            5
#define  USB_DESC_TYPE_DEVICE_QUALIFIER    6
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION  7
#define  USB_DESC_TYPE_BOS                 0x0F
//}}}
#define USB_CONFIG_REMOTE_WAKEUP   2
#define USB_CONFIG_SELF_POWERED    1
//{{{  feature
#define USB_FEATURE_EP_HALT        0
#define USB_FEATURE_REMOTE_WAKEUP  1
#define USB_FEATURE_TEST_MODE      2
//}}}

#define USB_DEVICE_CAPABITY_TYPE         0x10

#define USB_HS_MAX_PACKET_SIZE  512
#define USB_FS_MAX_PACKET_SIZE  64
#define USB_MAX_EP0_SIZE        64

/*  Device Status */
#define USBD_STATE_DEFAULT     1
#define USBD_STATE_ADDRESSED   2
#define USBD_STATE_CONFIGURED  3
#define USBD_STATE_SUSPENDED   4

//{{{  endpoint
/*  EP0 State */
#define USBD_EP0_IDLE        0
#define USBD_EP0_SETUP       1
#define USBD_EP0_DATA_IN     2
#define USBD_EP0_DATA_OUT    3
#define USBD_EP0_STATUS_IN   4
#define USBD_EP0_STATUS_OUT  5
#define USBD_EP0_STALL       6

#define USBD_EP_TYPE_CTRL    0
#define USBD_EP_TYPE_ISOC    1
#define USBD_EP_TYPE_BULK    2
#define USBD_EP_TYPE_INTR    3
//}}}
//}}}
//{{{
#ifdef __cplusplus
  extern "C" {
#endif
//}}}

typedef enum { USBD_OK = 0, USBD_BUSY, USBD_FAIL, } USBD_StatusTypeDef;
typedef enum { USBD_SPEED_HIGH = 0, USBD_SPEED_FULL= 1, USBD_SPEED_LOW = 2, } USBD_SpeedTypeDef;
//{{{  struct USBD_SetupReqTypedef
typedef struct usb_setup_req {
  uint8_t bmRequest;
  uint8_t bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
  } USBD_SetupReqTypedef;
//}}}
//{{{  struct USBD_ClassTypeDef
struct _USBD_HandleTypeDef;

typedef struct _Device_cb {
  uint8_t (*Init)             (struct _USBD_HandleTypeDef* device, uint8_t cfgidx);
  uint8_t (*DeInit)           (struct _USBD_HandleTypeDef* device, uint8_t cfgidx);

 /* Control Endpoints*/
  uint8_t (*Setup)            (struct _USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req);
  uint8_t (*EP0_TxSent)       (struct _USBD_HandleTypeDef* device);
  uint8_t (*EP0_RxReady)      (struct _USBD_HandleTypeDef* device);

  /* Class Specific Endpoints*/
  uint8_t (*DataIn)           (struct _USBD_HandleTypeDef* device, uint8_t epnum);
  uint8_t (*DataOut)          (struct _USBD_HandleTypeDef* device, uint8_t epnum);
  uint8_t (*SOF)              (struct _USBD_HandleTypeDef* device);
  uint8_t (*IsoINIncomplete)  (struct _USBD_HandleTypeDef* device, uint8_t epnum);
  uint8_t (*IsoOUTIncomplete) (struct _USBD_HandleTypeDef* device, uint8_t epnum);

  uint8_t* (*GetHSConfigDescriptor)(uint16_t* length);
  uint8_t* (*GetFSConfigDescriptor)(uint16_t* length);
  uint8_t* (*GetOtherSpeedConfigDescriptor)(uint16_t* length);
  uint8_t* (*GetDeviceQualifierDescriptor)(uint16_t* length);
  } USBD_ClassTypeDef;
//}}}
//{{{  struct USBD_DescriptorsTypeDef
/* USB Device descriptors structure */
typedef struct {
  uint8_t* (*GetDeviceDescriptor)(USBD_SpeedTypeDef speed, uint16_t* length);
  uint8_t* (*GetLangIDStrDescriptor)(USBD_SpeedTypeDef speed, uint16_t* length);
  uint8_t* (*GetManufacturerStrDescriptor)(USBD_SpeedTypeDef speed , uint16_t* length);
  uint8_t* (*GetProductStrDescriptor)(USBD_SpeedTypeDef speed, uint16_t* length);
  uint8_t* (*GetSerialStrDescriptor)(USBD_SpeedTypeDef speed, uint16_t* length);
  uint8_t* (*GetConfigurationStrDescriptor)(USBD_SpeedTypeDef speed, uint16_t* length);
  uint8_t* (*GetInterfaceStrDescriptor)(USBD_SpeedTypeDef speed, uint16_t* length);
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
  uint8_t                 id;
  uint32_t                dev_config;
  uint32_t                dev_default_config;
  uint32_t                dev_config_status;
  USBD_SpeedTypeDef       dev_speed;
  USBD_EndpointTypeDef    ep_in[15];
  USBD_EndpointTypeDef    ep_out[15];
  uint32_t                ep0_state;
  uint32_t                ep0_data_len;
  uint8_t                 dev_state;
  uint8_t                 dev_old_state;
  uint8_t                 dev_address;
  uint8_t                 dev_connection_status;
  uint8_t                 dev_test_mode;
  uint32_t                dev_remote_wakeup;

  USBD_SetupReqTypedef    request;
  USBD_DescriptorsTypeDef *pDesc;
  USBD_ClassTypeDef       *pClass;
  void                    *pClassData;
  void                    *pUserData;
  void                    *pData;
  } USBD_HandleTypeDef;
//}}}

USBD_HandleTypeDef gUsbDevice;
static PCD_HandleTypeDef gPcdHandle;
void OTG_FS_IRQHandler() { HAL_PCD_IRQHandler (&gPcdHandle); }

//{{{
uint8_t USBD_LL_IsStallEP (USBD_HandleTypeDef* device, uint8_t ep_addr) {

  auto pcdHandle = (PCD_HandleTypeDef*)device->pData;
  if ((ep_addr & 0x80) == 0x80)
    return pcdHandle->IN_ep[ep_addr & 0x7F].is_stall;
  else
    return pcdHandle->OUT_ep[ep_addr & 0x7F].is_stall;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_OpenEP (USBD_HandleTypeDef* device, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps) {
  HAL_PCD_EP_Open ((PCD_HandleTypeDef*)device->pData, ep_addr, ep_mps, ep_type);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_CloseEP (USBD_HandleTypeDef* device, uint8_t ep_addr) {
  HAL_PCD_EP_Close ((PCD_HandleTypeDef*)device->pData, ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_FlushEP (USBD_HandleTypeDef* device, uint8_t ep_addr) {
  HAL_PCD_EP_Flush ((PCD_HandleTypeDef*)device->pData, ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_StallEP (USBD_HandleTypeDef* device, uint8_t ep_addr) {
  HAL_PCD_EP_SetStall ((PCD_HandleTypeDef*)device->pData, ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_ClearStallEP (USBD_HandleTypeDef* device, uint8_t ep_addr) {
  HAL_PCD_EP_ClrStall ((PCD_HandleTypeDef*)device->pData, ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_Transmit (USBD_HandleTypeDef* device, uint8_t ep_addr, uint8_t* pbuf, uint16_t size) {
  HAL_PCD_EP_Transmit ((PCD_HandleTypeDef*)device->pData, ep_addr, pbuf, size);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_PrepareReceive (USBD_HandleTypeDef* device, uint8_t ep_addr, uint8_t* pbuf, uint16_t size) {
  HAL_PCD_EP_Receive ((PCD_HandleTypeDef*)device->pData, ep_addr, pbuf, size);
  return USBD_OK;
  }
//}}}

//{{{
void USBD_CtlError (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  USBD_LL_StallEP (device, 0x80);
  USBD_LL_StallEP (device, 0);
  }
//}}}
//{{{
void USBD_GetString (uint8_t* desc, uint8_t* unicode, uint16_t* len) {

  uint8_t idx = 0;
  if (desc != NULL) {
    auto buf = desc;
    uint8_t length = 0;
    while (*buf != '\0') {
      length++;
      buf++;
      }

    *len = length*2 + 2;
    unicode[idx++] = *len;
    unicode[idx++] =  USB_DESC_TYPE_STRING;

    while (*desc != '\0') {
      unicode[idx++] = *desc++;
      unicode[idx++] =  0x00;
      }
    }
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_CtlSendData (USBD_HandleTypeDef* device, uint8_t* pbuf, uint16_t len) {

  // Set EP0 State
  device->ep0_state = USBD_EP0_DATA_IN;
  device->ep_in[0].total_length = len;
  device->ep_in[0].rem_length = len;

  // Start the transfer
  USBD_LL_Transmit (device, 0x00, pbuf, len);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_CtlContinueSendData (USBD_HandleTypeDef* device, uint8_t* pbuf, uint16_t len) {

  // Start the next transfer
  USBD_LL_Transmit (device, 0x00, pbuf, len);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_CtlPrepareRx (USBD_HandleTypeDef* device, uint8_t* pbuf, uint16_t len) {

  // Set EP0 State
  device->ep0_state = USBD_EP0_DATA_OUT;
  device->ep_out[0].total_length = len;
  device->ep_out[0].rem_length = len;

  // Start the transfer
  USBD_LL_PrepareReceive (device, 0, pbuf, len);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_CtlContinueRx (USBD_HandleTypeDef* device, uint8_t *pbuf, uint16_t len) {

  USBD_LL_PrepareReceive (device, 0, pbuf, len);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_CtlSendStatus (USBD_HandleTypeDef* device) {

  // Set EP0 State
  device->ep0_state = USBD_EP0_STATUS_IN;

  // Start the transfer
  USBD_LL_Transmit (device, 0x00, NULL, 0);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_CtlReceiveStatus (USBD_HandleTypeDef* device) {

  // Set EP0 State
  device->ep0_state = USBD_EP0_STATUS_OUT;

  // Start the transfer
  USBD_LL_PrepareReceive (device, 0, NULL, 0);

  return USBD_OK;
  }
//}}}
//{{{
uint16_t USBD_GetRxCount (USBD_HandleTypeDef* device, uint8_t ep_addr) {
  return HAL_PCD_EP_GetRxCount ((PCD_HandleTypeDef*)device->pData, ep_addr);
  }
//}}}

//{{{
USBD_StatusTypeDef USBD_Init (USBD_HandleTypeDef* device, USBD_DescriptorsTypeDef* pdesc, uint8_t id) {

  device->pDesc = pdesc;

  // Set Device initial State
  device->dev_state = USBD_STATE_DEFAULT;
  device->id = id;

  // Set LL Driver parameters
  gPcdHandle.Instance = USB_OTG_FS;
  gPcdHandle.Init.dev_endpoints = 4;
  gPcdHandle.Init.use_dedicated_ep1 = 0;
  gPcdHandle.Init.ep0_mps = 0x40;
  gPcdHandle.Init.dma_enable = 0;
  gPcdHandle.Init.low_power_enable = 0;
  gPcdHandle.Init.phy_itface = PCD_PHY_EMBEDDED;
  gPcdHandle.Init.Sof_enable = 0;
  gPcdHandle.Init.speed = PCD_SPEED_FULL;
  gPcdHandle.Init.vbus_sensing_enable = 0;
  gPcdHandle.Init.lpm_enable = 0;

  // Link The driver to the stack
  gPcdHandle.pData = device;
  device->pData = &gPcdHandle;

  // Initialize LL Driver
  HAL_PCD_Init (&gPcdHandle);
  HAL_PCDEx_SetRxFiFo (&gPcdHandle, 0x80);
  //HAL_PCDEx_SetTxFiFo (&gPcdHandle, 0, 0x60);
  HAL_PCDEx_SetTxFiFo (&gPcdHandle, 0, 0x60);
  HAL_PCDEx_SetTxFiFo (&gPcdHandle, 1, 0x80);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_DeInit (USBD_HandleTypeDef* device) {

  // Set Default State
  device->dev_state = USBD_STATE_DEFAULT;

  // Free Class Resources
  device->pClass->DeInit (device, device->dev_config);

  // Stop the low level driver
  HAL_PCD_Stop ((PCD_HandleTypeDef*)device->pData);

  // Initialize low level driver
  HAL_PCD_DeInit ((PCD_HandleTypeDef*)device->pData);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_RegisterClass (USBD_HandleTypeDef* device, USBD_ClassTypeDef* pclass) {

  // link the class to the USB Device handle
  device->pClass = pclass;
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_Start (USBD_HandleTypeDef* device) {

  HAL_PCD_Start ((PCD_HandleTypeDef*)device->pData);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_Stop (USBD_HandleTypeDef* device) {

  // Free Class Resources
  device->pClass->DeInit (device, device->dev_config);

  // Stop the low level driver
  HAL_PCD_Stop ((PCD_HandleTypeDef*)device->pData);
  return USBD_OK;
  }
//}}}

//{{{
void HAL_PCD_MspInit (PCD_HandleTypeDef* pcdHandle) {

  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();

  // Configure DM DP Pins
  GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);

  // Enable USB FS Clock
  __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

  // Set USBFS Interrupt priority
  HAL_NVIC_SetPriority (OTG_FS_IRQn, 5, 0);

  // Enable USBFS Interrupt
  HAL_NVIC_EnableIRQ (OTG_FS_IRQn);
  }
//}}}
//{{{
void HAL_PCD_MspDeInit (PCD_HandleTypeDef* pcdHandle) {

  // Disable USB FS Clock
  __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
  __HAL_RCC_SYSCFG_CLK_DISABLE();
  }
//}}}

//{{{
void USBD_GetDescriptor (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  uint16_t len;
  uint8_t* pbuf;
  switch (req->wValue >> 8) {
    //{{{
    case USB_DESC_TYPE_DEVICE:
      pbuf = device->pDesc->GetDeviceDescriptor (device->dev_speed, &len);
      break;
    //}}}
    //{{{
    case USB_DESC_TYPE_CONFIGURATION:
      if (device->dev_speed == USBD_SPEED_HIGH) {
        pbuf = (uint8_t*)device->pClass->GetHSConfigDescriptor (&len);
        pbuf[1] = USB_DESC_TYPE_CONFIGURATION;
        }
      else {
        pbuf = (uint8_t*)device->pClass->GetFSConfigDescriptor (&len);
        pbuf[1] = USB_DESC_TYPE_CONFIGURATION;
        }
      break;
    //}}}
    //{{{
    case USB_DESC_TYPE_STRING:
      switch ((uint8_t)(req->wValue)) {
        //{{{
        case USBD_IDX_LANGID_STR:
          pbuf = device->pDesc->GetLangIDStrDescriptor(device->dev_speed, &len);
          break;
        //}}}
        //{{{
        case USBD_IDX_MFC_STR:
          pbuf = device->pDesc->GetManufacturerStrDescriptor (device->dev_speed, &len);
          break;
        //}}}
        //{{{
        case USBD_IDX_PRODUCT_STR:
          pbuf = device->pDesc->GetProductStrDescriptor (device->dev_speed, &len);
          break;
        //}}}
        //{{{
        case USBD_IDX_SERIAL_STR:
          pbuf = device->pDesc->GetSerialStrDescriptor (device->dev_speed, &len);
          break;
        //}}}
        //{{{
        case USBD_IDX_CONFIG_STR:
          pbuf = device->pDesc->GetConfigurationStrDescriptor (device->dev_speed, &len);
          break;
        //}}}
        //{{{
        case USBD_IDX_INTERFACE_STR:
          pbuf = device->pDesc->GetInterfaceStrDescriptor (device->dev_speed, &len);
          break;
        //}}}
        //{{{
        default:
          USBD_CtlError (device , req);
          return;
        //}}}
        }
      break;
    //}}}
    //{{{
    case USB_DESC_TYPE_DEVICE_QUALIFIER:
      if (device->dev_speed == USBD_SPEED_HIGH) {
        pbuf = (uint8_t *)device->pClass->GetDeviceQualifierDescriptor(&len);
        break;
        }
      else {
        USBD_CtlError(device , req);
        return;
        }
    //}}}
    //{{{
    case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
      if (device->dev_speed == USBD_SPEED_HIGH) {
        pbuf = (uint8_t *)device->pClass->GetOtherSpeedConfigDescriptor(&len);
        pbuf[1] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
        break;
        }
      else {
        USBD_CtlError(device , req);
        return;
        }
    //}}}
    //{{{
    default:
      USBD_CtlError(device , req);
      return;
    //}}}
    }

  if ((len != 0) && (req->wLength != 0)) {
    len = MIN(len, req->wLength);
    USBD_CtlSendData (device, pbuf, len);
    }
  }
//}}}
//{{{
void USBD_SetAddress (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  if ((req->wIndex == 0) && (req->wLength == 0)) {
    auto device_addr = (uint8_t)(req->wValue) & 0x7F;
    if (device->dev_state == USBD_STATE_CONFIGURED)
      USBD_CtlError (device, req);
    else {
      device->dev_address = device_addr;
      HAL_PCD_SetAddress ((PCD_HandleTypeDef*)device->pData, device_addr);
      USBD_CtlSendStatus (device);

      if (device_addr != 0)
        device->dev_state = USBD_STATE_ADDRESSED;
      else
        device->dev_state = USBD_STATE_DEFAULT;
      }
    }
  else
    USBD_CtlError (device, req);
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_SetClassConfig (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  if (device->pClass != NULL)
    if (device->pClass->Init (device, cfgidx) == 0)
      return USBD_OK;

  return USBD_FAIL;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_ClrClassConfig (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  // Clear configuration  and De-initialize the Class process
  device->pClass->DeInit (device, cfgidx);
  return USBD_OK;
  }
//}}}
//{{{
void USBD_SetConfig (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  auto cfgidx = (uint8_t)(req->wValue);
  if (cfgidx > USBD_MAX_NUM_CONFIGURATION )
    USBD_CtlError (device, req);
  else {
    switch (device->dev_state) {
      case USBD_STATE_ADDRESSED:
        if (cfgidx) {
          device->dev_config = cfgidx;
          device->dev_state = USBD_STATE_CONFIGURED;
          if (USBD_SetClassConfig (device , cfgidx) == USBD_FAIL) {
            USBD_CtlError (device , req);
            return;
            }
          USBD_CtlSendStatus (device);
          }
        else
          USBD_CtlSendStatus (device);
        break;

      case USBD_STATE_CONFIGURED:
        if (cfgidx == 0) {
          device->dev_state = USBD_STATE_ADDRESSED;
          device->dev_config = cfgidx;
          USBD_ClrClassConfig (device , cfgidx);
          USBD_CtlSendStatus (device);
          }
        else  if (cfgidx != device->dev_config) {
          /* Clear old configuration */
          USBD_ClrClassConfig (device , device->dev_config);

          /* set new configuration */
          device->dev_config = cfgidx;
          if (USBD_SetClassConfig (device , cfgidx) == USBD_FAIL) {
            USBD_CtlError (device , req);
            return;
            }
          USBD_CtlSendStatus (device);
          }
        else
          USBD_CtlSendStatus (device);
        break;

      default:
        USBD_CtlError (device , req);
        break;
      }
    }
  }
//}}}
//{{{
void USBD_GetConfig (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  if (req->wLength != 1)
    USBD_CtlError (device , req);
  else {
    switch (device->dev_state) {
      case USBD_STATE_ADDRESSED:
        device->dev_default_config = 0;
        USBD_CtlSendData (device, (uint8_t *)&device->dev_default_config, 1);
        break;

      case USBD_STATE_CONFIGURED:
        USBD_CtlSendData (device, (uint8_t *)&device->dev_config, 1);
        break;

      default:
        USBD_CtlError (device , req);
        break;
      }
   }
  }
//}}}
//{{{
void USBD_GetStatus (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  switch (device->dev_state) {
    case USBD_STATE_ADDRESSED:
    case USBD_STATE_CONFIGURED:
    #if (USBD_SELF_POWERED == 1)
      device->dev_config_status = USB_CONFIG_SELF_POWERED;
    #else
      device->dev_config_status = 0;
    #endif
      if (device->dev_remote_wakeup)
        device->dev_config_status |= USB_CONFIG_REMOTE_WAKEUP;
      USBD_CtlSendData (device, (uint8_t *)& device->dev_config_status, 2);
      break;

    default :
      USBD_CtlError (device , req);
      break;
    }
  }
//}}}
//{{{
void USBD_SetFeature (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  if (req->wValue == USB_FEATURE_REMOTE_WAKEUP) {
    device->dev_remote_wakeup = 1;
    device->pClass->Setup (device, req);
    USBD_CtlSendStatus (device);
    }
  }
//}}}
//{{{
void USBD_ClrFeature (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  switch (device->dev_state) {
    case USBD_STATE_ADDRESSED:
    case USBD_STATE_CONFIGURED:
      if (req->wValue == USB_FEATURE_REMOTE_WAKEUP) {
        device->dev_remote_wakeup = 0;
        device->pClass->Setup (device, req);
        USBD_CtlSendStatus (device);
        }
      break;

    default :
      USBD_CtlError (device , req);
      break;
    }
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_StdDevReq (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  switch (req->bRequest) {
    case USB_REQ_GET_DESCRIPTOR:    USBD_GetDescriptor (device, req); break;
    case USB_REQ_SET_ADDRESS:       USBD_SetAddress (device, req); break;
    case USB_REQ_SET_CONFIGURATION: USBD_SetConfig (device, req); break;
    case USB_REQ_GET_CONFIGURATION: USBD_GetConfig (device, req); break;
    case USB_REQ_GET_STATUS:        USBD_GetStatus (device, req); break;
    case USB_REQ_SET_FEATURE:       USBD_SetFeature (device, req); break;
    case USB_REQ_CLEAR_FEATURE:     USBD_ClrFeature (device, req); break;
    default: USBD_CtlError (device, req);
    }

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_StdItfReq (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  switch (device->dev_state) {
    case USBD_STATE_CONFIGURED:
      if (LOBYTE(req->wIndex) <= USBD_MAX_NUM_INTERFACES) {
        device->pClass->Setup (device, req);
        if (req->wLength == 0)
          USBD_CtlSendStatus (device);
        }
      else
        USBD_CtlError (device, req);
      break;

    default:
      USBD_CtlError (device, req);
      break;
    }

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_StdEPReq (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  // Check if it is a class request
  if ((req->bmRequest & 0x60) == 0x20) {
    device->pClass->Setup (device, req);
    return USBD_OK;
    }

  USBD_StatusTypeDef ret = USBD_OK;
  uint8_t ep_addr = LOBYTE(req->wIndex);
  switch (req->bRequest) {
    //{{{
    case USB_REQ_SET_FEATURE :
      switch (device->dev_state) {
        case USBD_STATE_ADDRESSED:
          if ((ep_addr != 0x00) && (ep_addr != 0x80))
            USBD_LL_StallEP (device, ep_addr);
          break;

        case USBD_STATE_CONFIGURED:
          if (req->wValue == USB_FEATURE_EP_HALT)
            if ((ep_addr != 0x00) && (ep_addr != 0x80))
              USBD_LL_StallEP (device , ep_addr);
          device->pClass->Setup (device, req);
          USBD_CtlSendStatus (device);
          break;

        default:
          USBD_CtlError(device , req);
          break;
        }
      break;
    //}}}
    //{{{
    case USB_REQ_CLEAR_FEATURE :
      switch (device->dev_state) {
        case USBD_STATE_ADDRESSED:
          if ((ep_addr != 0x00) && (ep_addr != 0x80))
            USBD_LL_StallEP (device, ep_addr);
          break;

        case USBD_STATE_CONFIGURED:
          if (req->wValue == USB_FEATURE_EP_HALT) {
            if ((ep_addr & 0x7F) != 0x00) {
              USBD_LL_ClearStallEP(device , ep_addr);
              device->pClass->Setup (device, req);
              }
            USBD_CtlSendStatus(device);
            }
          break;

        default:
          USBD_CtlError(device , req);
          break;
        }
      break;
    //}}}
    //{{{
    case USB_REQ_GET_STATUS:
      switch (device->dev_state) {
      case USBD_STATE_ADDRESSED:
        if ((ep_addr & 0x7F) != 0x00)
          USBD_LL_StallEP (device , ep_addr);
        break;

      case USBD_STATE_CONFIGURED: {
        auto pep = ((ep_addr & 0x80) == 0x80) ? &device->ep_in[ep_addr & 0x7F]: &device->ep_out[ep_addr & 0x7F];
        if (USBD_LL_IsStallEP (device, ep_addr))
          pep->status = 0x0001;
        else
          pep->status = 0x0000;

        USBD_CtlSendData (device, (uint8_t*)&pep->status, 2);
        break;
        }

      default:
        USBD_CtlError (device , req);
        break;
      }
      break;
    //}}}
    default:
      break;
    }

  return ret;
  }
//}}}
//{{{
void HAL_PCD_SetupStageCallback (PCD_HandleTypeDef* pcdHandle) {

  auto device = (USBD_HandleTypeDef*)pcdHandle->pData;
  auto psetup = (uint8_t*)pcdHandle->Setup;

  device->request.bmRequest = *(uint8_t*)(psetup);
  device->request.bRequest = *(uint8_t*)(psetup +  1);
  device->request.wValue = SWAPBYTE (psetup +  2);
  device->request.wIndex = SWAPBYTE (psetup +  4);
  device->request.wLength = SWAPBYTE (psetup +  6);

  device->ep0_state = USBD_EP0_SETUP;
  device->ep0_data_len = device->request.wLength;

  switch (device->request.bmRequest & 0x1F) {
    case USB_REQ_RECIPIENT_DEVICE:
      USBD_StdDevReq (device, &device->request);
      break;

    case USB_REQ_RECIPIENT_INTERFACE:
      USBD_StdItfReq (device, &device->request);
      break;

    case USB_REQ_RECIPIENT_ENDPOINT:
      USBD_StdEPReq (device, &device->request);
      break;

    default:
      USBD_LL_StallEP (device, device->request.bmRequest & 0x80);
      break;
    }
  }
//}}}
//{{{
void HAL_PCD_DataOutStageCallback (PCD_HandleTypeDef* pcdHandle, uint8_t epnum) {

  auto device = (USBD_HandleTypeDef*)pcdHandle->pData;
  auto pdata = pcdHandle->OUT_ep[epnum].xfer_buff;

  if (epnum == 0) {
    auto pep = &device->ep_out[0];
    if (device->ep0_state == USBD_EP0_DATA_OUT) {
      if (pep->rem_length > pep->maxpacket) {
        pep->rem_length -= pep->maxpacket;
        USBD_CtlContinueRx (device, pdata, MIN(pep->rem_length ,pep->maxpacket));
        }
      else {
        if ((device->pClass->EP0_RxReady != NULL) && (device->dev_state == USBD_STATE_CONFIGURED))
          device->pClass->EP0_RxReady (device);
        USBD_CtlSendStatus (device);
        }
      }
    }
  else if ((device->pClass->DataOut != NULL) && (device->dev_state == USBD_STATE_CONFIGURED))
    device->pClass->DataOut (device, epnum);
  }
//}}}
//{{{
void HAL_PCD_DataInStageCallback( PCD_HandleTypeDef* pcdHandle, uint8_t epnum) {

  auto device = (USBD_HandleTypeDef*)pcdHandle->pData;
  auto pdata = pcdHandle->IN_ep[epnum].xfer_buff;

  if (epnum == 0) {
    auto pep = &device->ep_in[0];
    if (device->ep0_state == USBD_EP0_DATA_IN) {
      if (pep->rem_length > pep->maxpacket) {
        pep->rem_length -=  pep->maxpacket;
        USBD_CtlContinueSendData (device, pdata, pep->rem_length);
        // Prepare endpoint for premature end of transfer
        USBD_LL_PrepareReceive (device, 0, NULL, 0);
        }
      else {
        // last packet is MPS multiple, so send ZLP packet
        if ((pep->total_length % pep->maxpacket == 0) &&
            (pep->total_length >= pep->maxpacket) &&
            (pep->total_length < device->ep0_data_len )) {
          USBD_CtlContinueSendData (device , NULL, 0);
          device->ep0_data_len = 0;

          // Prepare endpoint for premature end of transfer
          USBD_LL_PrepareReceive (device, 0, NULL, 0);
          }
        else {
          if ((device->pClass->EP0_TxSent != NULL) && (device->dev_state == USBD_STATE_CONFIGURED))
            device->pClass->EP0_TxSent (device);
          USBD_CtlReceiveStatus (device);
          }
        }
      }

    if (device->dev_test_mode == 1) {
      //USBD_RunTestMode (device);
      device->dev_test_mode = 0;
      }
    }
  else if ((device->pClass->DataIn != NULL) && (device->dev_state == USBD_STATE_CONFIGURED))
    device->pClass->DataIn (device, epnum);
  }
//}}}
//{{{
void HAL_PCD_SOFCallback (PCD_HandleTypeDef* pcdHandle) {

  auto device = (USBD_HandleTypeDef*)pcdHandle->pData;
  if (device->dev_state == USBD_STATE_CONFIGURED)
    if (device->pClass->SOF != NULL)
      device->pClass->SOF (device);
  }
//}}}
//{{{
void HAL_PCD_ResetCallback (PCD_HandleTypeDef* pcdHandle) {

  auto speed = USBD_SPEED_FULL;

  // Set USB Current Speed
  switch (pcdHandle->Init.speed) {
    case PCD_SPEED_HIGH: speed = USBD_SPEED_HIGH; break;
    case PCD_SPEED_FULL: speed = USBD_SPEED_FULL; break;
    default: speed = USBD_SPEED_FULL; break;
    }

  // Reset Device
  auto device = (USBD_HandleTypeDef*)pcdHandle->pData;

  // Open EP0 OUT
  USBD_LL_OpenEP (device, 0x00, USBD_EP_TYPE_CTRL, USB_MAX_EP0_SIZE);
  device->ep_out[0].maxpacket = USB_MAX_EP0_SIZE;

  // Open EP0 IN
  USBD_LL_OpenEP (device, 0x80, USBD_EP_TYPE_CTRL, USB_MAX_EP0_SIZE);
  device->ep_in[0].maxpacket = USB_MAX_EP0_SIZE;

  // Upon Reset call user call back
  device->dev_state = USBD_STATE_DEFAULT;

  if (device->pClassData)
    device->pClass->DeInit (device, device->dev_config);

  // setSpeed
  device->dev_speed = speed;
  }
//}}}
//{{{
void HAL_PCD_SuspendCallback (PCD_HandleTypeDef* pcdHandle) {

  auto device = (USBD_HandleTypeDef*)pcdHandle->pData;
  device->dev_old_state =  device->dev_state;
  device->dev_state  = USBD_STATE_SUSPENDED;
  __HAL_PCD_GATE_PHYCLOCK (pcdHandle);
  }
//}}}
//{{{
void HAL_PCD_ResumeCallback (PCD_HandleTypeDef* pcdHandle) {
  __HAL_PCD_UNGATE_PHYCLOCK (pcdHandle);
  auto device = (USBD_HandleTypeDef*)pcdHandle->pData;
  device->dev_state = device->dev_old_state;
  }
//}}}
void HAL_PCD_ConnectCallback (PCD_HandleTypeDef* pcdHandle) {}
//{{{
void HAL_PCD_DisconnectCallback (PCD_HandleTypeDef* pcdHandle) {
  auto device = (USBD_HandleTypeDef*)pcdHandle->pData;
  device->dev_state = USBD_STATE_DEFAULT;
  device->pClass->DeInit (device, device->dev_config);
  }
//}}}
void HAL_PCD_ISOOUTIncompleteCallback (PCD_HandleTypeDef* pcdHandle, uint8_t epnum) {}
void HAL_PCD_ISOINIncompleteCallback (PCD_HandleTypeDef* pcdHandle, uint8_t epnum) {}

//{{{
#ifdef __cplusplus
  }
#endif
//}}}
