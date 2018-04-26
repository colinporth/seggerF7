// sd.cpp
//{{{  includes
#include "sd.h"

#include "cmsis_os.h"
#include "cLcd.h"
#include "../FatFs/ff.h"
#include "../FatFs/diskio.h"
//}}}
//{{{  defines
#define QUEUE_SIZE      10
#define READ_CPLT_MSG   1
#define WRITE_CPLT_MSG  2
#define SD_TIMEOUT      2*1000
//}}}

bool gDebug = false;
volatile DSTATUS gStat = STA_NOINIT;
osMessageQId gSdQueueId;

SD_HandleTypeDef uSdHandle;
DMA_HandleTypeDef dma_rx_handle;
DMA_HandleTypeDef dma_tx_handle;

extern "C" {
  void BSP_SDMMC_IRQHandler() { HAL_SD_IRQHandler (&uSdHandle); }
  void BSP_SDMMC_DMA_Tx_IRQHandler() { HAL_DMA_IRQHandler (uSdHandle.hdmatx); }
  void BSP_SDMMC_DMA_Rx_IRQHandler() { HAL_DMA_IRQHandler (uSdHandle.hdmarx); }
  }

//{{{
uint8_t BSP_SD_Init() {

  // uSD device interface configuration
  uSdHandle.Instance = SDMMC1;
  uSdHandle.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
  uSdHandle.Init.ClockBypass         = SDMMC_CLOCK_BYPASS_DISABLE;
  uSdHandle.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  uSdHandle.Init.BusWide             = SDMMC_BUS_WIDE_1B;
  uSdHandle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  uSdHandle.Init.ClockDiv            = SDMMC_TRANSFER_CLK_DIV;

  // card inserted
  //{{{  GPIO configuration in input for uSD_Detect signal */
  SD_DETECT_GPIO_CLK_ENABLE();

  GPIO_InitTypeDef  gpio_init_structure;
  gpio_init_structure.Pin       = SD_DETECT_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_INPUT;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
  HAL_GPIO_Init (SD_DETECT_GPIO_PORT, &gpio_init_structure);
  //}}}
  if (BSP_SD_IsDetected() != SD_PRESENT)
    return MSD_ERROR_SD_NOT_PRESENT;
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "present");

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
  dma_rx_handle.Instance = SD_DMAx_Rx_STREAM;
  dma_rx_handle.Init.Channel             = SD_DMAx_Rx_CHANNEL;
  dma_rx_handle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  dma_rx_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
  dma_rx_handle.Init.MemInc              = DMA_MINC_ENABLE;
  dma_rx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dma_rx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  dma_rx_handle.Init.Mode                = DMA_PFCTRL;
  dma_rx_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
  dma_rx_handle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
  dma_rx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  dma_rx_handle.Init.MemBurst            = DMA_MBURST_INC4;
  dma_rx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;
  __HAL_LINKDMA (&uSdHandle, hdmarx, dma_rx_handle);
  HAL_DMA_DeInit(&dma_rx_handle);
  HAL_DMA_Init(&dma_rx_handle);
  //}}}
  //{{{  Configure DMA Tx parameters 
  dma_tx_handle.Instance = SD_DMAx_Tx_STREAM;
  dma_tx_handle.Init.Channel             = SD_DMAx_Tx_CHANNEL;
  dma_tx_handle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  dma_tx_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
  dma_tx_handle.Init.MemInc              = DMA_MINC_ENABLE;
  dma_tx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dma_tx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  dma_tx_handle.Init.Mode                = DMA_PFCTRL;
  dma_tx_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
  dma_tx_handle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
  dma_tx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  dma_tx_handle.Init.MemBurst            = DMA_MBURST_INC4;
  dma_tx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;
  __HAL_LINKDMA(&uSdHandle, hdmatx, dma_tx_handle);
  HAL_DMA_DeInit(&dma_tx_handle);
  HAL_DMA_Init(&dma_tx_handle);
  //}}}

  // NVIC configuration for DMA transfer complete interrupt
  HAL_NVIC_SetPriority(SD_DMAx_Rx_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(SD_DMAx_Rx_IRQn);

  // NVIC configuration for DMA transfer complete interrupt
  HAL_NVIC_SetPriority(SD_DMAx_Tx_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(SD_DMAx_Tx_IRQn);

  // NVIC configuration for SDIO interrupts
  HAL_NVIC_SetPriority(SDMMC1_IRQn, 0x0E, 0);
  HAL_NVIC_EnableIRQ(SDMMC1_IRQn);

  if (HAL_SD_Init (&uSdHandle) != HAL_OK)
    return MSD_ERROR;
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "mspinit");

  // Enable wide operation
  auto result = HAL_SD_ConfigWideBusOperation (&uSdHandle, SDMMC_BUS_WIDE_4B);
  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "wide result %d", result);

  return (result == HAL_OK) ? MSD_OK : MSD_ERROR;
  }
