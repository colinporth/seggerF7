// sdDiskioDmaRtos.cpp
//{{{  includes
#include "../FatFs/ff.h"
#include "../FatFs/diskio.h"

#include "cmsis_os.h"
#include "stm32746g_discovery_sd.h"
#include "cLcd.h"
//}}}

#define QUEUE_SIZE      10
#define READ_CPLT_MSG   1
#define WRITE_CPLT_MSG  2

#define SD_TIMEOUT      2*1000

volatile DSTATUS gStat = STA_NOINIT;
osMessageQId gSdQueueId;

void BSP_SD_WriteCpltCallback() { osMessagePut (gSdQueueId, WRITE_CPLT_MSG, osWaitForever); }
void BSP_SD_ReadCpltCallback()  { osMessagePut (gSdQueueId, READ_CPLT_MSG, osWaitForever); }

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
      cLcd::mLcd->debug (LCD_COLOR_YELLOW, "diskInit %d", gStat);
      }
    else
      cLcd::mLcd->debug (LCD_COLOR_RED, "diskInit bspSdInit failed %d", result);

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

  BSP_SD_CardInfo CardInfo;
  switch (cmd) {
    // Make sure that no pending write process
    case CTRL_SYNC :
      return RES_OK;

    // Get number of sectors on the disk (DWORD)
    case GET_SECTOR_COUNT :
      BSP_SD_GetCardInfo (&CardInfo);
      *(DWORD*)buf = CardInfo.LogBlockNbr;
      return RES_OK;

    // Get R/W sector size (WORD)
    case GET_SECTOR_SIZE :
      BSP_SD_GetCardInfo (&CardInfo);
      *(WORD*)buf = CardInfo.LogBlockSize;
      return RES_OK;

    // Get erase block size in unit of sector (DWORD)
    case GET_BLOCK_SIZE :
      BSP_SD_GetCardInfo (&CardInfo);
      *(DWORD*)buf = CardInfo.LogBlockSize / 512;
      return RES_OK;

    default:
      return RES_PARERR;
    }
  }
//}}}

//{{{
DRESULT diskRead (const BYTE* buf, uint32_t sector, uint32_t numSectors) {

  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "disk_read %p %d %d", buf, sector, numSectors);

  if ((uint32_t)buf & 0x3) {
    cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "disk_read %p align fail", buf);
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

  cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "disk_read %d:%d fail", sector, numSectors);
  return RES_ERROR;
  }
//}}}
//{{{
DRESULT diskWrite (const BYTE* buf, uint32_t sector, uint32_t numSectors) {

  //cLcd::mLcd->debug (LCD_COLOR_YELLOW, "disk_write %p %d %d", buf, sector, numSectors);
  if ((uint32_t)buf & 0x3) {
    cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "disk_write %p align fail", buf);
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
              cLcd::mLcd->debug (LCD_COLOR_YELLOW, "disk_write %7d:%2d %d:%d", sector, numSectors, writeTime, okTime);
            return  RES_OK;
            }
          osDelay (1);
          }
        }
      }
    }

  cLcd::mLcd->debug (LCD_COLOR_MAGENTA, "disk_write %d:%d fail", sector, numSectors);
  return RES_ERROR;
  }
//}}}
