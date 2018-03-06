// usbd_msc.cpp
#include "usbd_msc.h"

// msc descriptors
//{{{  defines
#define USBD_VID                      0x0483
#define USBD_PID                      0x5720

#define USBD_LANGID_STRING            0x409
#define USBD_MANUFACTURER_STRING      "STMicroelectronics"
#define USBD_PRODUCT_HS_STRING        "Mass Storage in HS Mode"
#define USBD_PRODUCT_FS_STRING        "Mass Storage in FS Mode"
#define USBD_CONFIGURATION_HS_STRING  "MSC Config"
#define USBD_INTERFACE_HS_STRING      "MSC Interface"
#define USBD_CONFIGURATION_FS_STRING  "MSC Config"
#define USBD_INTERFACE_FS_STRING      "MSC Interface"
//}}}
//{{{
__ALIGN_BEGIN const uint8_t USBD_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END = {
  0x12,                       /* bLength */
  USB_DESC_TYPE_DEVICE,       /* bDescriptorType */
  0x00,                       /* bcdUSB */
  0x02,
  0x00,                       /* bDeviceClass */
  0x00,                       /* bDeviceSubClass */
  0x00,                       /* bDeviceProtocol */
  USB_MAX_EP0_SIZE,           /* bMaxPacketSize */
  LOBYTE(USBD_VID),           /* idVendor */
  HIBYTE(USBD_VID),           /* idVendor */
  LOBYTE(USBD_PID),           /* idVendor */
  HIBYTE(USBD_PID),           /* idVendor */
  0x00,                       /* bcdDevice rel. 2.00 */
  0x02,
  USBD_IDX_MFC_STR,           /* Index of manufacturer string */
  USBD_IDX_PRODUCT_STR,       /* Index of product string */
  USBD_IDX_SERIAL_STR,        /* Index of serial number string */
  USBD_MAX_NUM_CONFIGURATION  /* bNumConfigurations */
  };
//}}}
//{{{
/* USB Standard Device Descriptor */
__ALIGN_BEGIN const uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
  USB_LEN_LANGID_STR_DESC,
  USB_DESC_TYPE_STRING,
  LOBYTE(USBD_LANGID_STRING),
  HIBYTE(USBD_LANGID_STRING),
  };
//}}}
//{{{
uint8_t USBD_StringSerial[USB_SIZ_STRING_SERIAL] = {
  USB_SIZ_STRING_SERIAL,
  USB_DESC_TYPE_STRING,
  };
//}}}
__ALIGN_BEGIN uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;
//{{{
void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len) {

  uint8_t idx = 0;
  for (idx = 0; idx < len; idx ++) {
    if (((value >> 28)) < 0xA )
      pbuf[ 2* idx] = (value >> 28) + '0';
    else
      pbuf[2* idx] = (value >> 28) + 'A' - 10;

    value = value << 4;
    pbuf[ 2* idx + 1] = 0;
    }
  }
//}}}
//{{{
void Get_SerialNum() {

  auto deviceserial0 = *(uint32_t*)DEVICE_ID1;
  auto deviceserial1 = *(uint32_t*)DEVICE_ID2;
  auto deviceserial2 = *(uint32_t*)DEVICE_ID3;
  deviceserial0 += deviceserial2;
  if (deviceserial0 != 0) {
    IntToUnicode (deviceserial0, (uint8_t*)&USBD_StringSerial[2] ,8);
    IntToUnicode (deviceserial1, (uint8_t*)&USBD_StringSerial[18] ,4);
    }
  }
//}}}
//{{{
uint8_t* USBD_MSC_DeviceDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  *length = sizeof(USBD_DeviceDesc);
  return (uint8_t*)USBD_DeviceDesc;
  }
//}}}
//{{{
uint8_t* USBD_MSC_LangIDStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  *length = sizeof(USBD_LangIDDesc);
  return (uint8_t*)USBD_LangIDDesc;
  }
//}}}
//{{{
uint8_t* USBD_MSC_ProductStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  if(speed == USBD_SPEED_HIGH)
    USBD_GetString((uint8_t *)USBD_PRODUCT_HS_STRING, USBD_StrDesc, length);
  else
    USBD_GetString((uint8_t *)USBD_PRODUCT_FS_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
  }
//}}}
//{{{
uint8_t* USBD_MSC_ManufacturerStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  USBD_GetString((uint8_t *)USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
  }
//}}}
//{{{
uint8_t* USBD_MSC_SerialStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  *length = USB_SIZ_STRING_SERIAL;
  Get_SerialNum();
  return (uint8_t*)USBD_StringSerial;
  }
//}}}
//{{{
uint8_t* USBD_MSC_ConfigStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  if(speed == USBD_SPEED_HIGH)
    USBD_GetString((uint8_t *)USBD_CONFIGURATION_HS_STRING, USBD_StrDesc, length);
  else
    USBD_GetString((uint8_t *)USBD_CONFIGURATION_FS_STRING, USBD_StrDesc, length);

  return USBD_StrDesc;
  }
//}}}
//{{{
uint8_t* USBD_MSC_InterfaceStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  if (speed == USBD_SPEED_HIGH)
    USBD_GetString((uint8_t *)USBD_INTERFACE_HS_STRING, USBD_StrDesc, length);
  else
    USBD_GetString((uint8_t *)USBD_INTERFACE_FS_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
  }
//}}}
//{{{
USBD_DescriptorsTypeDef kMscDesc = {
  USBD_MSC_DeviceDescriptor,
  USBD_MSC_LangIDStrDescriptor,
  USBD_MSC_ManufacturerStrDescriptor,
  USBD_MSC_ProductStrDescriptor,
  USBD_MSC_SerialStrDescriptor,
  USBD_MSC_ConfigStrDescriptor,
  USBD_MSC_InterfaceStrDescriptor,
  };
//}}}

