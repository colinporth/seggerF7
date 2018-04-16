// ethernetIf.cpp
//{{{  includes
#include "ethernetif.h"

#include <string.h>
#include "stm32f7xx_hal.h"
#include "lwip/opt.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"

#include "cLcd.h"
//}}}

ETH_HandleTypeDef EthHandle;
osSemaphoreId inputSemaphore = NULL;

extern "C" { void ETH_IRQHandler() { HAL_ETH_IRQHandler (&EthHandle); }}

//{{{
void ethernetInput (void const* argument) {

  struct netif* netif = (struct netif*)argument;

  while (true) {
    if (osSemaphoreWait (inputSemaphore, osWaitForever) == osOK) {
      struct pbuf* pbuf = NULL;
      do {
        // get received frame
        pbuf = NULL;
        if (HAL_ETH_GetReceivedFrame_IT (&EthHandle) == HAL_OK) {
          uint16_t len = EthHandle.RxFrameInfos.length;
          uint8_t* buf = (uint8_t*)EthHandle.RxFrameInfos.buffer;
          __IO ETH_DMADescTypeDef* dmaRxDesc = EthHandle.RxFrameInfos.FSRxDesc;

          //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "ethIn %x:%d", buf, len);

          if (len > 0) // allocate a pbuf chain of pbufs from Lwip bufferPool
            pbuf = pbuf_alloc (PBUF_RAW, len, PBUF_POOL);
          if (pbuf) {
            uint32_t bufOffset = 0;
            for (struct pbuf* qpbuf = pbuf; qpbuf != NULL; qpbuf = qpbuf->next) {
              // copy data to pbuf, check length in current pbuf is bigger than Rx buffer size
              uint32_t bytesLeft = qpbuf->len;
              uint32_t payloadOffset = 0;
              while ((bytesLeft + bufOffset) > ETH_RX_BUF_SIZE) {
                memcpy ((uint8_t*)qpbuf->payload + payloadOffset, buf + bufOffset, ETH_RX_BUF_SIZE - bufOffset);
                dmaRxDesc = (ETH_DMADescTypeDef*)(dmaRxDesc->Buffer2NextDescAddr);
                buf = (uint8_t*)(dmaRxDesc->Buffer1Addr);
                bytesLeft -= (ETH_RX_BUF_SIZE - bufOffset);
                payloadOffset += (ETH_RX_BUF_SIZE - bufOffset);
                bufOffset = 0;
                }
              memcpy ((uint8_t*)qpbuf->payload + payloadOffset, buf + bufOffset, bytesLeft);
              bufOffset += bytesLeft;
              }
            }

          // release descriptors to DMA, point to first, set ownBit in Rx descriptor
          dmaRxDesc = EthHandle.RxFrameInfos.FSRxDesc;
          for (uint32_t i = 0; i < EthHandle.RxFrameInfos.SegCount; i++) {
            dmaRxDesc->Status |= ETH_DMARXDESC_OWN;
            dmaRxDesc = (ETH_DMADescTypeDef*)(dmaRxDesc->Buffer2NextDescAddr);
            }

          // clear segCount
          EthHandle.RxFrameInfos.SegCount = 0;

          if ((EthHandle.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET) {
            // rxBuffer unavailable flag set, clear RBUS ETHERNET DMA flag, resume DMA reception
            EthHandle.Instance->DMASR = ETH_DMASR_RBUS;
            EthHandle.Instance->DMARPDR = 0;
            cLcd::mLcd->debug (LCD_COLOR_YELLOW, "ethIn rxBuffer unavailable");
            }

          if (pbuf)
            if (netif->input (pbuf, netif) != ERR_OK)
              pbuf_free (pbuf);
          }
        } while (pbuf);
      }
    }
  }
//}}}
//{{{
err_t lowLevelOutput (struct netif* netif, struct pbuf* pbuf) {

  err_t errval;

  // copy frame from pbufs to driver buffers
  uint32_t bufOffset = 0;
  uint32_t frameLength = 0;
  uint8_t* buf = (uint8_t *)(EthHandle.TxDesc->Buffer1Addr);

  for (struct pbuf* qpbuf = pbuf; qpbuf != NULL; qpbuf = qpbuf->next) {
    // is this buffer available? If not, goto error
    __IO ETH_DMADescTypeDef* dmaTxDesc = EthHandle.TxDesc;
    if ((dmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET) {
      //{{{  buffer not available, error exit
      if ((EthHandle.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET) {
        // transmitUnderflow flag set, clear it, issue txPollDemand to resume tx
        EthHandle.Instance->DMASR = ETH_DMASR_TUS;
        EthHandle.Instance->DMATPDR = 0;
        cLcd::mLcd->debug (LCD_COLOR_YELLOW, "ethOut Transmit Underflow1");
        }
      return ERR_USE;
      }
      //}}}

    // get bytes in current lwIP buffer
    uint32_t bytesLeft = qpbuf->len;
    uint32_t payloadOffset = 0;

    // check if the length of data to copy is bigger than txBuffer size
    while ((bytesLeft + bufOffset) > ETH_TX_BUF_SIZE) {
      // copy data to txBuffer
      memcpy ((uint8_t*)buf + bufOffset, (uint8_t*)qpbuf->payload + payloadOffset, ETH_TX_BUF_SIZE - bufOffset);

      // point to next descriptor
      dmaTxDesc = (ETH_DMADescTypeDef*)(dmaTxDesc->Buffer2NextDescAddr);
      if ((dmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET) {
        //{{{  buffer not available, error exit
        if ((EthHandle.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET) {
          // transmitUnderflow flag set, clear it, issue txPollDemand to resume tx
          EthHandle.Instance->DMASR = ETH_DMASR_TUS;
          EthHandle.Instance->DMATPDR = 0;
          cLcd::mLcd->debug (LCD_COLOR_YELLOW, "ethOut Transmit Underflow2");
          }
        return ERR_USE;
        }
        //}}}

      buf = (uint8_t*)(dmaTxDesc->Buffer1Addr);
      bytesLeft -= ETH_TX_BUF_SIZE - bufOffset;
      payloadOffset += ETH_TX_BUF_SIZE - bufOffset;
      frameLength += ETH_TX_BUF_SIZE - bufOffset;
      bufOffset = 0;
      }

    // copy the remaining bytes
    memcpy ((uint8_t*)buf + bufOffset, (uint8_t*)qpbuf->payload + payloadOffset, bytesLeft);
    bufOffset = bufOffset + bytesLeft;
    frameLength = frameLength + bytesLeft;
    }

  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "ethOut %d", frameLength);

  // prepare transmit descriptors to give to DMA
  HAL_ETH_TransmitFrame (&EthHandle, frameLength);

  if ((EthHandle.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET) {
    // transmitUnderflow flag set, clear it, issue txPollDemand to resume tx
    EthHandle.Instance->DMASR = ETH_DMASR_TUS;
    EthHandle.Instance->DMATPDR = 0;
    cLcd::mLcd->debug (LCD_COLOR_YELLOW, "ethOut Transmit Underflow3");
    }

  return ERR_OK;
  }
//}}}

//{{{
void HAL_ETH_MspInit (ETH_HandleTypeDef* heth) {
// RMII_REF_CLK ----------------------> PA1
// RMII_MDIO -------------------------> PA2
// RMII_MDC --------------------------> PC1
// RMII_MII_CRS_DV -------------------> PA7
// RMII_MII_RXD0 ---------------------> PC4
// RMII_MII_RXD1 ---------------------> PC5
// RMII_MII_RXER ---------------------> PG2
// RMII_MII_TX_EN --------------------> PG11
// RMII_MII_TXD0 ---------------------> PG13
// RMII_MII_TXD1 ---------------------> PG14

  // Enable GPIOs clocks
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  // Configure PA1, PA2 and PA7
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
  GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
  HAL_GPIO_Init (GPIOA, &GPIO_InitStructure);

  // Configure PC1, PC4 and PC5
  GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init (GPIOC, &GPIO_InitStructure);

  // Configure PG2, PG11, PG13 and PG14
  GPIO_InitStructure.Pin =  GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14;
  HAL_GPIO_Init (GPIOG, &GPIO_InitStructure);

  // Enable the Ethernet global Interrupt
  HAL_NVIC_SetPriority (ETH_IRQn, 0x7, 0);
  HAL_NVIC_EnableIRQ (ETH_IRQn);

  // Enable ETHERNET clock
  __HAL_RCC_ETH_CLK_ENABLE();
  }
//}}}
//{{{
void HAL_ETH_RxCpltCallback (ETH_HandleTypeDef* heth) {
  osSemaphoreRelease (inputSemaphore);
  }
//}}}

//{{{
u32_t sys_now() {
  return HAL_GetTick();
  }
//}}}
//{{{
err_t ethernetIfInit (struct netif* netif) {

  netif->hostname = "cam";
  netif->name[0] = 's';
  netif->name[1] = 't';

  // We directly use etharp_output() here to save a function call.
  // You can instead declare your own function an call etharp_output()
  // from it if you have to do some checks before sending (e.g. if link is available...)
  netif->output = etharp_output;
  netif->linkoutput = lowLevelOutput;

  // initialize the hardware
  uint8_t macAddress[6]= { MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5 };

  EthHandle.Instance = ETH;
  EthHandle.Init.MACAddr = macAddress;
  EthHandle.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
  EthHandle.Init.Speed = ETH_SPEED_100M;
  EthHandle.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
  EthHandle.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
  EthHandle.Init.RxMode = ETH_RXINTERRUPT_MODE;
  EthHandle.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  EthHandle.Init.PhyAddress = LAN8742A_PHY_ADDRESS;

  // configure ethernet peripheral (GPIOs, clocks, MAC, DMA)
  if (HAL_ETH_Init (&EthHandle) == HAL_OK)
    netif->flags |= NETIF_FLAG_LINK_UP;

  // init rxDescriptors,txDescriptors, Chain Mode, hardCode buffers in         CM 0x20000000 to 0x20003FFF
  // ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] // Ethernet Rx DMA Descriptors -   64*4 =  256 =   0x80
  // uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] // Ethernet Receive Buffers    - 1524*4 = 6096 = 0x17D0 pad to 0x1800
  // ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] // Ethernet Tx DMA Descriptors -   64*4 =  256 =   0x80
  // uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] // Ethernet Transmit Buffers   - 1524*4 = 6096 = 0x17D0 pad to 0x1800
  HAL_ETH_DMARxDescListInit (&EthHandle, (ETH_DMADescTypeDef*)0x20000000, (uint8_t*)0x20000080, ETH_RXBUFNB);
  HAL_ETH_DMATxDescListInit (&EthHandle, (ETH_DMADescTypeDef*)0x20002000, (uint8_t*)0x20002080, ETH_TXBUFNB);

  // set netif MAC hardware address
  netif->hwaddr[0] = MAC_ADDR0;
  netif->hwaddr[1] = MAC_ADDR1;
  netif->hwaddr[2] = MAC_ADDR2;
  netif->hwaddr[3] = MAC_ADDR3;
  netif->hwaddr[4] = MAC_ADDR4;
  netif->hwaddr[5] = MAC_ADDR5;
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  // set netif maximum transfer unit
  netif->mtu = 1500;

  // Accept broadcast address and ARP traffic
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

  // binary semaphore for frame reception
  osSemaphoreDef (SEM);
  inputSemaphore = osSemaphoreCreate (osSemaphore (SEM) , 1 );

  // create the task that handles the ETH_MAC
  osThreadDef (ethIf, ethernetInput, osPriorityRealtime, 0, 350);
  osThreadCreate (osThread (ethIf), netif);

  // enable MAC and DMA transmission and reception
  HAL_ETH_Start (&EthHandle);

  return ERR_OK;
  }
//}}}
