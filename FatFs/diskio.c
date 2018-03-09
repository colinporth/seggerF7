// diskio.c
#include "diskio.h"
#include "../common/stm32746g_discovery_sd.h"
#include "../common/stm32746g_lcd.h"

#define SD_DEFAULT_BLOCK_SIZE 512

static volatile DSTATUS Stat = STA_NOINIT;

//{{{
DSTATUS sdCheckStatus (BYTE lun) {

  Stat = STA_NOINIT;
  if (BSP_SD_GetCardState() == MSD_OK)
    Stat &= ~STA_NOINIT;

  return Stat;
  }
//}}}

//{{{
uint8_t disk_initialize (BYTE lun) {

  Stat = sdCheckStatus (lun);
  return Stat;
  }
//}}}
//{{{
uint8_t disk_status (BYTE lun) {
  return sdCheckStatus (lun);
  }
//}}}
//{{{
DRESULT disk_read (BYTE lun, BYTE* buff, DWORD sector, UINT count) {

  if (BSP_SD_ReadBlocks ((uint32_t*)buff, (uint32_t)sector, count, 1000) == MSD_OK) {
    while (BSP_SD_GetCardState() != MSD_OK) { }
    return RES_OK;
    }

  return RES_ERROR;
  }
//}}}
//{{{
DRESULT disk_write (BYTE lun, const BYTE* buff, DWORD sector, UINT count) {

  if (BSP_SD_WriteBlocks ((uint32_t*)buff, (uint32_t)sector, count, 1000) == MSD_OK) {
    while (BSP_SD_GetCardState() != MSD_OK) { }
    return RES_OK;
    }

  return RES_ERROR;
  }
//}}}
//{{{
DRESULT disk_ioctl (BYTE lun, BYTE cmd, void* buff) {

  if (Stat & STA_NOINIT)
    return RES_NOTRDY;

  BSP_SD_CardInfo CardInfo;
  switch (cmd) {
    // Make sure that no pending write process
    case CTRL_SYNC :
      return RES_OK;

    // Get number of sectors on the disk (DWORD)
    case GET_SECTOR_COUNT :
      BSP_SD_GetCardInfo (&CardInfo);
      *(DWORD*)buff = CardInfo.LogBlockNbr;
      return RES_OK;

    // Get R/W sector size (WORD)
    case GET_SECTOR_SIZE :
      BSP_SD_GetCardInfo (&CardInfo);
      *(WORD*)buff = CardInfo.LogBlockSize;
      return RES_OK;

    // Get erase block size in unit of sector (DWORD)
    case GET_BLOCK_SIZE :
      BSP_SD_GetCardInfo (&CardInfo);
      *(DWORD*)buff = CardInfo.LogBlockSize / SD_DEFAULT_BLOCK_SIZE;
      return RES_OK;

    default:
      return RES_PARERR;
    }

  return RES_ERROR;
  }
//}}}

//{{{
/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime()
{
  return 0;
}
//}}}