// msc class
//{{{  defines
//{{{  BOT
#define USBD_BOT_IDLE                      0       /* Idle state */
#define USBD_BOT_DATA_OUT                  1       /* Data Out state */
#define USBD_BOT_DATA_IN                   2       /* Data In state */
#define USBD_BOT_LAST_DATA_IN              3       /* Last Data In Last */
#define USBD_BOT_SEND_DATA                 4       /* Send Immediate data */
#define USBD_BOT_NO_DATA                   5       /* No data Stage */

#define USBD_BOT_CBW_SIGNATURE             0x43425355
#define USBD_BOT_CSW_SIGNATURE             0x53425355
#define USBD_BOT_CBW_LENGTH                31
#define USBD_BOT_CSW_LENGTH                13
#define USBD_BOT_MAX_DATA                  256

#define USBD_BOT_STATUS_NORMAL             0
#define USBD_BOT_STATUS_RECOVERY           1
#define USBD_BOT_STATUS_ERROR              2
//}}}
//{{{  CSW Status Definitions
#define USBD_CSW_CMD_PASSED                0x00
#define USBD_CSW_CMD_FAILED                0x01
#define USBD_CSW_PHASE_ERROR               0x02
//}}}

#define USBD_DIR_IN     0
#define USBD_DIR_OUT    1
#define USBD_BOTH_DIR   2

#define MSC_MAX_FS_PACKET        0x40
#define MSC_MAX_HS_PACKET        0x200
#define BOT_GET_MAX_LUN          0xFE
#define BOT_RESET                0xFF
#define USB_MSC_CONFIG_DESC_SIZ  32
#define MSC_EPIN_ADDR            0x81
#define MSC_EPOUT_ADDR           0x01

#define MODE_SENSE6_LEN          8
#define MODE_SENSE10_LEN         8
#define LENGTH_INQUIRY_PAGE00    7
#define LENGTH_FORMAT_CAPACITIES 20

#define SENSE_LIST_DEEPTH         4
//{{{  SCSI Commands
 #define SCSI_FORMAT_UNIT                            0x04
#define SCSI_INQUIRY                                0x12
#define SCSI_MODE_SELECT6                           0x15
#define SCSI_MODE_SELECT10                          0x55
#define SCSI_MODE_SENSE6                            0x1A
#define SCSI_MODE_SENSE10                           0x5A
#define SCSI_ALLOW_MEDIUM_REMOVAL                   0x1E
#define SCSI_READ6                                  0x08
#define SCSI_READ10                                 0x28
#define SCSI_READ12                                 0xA8
#define SCSI_READ16                                 0x88

#define SCSI_READ_CAPACITY10                        0x25
#define SCSI_READ_CAPACITY16                        0x9E

#define SCSI_REQUEST_SENSE                          0x03
#define SCSI_START_STOP_UNIT                        0x1B
#define SCSI_TEST_UNIT_READY                        0x00
#define SCSI_WRITE6                                 0x0A
#define SCSI_WRITE10                                0x2A
#define SCSI_WRITE12                                0xAA
#define SCSI_WRITE16                                0x8A

#define SCSI_VERIFY10                               0x2F
#define SCSI_VERIFY12                               0xAF
#define SCSI_VERIFY16                               0x8F

#define SCSI_SEND_DIAGNOSTIC                        0x1D
#define SCSI_READ_FORMAT_CAPACITIES                 0x23


#define NO_SENSE                                    0
#define RECOVERED_ERROR                             1
#define NOT_READY                                   2
#define MEDIUM_ERROR                                3
#define HARDWARE_ERROR                              4
#define ILLEGAL_REQUEST                             5
#define UNIT_ATTENTION                              6
#define DATA_PROTECT                                7
#define BLANK_CHECK                                 8
#define VENDOR_SPECIFIC                             9
#define COPY_ABORTED                                10
#define ABORTED_COMMAND                             11
#define VOLUME_OVERFLOW                             13
#define MISCOMPARE                                  14
//}}}

#define INVALID_CDB                       0x20
#define INVALID_FIELED_IN_COMMAND         0x24
#define PARAMETER_LIST_LENGTH_ERROR       0x1A
#define INVALID_FIELD_IN_PARAMETER_LIST   0x26
#define ADDRESS_OUT_OF_RANGE              0x21
#define MEDIUM_NOT_PRESENT                0x3A
#define MEDIUM_HAVE_CHANGED               0x28
#define WRITE_PROTECTED                   0x27
#define UNRECOVERED_READ_ERROR            0x11
#define WRITE_FAULT                       0x03

#define READ_FORMAT_CAPACITY_DATA_LEN     0x0C
#define READ_CAPACITY10_DATA_LEN          0x08
#define MODE_SENSE10_DATA_LEN             0x08
#define MODE_SENSE6_DATA_LEN              0x04
#define REQUEST_SENSE_DATA_LEN            0x12
#define BLKVFY                            0x04
//}}}
//{{{
const uint8_t MSC_Page00_Inquiry_Data[] = {
  // 7
  0x00,
  0x00,
  0x00,
  (LENGTH_INQUIRY_PAGE00 - 4),
  0x00,
  0x80,
  0x83
  };
//}}}
//{{{
const uint8_t MSC_Mode_Sense6_data[] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
  };
//}}}
//{{{
const uint8_t MSC_Mode_Sense10_data[] = {
  0x00,
  0x06,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
  };
