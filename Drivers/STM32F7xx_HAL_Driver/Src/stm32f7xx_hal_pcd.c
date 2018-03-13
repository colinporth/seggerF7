#include "stm32f7xx_hal.h"

#define PCD_MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define PCD_MAX(a, b)  (((a) > (b)) ? (a) : (b))

//{{{
static HAL_StatusTypeDef PCD_WriteEmptyTxFifo (PCD_HandleTypeDef* hpcd, uint32_t epnum) {

  USB_OTG_GlobalTypeDef* USBx = hpcd->Instance;

  USB_OTG_EPTypeDef* ep = &hpcd->IN_ep[epnum];
  int32_t len = ep->xfer_len - ep->xfer_count;
  if (len > ep->maxpacket)
    len = ep->maxpacket;

  uint32_t len32b = (len + 3) / 4;
  while (((USBx_INEP(epnum)->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) > len32b) &&
         (ep->xfer_count < ep->xfer_len) &&
         (ep->xfer_len != 0)) {
    // Write the FIFO
    len = ep->xfer_len - ep->xfer_count;
    if (len > ep->maxpacket)
      len = ep->maxpacket;
    len32b = (len + 3) / 4;

    USB_WritePacket (USBx, ep->xfer_buff, epnum, len, hpcd->Init.dma_enable);

    ep->xfer_buff  += len;
    ep->xfer_count += len;
    }

  if (len <= 0) {
    uint32_t fifoemptymsk = 0x1 << epnum;
    USBx_DEVICE->DIEPEMPMSK &= ~fifoemptymsk;
    }

  return HAL_OK;
  }
//}}}

//{{{
HAL_StatusTypeDef HAL_PCD_Init (PCD_HandleTypeDef *hpcd) {

  uint32_t i = 0;

  /* Check the PCD handle allocation */
  if (hpcd == NULL)
    return HAL_ERROR;

  /* Check the parameters */
  assert_param(IS_PCD_ALL_INSTANCE(hpcd->Instance));

  hpcd->State = HAL_PCD_STATE_BUSY;

  /* Init the low level hardware : GPIO, CLOCK, NVIC... */
  HAL_PCD_MspInit (hpcd);

  /* Disable the Interrupts */
 __HAL_PCD_DISABLE (hpcd);

 /*Init the Core (common init.) */
 USB_CoreInit (hpcd->Instance, hpcd->Init);

 /* Force Device Mode*/
 USB_SetCurrentMode (hpcd->Instance , USB_OTG_DEVICE_MODE);

 /* Init endpoints structures */
 for (i = 0; i < 15 ; i++) {
   /* Init ep structure */
   hpcd->IN_ep[i].is_in = 1;
   hpcd->IN_ep[i].num = i;
   hpcd->IN_ep[i].tx_fifo_num = i;
   /* Control until ep is activated */
   hpcd->IN_ep[i].type = EP_TYPE_CTRL;
   hpcd->IN_ep[i].maxpacket =  0;
   hpcd->IN_ep[i].xfer_buff = 0;
   hpcd->IN_ep[i].xfer_len = 0;
   }

 for (i = 0; i < 15 ; i++) {
   hpcd->OUT_ep[i].is_in = 0;
   hpcd->OUT_ep[i].num = i;
   hpcd->IN_ep[i].tx_fifo_num = i;
   /* Control until ep is activated */
   hpcd->OUT_ep[i].type = EP_TYPE_CTRL;
   hpcd->OUT_ep[i].maxpacket = 0;
   hpcd->OUT_ep[i].xfer_buff = 0;
   hpcd->OUT_ep[i].xfer_len = 0;
   hpcd->Instance->DIEPTXF[i] = 0;
   }

 /* Init Device */
 USB_DevInit (hpcd->Instance, hpcd->Init);

 hpcd->State = HAL_PCD_STATE_READY;

 /* Activate LPM */
 if (hpcd->Init.lpm_enable ==1)
   HAL_PCDEx_ActivateLPM (hpcd);
#if defined (USB_OTG_GCCFG_BCDEN)
 /* Activate Battery charging */
 if (hpcd->Init.battery_charging_enable ==1)
   HAL_PCDEx_ActivateBCD (hpcd);
#endif /* USB_OTG_GCCFG_BCDEN */

 USB_DevDisconnect (hpcd->Instance);
 return HAL_OK;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCD_DeInit (PCD_HandleTypeDef *hpcd) {

  /* Check the PCD handle allocation */
  if (hpcd == NULL)
    return HAL_ERROR;

  hpcd->State = HAL_PCD_STATE_BUSY;

  /* Stop Device */
  HAL_PCD_Stop (hpcd);

  /* DeInit the low level hardware */
  HAL_PCD_MspDeInit (hpcd);

  hpcd->State = HAL_PCD_STATE_RESET;

  return HAL_OK;
  }
//}}}

PCD_StateTypeDef HAL_PCD_GetState (PCD_HandleTypeDef* hpcd) { return hpcd->State; }
//{{{
uint16_t HAL_PCD_EP_GetRxCount (PCD_HandleTypeDef *hpcd, uint8_t ep_addr) {
  return hpcd->OUT_ep[ep_addr & 0xF].xfer_count;
  }
//}}}

//{{{
HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd) {

  __HAL_LOCK(hpcd);
  USB_DevConnect (hpcd->Instance);
  __HAL_PCD_ENABLE(hpcd);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd) {

  __HAL_LOCK(hpcd);
  __HAL_PCD_DISABLE(hpcd);
  USB_StopDevice(hpcd->Instance);
  USB_DevDisconnect (hpcd->Instance);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
  }
