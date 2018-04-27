#pragma once
//{{{  integer
#include "stdint.h"

// These types MUST be 16-bit or 32-bit
typedef int            INT;
typedef unsigned int   UINT;

// This type MUST be 8-bit
typedef unsigned char  BYTE;

// These types MUST be 16-bit
typedef short          SHORT;
typedef unsigned short WORD;
typedef unsigned short WCHAR;

// These types MUST be 32-bit
typedef long           LONG;
typedef unsigned long  DWORD;

// This type MUST be 64-bit (Remove this for ANSI C (C89) compatibility)
typedef unsigned long long QWORD;
//}}}
//{{{  ffconf
#define _FFCONF 68300 /* Revision ID */

#define _FS_READONLY  0
#define _FS_MINIMIZE  0

#define _USE_STRFUNC  1
#define _USE_FIND     1
#define _USE_MKFS     1
#define _USE_FASTSEEK 1
#define _USE_EXPAND   0
#define _USE_CHMOD    1
#define _USE_LABEL    1
#define _USE_FORWARD  0
#define _CODE_PAGE  850

#define _USE_LFN      3
#define _MAX_LFN    255

#define _FS_RPATH 2

#define _VOLUMES          2
#define _STR_VOLUME_ID    0
#define _VOLUME_STRS  "RAM","NAND","CF","SD","SD2","USB","USB2","USB3"

#define _MIN_SS   512
#define _MAX_SS   4096

#define _USE_TRIM     0
#define _FS_NOFSINFO  0
#define _FS_TINY      0
#define _FS_EXFAT     1
#define _FS_LOCK      2