//}}}
//{{{
/* USB Mass storage device Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
__ALIGN_BEGIN const uint8_t USBD_MSC_CfgHSDesc[USB_MSC_CONFIG_DESC_SIZ] __ALIGN_END = {
  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType: Configuration */
  USB_MSC_CONFIG_DESC_SIZ,

  0x00,
  0x01,   /* bNumInterfaces: 1 interface */
  0x01,   /* bConfigurationValue: */
  0x04,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0x32,   /* MaxPower 100 mA */

  /********************  Mass Storage interface ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0x08,   /* bInterfaceClass: MSC Class */
  0x06,   /* bInterfaceSubClass : SCSI transparent*/
  0x50,   /* nInterfaceProtocol */
  0x05,          /* iInterface: */

  /********************  Mass Storage Endpoints ********************/
  0x07,   /*Endpoint descriptor length = 7*/
  0x05,   /*Endpoint descriptor type */
  MSC_EPIN_ADDR,   /*Endpoint address (IN, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(MSC_MAX_HS_PACKET),
  HIBYTE(MSC_MAX_HS_PACKET),
  0x00,   /*Polling interval in milliseconds */

  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  MSC_EPOUT_ADDR,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(MSC_MAX_HS_PACKET),
  HIBYTE(MSC_MAX_HS_PACKET),
  0x00     /*Polling interval in milliseconds*/
  };
//}}}
//{{{
/* USB Mass storage device Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
__ALIGN_BEGIN const uint8_t USBD_MSC_CfgFSDesc[USB_MSC_CONFIG_DESC_SIZ] __ALIGN_END = {
  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType: Configuration */
  USB_MSC_CONFIG_DESC_SIZ,

  0x00,
  0x01,   /* bNumInterfaces: 1 interface */
  0x01,   /* bConfigurationValue: */
  0x04,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0x32,   /* MaxPower 100 mA */

  /********************  Mass Storage interface ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0x08,   /* bInterfaceClass: MSC Class */
  0x06,   /* bInterfaceSubClass : SCSI transparent*/
  0x50,   /* nInterfaceProtocol */
  0x05,          /* iInterface: */

  /********************  Mass Storage Endpoints ********************/
  0x07,   /*Endpoint descriptor length = 7*/
  0x05,   /*Endpoint descriptor type */
  MSC_EPIN_ADDR,   /*Endpoint address (IN, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(MSC_MAX_FS_PACKET),
  HIBYTE(MSC_MAX_FS_PACKET),
  0x00,   /*Polling interval in milliseconds */

  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  MSC_EPOUT_ADDR,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(MSC_MAX_FS_PACKET),
  HIBYTE(MSC_MAX_FS_PACKET),
  0x00     /*Polling interval in milliseconds*/
  };
//}}}
//{{{
__ALIGN_BEGIN const uint8_t USBD_MSC_OtherSpeedCfgDesc[USB_MSC_CONFIG_DESC_SIZ]  __ALIGN_END  = {
  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION,
  USB_MSC_CONFIG_DESC_SIZ,

  0x00,
  0x01,   /* bNumInterfaces: 1 interface */
  0x01,   /* bConfigurationValue: */
  0x04,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0x32,   /* MaxPower 100 mA */

  /********************  Mass Storage interface ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0x08,   /* bInterfaceClass: MSC Class */
  0x06,   /* bInterfaceSubClass : SCSI transparent command set*/
  0x50,   /* nInterfaceProtocol */
  0x05,          /* iInterface: */

  /********************  Mass Storage Endpoints ********************/
  0x07,   /*Endpoint descriptor length = 7*/
  0x05,   /*Endpoint descriptor type */
  MSC_EPIN_ADDR,   /*Endpoint address (IN, address 1) */
  0x02,   /*Bulk endpoint type */
  0x40,
  0x00,
  0x00,   /*Polling interval in milliseconds */

  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  MSC_EPOUT_ADDR,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  0x40,
  0x00,
  0x00     /*Polling interval in milliseconds*/
  };
//}}}
//{{{
__ALIGN_BEGIN const uint8_t USBD_MSC_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  MSC_MAX_FS_PACKET,
  0x01,
  0x00,
  };
//}}}

#define MSC_MEDIA_PACKET 512
//{{{  struct USBD_SCSI_SenseTypeDef
typedef struct _SENSE_ITEM {
  char Skey;
  union {
    struct _ASCs {
      char ASC;
      char ASCQ;
      } b;
    unsigned int  ASC;
    char* pData;
    } w;
  } USBD_SCSI_SenseTypeDef;
//}}}
//{{{  struct USBD_MSC_BOT_CBWTypeDef
typedef struct {
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataLength;
  uint8_t  bmFlags;
  uint8_t  bLUN;
  uint8_t  bCBLength;
  uint8_t  CB[16];
  uint8_t  ReservedForAlign;
  } USBD_MSC_BOT_CBWTypeDef;
//}}}
//{{{  struct USBD_MSC_BOT_CSWTypeDef
typedef struct {
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataResidue;
  uint8_t  bStatus;
  uint8_t  ReservedForAlign[3];
  } USBD_MSC_BOT_CSWTypeDef;
//}}}
//{{{  struct sMscData
typedef struct {
  uint32_t                 max_lun;
  uint32_t                 interface;
  uint8_t                  bot_state;
  uint8_t                  bot_status;
  uint16_t                 bot_data_length;
  uint8_t                  bot_data[MSC_MEDIA_PACKET];
  USBD_MSC_BOT_CBWTypeDef  cbw;
  USBD_MSC_BOT_CSWTypeDef  csw;

  USBD_SCSI_SenseTypeDef   scsi_sense [SENSE_LIST_DEEPTH];
  uint8_t                  scsi_sense_head;
  uint8_t                  scsi_sense_tail;

  uint16_t                 scsi_blk_size;
  uint32_t                 scsi_blk_nbr;

  uint32_t                 scsi_blk_addr;
  uint32_t                 scsi_blk_len;
  } sMscData;
//}}}

// msc_scsi
//{{{
void MSC_BOT_SendCSW (USBD_HandleTypeDef* usbdHandle, uint8_t CSW_Status) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  mscData->csw.dSignature = USBD_BOT_CSW_SIGNATURE;
  mscData->csw.bStatus = CSW_Status;
  mscData->bot_state = USBD_BOT_IDLE;

  USBD_LL_Transmit (usbdHandle, MSC_EPIN_ADDR, (uint8_t*)&mscData->csw, USBD_BOT_CSW_LENGTH);

  /* Prepare EP to Receive next Cmd */
  USBD_LL_PrepareReceive (usbdHandle, MSC_EPOUT_ADDR, (uint8_t*)&mscData->cbw, USBD_BOT_CBW_LENGTH);
  }
