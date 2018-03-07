// usbd.cpp
#include "usbd.h"
#include "../common/stm32746g_discovery_sd.h"

//{{{
void USBD_SetConfig (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req) {

  static uint8_t cfgidx = (uint8_t)(req->wValue);

  if (cfgidx > USBD_MAX_NUM_CONFIGURATION )
    USBD_CtlError(usbdHandle , req);
  else {
    switch (usbdHandle->dev_state) {
      case USBD_STATE_ADDRESSED:
        if (cfgidx) {
          usbdHandle->dev_config = cfgidx;
          usbdHandle->dev_state = USBD_STATE_CONFIGURED;
          if(USBD_SetClassConfig(usbdHandle , cfgidx) == USBD_FAIL) {
            USBD_CtlError(usbdHandle , req);
            return;
            }
          USBD_CtlSendStatus(usbdHandle);
          }
        else
           USBD_CtlSendStatus(usbdHandle);
        break;

      case USBD_STATE_CONFIGURED:
        if (cfgidx == 0) {
          usbdHandle->dev_state = USBD_STATE_ADDRESSED;
          usbdHandle->dev_config = cfgidx;
          USBD_ClrClassConfig(usbdHandle , cfgidx);
          USBD_CtlSendStatus(usbdHandle);
          }
        else  if (cfgidx != usbdHandle->dev_config) {
          /* Clear old configuration */
          USBD_ClrClassConfig(usbdHandle , usbdHandle->dev_config);

          /* set new configuration */
          usbdHandle->dev_config = cfgidx;
          if(USBD_SetClassConfig(usbdHandle , cfgidx) == USBD_FAIL) {
            USBD_CtlError(usbdHandle , req);
            return;
            }
          USBD_CtlSendStatus(usbdHandle);
          }
        else
          USBD_CtlSendStatus(usbdHandle);
        break;

      default:
         USBD_CtlError(usbdHandle , req);
        break;
        }
    }
  }
//}}}
//{{{
void USBD_GetConfig (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req) {

  if (req->wLength != 1)
    USBD_CtlError (usbdHandle , req);

  else {
    switch (usbdHandle->dev_state ) {
      case USBD_STATE_ADDRESSED:
        usbdHandle->dev_default_config = 0;
        USBD_CtlSendData (usbdHandle, (uint8_t *)&usbdHandle->dev_default_config, 1);
        break;

      case USBD_STATE_CONFIGURED:
        USBD_CtlSendData (usbdHandle, (uint8_t *)&usbdHandle->dev_config, 1);
        break;

      default:
         USBD_CtlError (usbdHandle , req);
        break;
      }
   }
  }
//}}}
//{{{
void USBD_GetStatus (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req) {

  switch (usbdHandle->dev_state) {
    case USBD_STATE_ADDRESSED:
    case USBD_STATE_CONFIGURED:
    #if ( USBD_SELF_POWERED == 1)
      usbdHandle->dev_config_status = USB_CONFIG_SELF_POWERED;
    #else
      usbdHandle->dev_config_status = 0;
    #endif

      if (usbdHandle->dev_remote_wakeup)
        usbdHandle->dev_config_status |= USB_CONFIG_REMOTE_WAKEUP;

      USBD_CtlSendData (usbdHandle, (uint8_t *)& usbdHandle->dev_config_status, 2);
      break;

    default :
      USBD_CtlError(usbdHandle , req);
      break;
   }
  }
//}}}
//{{{
void USBD_SetFeature (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req) {

  if (req->wValue == USB_FEATURE_REMOTE_WAKEUP) {
    usbdHandle->dev_remote_wakeup = 1;
    usbdHandle->pClass->Setup (usbdHandle, req);
    USBD_CtlSendStatus(usbdHandle);
  }
}
//}}}
//{{{
void USBD_ClrFeature (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req) {

  switch (usbdHandle->dev_state) {
    case USBD_STATE_ADDRESSED:
    case USBD_STATE_CONFIGURED:
      if (req->wValue == USB_FEATURE_REMOTE_WAKEUP) {
        usbdHandle->dev_remote_wakeup = 0;
        usbdHandle->pClass->Setup (usbdHandle, req);
        USBD_CtlSendStatus(usbdHandle);
        }
      break;

    default :
       USBD_CtlError(usbdHandle , req);
      break;
    }
  }
