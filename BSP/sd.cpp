// sd.cpp
//{{{  includes
#include "../FatFs/ff.h"
#include "../FatFs/diskio.h"

#include "cmsis_os.h"
#include "stm32746g_discovery.h"

#include "cLcd.h"
//}}}
//{{{  defines
#define QUEUE_SIZE      10
#define READ_CPLT_MSG   1
#define WRITE_CPLT_MSG  2
#define SD_TIMEOUT      2*1000

#define __DMAx_TxRx_CLK_ENABLE       __HAL_RCC_DMA2_CLK_ENABLE

#define SD_DMAx_Tx_CHANNEL           DMA_CHANNEL_4
#define SD_DMAx_Rx_CHANNEL           DMA_CHANNEL_4
#define SD_DMAx_Tx_STREAM            DMA2_Stream6
#define SD_DMAx_Rx_STREAM            DMA2_Stream3
#define SD_DMAx_Tx_IRQn              DMA2_Stream6_IRQn
#define SD_DMAx_Rx_IRQn              DMA2_Stream3_IRQn

#define SD_DATATIMEOUT  ((uint32_t)100000000)

#define SD_TRANSFER_OK    ((uint8_t)0x00)
#define SD_TRANSFER_BUSY  ((uint8_t)0x01)

#define SD_PRESENT      ((uint8_t)0x01)
#define SD_NOT_PRESENT  ((uint8_t)0x00)
//}}}

bool gDebug = false;
volatile DSTATUS gStat = STA_NOINIT;
osMessageQId gSdQueueId;

SD_HandleTypeDef gSdHandle;
DMA_HandleTypeDef gDmaRxHandle;
DMA_HandleTypeDef gDmaTxHandle;

extern "C" {
  void SDMMC1_IRQHandler() { HAL_SD_IRQHandler (&gSdHandle); }
  void DMA2_Stream6_IRQHandler() { HAL_DMA_IRQHandler (gSdHandle.hdmatx); }
  void DMA2_Stream3_IRQHandler() { HAL_DMA_IRQHandler (gSdHandle.hdmarx); }
  }