//}}}
//{{{
void SCSI_SenseCode (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t sKey, uint8_t ASC) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  mscData->scsi_sense[mscData->scsi_sense_tail].Skey  = sKey;
  mscData->scsi_sense[mscData->scsi_sense_tail].w.ASC = ASC << 8;
  mscData->scsi_sense_tail++;
  if (mscData->scsi_sense_tail == SENSE_LIST_DEEPTH)
    mscData->scsi_sense_tail = 0;
  }
//}}}
//{{{
int8_t SCSI_TestUnitReady (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  // case 9 : Hi > D0
  if (mscData->cbw.dDataLength != 0) {
    SCSI_SenseCode (usbdHandle, mscData->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
    return -1;
    }

  if (((USBD_StorageTypeDef *)usbdHandle->pUserData)->IsReady(lun) !=0 ) {
    SCSI_SenseCode (usbdHandle, lun, NOT_READY, MEDIUM_NOT_PRESENT);
    mscData->bot_state = USBD_BOT_NO_DATA;
    return -1;
    }

  mscData->bot_data_length = 0;
  return 0;
  }
//}}}
//{{{
int8_t SCSI_Inquiry (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  uint8_t* pPage;
  uint16_t len;
  auto mscData = (sMscData*)usbdHandle->pClassData;

  if (params[1] & 0x01) {/*Evpd is set*/
    pPage = (uint8_t *)MSC_Page00_Inquiry_Data;
    len = LENGTH_INQUIRY_PAGE00;
    }
  else {
    pPage = (uint8_t *)&((USBD_StorageTypeDef *)usbdHandle->pUserData)->pInquiry[lun * STANDARD_INQUIRY_DATA_LEN];
    len = pPage[4] + 5;
    if (params[4] <= len)
      len = params[4];
    }
  mscData->bot_data_length = len;

  while (len) {
    len--;
    mscData->bot_data[len] = pPage[len];
    }

  return 0;
  }
//}}}
//{{{
int8_t SCSI_ReadCapacity10 (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if (((USBD_StorageTypeDef*)usbdHandle->pUserData)->GetCapacity(lun, &mscData->scsi_blk_nbr, &mscData->scsi_blk_size) != 0) {
    SCSI_SenseCode (usbdHandle, lun, NOT_READY, MEDIUM_NOT_PRESENT);
    return -1;
    }
  else {
    mscData->bot_data[0] = (uint8_t)((mscData->scsi_blk_nbr - 1) >> 24);
    mscData->bot_data[1] = (uint8_t)((mscData->scsi_blk_nbr - 1) >> 16);
    mscData->bot_data[2] = (uint8_t)((mscData->scsi_blk_nbr - 1) >>  8);
    mscData->bot_data[3] = (uint8_t)(mscData->scsi_blk_nbr - 1);

    mscData->bot_data[4] = (uint8_t)(mscData->scsi_blk_size >>  24);
    mscData->bot_data[5] = (uint8_t)(mscData->scsi_blk_size >>  16);
    mscData->bot_data[6] = (uint8_t)(mscData->scsi_blk_size >>  8);
    mscData->bot_data[7] = (uint8_t)(mscData->scsi_blk_size);

    mscData->bot_data_length = 8;
    return 0;
    }
  }
//}}}
//{{{
int8_t SCSI_ReadFormatCapacity (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  uint16_t blk_size;
  uint32_t blk_nbr;
  uint16_t i;

  for (i = 0 ; i < 12 ; i++)
    mscData->bot_data[i] = 0;

  if (((USBD_StorageTypeDef*)usbdHandle->pUserData)->GetCapacity(lun, &blk_nbr, &blk_size) != 0) {
    SCSI_SenseCode (usbdHandle, lun, NOT_READY, MEDIUM_NOT_PRESENT);
    return -1;
    }
  else {
    mscData->bot_data[3] = 0x08;
    mscData->bot_data[4] = (uint8_t)((blk_nbr - 1) >> 24);
    mscData->bot_data[5] = (uint8_t)((blk_nbr - 1) >> 16);
    mscData->bot_data[6] = (uint8_t)((blk_nbr - 1) >>  8);
    mscData->bot_data[7] = (uint8_t)(blk_nbr - 1);

    mscData->bot_data[8] = 0x02;
    mscData->bot_data[9] = (uint8_t)(blk_size >>  16);
    mscData->bot_data[10] = (uint8_t)(blk_size >>  8);
    mscData->bot_data[11] = (uint8_t)(blk_size);

    mscData->bot_data_length = 12;
    return 0;
    }
  }
//}}}
//{{{
int8_t SCSI_ModeSense6 (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;
  uint16_t len = 8 ;
  mscData->bot_data_length = len;

  while (len) {
    len--;
    mscData->bot_data[len] = MSC_Mode_Sense6_data[len];
    }
  return 0;
  }
//}}}
//{{{
int8_t SCSI_ModeSense10 (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  uint16_t len = 8;
  mscData->bot_data_length = len;
  while (len) {
    len--;
    mscData->bot_data[len] = MSC_Mode_Sense10_data[len];
    }
  return 0;
  }
//}}}
//{{{
int8_t SCSI_RequestSense (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  for (uint8_t i = 0 ; i < REQUEST_SENSE_DATA_LEN ; i++)
    mscData->bot_data[i] = 0;

  mscData->bot_data[0] = 0x70;
  mscData->bot_data[7] = REQUEST_SENSE_DATA_LEN - 6;

  if((mscData->scsi_sense_head != mscData->scsi_sense_tail)) {
    mscData->bot_data[2]     = mscData->scsi_sense[mscData->scsi_sense_head].Skey;
    mscData->bot_data[12]    = mscData->scsi_sense[mscData->scsi_sense_head].w.b.ASCQ;
    mscData->bot_data[13]    = mscData->scsi_sense[mscData->scsi_sense_head].w.b.ASC;
    mscData->scsi_sense_head++;

    if (mscData->scsi_sense_head == SENSE_LIST_DEEPTH)
      mscData->scsi_sense_head = 0;
    }
  mscData->bot_data_length = REQUEST_SENSE_DATA_LEN;

  if (params[4] <= REQUEST_SENSE_DATA_LEN)
    mscData->bot_data_length = params[4];

  return 0;
  }
//}}}
//{{{
int8_t SCSI_StartStopUnit (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*) usbdHandle->pClassData;
  mscData->bot_data_length = 0;
  return 0;
  }