//}}}
//{{{
void USBD_GetDescriptor (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req) {

  uint16_t len;
  uint8_t *pbuf;
  switch (req->wValue >> 8) {
    #if (USBD_LPM_ENABLED == 1)
    //{{{
    case USB_DESC_TYPE_BOS:
      pbuf = usbdHandle->pDesc->GetBOSDescriptor (usbdHandle->dev_speed, &len);
      break;
    //}}}
    #endif
    //{{{
    case USB_DESC_TYPE_DEVICE:
      pbuf = usbdHandle->pDesc->GetDeviceDescriptor(usbdHandle->dev_speed, &len);
      break;
    //}}}
    //{{{
    case USB_DESC_TYPE_CONFIGURATION:
      if(usbdHandle->dev_speed == USBD_SPEED_HIGH ) {
        pbuf   = (uint8_t *)usbdHandle->pClass->GetHSConfigDescriptor(&len);
        pbuf[1] = USB_DESC_TYPE_CONFIGURATION;
        }
      else {
        pbuf   = (uint8_t *)usbdHandle->pClass->GetFSConfigDescriptor(&len);
        pbuf[1] = USB_DESC_TYPE_CONFIGURATION;
        }
      break;
    //}}}
    //{{{
    case USB_DESC_TYPE_STRING:
      switch ((uint8_t)(req->wValue)) {
        //{{{
        case USBD_IDX_LANGID_STR:
          pbuf = usbdHandle->pDesc->GetLangIDStrDescriptor(usbdHandle->dev_speed, &len);
          break;
        //}}}
        //{{{
        case USBD_IDX_MFC_STR:
          pbuf = usbdHandle->pDesc->GetManufacturerStrDescriptor(usbdHandle->dev_speed, &len);
          break;
        //}}}
        //{{{
        case USBD_IDX_PRODUCT_STR:
          pbuf = usbdHandle->pDesc->GetProductStrDescriptor(usbdHandle->dev_speed, &len);
          break;
        //}}}
        //{{{
        case USBD_IDX_SERIAL_STR:
          pbuf = usbdHandle->pDesc->GetSerialStrDescriptor(usbdHandle->dev_speed, &len);
          break;
        //}}}
        //{{{
        case USBD_IDX_CONFIG_STR:
          pbuf = usbdHandle->pDesc->GetConfigurationStrDescriptor(usbdHandle->dev_speed, &len);
          break;
        //}}}
        //{{{
        case USBD_IDX_INTERFACE_STR:
          pbuf = usbdHandle->pDesc->GetInterfaceStrDescriptor(usbdHandle->dev_speed, &len);
          break;
        //}}}
        //{{{
        default:
        #if (USBD_SUPPORT_USER_STRING == 1)
          pbuf = usbdHandle->pClass->GetUsrStrDescriptor(usbdHandle, (req->wValue) , &len);
          break;
        #else
          USBD_CtlError(usbdHandle , req);
          return;
        #endif
        //}}}
        }
      break;
    //}}}
    //{{{
    case USB_DESC_TYPE_DEVICE_QUALIFIER:
      if (usbdHandle->dev_speed == USBD_SPEED_HIGH  ) {
        pbuf = (uint8_t *)usbdHandle->pClass->GetDeviceQualifierDescriptor(&len);
        break;
        }
      else {
        USBD_CtlError (usbdHandle , req);
        return;
        }
    //}}}
    //{{{
    case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
      if (usbdHandle->dev_speed == USBD_SPEED_HIGH  ) {
        pbuf   = (uint8_t *)usbdHandle->pClass->GetOtherSpeedConfigDescriptor(&len);
        pbuf[1] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
        break;
        }
      else {
        USBD_CtlError (usbdHandle , req);
        return;
      }
    //}}}
    //{{{
    default:
      USBD_CtlError(usbdHandle , req);
      return;
    //}}}
    }

  if ((len != 0)&& (req->wLength != 0)) {
    len = MIN(len , req->wLength);
    USBD_CtlSendData (usbdHandle, pbuf, len);
    }
  }
//}}}
//{{{
void USBD_SetAddress (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req) {

  uint8_t  dev_addr;
  if ((req->wIndex == 0) && (req->wLength == 0)) {
    dev_addr = (uint8_t)(req->wValue) & 0x7F;

    if (usbdHandle->dev_state == USBD_STATE_CONFIGURED)
      USBD_CtlError(usbdHandle , req);
    else {
      usbdHandle->dev_address = dev_addr;
      usbdLowLevelSetUSBAddress (usbdHandle, dev_addr);
      USBD_CtlSendStatus (usbdHandle);
      if (dev_addr != 0)
        usbdHandle->dev_state  = USBD_STATE_ADDRESSED;
      else
        usbdHandle->dev_state  = USBD_STATE_DEFAULT;
      }
    }
  else
    USBD_CtlError(usbdHandle , req);
  }