//{{{
bool init() {

  // uSD device interface configuration
  gSdHandle.Instance = SDMMC1;
  gSdHandle.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
  gSdHandle.Init.ClockBypass         = SDMMC_CLOCK_BYPASS_DISABLE;
  gSdHandle.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  gSdHandle.Init.BusWide             = SDMMC_BUS_WIDE_1B;
  gSdHandle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  gSdHandle.Init.ClockDiv            = SDMMC_TRANSFER_CLK_DIV;

  // card inserted
  //{{{  GPIO configuration in input for uSD_Detect signal
  SD_DETECT_GPIO_CLK_ENABLE();

  GPIO_InitTypeDef  gpio_init_structure;
  gpio_init_structure.Pin       = SD_DETECT_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_INPUT;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
  HAL_GPIO_Init (SD_DETECT_GPIO_PORT, &gpio_init_structure);
  //}}}
  if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_PORT, SD_DETECT_PIN) == GPIO_PIN_SET) {
    cLcd::mLcd->debug (LCD_COLOR_YELLOW, "sdCard not present");
    return false;
    }
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "sdCard present");

  // card init
  __HAL_RCC_SDMMC1_CLK_ENABLE();
  __DMAx_TxRx_CLK_ENABLE();
  //{{{  enable GPIOs
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* Common GPIO configuration */
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
  gpio_init_structure.Alternate = GPIO_AF12_SDMMC1;

  /* GPIOC configuration */
  gpio_init_structure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
  HAL_GPIO_Init(GPIOC, &gpio_init_structure);

  /* GPIOD configuration */
  gpio_init_structure.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);
  //}}}

  //{{{  Configure DMA Rx parameters
  gDmaRxHandle.Instance = SD_DMAx_Rx_STREAM;
  gDmaRxHandle.Init.Channel             = SD_DMAx_Rx_CHANNEL;
  gDmaRxHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  gDmaRxHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  gDmaRxHandle.Init.MemInc              = DMA_MINC_ENABLE;
  gDmaRxHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  gDmaRxHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  gDmaRxHandle.Init.Mode                = DMA_PFCTRL;
  gDmaRxHandle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
  gDmaRxHandle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
  gDmaRxHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  gDmaRxHandle.Init.MemBurst            = DMA_MBURST_INC4;
  gDmaRxHandle.Init.PeriphBurst         = DMA_PBURST_INC4;
  __HAL_LINKDMA (&gSdHandle, hdmarx, gDmaRxHandle);
  HAL_DMA_Init (&gDmaRxHandle);
  //}}}
  //{{{  Configure DMA Tx parameters
  gDmaTxHandle.Instance = SD_DMAx_Tx_STREAM;
  gDmaTxHandle.Init.Channel             = SD_DMAx_Tx_CHANNEL;
  gDmaTxHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  gDmaTxHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  gDmaTxHandle.Init.MemInc              = DMA_MINC_ENABLE;
  gDmaTxHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  gDmaTxHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  gDmaTxHandle.Init.Mode                = DMA_PFCTRL;
  gDmaTxHandle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
  gDmaTxHandle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
  gDmaTxHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  gDmaTxHandle.Init.MemBurst            = DMA_MBURST_INC4;
  gDmaTxHandle.Init.PeriphBurst         = DMA_PBURST_INC4;
  __HAL_LINKDMA(&gSdHandle, hdmatx, gDmaTxHandle);
  HAL_DMA_Init (&gDmaTxHandle);
  //}}}

  // NVIC configuration for DMA transfer complete interrupt
  HAL_NVIC_SetPriority (SD_DMAx_Rx_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (SD_DMAx_Rx_IRQn);

  // NVIC configuration for DMA transfer complete interrupt
  HAL_NVIC_SetPriority (SD_DMAx_Tx_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (SD_DMAx_Tx_IRQn);

  // NVIC configuration for SDIO interrupts
  HAL_NVIC_SetPriority (SDMMC1_IRQn, 0x0E, 0);
  HAL_NVIC_EnableIRQ (SDMMC1_IRQn);

  if (HAL_SD_Init (&gSdHandle) != HAL_OK)
    return false;
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "mspinit");

  // Enable wide operation
  auto result = HAL_SD_ConfigWideBusOperation (&gSdHandle, SDMMC_BUS_WIDE_4B);
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "wide result %d", result);

  return result == HAL_OK;
  }
//}}}

//{{{
void HAL_SD_TxCpltCallback (SD_HandleTypeDef* hsd) {
  osMessagePut (gSdQueueId, WRITE_CPLT_MSG, osWaitForever);
  }
//}}}
//{{{
void HAL_SD_RxCpltCallback (SD_HandleTypeDef* hsd) {
  osMessagePut (gSdQueueId, READ_CPLT_MSG, osWaitForever);
  }
//}}}

// fatfs interface
//{{{
DSTATUS checkStatus() {

  gStat = STA_NOINIT;
  if (HAL_SD_GetCardState (&gSdHandle) == HAL_SD_CARD_TRANSFER)
    gStat &= ~STA_NOINIT;

  return gStat;
  }
//}}}

DWORD getFatTime() {}
DSTATUS diskStatus() { return checkStatus(); }
//{{{
DSTATUS diskInit() {

  gStat = STA_NOINIT;

  if (osKernelRunning()) {
    if (init())
      gStat = checkStatus();
    else
      cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "diskInit failed");

    if (gStat != STA_NOINIT) {
      osMessageQDef (sdQueue, QUEUE_SIZE, uint16_t);
      gSdQueueId = osMessageCreate (osMessageQ (sdQueue), NULL);
      }
    }

  return gStat;
  }