//}}}
//{{{
int8_t SCSI_ProcessRead (USBD_HandleTypeDef* usbdHandle, uint8_t lun) {

  auto mscData = (sMscData*)usbdHandle->pClassData;
  uint32_t len = MIN(mscData->scsi_blk_len, MSC_MEDIA_PACKET);

  if (((USBD_StorageTypeDef*)usbdHandle->pUserData)->Read (
      lun , mscData->bot_data, mscData->scsi_blk_addr / mscData->scsi_blk_size, len / mscData->scsi_blk_size) < 0) {
    SCSI_SenseCode (usbdHandle, lun, HARDWARE_ERROR, UNRECOVERED_READ_ERROR);
    return -1;
    }

  USBD_LL_Transmit (usbdHandle, MSC_EPIN_ADDR, mscData->bot_data, len);

  mscData->scsi_blk_addr += len;
  mscData->scsi_blk_len -= len;

  /* case 6 : Hi = Di */
  mscData->csw.dDataResidue -= len;

  if (mscData->scsi_blk_len == 0)
    mscData->bot_state = USBD_BOT_LAST_DATA_IN;
  return 0;
  }
//}}}
//{{{
int8_t SCSI_ProcessWrite (USBD_HandleTypeDef* usbdHandle, uint8_t lun) {

  auto mscData = (sMscData*) usbdHandle->pClassData;
  uint32_t len = MIN(mscData->scsi_blk_len, MSC_MEDIA_PACKET);

  if (((USBD_StorageTypeDef*)usbdHandle->pUserData)->Write (
    lun , mscData->bot_data, mscData->scsi_blk_addr / mscData->scsi_blk_size, len / mscData->scsi_blk_size) < 0) {
    SCSI_SenseCode (usbdHandle, lun, HARDWARE_ERROR, WRITE_FAULT);
    return -1;
    }

  mscData->scsi_blk_addr  += len;
  mscData->scsi_blk_len   -= len;

  /* case 12 : Ho = Do */
  mscData->csw.dDataResidue -= len;

  if (mscData->scsi_blk_len == 0)
    MSC_BOT_SendCSW (usbdHandle, USBD_CSW_CMD_PASSED);
  else
    /* Prepare EP to Receive next packet */
    USBD_LL_PrepareReceive (usbdHandle, MSC_EPOUT_ADDR, mscData->bot_data, MIN (mscData->scsi_blk_len, MSC_MEDIA_PACKET));

  return 0;
  }
//}}}
//{{{
int8_t SCSI_CheckAddressRange (USBD_HandleTypeDef* usbdHandle, uint8_t lun , uint32_t blk_offset ,uint16_t blk_nbr) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if ((blk_offset + blk_nbr) > mscData->scsi_blk_nbr ) {
    SCSI_SenseCode (usbdHandle, lun, ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
    return -1;
    }
  return 0;
  }