//}}}
//{{{
uint8_t USBD_GetLen (uint8_t* buf) {

  uint8_t  len = 0;
  while (*buf != '\0') {
    len++;
    buf++;
    }

  return len;
  }
//}}}

//{{{
USBD_StatusTypeDef USBD_StdDevReq (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req)
{
  USBD_StatusTypeDef ret = USBD_OK;

  switch (req->bRequest) {
    case USB_REQ_GET_DESCRIPTOR:
      USBD_GetDescriptor (usbdHandle, req) ;
      break;

    case USB_REQ_SET_ADDRESS:
      USBD_SetAddress(usbdHandle, req);
      break;

    case USB_REQ_SET_CONFIGURATION:
      USBD_SetConfig (usbdHandle , req);
      break;

    case USB_REQ_GET_CONFIGURATION:
      USBD_GetConfig (usbdHandle , req);
      break;

    case USB_REQ_GET_STATUS:
      USBD_GetStatus (usbdHandle , req);
      break;

    case USB_REQ_SET_FEATURE:
      USBD_SetFeature (usbdHandle , req);
      break;

    case USB_REQ_CLEAR_FEATURE:
      USBD_ClrFeature (usbdHandle , req);
      break;

    default:
      USBD_CtlError(usbdHandle , req);
      break;
    }

  return ret;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_StdItfReq (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req) {

  USBD_StatusTypeDef ret = USBD_OK;

  switch (usbdHandle->dev_state) {
    case USBD_STATE_CONFIGURED:
      if (LOBYTE(req->wIndex) <= USBD_MAX_NUM_INTERFACES) {
        usbdHandle->pClass->Setup (usbdHandle, req);
        if((req->wLength == 0)&& (ret == USBD_OK))
           USBD_CtlSendStatus(usbdHandle);
        }
      else
         USBD_CtlError(usbdHandle , req);
      break;

    default:
      USBD_CtlError(usbdHandle , req);
      break;
    }

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_StdEPReq (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req) {

  uint8_t ep_addr;
  USBD_StatusTypeDef ret = USBD_OK;
  USBD_EndpointTypeDef   *pep;
  ep_addr = LOBYTE(req->wIndex);

  /* Check if it is a class request */
  if ((req->bmRequest & 0x60) == 0x20) {
    usbdHandle->pClass->Setup (usbdHandle, req);
    return USBD_OK;
    }

  switch (req->bRequest) {
    case USB_REQ_SET_FEATURE :
      switch (usbdHandle->dev_state) {
        //{{{
        case USBD_STATE_ADDRESSED:
          if ((ep_addr != 0x00) && (ep_addr != 0x80))
            usbdLowLevelStallEP(usbdHandle , ep_addr);
          break;
        //}}}
        //{{{
        case USBD_STATE_CONFIGURED:
          if (req->wValue == USB_FEATURE_EP_HALT) {
            if ((ep_addr != 0x00) && (ep_addr != 0x80))
              usbdLowLevelStallEP(usbdHandle , ep_addr);
          }
          usbdHandle->pClass->Setup (usbdHandle, req);
          USBD_CtlSendStatus(usbdHandle);

          break;

        //}}}
        //{{{
        default:
          USBD_CtlError(usbdHandle , req);
          break;
        //}}}
      }
      break;

    case USB_REQ_CLEAR_FEATURE :
      switch (usbdHandle->dev_state) {
        //{{{
        case USBD_STATE_ADDRESSED:
          if ((ep_addr != 0x00) && (ep_addr != 0x80))
            usbdLowLevelStallEP(usbdHandle , ep_addr);
          break;
        //}}}
        //{{{
        case USBD_STATE_CONFIGURED:
          if (req->wValue == USB_FEATURE_EP_HALT) {
            if ((ep_addr & 0x7F) != 0x00) {
              usbdLowLevelClearStallEP(usbdHandle , ep_addr);
              usbdHandle->pClass->Setup (usbdHandle, req);
            }
            USBD_CtlSendStatus(usbdHandle);
          }
          break;
        //}}}
        //{{{
        default:
          USBD_CtlError(usbdHandle , req);
          break;
        //}}}
      }
      break;

    case USB_REQ_GET_STATUS:
      switch (usbdHandle->dev_state) {
      //{{{
      case USBD_STATE_ADDRESSED:
        if ((ep_addr & 0x7F) != 0x00)
          usbdLowLevelStallEP(usbdHandle , ep_addr);
        break;
      //}}}
      //{{{
      case USBD_STATE_CONFIGURED:
        pep = ((ep_addr & 0x80) == 0x80) ? &usbdHandle->ep_in[ep_addr & 0x7F]:\
                                           &usbdHandle->ep_out[ep_addr & 0x7F];
        if(usbdLowLevelIsStallEP(usbdHandle, ep_addr))
          pep->status = 0x0001;
        else
          pep->status = 0x0000;

        USBD_CtlSendData (usbdHandle, (uint8_t *)&pep->status, 2);
        break;
      //}}}
      //{{{
      default:
        USBD_CtlError(usbdHandle , req);
        break;
      //}}}
      }
      break;

    default:
      break;
    }

  return ret;
  }
//}}}

//{{{
void USBD_CtlError (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req) {
  usbdLowLevelStallEP (usbdHandle, 0x80);
  usbdLowLevelStallEP (usbdHandle, 0);
  }
//}}}
//{{{
void USBD_GetString (uint8_t* desc, uint8_t* unicode, uint16_t* len) {

  uint8_t idx = 0;
  if (desc != NULL) {
    *len =  USBD_GetLen(desc) * 2 + 2;
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
USBD_StatusTypeDef USBD_CtlSendData (USBD_HandleTypeDef* usbdHandle, uint8_t* pbuf, uint16_t len) {

  /* Set EP0 State */
  usbdHandle->ep0_state          = USBD_EP0_DATA_IN;
  usbdHandle->ep_in[0].total_length = len;
  usbdHandle->ep_in[0].rem_length   = len;

  /* Start the transfer */
  usbdLowLevelTransmit (usbdHandle, 0x00, pbuf, len);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_CtlContinueSendData (USBD_HandleTypeDef* usbdHandle, uint8_t* pbuf, uint16_t len) {

  /* Start the next transfer */
  usbdLowLevelTransmit (usbdHandle, 0x00, pbuf, len);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_CtlPrepareRx (USBD_HandleTypeDef* usbdHandle, uint8_t* pbuf, uint16_t len) {

  /* Set EP0 State */
  usbdHandle->ep0_state = USBD_EP0_DATA_OUT;
  usbdHandle->ep_out[0].total_length = len;
  usbdHandle->ep_out[0].rem_length = len;

  /* Start the transfer */
  usbdLowLevelPrepareReceive (usbdHandle, 0, pbuf, len);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_CtlContinueRx (USBD_HandleTypeDef* usbdHandle, uint8_t* pbuf, uint16_t len) {

  usbdLowLevelPrepareReceive (usbdHandle, 0, pbuf, len);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_CtlSendStatus (USBD_HandleTypeDef* usbdHandle) {

  /* Set EP0 State */
  usbdHandle->ep0_state = USBD_EP0_STATUS_IN;

  /* Start the transfer */
  usbdLowLevelTransmit (usbdHandle, 0x00, NULL, 0);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_CtlReceiveStatus (USBD_HandleTypeDef* usbdHandle) {
  /* Set EP0 State */
  usbdHandle->ep0_state = USBD_EP0_STATUS_OUT;

  /* Start the transfer */
  usbdLowLevelPrepareReceive (usbdHandle, 0, NULL, 0);

  return USBD_OK;
  }
//}}}
//{{{
uint16_t USBD_GetRxCount (USBD_HandleTypeDef* usbdHandle, uint8_t ep_addr) {
  return usbdLowLevelGetRxDataSize(usbdHandle, ep_addr);
  }
//}}}

//{{{
USBD_StatusTypeDef usbdLowLevelSetupStage (USBD_HandleTypeDef* usbdHandle, uint8_t* psetup) {

  usbdHandle->request.bmRequest = *psetup++;
  usbdHandle->request.bRequest = *psetup++;
  usbdHandle->request.wValue = SWAPBYTE (psetup);
  psetup += 2;
  usbdHandle->request.wIndex = SWAPBYTE (psetup);
  psetup += 2;
  usbdHandle->request.wLength = SWAPBYTE (psetup);

  usbdHandle->ep0_state = USBD_EP0_SETUP;
  usbdHandle->ep0_data_len = usbdHandle->request.wLength;

  switch (usbdHandle->request.bmRequest & 0x1F) {
    case USB_REQ_RECIPIENT_DEVICE:
      USBD_StdDevReq (usbdHandle, &usbdHandle->request);
      break;

    case USB_REQ_RECIPIENT_INTERFACE:
      USBD_StdItfReq (usbdHandle, &usbdHandle->request);
      break;

    case USB_REQ_RECIPIENT_ENDPOINT:
      USBD_StdEPReq (usbdHandle, &usbdHandle->request);
      break;

    default:
      usbdLowLevelStallEP (usbdHandle , usbdHandle->request.bmRequest & 0x80);
      break;
    }

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef usbdLowLevelDataOutStage (USBD_HandleTypeDef* usbdHandle, uint8_t epnum, uint8_t* pdata) {

  if (epnum == 0) {
    auto pep = &usbdHandle->ep_out[0];
    if (usbdHandle->ep0_state == USBD_EP0_DATA_OUT) {
      if (pep->rem_length > pep->maxpacket) {
        pep->rem_length -=  pep->maxpacket;
        USBD_CtlContinueRx (usbdHandle, pdata, MIN(pep->rem_length ,pep->maxpacket));
        }
      else {
        if ((usbdHandle->pClass->EP0_RxReady != NULL)&&
            (usbdHandle->dev_state == USBD_STATE_CONFIGURED))
          usbdHandle->pClass->EP0_RxReady (usbdHandle);
        USBD_CtlSendStatus (usbdHandle);
        }
      }
    }
  else if ((usbdHandle->pClass->DataOut != NULL)&&
           (usbdHandle->dev_state == USBD_STATE_CONFIGURED))
    usbdHandle->pClass->DataOut(usbdHandle, epnum);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef usbdLowLevelDataInStage (USBD_HandleTypeDef* usbdHandle, uint8_t epnum, uint8_t* pdata) {

  if (epnum == 0) {
    auto pep = &usbdHandle->ep_in[0];
    if (usbdHandle->ep0_state == USBD_EP0_DATA_IN) {
      if (pep->rem_length > pep->maxpacket) {
        pep->rem_length -=  pep->maxpacket;
        USBD_CtlContinueSendData (usbdHandle, pdata, pep->rem_length);

        // Prepare endpoint for premature end of transfer
        usbdLowLevelPrepareReceive (usbdHandle, 0, NULL, 0);
        }

      else {
        // last packet is MPS multiple, so send ZLP packet
        if ((pep->total_length % pep->maxpacket == 0) &&
            (pep->total_length >= pep->maxpacket) &&
            (pep->total_length < usbdHandle->ep0_data_len)) {
          USBD_CtlContinueSendData(usbdHandle , NULL, 0);
          usbdHandle->ep0_data_len = 0;

          /* Prepare endpoint for premature end of transfer */
          usbdLowLevelPrepareReceive (usbdHandle, 0, NULL, 0);
          }
        else {
          if ((usbdHandle->pClass->EP0_TxSent != NULL)&&
              (usbdHandle->dev_state == USBD_STATE_CONFIGURED))
            usbdHandle->pClass->EP0_TxSent(usbdHandle);
          USBD_CtlReceiveStatus(usbdHandle);
          }
        }
      }

    if (usbdHandle->dev_test_mode == 1)
      usbdHandle->dev_test_mode = 0;
    }

  else if ((usbdHandle->pClass->DataIn != NULL)&&
           (usbdHandle->dev_state == USBD_STATE_CONFIGURED))
    usbdHandle->pClass->DataIn(usbdHandle, epnum);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef usbdLowLevelReset (USBD_HandleTypeDef* usbdHandle) {

  // Open EP0 OUT
  usbdLowLevelOpenEP (usbdHandle, 0x00, USBD_EP_TYPE_CTRL, USB_MAX_EP0_SIZE);
  usbdHandle->ep_out[0].maxpacket = USB_MAX_EP0_SIZE;

  // Open EP0 IN
  usbdLowLevelOpenEP (usbdHandle, 0x80, USBD_EP_TYPE_CTRL, USB_MAX_EP0_SIZE);
  usbdHandle->ep_in[0].maxpacket = USB_MAX_EP0_SIZE;

  // Upon Reset call user call back */
  usbdHandle->dev_state = USBD_STATE_DEFAULT;

  if (usbdHandle->pClassData)
    usbdHandle->pClass->DeInit(usbdHandle, usbdHandle->dev_config);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef usbdLowLevelSetSpeed (USBD_HandleTypeDef* usbdHandle, USBD_SpeedTypeDef speed) {

  usbdHandle->dev_speed = speed;
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef usbdLowLevelSuspend (USBD_HandleTypeDef* usbdHandle) {

  usbdHandle->dev_old_state = usbdHandle->dev_state;
  usbdHandle->dev_state = USBD_STATE_SUSPENDED;
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef usbdLowLevelResume (USBD_HandleTypeDef* usbdHandle) {

  usbdHandle->dev_state = usbdHandle->dev_old_state;
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef usbdLowLevelSOF (USBD_HandleTypeDef* usbdHandle) {

  if (usbdHandle->dev_state == USBD_STATE_CONFIGURED)
    if (usbdHandle->pClass->SOF != NULL)
      usbdHandle->pClass->SOF (usbdHandle);

  return USBD_OK;
  }
//}}}
USBD_StatusTypeDef usbdLowLevelIsoINIncomplete (USBD_HandleTypeDef* usbdHandle, uint8_t epnum) { return USBD_OK; }
USBD_StatusTypeDef usbdLowLevelIsoOUTIncomplete (USBD_HandleTypeDef* usbdHandle, uint8_t epnum) { return USBD_OK; }
USBD_StatusTypeDef usbdLowLevelDevConnected (USBD_HandleTypeDef* usbdHandle) { return USBD_OK; }
//{{{
USBD_StatusTypeDef usbdLowLevelDevDisconnected (USBD_HandleTypeDef* usbdHandle) {

  // Free Class Resources
  usbdHandle->dev_state = USBD_STATE_DEFAULT;
  usbdHandle->pClass->DeInit(usbdHandle, usbdHandle->dev_config);

  return USBD_OK;
  }
//}}}

//{{{
USBD_StatusTypeDef USBD_Init (USBD_HandleTypeDef* usbdHandle, USBD_DescriptorsTypeDef* pdesc, uint8_t id) {

  // Check whether the USB Host handle is valid
  if (usbdHandle == NULL)
    return USBD_FAIL;

  //  Unlink previous class
  if (usbdHandle->pClass != NULL)
    usbdHandle->pClass = NULL;

  // Assign USBD Descriptors
  if (pdesc != NULL)
    usbdHandle->pDesc = pdesc;

  // Set Device initial State
  usbdHandle->dev_state = USBD_STATE_DEFAULT;
  usbdHandle->id = id;

  // Initialize low level driver
  usbdLowLevelInit (usbdHandle);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_DeInit (USBD_HandleTypeDef* usbdHandle) {

  // Set Default State
  usbdHandle->dev_state  = USBD_STATE_DEFAULT;

  // Free Class Resources
  usbdHandle->pClass->DeInit(usbdHandle, usbdHandle->dev_config);

  // Stop the low level driver
  usbdLowLevelStop(usbdHandle);

  // Initialize low level driver
  usbdLowLevelDeInit(usbdHandle);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_RegisterClass (USBD_HandleTypeDef* usbdHandle, USBD_ClassTypeDef* pclass) {

  USBD_StatusTypeDef status = USBD_OK;
  if (pclass != 0) {
    // link the class to the USB Device handle
    usbdHandle->pClass = pclass;
    status = USBD_OK;
    }
  else
    status = USBD_FAIL;

  return status;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_Start (USBD_HandleTypeDef* usbdHandle) {

  // Start the low level driver
  usbdLowLevelStart (usbdHandle);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_Stop (USBD_HandleTypeDef* usbdHandle) {

  //  Free Class Resources
  usbdHandle->pClass->DeInit (usbdHandle, usbdHandle->dev_config);

  // Stop the low level driver
  usbdLowLevelStop (usbdHandle);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_SetClassConfig (USBD_HandleTypeDef* usbdHandle, uint8_t cfgidx) {

  USBD_StatusTypeDef   ret = USBD_FAIL;
  if (usbdHandle->pClass != NULL)
    // Set configuration  and Start the Class
    if (usbdHandle->pClass->Init (usbdHandle, cfgidx) == 0)
      ret = USBD_OK;
  return ret;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_ClrClassConfig (USBD_HandleTypeDef* usbdHandle, uint8_t cfgidx) {

  // Clear configuration  and De-initialize the Class process*/
  usbdHandle->pClass->DeInit(usbdHandle, cfgidx);
  return USBD_OK;
  }
//}}}
