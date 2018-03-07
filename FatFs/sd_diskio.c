// sd_diskio.c
#include "ff_gen_drv.h"
#include "sd_diskio.h"

#if defined (SDMMC_DATATIMEOUT)
  #define SD_TIMEOUT SDMMC_DATATIMEOUT
#elif define d(SD_DATATIMEOUT)
  #define SD_TIMEOUT SD_DATATIMEOUT
#else
  #define SD_TIMEOUT 30 * 1000
#endif

#define SD_DEFAULT_BLOCK_SIZE 512

static volatile DSTATUS Stat = STA_NOINIT;

//{{{
static DSTATUS sdCheckStatus (BYTE lun) {

  Stat = STA_NOINIT;
  if (BSP_SD_GetCardState() == MSD_OK)
    Stat &= ~STA_NOINIT;

  return Stat;
  }
//}}}

//{{{
DSTATUS sdInitialize (BYTE lun) {

  Stat = sdCheckStatus (lun);
  return Stat;
  }
//}}}
//{{{
DSTATUS sdStatus (BYTE lun) {
  return sdCheckStatus(lun);
  }
//}}}
//{{{
DRESULT sdRead (BYTE lun, BYTE *buff, DWORD sector, UINT count) {

  if (BSP_SD_ReadBlocks ((uint32_t*)buff, (uint32_t) (sector), count, SD_TIMEOUT) == MSD_OK) {
    /* wait until the read operation is finished */
    while (BSP_SD_GetCardState()!= MSD_OK) { }
    return RES_OK;
    }

  return RES_ERROR;
  }
//}}}
//{{{
DRESULT sdWrite (BYTE lun, const BYTE *buff, DWORD sector, UINT count) {

  if (BSP_SD_WriteBlocks ((uint32_t*)buff, (uint32_t)(sector), count, SD_TIMEOUT) == MSD_OK) {
    /* wait until the Write operation is finished */
    while(BSP_SD_GetCardState() != MSD_OK) { }
    return RES_OK;
    }

  return RES_ERROR;
  }
//}}}
//{{{
DRESULT sdIoctl (BYTE lun, BYTE cmd, void *buff) {

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

const Diskio_drvTypeDef SD_Driver = {
  sdInitialize,
  sdStatus,
  sdRead,
  sdWrite,
  sdIoctl,
  };