//}}}
//{{{
int8_t SCSI_Read10 (USBD_HandleTypeDef* usbdHandle, uint8_t lun , uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if (mscData->bot_state == USBD_BOT_IDLE) { /* Idle */
    /* case 10 : Ho <> Di */
    if ((mscData->cbw.bmFlags & 0x80) != 0x80) {
      SCSI_SenseCode (usbdHandle, mscData->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
      return -1;
      }

    if(((USBD_StorageTypeDef*)usbdHandle->pUserData)->IsReady(lun) !=0 ) {
      SCSI_SenseCode (usbdHandle, lun, NOT_READY, MEDIUM_NOT_PRESENT);
      return -1;
      }

    mscData->scsi_blk_addr = (params[2] << 24) | (params[3] << 16) | (params[4] <<  8) | params[5];
    mscData->scsi_blk_len =  (params[7] <<  8) | params[8];
    if (SCSI_CheckAddressRange (usbdHandle, lun, mscData->scsi_blk_addr, mscData->scsi_blk_len) < 0)
      return -1; /* error */

    mscData->bot_state = USBD_BOT_DATA_IN;
    mscData->scsi_blk_addr *= mscData->scsi_blk_size;
    mscData->scsi_blk_len *= mscData->scsi_blk_size;

    /* cases 4,5 : Hi <> Dn */
    if (mscData->cbw.dDataLength != mscData->scsi_blk_len) {
      SCSI_SenseCode (usbdHandle, mscData->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
      return -1;
      }
    }

  mscData->bot_data_length = MSC_MEDIA_PACKET;
  return SCSI_ProcessRead (usbdHandle, lun);
  }
//}}}
//{{{
int8_t SCSI_Write10 (USBD_HandleTypeDef* usbdHandle, uint8_t lun , uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if (mscData->bot_state == USBD_BOT_IDLE) { /* Idle */
    /* case 8 : Hi <> Do */
    if ((mscData->cbw.bmFlags & 0x80) == 0x80) {
      SCSI_SenseCode (usbdHandle, mscData->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
      return -1;
      }

    /* Check whether Media is ready */
    if (((USBD_StorageTypeDef*)usbdHandle->pUserData)->IsReady(lun) !=0 ) {
      SCSI_SenseCode (usbdHandle, lun, NOT_READY, MEDIUM_NOT_PRESENT);
      return -1;
      }

    /* Check If media is write-protected */
    if (((USBD_StorageTypeDef*)usbdHandle->pUserData)->IsWriteProtected(lun) !=0 ) {
      SCSI_SenseCode (usbdHandle, lun, NOT_READY, WRITE_PROTECTED);
      return -1;
      }

    mscData->scsi_blk_addr = (params[2] << 24) | (params[3] << 16) | (params[4] <<  8) | params[5];
    mscData->scsi_blk_len = (params[7] <<  8) | params[8];

    /* check if LBA address is in the right range */
    if(SCSI_CheckAddressRange (usbdHandle, lun, mscData->scsi_blk_addr, mscData->scsi_blk_len) < 0)
      return -1; /* error */

    mscData->scsi_blk_addr *= mscData->scsi_blk_size;
    mscData->scsi_blk_len  *= mscData->scsi_blk_size;

    /* cases 3,11,13 : Hn,Ho <> D0 */
    if (mscData->cbw.dDataLength != mscData->scsi_blk_len) {
      SCSI_SenseCode (usbdHandle, mscData->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
      return -1;
      }

    /* Prepare EP to receive first data packet */
    mscData->bot_state = USBD_BOT_DATA_OUT;
    USBD_LL_PrepareReceive (usbdHandle, MSC_EPOUT_ADDR, mscData->bot_data, MIN (mscData->scsi_blk_len, MSC_MEDIA_PACKET));
    }
  else /* Write Process ongoing */
    return SCSI_ProcessWrite (usbdHandle, lun);

  return 0;
  }
//}}}
//{{{
int8_t SCSI_Verify10 (USBD_HandleTypeDef* usbdHandle, uint8_t lun , uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if ((params[1]& 0x02) == 0x02) {
    SCSI_SenseCode (usbdHandle, lun, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
    return -1; /* Error, Verify Mode Not supported*/
    }

  if(SCSI_CheckAddressRange (usbdHandle, lun, mscData->scsi_blk_addr, mscData->scsi_blk_len) < 0)
    return -1; /* error */
  mscData->bot_data_length = 0;
  return 0;
  }
//}}}
//{{{
int8_t SCSI_ProcessCmd (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t *params) {

  switch (params[0]) {
    case SCSI_TEST_UNIT_READY:
      return SCSI_TestUnitReady (usbdHandle, lun, params);

    case SCSI_REQUEST_SENSE:
      return SCSI_RequestSense (usbdHandle, lun, params);

    case SCSI_INQUIRY:
      return SCSI_Inquiry(usbdHandle, lun, params);

    case SCSI_START_STOP_UNIT:
      return SCSI_StartStopUnit (usbdHandle, lun, params);

    case SCSI_ALLOW_MEDIUM_REMOVAL:
      return SCSI_StartStopUnit (usbdHandle, lun, params);

    case SCSI_MODE_SENSE6:
      return SCSI_ModeSense6 (usbdHandle, lun, params);

    case SCSI_MODE_SENSE10:
      return SCSI_ModeSense10 (usbdHandle, lun, params);

    case SCSI_READ_FORMAT_CAPACITIES:
      return SCSI_ReadFormatCapacity (usbdHandle, lun, params);

    case SCSI_READ_CAPACITY10:
      return SCSI_ReadCapacity10 (usbdHandle, lun, params);

    case SCSI_READ10:
      return SCSI_Read10 (usbdHandle, lun, params);

    case SCSI_WRITE10:
      return SCSI_Write10 (usbdHandle, lun, params);

    case SCSI_VERIFY10:
      return SCSI_Verify10 (usbdHandle, lun, params);

    default:
      SCSI_SenseCode (usbdHandle, lun, ILLEGAL_REQUEST, INVALID_CDB);
      return -1;
    }
  }
//}}}

//{{{
void MSC_BOT_Abort (USBD_HandleTypeDef* usbdHandle) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if ((mscData->cbw.bmFlags == 0) &&
      (mscData->cbw.dDataLength != 0) &&
      (mscData->bot_status == USBD_BOT_STATUS_NORMAL))
    USBD_LL_StallEP (usbdHandle, MSC_EPOUT_ADDR);

  USBD_LL_StallEP (usbdHandle, MSC_EPIN_ADDR);

  if (mscData->bot_status == USBD_BOT_STATUS_ERROR)
    USBD_LL_PrepareReceive (usbdHandle, MSC_EPOUT_ADDR, (uint8_t*)&mscData->cbw, USBD_BOT_CBW_LENGTH);
  }
//}}}
//{{{
void MSC_BOT_SendData(USBD_HandleTypeDef* usbdHandle, uint8_t* buf, uint16_t len) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  len = MIN (mscData->cbw.dDataLength, len);
  mscData->csw.dDataResidue -= len;
  mscData->csw.bStatus = USBD_CSW_CMD_PASSED;
  mscData->bot_state = USBD_BOT_SEND_DATA;

  USBD_LL_Transmit (usbdHandle, MSC_EPIN_ADDR, buf, len);
  }
//}}}
//{{{
void MSC_BOT_CBW_Decode (USBD_HandleTypeDef* usbdHandle) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  mscData->csw.dTag = mscData->cbw.dTag;
  mscData->csw.dDataResidue = mscData->cbw.dDataLength;

  if ((USBD_LL_GetRxDataSize (usbdHandle ,MSC_EPOUT_ADDR) != USBD_BOT_CBW_LENGTH) ||
      (mscData->cbw.dSignature != USBD_BOT_CBW_SIGNATURE)||
        (mscData->cbw.bLUN > 1) ||
          (mscData->cbw.bCBLength < 1) ||
            (mscData->cbw.bCBLength > 16)) {

    SCSI_SenseCode(usbdHandle, mscData->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
    mscData->bot_status = USBD_BOT_STATUS_ERROR;
    MSC_BOT_Abort(usbdHandle);
    }
  else {
    if(SCSI_ProcessCmd(usbdHandle, mscData->cbw.bLUN, &mscData->cbw.CB[0]) < 0) {
      if (mscData->bot_state == USBD_BOT_NO_DATA)
       MSC_BOT_SendCSW (usbdHandle, USBD_CSW_CMD_FAILED);
      else
        MSC_BOT_Abort(usbdHandle);
      }

    /*Burst xfer handled internally*/
    else if ((mscData->bot_state != USBD_BOT_DATA_IN) &&
             (mscData->bot_state != USBD_BOT_DATA_OUT) &&
             (mscData->bot_state != USBD_BOT_LAST_DATA_IN)) {
      if (mscData->bot_data_length > 0)
        MSC_BOT_SendData(usbdHandle, mscData->bot_data, mscData->bot_data_length);
      else if (mscData->bot_data_length == 0)
        MSC_BOT_SendCSW (usbdHandle, USBD_CSW_CMD_PASSED);
      }
    }
  }
//}}}
//{{{
void MSC_BOT_Init (USBD_HandleTypeDef* usbdHandle) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  mscData->bot_state = USBD_BOT_IDLE;
  mscData->bot_status = USBD_BOT_STATUS_NORMAL;
  mscData->scsi_sense_tail = 0;
  mscData->scsi_sense_head = 0;

  ((USBD_StorageTypeDef*)usbdHandle->pUserData)->Init(0);

  USBD_LL_FlushEP (usbdHandle, MSC_EPOUT_ADDR);
  USBD_LL_FlushEP (usbdHandle, MSC_EPIN_ADDR);

  /* Prapare EP to Receive First BOT Cmd */
  USBD_LL_PrepareReceive (usbdHandle, MSC_EPOUT_ADDR, (uint8_t *)&mscData->cbw, USBD_BOT_CBW_LENGTH);
  }
//}}}
//{{{
void MSC_BOT_Reset (USBD_HandleTypeDef* usbdHandle) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  mscData->bot_state = USBD_BOT_IDLE;
  mscData->bot_status = USBD_BOT_STATUS_RECOVERY;

  /* Prapare EP to Receive First BOT Cmd */
  USBD_LL_PrepareReceive (usbdHandle, MSC_EPOUT_ADDR, (uint8_t *)&mscData->cbw, USBD_BOT_CBW_LENGTH);
  }
//}}}
//{{{
void MSC_BOT_DeInit (USBD_HandleTypeDef* usbdHandle) {

  auto mscData = (sMscData*)usbdHandle->pClassData;
  mscData->bot_state  = USBD_BOT_IDLE;
  }
//}}}
//{{{
void MSC_BOT_DataIn (USBD_HandleTypeDef* usbdHandle, uint8_t epnum) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  switch (mscData->bot_state) {
    case USBD_BOT_DATA_IN:
      if(SCSI_ProcessCmd(usbdHandle, mscData->cbw.bLUN, &mscData->cbw.CB[0]) < 0)
        MSC_BOT_SendCSW (usbdHandle, USBD_CSW_CMD_FAILED);
      break;

    case USBD_BOT_SEND_DATA:
    case USBD_BOT_LAST_DATA_IN:
      MSC_BOT_SendCSW (usbdHandle, USBD_CSW_CMD_PASSED);
      break;

    default:
      break;
    }
  }