//}}}
//{{{
uint8_t BSP_SD_DeInit()
{
  uint8_t sd_state = MSD_OK;

  uSdHandle.Instance = SDMMC1;

  /* HAL SD deinitialization */
  if (HAL_SD_DeInit(&uSdHandle) != HAL_OK)
    sd_state = MSD_ERROR;

  /* Msp SD deinitialization */
  uSdHandle.Instance = SDMMC1;
  BSP_SD_MspDeInit (&uSdHandle, NULL);

  return  sd_state;
}
//}}}

//{{{
uint8_t BSP_SD_ITConfig() {

  GPIO_InitTypeDef gpio_init_structure;

  /* Configure Interrupt mode for SD detection pin */
  gpio_init_structure.Pin = SD_DETECT_PIN;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FAST;
  gpio_init_structure.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(SD_DETECT_GPIO_PORT, &gpio_init_structure);

  /* Enable and set SD detect EXTI Interrupt to the lowest priority */
  HAL_NVIC_SetPriority((IRQn_Type)(SD_DETECT_EXTI_IRQn), 0x0F, 0x00);
  HAL_NVIC_EnableIRQ((IRQn_Type)(SD_DETECT_EXTI_IRQn));

  return MSD_OK;
  }
//}}}
//{{{
uint8_t BSP_SD_IsDetected() {

  __IO uint8_t status = SD_PRESENT;

  /* Check SD card detect pin */
  if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_PORT, SD_DETECT_PIN) == GPIO_PIN_SET)
    status = SD_NOT_PRESENT;

  return status;
  }
//}}}

//{{{
uint8_t BSP_SD_ReadBlocks (uint32_t* pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout) {

  if (HAL_SD_ReadBlocks(&uSdHandle, (uint8_t*)pData, ReadAddr, NumOfBlocks, Timeout) != HAL_OK)
    return MSD_ERROR;
  else
    return MSD_OK;
  }
//}}}
//{{{
uint8_t BSP_SD_WriteBlocks (uint32_t* pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout) {

  if (HAL_SD_WriteBlocks (&uSdHandle, (uint8_t*)pData, WriteAddr, NumOfBlocks, Timeout) != HAL_OK)
    return MSD_ERROR;
  else
    return MSD_OK;
  }
//}}}
//{{{
uint8_t BSP_SD_ReadBlocks_DMA (uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks) {

  /* Read block(s) in DMA transfer mode */
  if (HAL_SD_ReadBlocks_DMA(&uSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks) != HAL_OK)
    return MSD_ERROR;
  else
    return MSD_OK;
  }
//}}}
//{{{
uint8_t BSP_SD_WriteBlocks_DMA (uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks) {

  /* Write block(s) in DMA transfer mode */
  if (HAL_SD_WriteBlocks_DMA (&uSdHandle, (uint8_t*)pData, WriteAddr, NumOfBlocks) != HAL_OK)
    return MSD_ERROR;
  else
    return MSD_OK;
  }
//}}}
//{{{
uint8_t BSP_SD_Erase (uint32_t StartAddr, uint32_t EndAddr) {

  if (HAL_SD_Erase (&uSdHandle, StartAddr, EndAddr) != HAL_OK)
    return MSD_ERROR;
  else
    return MSD_OK;
  }
//}}}

//{{{
__weak void BSP_SD_MspInit (SD_HandleTypeDef *hsd, void *Params) {

  GPIO_InitTypeDef gpio_init_structure;

  /* Enable SDIO clock */
  __HAL_RCC_SDMMC1_CLK_ENABLE();

  /* Enable DMA2 clocks */
  __DMAx_TxRx_CLK_ENABLE();

  /* Enable GPIOs clock */
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

  /* NVIC configuration for SDIO interrupts */
  HAL_NVIC_SetPriority(SDMMC1_IRQn, 0x0E, 0);
  HAL_NVIC_EnableIRQ(SDMMC1_IRQn);

  /* Configure DMA Rx parameters */
  dma_rx_handle.Init.Channel             = SD_DMAx_Rx_CHANNEL;
  dma_rx_handle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  dma_rx_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
  dma_rx_handle.Init.MemInc              = DMA_MINC_ENABLE;
  dma_rx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dma_rx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  dma_rx_handle.Init.Mode                = DMA_PFCTRL;
  dma_rx_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
  dma_rx_handle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
  dma_rx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  dma_rx_handle.Init.MemBurst            = DMA_MBURST_INC4;
  dma_rx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;

  dma_rx_handle.Instance = SD_DMAx_Rx_STREAM;

  /* Associate the DMA handle */
  __HAL_LINKDMA(hsd, hdmarx, dma_rx_handle);

  /* Deinitialize the stream for new transfer */
  HAL_DMA_DeInit(&dma_rx_handle);

  /* Configure the DMA stream */
  HAL_DMA_Init(&dma_rx_handle);

  /* Configure DMA Tx parameters */
  dma_tx_handle.Init.Channel             = SD_DMAx_Tx_CHANNEL;
  dma_tx_handle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  dma_tx_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
  dma_tx_handle.Init.MemInc              = DMA_MINC_ENABLE;
  dma_tx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dma_tx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  dma_tx_handle.Init.Mode                = DMA_PFCTRL;
  dma_tx_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
  dma_tx_handle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
  dma_tx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  dma_tx_handle.Init.MemBurst            = DMA_MBURST_INC4;
  dma_tx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;

  dma_tx_handle.Instance = SD_DMAx_Tx_STREAM;

  /* Associate the DMA handle */
  __HAL_LINKDMA(hsd, hdmatx, dma_tx_handle);

  /* Deinitialize the stream for new transfer */
  HAL_DMA_DeInit(&dma_tx_handle);

  /* Configure the DMA stream */
  HAL_DMA_Init(&dma_tx_handle);

  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SD_DMAx_Rx_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(SD_DMAx_Rx_IRQn);

  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SD_DMAx_Tx_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(SD_DMAx_Tx_IRQn);
}
//}}}
//{{{
__weak void BSP_SD_Detect_MspInit (SD_HandleTypeDef *hsd, void *Params) {

  GPIO_InitTypeDef  gpio_init_structure;
  SD_DETECT_GPIO_CLK_ENABLE();

  /* GPIO configuration in input for uSD_Detect signal */

  gpio_init_structure.Pin       = SD_DETECT_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_INPUT;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
  HAL_GPIO_Init (SD_DETECT_GPIO_PORT, &gpio_init_structure);
  }