//}}}
//{{{
DRESULT diskIoctl (BYTE cmd, void* buf) {

  if (gStat & STA_NOINIT)
    return RES_NOTRDY;

  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "diskIoctl");

  HAL_SD_CardInfoTypeDef CardInfo;
  switch (cmd) {
    // Make sure that no pending write process
    case CTRL_SYNC :
      return RES_OK;

    // Get number of sectors on the disk (DWORD)
    case GET_SECTOR_COUNT :
      HAL_SD_GetCardInfo (&gSdHandle, &CardInfo);
      *(DWORD*)buf = CardInfo.LogBlockNbr;
      return RES_OK;

    // Get R/W sector size (WORD)
    case GET_SECTOR_SIZE :
      HAL_SD_GetCardInfo (&gSdHandle, &CardInfo);
      *(WORD*)buf = CardInfo.LogBlockSize;
      return RES_OK;

    // Get erase block size in unit of sector (DWORD)
    case GET_BLOCK_SIZE :
      HAL_SD_GetCardInfo (&gSdHandle, &CardInfo);
      *(DWORD*)buf = CardInfo.LogBlockSize / 512;
      return RES_OK;

    default:
      return RES_PARERR;
    }
  }
//}}}

//{{{
DRESULT diskRead (const BYTE* buf, uint32_t sector, uint32_t numSectors) {

  if (gDebug)
    cLcd::mLcd->debug (LCD_COLOR_YELLOW, "diskRead %p %d %d", buf, sector, numSectors);

  if ((uint32_t)buf & 0x3) {
    cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "diskRead %p align fail", buf);
    return RES_ERROR;
    }

  if (HAL_SD_ReadBlocks_DMA (&gSdHandle, (uint8_t*)buf, sector, numSectors) == HAL_OK) {
    osEvent event = osMessageGet (gSdQueueId, SD_TIMEOUT);
    if (event.status == osEventMessage) {
      if (event.value.v == READ_CPLT_MSG) {
        uint32_t timer = osKernelSysTick();
        while (timer < osKernelSysTick() + SD_TIMEOUT) {
          if (HAL_SD_GetCardState (&gSdHandle) == HAL_SD_CARD_TRANSFER) {
            if (buf + (numSectors * 512) >= (uint8_t*)0x20010000) {
              uint32_t alignedAddr = (uint32_t)buf & ~0x1F;
              SCB_InvalidateDCache_by_Addr ((uint32_t*)alignedAddr, numSectors * 512 + ((uint32_t)buf - alignedAddr));
              }
            return RES_OK;
            }
          osDelay (1);
          }
        }
      }
    }

  cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "diskRead %d:%d fail", sector, numSectors);
  return RES_ERROR;
  }
//}}}
//{{{
DRESULT diskWrite (const BYTE* buf, uint32_t sector, uint32_t numSectors) {

  if (gDebug)
    cLcd::mLcd->debug (LCD_COLOR_YELLOW, "diskWrite %p %d %d", buf, sector, numSectors);

  if ((uint32_t)buf & 0x3) {
    cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "diskWrite %p align fail", buf);
    return RES_ERROR;
    }

  uint32_t alignedAddr = (uint32_t)buf & ~0x1F;
  SCB_CleanDCache_by_Addr ((uint32_t*)alignedAddr, (numSectors * 512) + ((uint32_t)buf - alignedAddr));

  auto ticks1 = osKernelSysTick();
  if (HAL_SD_WriteBlocks_DMA (&gSdHandle, (uint8_t*)buf, sector, numSectors) == HAL_OK) {
    auto event = osMessageGet (gSdQueueId, SD_TIMEOUT);
    if (event.status == osEventMessage) {
      if (event.value.v == WRITE_CPLT_MSG) {
        auto ticks2 = osKernelSysTick();
        while (ticks2 < osKernelSysTick() + SD_TIMEOUT) {
          if (HAL_SD_GetCardState (&gSdHandle) == HAL_SD_CARD_TRANSFER) {
            auto writeTime = ticks2 - ticks1;
            auto okTime = osKernelSysTick() - ticks2;
            if ((writeTime > 200) || (okTime > 200))
              cLcd::mLcd->debug (LCD_COLOR_YELLOW, "diskWrite %7d:%2d %d:%d", sector, numSectors, writeTime, okTime);
            return  RES_OK;
            }
          osDelay (1);
          }
        }
      }
    }

  cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "diskWrite %d:%d fail", sector, numSectors);
  return RES_ERROR;
  }
//}}}

//{{{
void diskDebugEnable() {
  gDebug = true;
  }
//}}}
//{{{
void diskDebugDisable() {
  gDebug = false;
  }
//}}}
