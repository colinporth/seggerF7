// sdDiskioDmaRtos.cpp
//{{{  includes
#include "../FatFs/ff.h"
#include "../FatFs/diskio.h"
#include "cmsis_os.h"
#include "stm32746g_discovery_sd.h"
#include "cLcd.h"
//}}}

#define QUEUE_SIZE             10
#define READ_CPLT_MSG          1
#define WRITE_CPLT_MSG         2
#define SD_TIMEOUT             2 * 1000
#define SD_DEFAULT_BLOCK_SIZE  512

static volatile DSTATUS Stat = STA_NOINIT;

static osMessageQId SDQueueID;
void BSP_SD_WriteCpltCallback() { osMessagePut (SDQueueID, WRITE_CPLT_MSG, osWaitForever); }
void BSP_SD_ReadCpltCallback()  { osMessagePut (SDQueueID, READ_CPLT_MSG, osWaitForever); }

//{{{
DSTATUS SD_CheckStatus (uint8_t pdrv) {

  Stat = STA_NOINIT;

  if(BSP_SD_GetCardState() == MSD_OK)
    Stat &= ~STA_NOINIT;

  return Stat;
  }
//}}}

DWORD get_fattime() {}
DSTATUS disk_status (uint8_t pdrv) { return SD_CheckStatus (pdrv); }
//{{{
DSTATUS disk_initialize (uint8_t pdrv) {

  Stat = STA_NOINIT;

  if (osKernelRunning()) {
    if (BSP_SD_Init() == MSD_OK)
      Stat = SD_CheckStatus(pdrv);

    if (Stat != STA_NOINIT) {
      osMessageQDef(SD_Queue, QUEUE_SIZE, uint16_t);
      SDQueueID = osMessageCreate (osMessageQ(SD_Queue), NULL);
      }
    cLcd::mLcd->debug (LCD_COLOR_YELLOW, "disk_initialize");
    }

  return Stat;
  }
//}}}
//{{{
DRESULT disk_ioctl (uint8_t pdrv, BYTE cmd, void* buff) {

  if (Stat & STA_NOINIT)
    return RES_NOTRDY;

  DRESULT res = RES_ERROR;
  BSP_SD_CardInfo CardInfo;

  switch (cmd) {
    // Make sure that no pending write process
    case CTRL_SYNC :
      res = RES_OK;
      break;

    // Get number of sectors on the disk (DWORD)
    case GET_SECTOR_COUNT :
      BSP_SD_GetCardInfo (&CardInfo);
      *(DWORD*)buff = CardInfo.LogBlockNbr;
      res = RES_OK;
      break;

    // Get R/W sector size (WORD)
    case GET_SECTOR_SIZE :
      BSP_SD_GetCardInfo (&CardInfo);
      *(WORD*)buff = CardInfo.LogBlockSize;
      res = RES_OK;
      break;

    // Get erase block size in unit of sector (DWORD)
    case GET_BLOCK_SIZE :
      BSP_SD_GetCardInfo (&CardInfo);
      *(DWORD*)buff = CardInfo.LogBlockSize / SD_DEFAULT_BLOCK_SIZE;
      res = RES_OK;
      break;

    default:
      res = RES_PARERR;
    }

  return res;
  }
//}}}
//{{{
DRESULT disk_read (uint8_t pdrv, BYTE* buff, uint32_t sector, uint16_t count) {

  if (BSP_SD_ReadBlocks_DMA ((uint32_t*)buff, sector, count) == MSD_OK) {
    osEvent event = osMessageGet (SDQueueID, SD_TIMEOUT);
    if (event.status == osEventMessage) {
      if (event.value.v == READ_CPLT_MSG) {
        uint32_t timer = osKernelSysTick() + SD_TIMEOUT;
        while (timer > osKernelSysTick()) {
          if (BSP_SD_GetCardState() == SD_TRANSFER_OK) {
            uint32_t alignedAddr = (uint32_t)buff & ~0x1F;
            SCB_InvalidateDCache_by_Addr ((uint32_t*)alignedAddr, count*BLOCKSIZE + ((uint32_t)buff - alignedAddr));
            return RES_OK;
            }
          osDelay (1);
          }
        }
      }
    }

  return RES_ERROR;
  }
//}}}
//{{{
DRESULT disk_write (uint8_t pdrv, const BYTE* buff, uint32_t sector, uint16_t count) {

  uint32_t alignedAddr = (uint32_t)buff & ~0x1F;
  SCB_CleanDCache_by_Addr ((uint32_t*)alignedAddr, count*BLOCKSIZE + ((uint32_t)buff - alignedAddr));

  if (BSP_SD_WriteBlocks_DMA ((uint32_t*)buff, sector, count) == MSD_OK) {
    osEvent event = osMessageGet (SDQueueID, SD_TIMEOUT);
    if (event.status == osEventMessage) {
      if (event.value.v == WRITE_CPLT_MSG) {
        uint32_t timer = osKernelSysTick() + SD_TIMEOUT;
        while (timer > osKernelSysTick()) {
          if (BSP_SD_GetCardState() == SD_TRANSFER_OK)
            return  RES_OK;
          osDelay (1);
          }
        }
      }
    }

  return RES_ERROR;
  }
//}}}
