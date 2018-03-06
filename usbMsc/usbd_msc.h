#pragma once
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}

#include "usbd.h"

extern USBD_DescriptorsTypeDef kMscDesc;
extern USBD_ClassTypeDef kUsbdMsc;

#define STANDARD_INQUIRY_DATA_LEN  0x24

//{{{  struct USBD_StorageTypeDef
typedef struct _USBD_STORAGE {
  int8_t (* Init) (uint8_t lun);
  int8_t (* GetCapacity) (uint8_t lun, uint32_t *block_num, uint16_t *block_size);
  int8_t (* IsReady) (uint8_t lun);
  int8_t (* IsWriteProtected) (uint8_t lun);
  int8_t (* Read) (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* Write)(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* GetMaxLun)(void);
  int8_t *pInquiry;
  } USBD_StorageTypeDef;
//}}}

int8_t SCSI_ProcessCmd (USBD_HandleTypeDef* pdev, uint8_t lun, uint8_t* cmd);
uint8_t USBD_MSC_RegisterStorage (USBD_HandleTypeDef* pdev, USBD_StorageTypeDef* fops);

//{{{
#ifdef __cplusplus
}
#endif
//}}}