//}}}
//{{{
__weak void BSP_SD_MspDeInit (SD_HandleTypeDef *hsd, void *Params) {

  /* Disable NVIC for DMA transfer complete interrupts */
  HAL_NVIC_DisableIRQ (SD_DMAx_Rx_IRQn);
  HAL_NVIC_DisableIRQ (SD_DMAx_Tx_IRQn);

  /* Deinitialize the stream for new transfer */
  dma_rx_handle.Instance = SD_DMAx_Rx_STREAM;
  HAL_DMA_DeInit (&dma_rx_handle);

  /* Deinitialize the stream for new transfer */
  dma_tx_handle.Instance = SD_DMAx_Tx_STREAM;
  HAL_DMA_DeInit (&dma_tx_handle);

  /* Disable NVIC for SDIO interrupts */
  HAL_NVIC_DisableIRQ (SDMMC1_IRQn);

  /* DeInit GPIO pins can be done in the application
     (by surcharging this __weak function) */

  /* Disable SDMMC1 clock */
  __HAL_RCC_SDMMC1_CLK_DISABLE();

  /* GPIO pins clock and DMA clocks can be shut down in the application
     by surcharging this __weak function */
  }
//}}}
//{{{
uint8_t BSP_SD_GetCardState() {

  return((HAL_SD_GetCardState(&uSdHandle) == HAL_SD_CARD_TRANSFER ) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
  }
//}}}

//{{{
void HAL_SD_TxCpltCallback (SD_HandleTypeDef *hsd) {
  osMessagePut (gSdQueueId, WRITE_CPLT_MSG, osWaitForever);
  }
//}}}
//{{{
void HAL_SD_RxCpltCallback (SD_HandleTypeDef *hsd) {
  osMessagePut (gSdQueueId, READ_CPLT_MSG, osWaitForever);
  }
//}}}

//{{{
DSTATUS checkStatus() {

  gStat = STA_NOINIT;

  if (BSP_SD_GetCardState() == MSD_OK)
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
    auto result = BSP_SD_Init();
    if (result == MSD_OK) {
      gStat = checkStatus();
      //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "diskInit %d", gStat);
      }
    else
      cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "diskInit failed %d", result);

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
      HAL_SD_GetCardInfo (&uSdHandle, &CardInfo);
      *(DWORD*)buf = CardInfo.LogBlockNbr;
      return RES_OK;

    // Get R/W sector size (WORD)
    case GET_SECTOR_SIZE :
      HAL_SD_GetCardInfo (&uSdHandle, &CardInfo);
      *(WORD*)buf = CardInfo.LogBlockSize;
      return RES_OK;

    // Get erase block size in unit of sector (DWORD)
    case GET_BLOCK_SIZE :
      HAL_SD_GetCardInfo (&uSdHandle, &CardInfo);
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

  if (BSP_SD_ReadBlocks_DMA ((uint32_t*)buf, sector, numSectors) == MSD_OK) {
    osEvent event = osMessageGet (gSdQueueId, SD_TIMEOUT);
    if (event.status == osEventMessage) {
      if (event.value.v == READ_CPLT_MSG) {
        uint32_t timer = osKernelSysTick();
        while (timer < osKernelSysTick() + SD_TIMEOUT) {
          if (BSP_SD_GetCardState() == SD_TRANSFER_OK) {
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
  if (BSP_SD_WriteBlocks_DMA ((uint32_t*)buf, sector, numSectors) == MSD_OK) {
    auto event = osMessageGet (gSdQueueId, SD_TIMEOUT);
    if (event.status == osEventMessage) {
      if (event.value.v == WRITE_CPLT_MSG) {
        auto ticks2 = osKernelSysTick();
        while (ticks2 < osKernelSysTick() + SD_TIMEOUT) {
          if (BSP_SD_GetCardState() == SD_TRANSFER_OK) {
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