//}}}
//{{{
void MSC_BOT_DataOut (USBD_HandleTypeDef* usbdHandle, uint8_t epnum) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  switch (mscData->bot_state) {
    case USBD_BOT_IDLE:
      MSC_BOT_CBW_Decode (usbdHandle);
      break;

    case USBD_BOT_DATA_OUT:
      if (SCSI_ProcessCmd (usbdHandle, mscData->cbw.bLUN, &mscData->cbw.CB[0]) < 0)
        MSC_BOT_SendCSW (usbdHandle, USBD_CSW_CMD_FAILED);
      break;

    default:
      break;
    }
  }
//}}}
//{{{
void MSC_BOT_CplClrFeature (USBD_HandleTypeDef* usbdHandle, uint8_t epnum) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if (mscData->bot_status == USBD_BOT_STATUS_ERROR ) {
    /* Bad CBW Signature */
    USBD_LL_StallEP (usbdHandle, MSC_EPIN_ADDR);
    mscData->bot_status = USBD_BOT_STATUS_NORMAL;
    }
  else if (((epnum & 0x80) == 0x80) && (mscData->bot_status != USBD_BOT_STATUS_RECOVERY))
    MSC_BOT_SendCSW (usbdHandle, USBD_CSW_CMD_FAILED);
  }
//}}}

//{{{
uint8_t USBD_MSC_Init (USBD_HandleTypeDef* usbdHandle, uint8_t cfgidx) {

  if (usbdHandle->dev_speed == USBD_SPEED_HIGH  ) {
    USBD_LL_OpenEP (usbdHandle, MSC_EPOUT_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_HS_PACKET);
    USBD_LL_OpenEP (usbdHandle, MSC_EPIN_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_HS_PACKET);
    }
  else {
    USBD_LL_OpenEP (usbdHandle, MSC_EPOUT_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_FS_PACKET);
    USBD_LL_OpenEP (usbdHandle, MSC_EPIN_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_FS_PACKET);
    }
  usbdHandle->pClassData = malloc (sizeof (sMscData));

  if (usbdHandle->pClassData) {
    MSC_BOT_Init (usbdHandle);
    return 0;
    }

  return 1;
  }
