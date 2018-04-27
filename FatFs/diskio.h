#pragma once
#include "ff.h"
//{{{
#ifdef __cplusplus
extern "C" {
#endif
//}}}

// diskStatus bits
typedef BYTE DSTATUS;
#define STA_NOINIT   0x01  // Drive not initialized
#define STA_NODISK   0x02  // No medium in the drive
#define STA_PROTECT  0x04  // Write protected

// Results of Disk Functions
typedef enum { 
  RES_OK = 0, // 0: Successful
  RES_ERROR,  // 1: R/W Error
  RES_WRPRT,  // 2: Write Protected
  RES_NOTRDY, // 3: Not Ready
  RES_PARERR  // 4: Invalid Parameter
  } DRESULT;

// Command code for disk_ioctrl fucntion
#define CTRL_SYNC         0 // Complete pending write process (needed at _FS_READONLY == 0)
#define GET_SECTOR_COUNT  1 // Get media size (needed at _USE_MKFS == 1)
#define GET_SECTOR_SIZE   2 // Get sector size (needed at _MAX_SS != _MIN_SS)
#define GET_BLOCK_SIZE    3 // Get erase block size (needed at _USE_MKFS == 1)
#define CTRL_TRIM         4

// Prototypes for disk control functions
DSTATUS diskStatus();
DSTATUS diskInit();
DRESULT diskIoctl (BYTE cmd, void* buf);
DRESULT diskRead (const BYTE* buf, uint32_t sector, uint32_t numSectors);
DRESULT diskWrite (const BYTE* buf, uint32_t sector, uint32_t numSectors);

DWORD getFatTime();
void diskDebugEnable();
void diskDebugDisable();

//{{{
#ifdef __cplusplus
}
#endif
//}}}