//}}}

//{{{
HAL_StatusTypeDef HAL_PCD_DevConnect (PCD_HandleTypeDef *hpcd) {

  __HAL_LOCK(hpcd);
  USB_DevConnect(hpcd->Instance);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCD_DevDisconnect (PCD_HandleTypeDef *hpcd) {

  __HAL_LOCK(hpcd);
  USB_DevDisconnect(hpcd->Instance);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCD_SetAddress (PCD_HandleTypeDef *hpcd, uint8_t address) {

  __HAL_LOCK(hpcd);
  USB_SetDevAddress(hpcd->Instance, address);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCD_EP_Open (PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type) {

  HAL_StatusTypeDef  ret = HAL_OK;
  USB_OTG_EPTypeDef *ep;

  if ((ep_addr & 0x80) == 0x80)
    ep = &hpcd->IN_ep[ep_addr & 0x7F];
  else
    ep = &hpcd->OUT_ep[ep_addr & 0x7F];
  ep->num   = ep_addr & 0x7F;

  ep->is_in = (0x80 & ep_addr) != 0;
  ep->maxpacket = ep_mps;
  ep->type = ep_type;
  if (ep->is_in)
    /* Assign a Tx FIFO */
    ep->tx_fifo_num = ep->num;
  /* Set initial data PID. */
  if (ep_type == EP_TYPE_BULK )
    ep->data_pid_start = 0;

  __HAL_LOCK(hpcd);
  USB_ActivateEndpoint(hpcd->Instance , ep);
  __HAL_UNLOCK(hpcd);

  return ret;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCD_EP_Close (PCD_HandleTypeDef *hpcd, uint8_t ep_addr) {

  USB_OTG_EPTypeDef* ep;
  if ((ep_addr & 0x80) == 0x80)
    ep = &hpcd->IN_ep[ep_addr & 0x7F];
  else
    ep = &hpcd->OUT_ep[ep_addr & 0x7F];
  ep->num   = ep_addr & 0x7F;

  ep->is_in = (0x80 & ep_addr) != 0;

  __HAL_LOCK(hpcd);
  USB_DeactivateEndpoint(hpcd->Instance , ep);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCD_EP_Receive (PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len) {

  USB_OTG_EPTypeDef* ep = &hpcd->OUT_ep[ep_addr & 0x7F];

  /*setup and start the Xfer */
  ep->xfer_buff = pBuf;
  ep->xfer_len = len;
  ep->xfer_count = 0;
  ep->is_in = 0;
  ep->num = ep_addr & 0x7F;

  if (hpcd->Init.dma_enable == 1)
    ep->dma_addr = (uint32_t)pBuf;

  if ((ep_addr & 0x7F) == 0)
    USB_EP0StartXfer(hpcd->Instance, ep, hpcd->Init.dma_enable);
  else
    USB_EPStartXfer(hpcd->Instance, ep, hpcd->Init.dma_enable);

  return HAL_OK;
  }
//}}}

//{{{
HAL_StatusTypeDef HAL_PCD_EP_Transmit (PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len) {

  USB_OTG_EPTypeDef* ep = &hpcd->IN_ep[ep_addr & 0x7F];

  /*setup and start the Xfer */
  ep->xfer_buff = pBuf;
  ep->xfer_len = len;
  ep->xfer_count = 0;
  ep->is_in = 1;
  ep->num = ep_addr & 0x7F;

  if (hpcd->Init.dma_enable == 1)
    ep->dma_addr = (uint32_t)pBuf;

  if ((ep_addr & 0x7F) == 0)
    USB_EP0StartXfer (hpcd->Instance, ep, hpcd->Init.dma_enable);
  else
    USB_EPStartXfer (hpcd->Instance, ep, hpcd->Init.dma_enable);

  return HAL_OK;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCD_EP_SetStall (PCD_HandleTypeDef *hpcd, uint8_t ep_addr) {

  USB_OTG_EPTypeDef *ep;
  if ((ep_addr & 0x0F) > hpcd->Init.dev_endpoints)
    return HAL_ERROR;

  if ((0x80 & ep_addr) == 0x80)
    ep = &hpcd->IN_ep[ep_addr & 0x7F];
  else
    ep = &hpcd->OUT_ep[ep_addr];

  ep->is_stall = 1;
  ep->num   = ep_addr & 0x7F;
  ep->is_in = ((ep_addr & 0x80) == 0x80);


  __HAL_LOCK(hpcd);
  USB_EPSetStall(hpcd->Instance , ep);
  if((ep_addr & 0x7F) == 0)
    USB_EP0_OutStart(hpcd->Instance, hpcd->Init.dma_enable, (uint8_t *)hpcd->Setup);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCD_EP_ClrStall (PCD_HandleTypeDef *hpcd, uint8_t ep_addr) {

  USB_OTG_EPTypeDef *ep;
  if ((ep_addr & 0x0F) > hpcd->Init.dev_endpoints)
    return HAL_ERROR;

  if ((0x80 & ep_addr) == 0x80)
    ep = &hpcd->IN_ep[ep_addr & 0x7F];
  else
    ep = &hpcd->OUT_ep[ep_addr];

  ep->is_stall = 0;
  ep->num   = ep_addr & 0x7F;
  ep->is_in = ((ep_addr & 0x80) == 0x80);

  __HAL_LOCK(hpcd);
  USB_EPClearStall(hpcd->Instance , ep);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCD_EP_Flush (PCD_HandleTypeDef *hpcd, uint8_t ep_addr) {

  __HAL_LOCK(hpcd);

  if ((ep_addr & 0x80) == 0x80)
    USB_FlushTxFifo(hpcd->Instance, ep_addr & 0x7F);
  else
    USB_FlushRxFifo(hpcd->Instance);

  __HAL_UNLOCK(hpcd);

  return HAL_OK;
  }
//}}}

//{{{
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup (PCD_HandleTypeDef *hpcd) {

  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

  if ((USBx_DEVICE->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS)
    /* Activate Remote wakeup signaling */
    USBx_DEVICE->DCTL |= USB_OTG_DCTL_RWUSIG;

  return HAL_OK;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup (PCD_HandleTypeDef *hpcd) {

  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
  /* De-activate Remote wakeup signaling */
  USBx_DEVICE->DCTL &= ~(USB_OTG_DCTL_RWUSIG);

  return HAL_OK;
  }
//}}}

//{{{
HAL_StatusTypeDef HAL_PCDEx_SetTxFiFo (PCD_HandleTypeDef *hpcd, uint8_t fifo, uint16_t size)
{
  uint8_t i = 0;
  uint32_t Tx_Offset = 0;

  /*  TXn min size = 16 words. (n  : Transmit FIFO index)
      When a TxFIFO is not used, the Configuration should be as follows:
          case 1 :  n > m    and Txn is not used    (n,m  : Transmit FIFO indexes)
         --> Txm can use the space allocated for Txn.
         case2  :  n < m    and Txn is not used    (n,m  : Transmit FIFO indexes)
         --> Txn should be configured with the minimum space of 16 words
     The FIFO is used optimally when used TxFIFOs are allocated in the top
         of the FIFO.Ex: use EP1 and EP2 as IN instead of EP1 and EP3 as IN ones.
     When DMA is used 3n * FIFO locations should be reserved for internal DMA registers */

  Tx_Offset = hpcd->Instance->GRXFSIZ;

  if (fifo == 0)
    hpcd->Instance->DIEPTXF0_HNPTXFSIZ = (uint32_t)(((uint32_t)size << 16) | Tx_Offset);
  else {
    Tx_Offset += (hpcd->Instance->DIEPTXF0_HNPTXFSIZ) >> 16;
    for (i = 0; i < (fifo - 1); i++)
      Tx_Offset += (hpcd->Instance->DIEPTXF[i] >> 16);

    /* Multiply Tx_Size by 2 to get higher performance */
    hpcd->Instance->DIEPTXF[fifo - 1] = (uint32_t)(((uint32_t)size << 16) | Tx_Offset);
    }

  return HAL_OK;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCDEx_SetRxFiFo (PCD_HandleTypeDef *hpcd, uint16_t size) {
  hpcd->Instance->GRXFSIZ = size;
  return HAL_OK;
  }
//}}}

//{{{
HAL_StatusTypeDef HAL_PCDEx_ActivateLPM (PCD_HandleTypeDef *hpcd) {

  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

  hpcd->lpm_active = ENABLE;
  hpcd->LPM_State = LPM_L0;
  USBx->GINTMSK |= USB_OTG_GINTMSK_LPMINTM;
  USBx->GLPMCFG |= (USB_OTG_GLPMCFG_LPMEN | USB_OTG_GLPMCFG_LPMACK | USB_OTG_GLPMCFG_ENBESL);

  return HAL_OK;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_PCDEx_DeActivateLPM (PCD_HandleTypeDef *hpcd) {

  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

  hpcd->lpm_active = DISABLE;
  USBx->GINTMSK &= ~USB_OTG_GINTMSK_LPMINTM;
  USBx->GLPMCFG &= ~(USB_OTG_GLPMCFG_LPMEN | USB_OTG_GLPMCFG_LPMACK | USB_OTG_GLPMCFG_ENBESL);

  return HAL_OK;
  }
//}}}

//{{{
/**
  * @brief  Send LPM message to user layer callback.
  * @param  hpcd PCD handle
  * @param  msg LPM message
  * @retval HAL status
  */
//{{{
//}}}
__weak void HAL_PCDEx_LPM_Callback (PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  UNUSED(msg);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCDEx_LPM_Callback could be implemented in the user file
   */
}
//}}}

//{{{
void HAL_PCD_IRQHandler (PCD_HandleTypeDef *hpcd) {

  if (__HAL_PCD_IS_INVALID_INTERRUPT (hpcd))
    return;

  if (__HAL_PCD_GET_FLAG (hpcd, USB_OTG_GINTSTS_MMIS))
    /* incorrect mode, acknowledge the interrupt */
    __HAL_PCD_CLEAR_FLAG (hpcd, USB_OTG_GINTSTS_MMIS);

  USB_OTG_GlobalTypeDef* USBx = hpcd->Instance;
  if (__HAL_PCD_GET_FLAG (hpcd, USB_OTG_GINTSTS_OEPINT)) {
    //{{{  Read in the device interrupt bits
    uint32_t epnum = 0;

    uint32_t ep_intr = USB_ReadDevAllOutEpInterrupt (hpcd->Instance);
    while (ep_intr) {
      if (ep_intr & 0x1) {
        uint32_t epint = USB_ReadDevOutEPInterrupt (hpcd->Instance, epnum);
        if ((epint & USB_OTG_DOEPINT_XFRC) == USB_OTG_DOEPINT_XFRC) {
          CLEAR_OUT_EP_INTR (epnum, USB_OTG_DOEPINT_XFRC);

          /* setup/out transaction management for Core ID >= 310A */
          if (hpcd->Init.dma_enable == 1)
            if (USBx_OUTEP(0)->DOEPINT & (1 << 15))
              CLEAR_OUT_EP_INTR (epnum, (1 << 15));

          if(hpcd->Init.dma_enable == 1) {
            hpcd->OUT_ep[epnum].xfer_count = hpcd->OUT_ep[epnum].maxpacket - (USBx_OUTEP(epnum)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ);
            hpcd->OUT_ep[epnum].xfer_buff += hpcd->OUT_ep[epnum].maxpacket;
            }

          HAL_PCD_DataOutStageCallback (hpcd, epnum);
          if (hpcd->Init.dma_enable == 1) {
            if ((epnum == 0) && (hpcd->OUT_ep[epnum].xfer_len == 0))
               /* this is ZLP, so prepare EP0 for next setup */
              USB_EP0_OutStart (hpcd->Instance, 1, (uint8_t *)hpcd->Setup);
            }
          }

        if ((epint & USB_OTG_DOEPINT_STUP) == USB_OTG_DOEPINT_STUP) {
          /* setup/out transaction management for Core ID >= 310A */
          if (hpcd->Init.dma_enable == 1)
            if (USBx_OUTEP(0)->DOEPINT & (1 << 15))
              CLEAR_OUT_EP_INTR (epnum, (1 << 15));

          /* Inform the upper layer that a setup packet is available */
          HAL_PCD_SetupStageCallback (hpcd);
          CLEAR_OUT_EP_INTR (epnum, USB_OTG_DOEPINT_STUP);
          }

        if ((epint & USB_OTG_DOEPINT_OTEPDIS) == USB_OTG_DOEPINT_OTEPDIS)
          CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPDIS);

        /* Clear Status Phase Received interrupt */
        if ((epint & USB_OTG_DOEPINT_OTEPSPR) == USB_OTG_DOEPINT_OTEPSPR)
          CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPSPR);
        }

      epnum++;
      ep_intr >>= 1;
      }
    }
    //}}}
  if (__HAL_PCD_GET_FLAG (hpcd, USB_OTG_GINTSTS_IEPINT)) {
    //{{{  Read in the device interrupt bits
    uint32_t epnum = 0;

    uint32_t ep_intr = USB_ReadDevAllInEpInterrupt(hpcd->Instance);
    while (ep_intr) {
      if (ep_intr & 0x1) {
        /* In ITR */
        uint32_t epint = USB_ReadDevInEPInterrupt (hpcd->Instance, epnum);

         if (( epint & USB_OTG_DIEPINT_XFRC) == USB_OTG_DIEPINT_XFRC) {
          uint32_t fifoemptymsk = 0x1 << epnum;
          USBx_DEVICE->DIEPEMPMSK &= ~fifoemptymsk;
          CLEAR_IN_EP_INTR (epnum, USB_OTG_DIEPINT_XFRC);
          if (hpcd->Init.dma_enable == 1)
            hpcd->IN_ep[epnum].xfer_buff += hpcd->IN_ep[epnum].maxpacket;

          HAL_PCD_DataInStageCallback (hpcd, epnum);

          if (hpcd->Init.dma_enable == 1)
            /* this is ZLP, so prepare EP0 for next setup */
            if( (epnum == 0) && (hpcd->IN_ep[epnum].xfer_len == 0))
              /* prepare to rx more setup packets */
              USB_EP0_OutStart (hpcd->Instance, 1, (uint8_t *)hpcd->Setup);
          }

        if(( epint & USB_OTG_DIEPINT_TOC) == USB_OTG_DIEPINT_TOC)
          CLEAR_IN_EP_INTR (epnum, USB_OTG_DIEPINT_TOC);

        if(( epint & USB_OTG_DIEPINT_ITTXFE) == USB_OTG_DIEPINT_ITTXFE)
          CLEAR_IN_EP_INTR (epnum, USB_OTG_DIEPINT_ITTXFE);

        if(( epint & USB_OTG_DIEPINT_INEPNE) == USB_OTG_DIEPINT_INEPNE)
          CLEAR_IN_EP_INTR (epnum, USB_OTG_DIEPINT_INEPNE);

        if(( epint & USB_OTG_DIEPINT_EPDISD) == USB_OTG_DIEPINT_EPDISD)
          CLEAR_IN_EP_INTR (epnum, USB_OTG_DIEPINT_EPDISD);

        if(( epint & USB_OTG_DIEPINT_TXFE) == USB_OTG_DIEPINT_TXFE)
          PCD_WriteEmptyTxFifo (hpcd , epnum);
        }

      epnum++;
      ep_intr >>= 1;
      }
    }
    //}}}

  if (__HAL_PCD_GET_FLAG (hpcd, USB_OTG_GINTSTS_WKUINT)) {
    //{{{  Handle Resume Interrupt, Clear the Remote Wake-up Signaling
    USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;

    if(hpcd->LPM_State == LPM_L1) {
      hpcd->LPM_State = LPM_L0;
      HAL_PCDEx_LPM_Callback(hpcd, PCD_LPM_L0_ACTIVE);
      }
    else
      HAL_PCD_ResumeCallback(hpcd);
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_WKUINT);
    }
    //}}}
  if (__HAL_PCD_GET_FLAG (hpcd, USB_OTG_GINTSTS_USBSUSP)) {
    //{{{  Handle Suspend Interrupt
    if ((USBx_DEVICE->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS)
      HAL_PCD_SuspendCallback(hpcd);

    __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_USBSUSP);
    }
    //}}}

  //{{{  Handle LPM Interrupt
  if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_LPMINT)) {
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_LPMINT);
    if ( hpcd->LPM_State == LPM_L0) {
      hpcd->LPM_State = LPM_L1;
      hpcd->BESL = (hpcd->Instance->GLPMCFG & USB_OTG_GLPMCFG_BESL) >>2 ;
      HAL_PCDEx_LPM_Callback(hpcd, PCD_LPM_L1_ACTIVE);
      }
    else
      HAL_PCD_SuspendCallback(hpcd);
    }
  //}}}
  //{{{  Handle Reset Interrupt
  if (__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_USBRST)) {
    USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;
    USB_FlushTxFifo(hpcd->Instance, 0x10);

    for (uint32_t i = 0; i < hpcd->Init.dev_endpoints ; i++) {
      USBx_INEP(i)->DIEPINT = 0xFF;
      USBx_INEP(i)->DIEPCTL &= ~USB_OTG_DIEPCTL_STALL;
      USBx_OUTEP(i)->DOEPINT = 0xFF;
      USBx_OUTEP(i)->DOEPCTL &= ~USB_OTG_DOEPCTL_STALL;
      }
    USBx_DEVICE->DAINT = 0xFFFFFFFF;
    USBx_DEVICE->DAINTMSK |= 0x10001;

    if (hpcd->Init.use_dedicated_ep1) {
      USBx_DEVICE->DOUTEP1MSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM);
      USBx_DEVICE->DINEP1MSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);
      }
    else {
      USBx_DEVICE->DOEPMSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM | USB_OTG_DOEPMSK_OTEPSPRM);
      USBx_DEVICE->DIEPMSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);
      }

    /* Set Default Address to 0 */
    USBx_DEVICE->DCFG &= ~USB_OTG_DCFG_DAD;

    /* setup EP0 to receive SETUP packets */
    USB_EP0_OutStart(hpcd->Instance, hpcd->Init.dma_enable, (uint8_t *)hpcd->Setup);

    __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_USBRST);
    }
  //}}}
  //{{{  Handle Enumeration done Interrupt
  if (__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_ENUMDNE)) {
    USB_ActivateSetup(hpcd->Instance);
    hpcd->Instance->GUSBCFG &= ~USB_OTG_GUSBCFG_TRDT;

    if (USB_GetDevSpeed(hpcd->Instance) == USB_OTG_SPEED_HIGH) {
      hpcd->Init.speed            = USB_OTG_SPEED_HIGH;
      hpcd->Init.ep0_mps          = USB_OTG_HS_MAX_PACKET_SIZE ;
      hpcd->Instance->GUSBCFG |= (uint32_t)((USBD_HS_TRDT_VALUE << 10) & USB_OTG_GUSBCFG_TRDT);
      }
    else {
      hpcd->Init.speed            = USB_OTG_SPEED_FULL;
      hpcd->Init.ep0_mps          = USB_OTG_FS_MAX_PACKET_SIZE ;

      /* The USBTRD is configured according to the tables below, depending on AHB frequency
      used by application. In the low AHB frequency range it is used to stretch enough the USB response
      time to IN tokens, the USB turnaround time, so to compensate for the longer AHB read access
      latency to the Data FIFO */
      /* Get hclk frequency value */
      uint32_t hclk = HAL_RCC_GetHCLKFreq();
      if ((hclk >= 14200000)&&(hclk < 15000000))
        /* hclk Clock Range between 14.2-15 MHz */
        hpcd->Instance->GUSBCFG |= (uint32_t)((0xF << 10) & USB_OTG_GUSBCFG_TRDT);
      else if((hclk >= 15000000)&&(hclk < 16000000))
        /* hclk Clock Range between 15-16 MHz */
        hpcd->Instance->GUSBCFG |= (uint32_t)((0xE << 10) & USB_OTG_GUSBCFG_TRDT);
      else if((hclk >= 16000000)&&(hclk < 17200000))
        /* hclk Clock Range between 16-17.2 MHz */
        hpcd->Instance->GUSBCFG |= (uint32_t)((0xD << 10) & USB_OTG_GUSBCFG_TRDT);
      else if((hclk >= 17200000)&&(hclk < 18500000))
        /* hclk Clock Range between 17.2-18.5 MHz */
        hpcd->Instance->GUSBCFG |= (uint32_t)((0xC << 10) & USB_OTG_GUSBCFG_TRDT);
      else if((hclk >= 18500000)&&(hclk < 20000000))
        /* hclk Clock Range between 18.5-20 MHz */
        hpcd->Instance->GUSBCFG |= (uint32_t)((0xB << 10) & USB_OTG_GUSBCFG_TRDT);
      else if((hclk >= 20000000)&&(hclk < 21800000))
        /* hclk Clock Range between 20-21.8 MHz */
        hpcd->Instance->GUSBCFG |= (uint32_t)((0xA << 10) & USB_OTG_GUSBCFG_TRDT);
      else if((hclk >= 21800000)&&(hclk < 24000000))
        /* hclk Clock Range between 21.8-24 MHz */
        hpcd->Instance->GUSBCFG |= (uint32_t)((0x9 << 10) & USB_OTG_GUSBCFG_TRDT);
      else if((hclk >= 24000000)&&(hclk < 27700000))
        /* hclk Clock Range between 24-27.7 MHz */
        hpcd->Instance->GUSBCFG |= (uint32_t)((0x8 << 10) & USB_OTG_GUSBCFG_TRDT);
      else if((hclk >= 27700000)&&(hclk < 32000000))
        /* hclk Clock Range between 27.7-32 MHz */
        hpcd->Instance->GUSBCFG |= (uint32_t)((0x7 << 10) & USB_OTG_GUSBCFG_TRDT);
      else /* if(hclk >= 32000000) */
        /* hclk Clock Range between 32-200 MHz */
        hpcd->Instance->GUSBCFG |= (uint32_t)((0x6 << 10) & USB_OTG_GUSBCFG_TRDT);
      }

    HAL_PCD_ResetCallback(hpcd);
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_ENUMDNE);
    }
  //}}}
  //{{{  Handle RxQLevel Interrupt
  if(__HAL_PCD_GET_FLAG (hpcd, USB_OTG_GINTSTS_RXFLVL)) {
    USB_MASK_INTERRUPT (hpcd->Instance, USB_OTG_GINTSTS_RXFLVL);
    uint32_t temp = USBx->GRXSTSP;
    USB_OTG_EPTypeDef* ep = &hpcd->OUT_ep[temp & USB_OTG_GRXSTSP_EPNUM];

    if(((temp & USB_OTG_GRXSTSP_PKTSTS) >> 17) ==  STS_DATA_UPDT) {
      if((temp & USB_OTG_GRXSTSP_BCNT) != 0) {
        USB_ReadPacket (USBx, ep->xfer_buff, (temp & USB_OTG_GRXSTSP_BCNT) >> 4);
        ep->xfer_buff += (temp & USB_OTG_GRXSTSP_BCNT) >> 4;
        ep->xfer_count += (temp & USB_OTG_GRXSTSP_BCNT) >> 4;
        }
      }
    else if (((temp & USB_OTG_GRXSTSP_PKTSTS) >> 17) ==  STS_SETUP_UPDT) {
      USB_ReadPacket (USBx, (uint8_t *)hpcd->Setup, 8);
      ep->xfer_count += (temp & USB_OTG_GRXSTSP_BCNT) >> 4;
      }

    USB_UNMASK_INTERRUPT (hpcd->Instance, USB_OTG_GINTSTS_RXFLVL);
    }
  //}}}
  //{{{  Handle SOF Interrupt
  if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_SOF)) {
    HAL_PCD_SOFCallback(hpcd);
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_SOF);
    }
  //}}}
  //{{{  Handle Incomplete ISO IN Interrupt
  if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_IISOIXFR)) {
    HAL_PCD_ISOINIncompleteCallback (hpcd, 0);
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_IISOIXFR);
    }
  //}}}
  //{{{  Handle Incomplete ISO OUT Interrupt
  if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT)) {
    HAL_PCD_ISOOUTIncompleteCallback(hpcd, 0);
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT);
    }
  //}}}
  //{{{  Handle Connection event Interrupt
  if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_SRQINT)) {
    HAL_PCD_ConnectCallback(hpcd);
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_SRQINT);
    }
  //}}}
  //{{{  Handle Disconnection event Interrupt
  if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_OTGINT)) {
    uint32_t temp = hpcd->Instance->GOTGINT;

    if((temp & USB_OTG_GOTGINT_SEDET) == USB_OTG_GOTGINT_SEDET)
      HAL_PCD_DisconnectCallback(hpcd);
    hpcd->Instance->GOTGINT |= temp;
    }
  //}}}
  }
//}}}