#define _FS_REENTRANT 0
#define _SYNC_t  int
//}}}
//{{{
#ifdef __cplusplus
extern "C" {
#endif
//}}}

//{{{  enum FRESULT
typedef enum {
  FR_OK = 0,        /* (0) Succeeded */
  FR_DISK_ERR,      /* (1) A hard error occurred in the low level disk I/O layer */
  FR_INT_ERR,       /* (2) Assertion failed */
  FR_NOT_READY,     /* (3) The physical drive cannot work */
  FR_NO_FILE,       /* (4) Could not find the file */
  FR_NO_PATH,       /* (5) Could not find the path */
  FR_INVALID_NAME,  /* (6) The path name format is invalid */
  FR_DENIED,        /* (7) Access denied due to prohibited access or directory full */
  FR_EXIST,         /* (8) Access denied due to prohibited access */
  FR_INVALID_OBJECT,   /* (9) The file/directory object is invalid */
  FR_WRITE_PROTECTED,  /* (10) The physical drive is write protected */
  FR_INVALID_DRIVE,    /* (11) The logical drive number is invalid */
  FR_NOT_ENABLED,      /* (12) The volume has no work area */
  FR_NO_FILESYSTEM,    /* (13) There is no valid FAT volume */
  FR_MKFS_ABORTED,     /* (14) The f_mkfs() aborted due to any problem */
  FR_TIMEOUT,          /* (15) Could not get a grant to access the volume within defined period */
  FR_LOCKED,           /* (16) The operation is rejected according to the file sharing policy */
  FR_NOT_ENOUGH_CORE,  /* (17) LFN working buffer could not be allocated */
  FR_TOO_MANY_OPEN_FILES, /* (18) Number of open files > _FS_LOCK */
  FR_INVALID_PARAMETER    /* (19) Given parameter is invalid */
  } FRESULT;
//}}}
//{{{  struct FATFS
typedef struct {
  BYTE  fs_type;    /* File system type (0:N/A) */
  BYTE  drv;        /* Physical drive number */
  BYTE  n_fats;     /* Number of FATs (1 or 2) */
  BYTE  wflag;      /* win[] flag (b0:dirty) */
  BYTE  fsi_flag;   /* FSINFO flags (b7:disabled, b0:dirty) */
  WORD  id;         /* File system mount ID */
  WORD  n_rootdir;  /* Number of root directory entries (FAT12/16) */
  WORD  csize;      /* Cluster size [sectors] */
  WORD  ssize;      /* Sector size (512, 1024, 2048 or 4096) */
  WCHAR*  lfnbuf;   /* LFN working buffer */
  BYTE* dirbuf;     /* Directory entry block scratchpad buffer */
  _SYNC_t sobj;     /* Identifier of sync object */
  DWORD last_clst;  /* Last allocated cluster */
  DWORD free_clst;  /* Number of free clusters */
  DWORD cdir;       /* Current directory start cluster (0:root) */
  DWORD cdc_scl;    /* Containing directory start cluster (invalid when cdir is 0) */
  DWORD cdc_size;   /* b31-b8:Size of containing directory, b7-b0: Chain status */
  DWORD cdc_ofs;    /* Offset in the containing directory (invalid when cdir is 0) */
  DWORD n_fatent;   /* Number of FAT entries (number of clusters + 2) */
  DWORD fsize;      /* Size of an FAT [sectors] */
  DWORD volbase;    /* Volume base sector */
  DWORD fatbase;    /* FAT base sector */
  DWORD dirbase;    /* Root directory base sector/cluster */
  DWORD database;   /* Data base sector */
  DWORD winsect;    /* Current sector appearing in the win[] */
  BYTE win[_MAX_SS];/* Disk access window for Directory, FAT (and file data at tiny cfg) */
  } FATFS;
//}}}
//{{{  struct _FDID
typedef struct {
  FATFS*  fs;      /* Pointer to the owner file system object */
  WORD  id;        /* Owner file system mount ID */
  BYTE  attr;      /* Object attribute */
  BYTE  stat;      /* Object chain status (b1-0: =0:not contiguous, =2:contiguous (no data on FAT), =3:flagmented in this session, b2:sub-directory stretched) */
  DWORD sclust;    /* Object start cluster (0:no cluster or root directory) */
  QWORD objsize; /* Object size (valid when sclust != 0) */
  DWORD n_cont;    /* Size of first fragment, clusters - 1 (valid when stat == 3) */
  DWORD n_frag;    /* Size of last fragment needs to be written (valid when not zero) */
  DWORD c_scl;     /* Containing directory start cluster (valid when sclust != 0) */
  DWORD c_size;    /* b31-b8:Size of containing directory, b7-b0: Chain status (valid when c_scl != 0) */
  DWORD c_ofs;     /* Offset in the containing directory (valid when sclust != 0 and non-directory object) */
  UINT  lockid;    /* File lock ID origin from 1 (index of file semaphore table Files[]) */
  } _FDID;
//}}}
//{{{  struct FIL
typedef struct {
  _FDID obj;      /* Object identifier (must be the 1st member to detect invalid object pointer) */
  BYTE  flag;     /* File status flags */
  BYTE  err;      /* Abort flag (error code) */
  QWORD fptr;   /* File read/write pointer (Zeroed on file open) */
  DWORD clust;    /* Current cluster of fpter (invalid when fptr is 0) */
  DWORD sect;     /* Sector number appearing in buf[] (0:invalid) */
  DWORD dir_sect; /* Sector number containing the directory entry */
  BYTE* dir_ptr;  /* Pointer to the directory entry in the win[] */
  DWORD*  cltbl;  /* Pointer to the cluster link map table (nulled on open, set by application) */
  BYTE  buf[_MAX_SS]; /* File private data read/write window */
  } FIL;

//}}}
//{{{  struct DIR
typedef struct {
  _FDID obj;      /* Object identifier */
  DWORD dptr;     /* Current read/write offset */
  DWORD clust;    /* Current cluster */
  DWORD sect;     /* Current sector (0:Read operation has terminated) */
  BYTE* dir;      /* Pointer to the directory item in the win[] */
  BYTE  fn[12];   /* SFN (in/out) {body[8],ext[3],status[1]} */
  DWORD blk_ofs;  /* Offset of current entry block being processed (0xFFFFFFFF:Invalid) */
  const char* pat;  /* Pointer to the name matching pattern */
  } DIR;
//}}}
//{{{  struct FILINFO
typedef struct {
  QWORD fsize;    /* File size */
  WORD  fdate;      /* Modified date */
  WORD  ftime;      /* Modified time */
  BYTE  fattrib;    /* File attribute */
  char altname[13];   /* Alternative file name */
  char fname[_MAX_LFN + 1];  /* Primary file name */
  } FILINFO;
//}}}

FRESULT f_mkfs (const char* path, BYTE opt, DWORD au, void* work, UINT len); // Create a FAT volume
FRESULT f_setlabel (const char* label);                                      // Set volume label

FRESULT f_mount (FATFS* fs, const char* path, BYTE opt);                     // Mount/Unmount logical drive
FRESULT f_getlabel (const char* path, char* label, DWORD* vsn);              // Get volume label
FRESULT f_getfree (const char* path, DWORD* nclst, FATFS** fatfs);           // Get number free clusters

FRESULT f_stat (const char* path, FILINFO* fno);                             // Get file status
FRESULT f_open (FIL* fp, const char* path, BYTE mode);                       // Open or create file
FRESULT f_lseek (FIL* fp, QWORD ofs);                                        // Move file pointer of file
FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br);                    // Read data from file
FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);             // Write data to file
FRESULT f_sync (FIL* fp);                                                    // Flush cached data of writing file
FRESULT f_truncate (FIL* fp);                                                // Truncate  file
FRESULT f_close (FIL* fp);                                                   // Close open file object