//}}}
//{{{
uint8_t USBD_MSC_DeInit (USBD_HandleTypeDef* usbdHandle, uint8_t cfgidx) {

  /* Close MSC EPs */
  USBD_LL_CloseEP(usbdHandle, MSC_EPOUT_ADDR);

  /* Open EP IN */
  USBD_LL_CloseEP(usbdHandle, MSC_EPIN_ADDR);

  /* De-Init the BOT layer */
  MSC_BOT_DeInit(usbdHandle);

  /* Free MSC Class Resources */
  if (usbdHandle->pClassData != NULL) {
    free (usbdHandle->pClassData);
    usbdHandle->pClassData  = NULL;
    }

  return 0;
  }
//}}}
//{{{
uint8_t USBD_MSC_Setup (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef *req) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    /* Class request */
    case USB_REQ_TYPE_CLASS :
      switch (req->bRequest) {
        //{{{
        case BOT_GET_MAX_LUN :
          if((req->wValue  == 0) && (req->wLength == 1) && ((req->bmRequest & 0x80) == 0x80)) {
            mscData->max_lun = ((USBD_StorageTypeDef *)usbdHandle->pUserData)->GetMaxLun();
            USBD_CtlSendData (usbdHandle, (uint8_t*)&mscData->max_lun, 1);
            }
          else {
            USBD_CtlError(usbdHandle, req);
            return USBD_FAIL;
            }
          break;
        //}}}
        //{{{
        case BOT_RESET :
          if ((req->wValue  == 0) && (req->wLength == 0) && ((req->bmRequest & 0x80) != 0x80))
             MSC_BOT_Reset (usbdHandle);
          else {
            USBD_CtlError (usbdHandle , req);
            return USBD_FAIL;
            }
          break;

        //}}}
        //{{{
        default:
          USBD_CtlError (usbdHandle , req);
          return USBD_FAIL;
        //}}}
        }
      break;

    /* Interface & Endpoint request */
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest) {
        //{{{
        case USB_REQ_GET_INTERFACE :
          USBD_CtlSendData (usbdHandle, (uint8_t *)&mscData->interface, 1);
          break;
        //}}}
        //{{{
        case USB_REQ_SET_INTERFACE :
          mscData->interface = (uint8_t)(req->wValue);
          break;
        //}}}
        //{{{
        case USB_REQ_CLEAR_FEATURE:
          /* Flush the FIFO and Clear the stall status */
          USBD_LL_FlushEP (usbdHandle, (uint8_t)req->wIndex);

          /* Reactivate the EP */
          USBD_LL_CloseEP (usbdHandle , (uint8_t)req->wIndex);
          if ((((uint8_t)req->wIndex) & 0x80) == 0x80) {
            if(usbdHandle->dev_speed == USBD_SPEED_HIGH  ) /* Open EP IN */
              USBD_LL_OpenEP (usbdHandle, MSC_EPIN_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_HS_PACKET);
            else /* Open EP IN */
              USBD_LL_OpenEP (usbdHandle, MSC_EPIN_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_FS_PACKET);
            }
          else {
            if (usbdHandle->dev_speed == USBD_SPEED_HIGH  ) /* Open EP IN */
              USBD_LL_OpenEP (usbdHandle, MSC_EPOUT_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_HS_PACKET);
            else /* Open EP IN */
              USBD_LL_OpenEP (usbdHandle, MSC_EPOUT_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_FS_PACKET);
            }

          /* Handle BOT error */
          MSC_BOT_CplClrFeature (usbdHandle, (uint8_t)req->wIndex);
          break;
        //}}}
        }
      break;

    default:
      break;
    }

  return 0;
  }
//}}}
//{{{
uint8_t USBD_MSC_DataIn (USBD_HandleTypeDef* usbdHandle, uint8_t epnum) {

  MSC_BOT_DataIn (usbdHandle , epnum);
  return 0;
  }
//}}}
//{{{
uint8_t USBD_MSC_DataOut (USBD_HandleTypeDef* usbdHandle, uint8_t epnum) {

  MSC_BOT_DataOut (usbdHandle , epnum);
  return 0;
  }
//}}}
//{{{
uint8_t* USBD_MSC_GetHSCfgDesc (uint16_t* length) {
  *length = sizeof (USBD_MSC_CfgHSDesc);
  return (uint8_t*)USBD_MSC_CfgHSDesc;
  }
//}}}
//{{{
uint8_t* USBD_MSC_GetFSCfgDesc (uint16_t* length) {

  *length = sizeof (USBD_MSC_CfgFSDesc);
  return (uint8_t*)USBD_MSC_CfgFSDesc;
  }
//}}}
//{{{
uint8_t* USBD_MSC_GetOtherSpeedCfgDesc (uint16_t* length) {

  *length = sizeof (USBD_MSC_OtherSpeedCfgDesc);
  return (uint8_t*)USBD_MSC_OtherSpeedCfgDesc;
  }
//}}}
//{{{
uint8_t* USBD_MSC_GetDeviceQualifierDescriptor (uint16_t* length) {

  *length = sizeof (USBD_MSC_DeviceQualifierDesc);
  return (uint8_t*)USBD_MSC_DeviceQualifierDesc;
  }
//}}}
//{{{
uint8_t USBD_MSC_RegisterStorage  (USBD_HandleTypeDef* usbdHandle, USBD_StorageTypeDef* fops) {

  if (fops != NULL)
    usbdHandle->pUserData= fops;
  return 0;
  }
//}}}
//{{{
USBD_ClassTypeDef kUsbdMsc = {
  USBD_MSC_Init,
  USBD_MSC_DeInit,
  USBD_MSC_Setup,
  NULL, /*EP0_TxSent*/
  NULL, /*EP0_RxReady*/
  USBD_MSC_DataIn,
  USBD_MSC_DataOut,
  NULL, /*SOF */
  NULL,
  NULL,
  USBD_MSC_GetHSCfgDesc,
  USBD_MSC_GetFSCfgDesc,
  USBD_MSC_GetOtherSpeedCfgDesc,
  USBD_MSC_GetDeviceQualifierDescriptor,
  };
//}}}
