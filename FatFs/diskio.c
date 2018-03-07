// diskio.c
#include "diskio.h"
#include "ff_gen_drv.h"

#if defined ( __GNUC__ )
	#ifndef __weak
		#define __weak __attribute__((weak))
	#endif
#endif

extern Disk_drvTypeDef disk;

//{{{
DSTATUS disk_status (BYTE pdrv) {
	return disk.drv[pdrv]->disk_status(disk.lun[pdrv]);
	}
//}}}
//{{{
DSTATUS disk_initialize (BYTE pdrv) {

	DSTATUS stat = RES_OK;
	if (disk.is_initialized[pdrv] == 0) {
		disk.is_initialized[pdrv] = 1;
		stat = disk.drv[pdrv]->disk_initialize(disk.lun[pdrv]);
		}

	return stat;
	}
//}}}
//{{{
DRESULT disk_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {

	return disk.drv[pdrv]->disk_read(disk.lun[pdrv], buff, sector, count);
	}
//}}}
//{{{
DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
	return disk.drv[pdrv]->disk_write(disk.lun[pdrv], buff, sector, count);
	}
//}}}
//{{{
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff) {
	return disk.drv[pdrv]->disk_ioctl(disk.lun[pdrv], cmd, buff);
	}
//}}}

//{{{
/**
	* @brief  Gets Time from RTC
	* @param  None
	* @retval Time in DWORD
	*/
__weak DWORD get_fattime()
{
	return 0;
}
//}}}