FRESULT f_chdir (const char* path);                                          // Change current directory
FRESULT f_getcwd (char* buff, UINT len);                                     // Get current directory
FRESULT f_opendir (DIR* dp, const char* path);                               // Open directory
FRESULT f_readdir (DIR* dp, FILINFO* fno);                                   // Read directory item
FRESULT f_findfirst (DIR* dp, FILINFO* fno, const char* path, const char* pattern); // Find first file
FRESULT f_findnext (DIR* dp, FILINFO* fno);                                  // Find next file
FRESULT f_closedir (DIR* dp);                                                // Close open directory

FRESULT f_mkdir (const char* path);                                          // Create sub directory
FRESULT f_unlink (const char* path);                                         // Delete existing file or directory
FRESULT f_rename (const char* path_old, const char* path_new);               // Rename/Move file or directory
FRESULT f_chmod (const char* path, BYTE attr, BYTE mask);                    // Change attribute of file/dir
FRESULT f_utime (const char* path, const FILINFO* fno);                      // Change timestamp of file/dir

FRESULT f_expand (FIL* fp, QWORD szf, BYTE opt);                             // Allocate a contiguous block of file
FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf); // Forward data to the stream

int f_putc (char c, FIL* fp);                                                // Put a character to file
int f_puts (const char* str, FIL* cp);                                       // Put a string to file
int f_printf (FIL* fp, const char* str, ...);                                // Put a formatted string to file
char* f_gets (char* buff, int len, FIL* fp);                                 // Get a string from file

#define f_eof(fp) ((int)((fp)->fptr == (fp)->obj.objsize))
#define f_error(fp) ((fp)->err)
#define f_tell(fp) ((fp)->fptr)
#define f_size(fp) ((fp)->obj.objsize)
#define f_rewind(fp) f_lseek((fp), 0)
#define f_rewinddir(dp) f_readdir((dp), 0)
#define f_rmdir(path) f_unlink(path)

#ifndef EOF
  #define EOF (-1)
#endif

// RTC function */
DWORD get_fattime();

/* Sync functions */
int ff_cre_syncobj (BYTE vol, _SYNC_t* sobj); // Create a sync object
int ff_req_grant (_SYNC_t sobj);              // Lock sync object
void ff_rel_grant (_SYNC_t sobj);             // Unlock sync object
int ff_del_syncobj (_SYNC_t sobj);            // Delete a sync object

// File access mode and open method flags (3rd argument of f_open)
#define FA_READ           0x01
#define FA_WRITE          0x02
#define FA_OPEN_EXISTING  0x00
#define FA_CREATE_NEW     0x04
#define FA_CREATE_ALWAYS  0x08
#define FA_OPEN_ALWAYS    0x10
#define FA_OPEN_APPEND    0x30

// Fast seek controls (2nd argument of f_lseek)
#define CREATE_LINKMAP  ((QWORD)0 - 1)

// Format options (2nd argument of f_mkfs)
#define FM_FAT    0x01
#define FM_FAT32  0x02
#define FM_EXFAT  0x04
#define FM_ANY    0x07

// Filesystem type (FATFS.fs_type)
#define FS_FAT12  1
#define FS_FAT16  2
#define FS_FAT32  3
#define FS_EXFAT  4

// File attribute bits for directory entry (FILINFO.fattrib)
#define AM_RDO  0x01  /* Read only */
#define AM_HID  0x02  /* Hidden */
#define AM_SYS  0x04  /* System */
#define AM_DIR  0x10  /* Directory */
#define AM_ARC  0x20  /* Archive */

//{{{
#ifdef __cplusplus
}
#endif
//}}}
