// usbd_msc.cpp
//{{{  includes
#include "usbd_msc.h"
#include "../FatFs/diskio.h"

#include "cLcd.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_sdram.h"

//}}}
//#define sdDma

const bool kUsbFullSpeed = false;
const int kMscMediaPacket = 32*1024;
const int kSectorScale = 256;

const bool kSectorDebug = false;

cLcd* gLcd = nullptr;

PCD_HandleTypeDef gPcdHandle;
USBD_HandleTypeDef gUsbDevice;
//{{{  pcd_IRQ handler routing
extern "C" {
  void OTG_FS_IRQHandler() { HAL_PCD_IRQHandler (&gPcdHandle); }
  void OTG_HS_IRQHandler() { HAL_PCD_IRQHandler (&gPcdHandle); }
  }
//}}}


bool gSdChanged = false;
//uint32_t* gSectors = (uint32_t*)SDRAM_DEVICE_ADDR;

//{{{  sd card handlers
// BSP
__IO uint32_t gReadstatus = 0;
void BSP_SD_ReadCpltCallback() { gReadstatus = 1; }
__IO uint32_t writestatus = 0;
void BSP_SD_WriteCpltCallback() { writestatus = 1; }
//{{{
void BSP_SD_MspInit (SD_HandleTypeDef* hsd, void* Params) {

  static DMA_HandleTypeDef dma_rx_handle;
  static DMA_HandleTypeDef dma_tx_handle;

  __HAL_RCC_SDMMC1_CLK_ENABLE();
  __DMAx_TxRx_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  // Common GPIO configuration
  GPIO_InitTypeDef gpio_init_structure;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  gpio_init_structure.Alternate = GPIO_AF12_SDMMC1;

  // GPIOC configuration
  gpio_init_structure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
  HAL_GPIO_Init (GPIOC, &gpio_init_structure);

  // GPIOD configuration
  gpio_init_structure.Pin = GPIO_PIN_2;
  HAL_GPIO_Init (GPIOD, &gpio_init_structure);

  // NVIC configuration for SDIO interrupts
  HAL_NVIC_SetPriority (SDMMC1_IRQn, 0x05, 0);
  HAL_NVIC_EnableIRQ (SDMMC1_IRQn);

  //{{{  Configure DMA Rx parameters
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
  __HAL_LINKDMA(hsd, hdmarx, dma_rx_handle);
  HAL_DMA_DeInit(&dma_rx_handle);
  HAL_DMA_Init(&dma_rx_handle);
  //}}}
  //{{{  Configure DMA Tx parameters
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
  __HAL_LINKDMA(hsd, hdmatx, dma_tx_handle);
  HAL_DMA_DeInit(&dma_tx_handle);
  HAL_DMA_Init(&dma_tx_handle);
  //}}}

  // NVIC configuration for DMA transfer complete interrupt
  HAL_NVIC_SetPriority (SD_DMAx_Rx_IRQn, 0x06, 0);
  HAL_NVIC_EnableIRQ (SD_DMAx_Rx_IRQn);

  // NVIC configuration for DMA transfer complete interrupt
  HAL_NVIC_SetPriority (SD_DMAx_Tx_IRQn, 0x06, 0);
  HAL_NVIC_EnableIRQ (SD_DMAx_Tx_IRQn);
  }
//}}}

//{{{
bool hasSdChanged() {
  bool changed = gSdChanged;
  gSdChanged = false;
  return changed;
  }
//}}}

// msc sdCard interface
#define STANDARD_INQUIRY_DATA_LEN  36
//{{{
const uint8_t kSdInquiryData[STANDARD_INQUIRY_DATA_LEN] = {
  0x00,  // LUN 0
  0x80,
  0x02,
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,
  0x00,
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer: 8 bytes  */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product     : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0','1',                      /* Version     : 4 Bytes  */
  };
//}}}
//{{{
bool sdIsReady (uint8_t lun) {

  static int8_t prev_status = 0;
  if (BSP_SD_IsDetected() != SD_NOT_PRESENT) {
    if (prev_status < 0) {
      BSP_SD_Init();
      prev_status = 0;
      }
    if (BSP_SD_GetCardState() == SD_TRANSFER_OK)
      return true;
    }
  else if (prev_status == 0)
    prev_status = -1;

  return false;
  }
//}}}
//{{{
bool sdIsWriteProtected (uint8_t lun) {
  return false;
  }
//}}}
//{{{
bool sdGetCapacity (uint8_t lun, uint32_t& block_num, uint16_t& block_size) {

  if (BSP_SD_IsDetected() != SD_NOT_PRESENT) {
    HAL_SD_CardInfoTypeDef info;
    BSP_SD_GetCardInfo (&info);
    block_num = info.LogBlockNbr - 1;
    block_size = info.LogBlockSize;
    //gLcd->debug (LCD_COLOR_YELLOW, "getCapacity %dk blocks size:%d", int(block_num)/1024, int(block_size));
    return true;
    }
  else
    gLcd->debug (LCD_COLOR_RED, "getCapacity SD_NOT_PRESENT");

  return false;
  }
//}}}
//{{{
bool sdRead (uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len) {

  if (kSectorDebug && ((uint32_t)buf & 0x3))
    gLcd->debug (LCD_COLOR_RED, "sdRead buf alignment %x", buf);

  if (BSP_SD_IsDetected() != SD_NOT_PRESENT) {
  #ifdef sdDma
    gReadstatus = 0;
    BSP_SD_ReadBlocks_DMA ((uint32_t*)buf, blk_addr, blk_len);
    auto ticks = HAL_GetTick();
    while (!gReadstatus)
      if (HAL_GetTick() - ticks > 1000) {
        gLcd->debug (LCD_COLOR_RED, "sdRead %p %7d %2d", buf, (int)blk_addr, (int)blk_len);
        return false;
        }
    while (BSP_SD_GetCardState() != SD_TRANSFER_OK) {}
    uint32_t alignedAddr = (uint32_t)buf & ~0x1F;
    SCB_InvalidateDCache_by_Addr ((uint32_t*)alignedAddr, ((uint32_t)buf - alignedAddr) + blk_len*512);
  #else
    BSP_SD_ReadBlocks ((uint32_t*)buf, blk_addr, blk_len, 1000);
  #endif

    if (kSectorDebug)
      gLcd->debug (LCD_COLOR_CYAN, "r %p %7d %2d", buf, (int)blk_addr, (int)blk_len);
    //gSectors[blk_addr / kSectorScale] = LCD_COLOR_GREEN;
    //gSectors[(blk_addr / kSectorScale)+1] = LCD_COLOR_GREEN;
    //gSectors[(blk_addr / kSectorScale)+480] = LCD_COLOR_GREEN;
    //gSectors[(blk_addr / kSectorScale)+481] = LCD_COLOR_GREEN;
    return true;
    }

  return false;
  }
//}}}
//{{{
bool sdWrite (uint8_t lun, const uint8_t* buf, uint32_t blk_addr, uint16_t blk_len) {

  if (kSectorDebug && ((uint32_t)buf & 0x3))
    gLcd->debug (LCD_COLOR_RED, "sdWrite buf alignment %p", buf);

  if (BSP_SD_IsDetected() != SD_NOT_PRESENT) {
    //if (kSectorDebug)
    gLcd->debug (LCD_COLOR_MAGENTA, "w %p %7d %2d", buf, (int)blk_addr, (int)blk_len);

    //uint32_t alignedAddr = (uint32_t)buf &  ~0x1F;
    //SCB_CleanDCache_by_Addr ((uint32_t*)alignedAddr, blk_len*512 + ((uint32_t)buf - alignedAddr));
    //BSP_SD_WriteBlocks_DMA ((uint32_t*)buf, blk_addr, blk_len);

    //uint32_t timeout = HAL_GetTick();
    //while  ((writestatus == 0) && ((HAL_GetTick() - timeout) < 2000)) {}
    //writestatus = 0;
    //while ((HAL_GetTick() - timeout) < 2000) {
    //  if (BSP_SD_GetCardState() == SD_TRANSFER_OK) {
    //    gSdChanged = true;
        //gSectors[blk_addr / kSectorScale] = LCD_COLOR_RED;
        //gSectors[(blk_addr / kSectorScale)+1] = LCD_COLOR_RED;
        //gSectors[(blk_addr / kSectorScale)+480] = LCD_COLOR_RED;
        //gSectors[(blk_addr / kSectorScale)+481] = LCD_COLOR_RED;
    //    return true;
    //    }
    //  }
    BSP_SD_WriteBlocks ((uint32_t*)buf, blk_addr, blk_len, 1000);
    if (BSP_SD_GetCardState() == SD_TRANSFER_OK)
      return true;
    }

  return false;
  }
//}}}

// fatFs sdCard interface
volatile DSTATUS Stat = STA_NOINIT;
//{{{
uint8_t disk_initialize (uint8_t lun) {
  return disk_status (lun);
  }
//}}}
//{{{
uint8_t disk_status (uint8_t lun) {

  HAL_NVIC_DisableIRQ (OTG_HS_IRQn);
  Stat = STA_NOINIT;
  if (BSP_SD_GetCardState() == MSD_OK)
    Stat &= ~STA_NOINIT;
  HAL_NVIC_EnableIRQ (OTG_HS_IRQn);

  return Stat;
  }
//}}}
//{{{
DRESULT disk_ioctl (uint8_t lun, BYTE cmd, void* buff) {

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
      *(DWORD*)buff = CardInfo.LogBlockSize / 512;
      return RES_OK;

    default:
      return RES_PARERR;
    }

  return RES_ERROR;
  }
//}}}
//{{{
DRESULT disk_read (uint8_t lun, uint8_t* buff, uint32_t sector, uint16_t count) {

  HAL_NVIC_DisableIRQ (OTG_HS_IRQn);
  auto result = sdRead (lun, buff, sector, count) ? RES_OK : RES_ERROR;
  HAL_NVIC_EnableIRQ (OTG_HS_IRQn);

  return result;
  }
//}}}
//{{{
DRESULT disk_write (uint8_t lun, const uint8_t* buff, uint32_t sector, uint16_t count) {

  HAL_NVIC_DisableIRQ (OTG_HS_IRQn);
  auto result = sdWrite (lun, buff, sector, count) ? RES_OK : RES_ERROR;
  HAL_NVIC_EnableIRQ (OTG_HS_IRQn);

  return result;
  }
//}}}

DWORD get_fattime() { return 0; }
//}}}
//{{{  msc common descriptors
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

//{{{
const uint8_t kMscDeviceDesc[] __attribute__((aligned(4))) = {
  0x12, USB_DESC_TYPE_DEVICE,
  0x00, 0x02,                 /* bcdUSB */
  0x00,                       /* bDeviceClass */
  0x00,                       /* bDeviceSubClass */
  0x00,                       /* bDeviceProtocol */
  USB_MAX_EP0_SIZE,           /* bMaxPacketSize */
  LOBYTE(USBD_VID), HIBYTE(USBD_VID),
  LOBYTE(USBD_PID), HIBYTE(USBD_PID),
  0x00, 0x02,                 /* bcdDevice rel. 2.00 */
  USBD_IDX_MFC_STR,           /* Index of manufacturer string */
  USBD_IDX_PRODUCT_STR,       /* Index of product string */
  USBD_IDX_SERIAL_STR,        /* Index of serial number string */
  USBD_MAX_NUM_CONFIGURATION  /* bNumConfigurations */
  };
//}}}
//{{{
/* USB Standard Device Descriptor */
const uint8_t kMscLangIDDesc[] __attribute__((aligned(4))) = {
  USB_LEN_LANGID_STR_DESC,
  USB_DESC_TYPE_STRING,
  LOBYTE(USBD_LANGID_STRING),
  HIBYTE(USBD_LANGID_STRING),
  };
//}}}
//{{{
uint8_t mscStringSerial[USB_SIZ_STRING_SERIAL] = {
  USB_SIZ_STRING_SERIAL,
  USB_DESC_TYPE_STRING,
  };
//}}}
uint8_t mscStrDesc[USBD_MAX_STR_DESC_SIZ] __attribute__((aligned(4)));

//{{{
void intToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len) {

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
void getSerialNum() {

  auto deviceserial0 = *(uint32_t*)DEVICE_ID1;
  auto deviceserial1 = *(uint32_t*)DEVICE_ID2;
  auto deviceserial2 = *(uint32_t*)DEVICE_ID3;
  deviceserial0 += deviceserial2;
  if (deviceserial0 != 0) {
    intToUnicode (deviceserial0, (uint8_t*)&mscStringSerial[2] ,8);
    intToUnicode (deviceserial1, (uint8_t*)&mscStringSerial[18] ,4);
    }
  }
//}}}
//{{{
uint8_t* mscDeviceDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  *length = sizeof(kMscDeviceDesc);
  return (uint8_t*)kMscDeviceDesc;
  }
//}}}
//{{{
uint8_t* mscLangIDStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  *length = sizeof(kMscLangIDDesc);
  return (uint8_t*)kMscLangIDDesc;
  }
//}}}
//{{{
uint8_t* mscProductStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  if (speed == USBD_SPEED_HIGH)
    USBD_GetString ((uint8_t*)USBD_PRODUCT_HS_STRING, mscStrDesc, length);
  else
    USBD_GetString ((uint8_t*)USBD_PRODUCT_FS_STRING, mscStrDesc, length);
  return mscStrDesc;
  }
//}}}
//{{{
uint8_t* mscManufacturerStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  USBD_GetString ((uint8_t*)USBD_MANUFACTURER_STRING, mscStrDesc, length);
  return mscStrDesc;
  }
//}}}
//{{{
uint8_t* mscSerialStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  *length = USB_SIZ_STRING_SERIAL;
  getSerialNum();
  return (uint8_t*)mscStringSerial;
  }
//}}}
//{{{
uint8_t* mscConfigStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  if (speed == USBD_SPEED_HIGH)
    USBD_GetString ((uint8_t*)USBD_CONFIGURATION_HS_STRING, mscStrDesc, length);
  else
    USBD_GetString ((uint8_t*)USBD_CONFIGURATION_FS_STRING, mscStrDesc, length);

  return mscStrDesc;
  }
//}}}
//{{{
uint8_t* mscInterfaceStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  if (speed == USBD_SPEED_HIGH)
    USBD_GetString ((uint8_t*)USBD_INTERFACE_HS_STRING, mscStrDesc, length);
  else
    USBD_GetString ((uint8_t*)USBD_INTERFACE_FS_STRING, mscStrDesc, length);
  return mscStrDesc;
  }
//}}}
//{{{
USBD_DescriptorsTypeDef kMscDescriptors = {
  mscDeviceDescriptor,
  mscLangIDStrDescriptor,
  mscManufacturerStrDescriptor,
  mscProductStrDescriptor,
  mscSerialStrDescriptor,
  mscConfigStrDescriptor,
  mscInterfaceStrDescriptor,
  };
//}}}
//}}}
//{{{  msc class descriptors
#define MSC_MAX_FS_PACKET  0x40
#define MSC_MAX_HS_PACKET  0x200

#define MSC_EPIN_ADDR   0x81
#define MSC_EPOUT_ADDR  0x01

#define USB_MSC_CONFIG_DESC_SIZ  32

//{{{
/* USB Mass storage device Configuration Descriptor */
const uint8_t kMscHighSpeedConfigDesc[] __attribute__((aligned(4))) = {
  0x09, USB_DESC_TYPE_CONFIGURATION,
  USB_MSC_CONFIG_DESC_SIZ,
  0x00,
  0x01,   /* bNumInterfaces: 1 interface */
  0x01,   /* bConfigurationValue: */
  0x04,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0x32,   /* MaxPower 100 mA */

  // Mass Storage interface
  0x09, 0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0x08,   /* bInterfaceClass: MSC Class */
  0x06,   /* bInterfaceSubClass : SCSI transparent*/
  0x50,   /* nInterfaceProtocol */
  0x05,          /* iInterface: */

  // Mass Storage Endpoints
  0x07, 0x05,     /*Endpoint descriptor type */
  MSC_EPIN_ADDR,  /*Endpoint address (IN, address 1) */
  0x02,           /*Bulk endpoint type */
  LOBYTE(MSC_MAX_HS_PACKET), HIBYTE(MSC_MAX_HS_PACKET),
  0x00,           /*Polling interval in milliseconds */

  0x07, 0x05,     /*Endpoint descriptor type */
  MSC_EPOUT_ADDR, /*Endpoint address (OUT, address 1) */
  0x02,           /*Bulk endpoint type */
  LOBYTE(MSC_MAX_HS_PACKET), HIBYTE(MSC_MAX_HS_PACKET),
  0x00            /*Polling interval in milliseconds*/
  };
//}}}
//{{{
/* USB Mass storage device Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t kMscFullSpeedConfigDesc[] __attribute__((aligned(4))) = {
  0x09, USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType: Configuration */
  USB_MSC_CONFIG_DESC_SIZ,
  0x00,
  0x01,  /* bNumInterfaces: 1 interface */
  0x01,  /* bConfigurationValue: */
  0x04,  /* iConfiguration: */
  0xC0,  /* bmAttributes: */
  0x32,  /* MaxPower 100 mA */

  //  Mass Storage interface
  0x09, 0x04,  /* bDescriptorType: */
  0x00,  /* bInterfaceNumber: Number of Interface */
  0x00,  /* bAlternateSetting: Alternate setting */
  0x02,  /* bNumEndpoints*/
  0x08,  /* bInterfaceClass: MSC Class */
  0x06,  /* bInterfaceSubClass : SCSI transparent*/
  0x50,  /* nInterfaceProtocol */
  0x05,  /* iInterface: */

  //  Mass Storage Endpoints
  0x07, 0x05,     /*Endpoint descriptor type */
  MSC_EPIN_ADDR,  /*Endpoint address (IN, address 1) */
  0x02,           /*Bulk endpoint type */
  LOBYTE(MSC_MAX_FS_PACKET), HIBYTE(MSC_MAX_FS_PACKET),
  0x00,           /*Polling interval in milliseconds */

  0x07, 0x05,     /*Endpoint descriptor type */
  MSC_EPOUT_ADDR, /*Endpoint address (OUT, address 1) */
  0x02,           /*Bulk endpoint type */
  LOBYTE(MSC_MAX_FS_PACKET), HIBYTE(MSC_MAX_FS_PACKET),
  0x00            /*Polling interval in milliseconds*/
  };
//}}}
//{{{
const uint8_t kMscOtherSpeedConfigDesc[] __attribute__((aligned(4)))  = {
  0x09, USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION,
  USB_MSC_CONFIG_DESC_SIZ,
  0x00,
  0x01,  /* bNumInterfaces: 1 interface */
  0x01,  /* bConfigurationValue: */
  0x04,  /* iConfiguration: */
  0xC0,  /* bmAttributes: */
  0x32,  /* MaxPower 100 mA */

  // Mass Storage interface
  0x09, 0x04,  /* bDescriptorType: */
  0x00,  /* bInterfaceNumber: Number of Interface */
  0x00,  /* bAlternateSetting: Alternate setting */
  0x02,  /* bNumEndpoints*/
  0x08,  /* bInterfaceClass: MSC Class */
  0x06,  /* bInterfaceSubClass : SCSI transparent command set*/
  0x50,  /* nInterfaceProtocol */
  0x05,  /* iInterface: */

  // Mass Storage Endpoints
  0x07, 0x05,     /* Endpoint descriptor type */
  MSC_EPIN_ADDR,  /* Endpoint address (IN, address 1) */
  0x02,           /* Bulk endpoint type */
  0x40, 0x00,
  0x00,           /* Polling interval in milliseconds */

  0x07, 0x05,     /* Endpoint descriptor type */
  MSC_EPOUT_ADDR, /* Endpoint address (OUT, address 1) */
  0x02,           /* Bulk endpoint type */
  0x40, 0x00,
  0x00            /* Polling interval in milliseconds*/
  };
//}}}
//{{{
const uint8_t kMscDeviceQualifierDesc[] __attribute__((aligned(4))) = {
  USB_LEN_DEV_QUALIFIER_DESC, USB_DESC_TYPE_DEVICE_QUALIFIER,
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
//}}}
//{{{  msc scsi handlers
//{{{  defines
#define BOT_GET_MAX_LUN          0xFE
#define BOT_RESET                0xFF

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

#define MODE_SENSE6_LEN          8
#define MODE_SENSE10_LEN         8
#define LENGTH_FORMAT_CAPACITIES 20

#define SENSE_LIST_DEPTH         4
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
const uint8_t kMscPage00InquiryData[] = {
  0x00,
  0x00,
  0x00,
  0x03, // (LENGTH_INQUIRY_PAGE00 - 4) = 7-4
  0x00,
  0x80,
  0x83
  };
//}}}
//{{{
const uint8_t kMscModeSense6Data[] = {
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
const uint8_t kMscModeSense10Data[] = {
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

//{{{  struct sUsbdScsiSenseTypeDef
typedef struct _SENSE_ITEM {
  char Skey;
  union {
    struct _ASCs {
      char ASC;
      char ASCQ;
      } b;
    unsigned int ASC;
    char* pData;
    } w;
  } sUsbdScsiSenseTypeDef;
//}}}
//{{{  struct sMscBotCbwTypeDef
typedef struct {
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataLength;
  uint8_t  bmFlags;
  uint8_t  bLUN;
  uint8_t  bCBLength;
  uint8_t  CB[16];
  uint8_t  ReservedForAlign;
  } sMscBotCbwTypeDef;
//}}}
//{{{  struct sMscBotCswTypeDef
typedef struct {
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataResidue;
  uint8_t  bStatus;
  uint8_t  ReservedForAlign[3];
  } sMscBotCswTypeDef;
//}}}
//{{{  struct sMscData
typedef struct {
  uint32_t              interface;

  uint8_t               bot_state;
  uint8_t               bot_status;
  uint16_t              bot_data_length;
  uint8_t               bot_data[kMscMediaPacket];
  sMscBotCbwTypeDef     cbw;
  sMscBotCswTypeDef     csw;

  sUsbdScsiSenseTypeDef scsi_sense [SENSE_LIST_DEPTH];
  uint8_t               scsi_sense_head;
  uint8_t               scsi_sense_tail;
  uint16_t              scsiBlocksize;
  uint32_t              scsiBlocknbr;
  uint32_t              scsiBlockaddr;
  uint32_t              scsiBlocklen;
  } sMscData;
//}}}

//{{{
void mscBotSendCsw (USBD_HandleTypeDef* usbdHandle, uint8_t CSW_Status) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  mscData->csw.dSignature = USBD_BOT_CSW_SIGNATURE;
  mscData->csw.bStatus = CSW_Status;
  mscData->bot_state = USBD_BOT_IDLE;

  usbdLowLevelTransmit (usbdHandle, MSC_EPIN_ADDR, (uint8_t*)&mscData->csw, USBD_BOT_CSW_LENGTH);

  // Prepare EP to Receive next Cmd
  usbdLowLevelPrepareReceive (usbdHandle, MSC_EPOUT_ADDR, (uint8_t*)&mscData->cbw, USBD_BOT_CBW_LENGTH);
  }
//}}}
//{{{
void scsiSenseCode (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t sKey, uint8_t ASC) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  mscData->scsi_sense[mscData->scsi_sense_tail].Skey  = sKey;
  mscData->scsi_sense[mscData->scsi_sense_tail].w.ASC = ASC << 8;

  mscData->scsi_sense_tail++;
  if (mscData->scsi_sense_tail == SENSE_LIST_DEPTH)
    mscData->scsi_sense_tail = 0;
  }
//}}}
//{{{
void mscBotAbort (USBD_HandleTypeDef* usbdHandle) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if ((mscData->cbw.bmFlags == 0) &&
      (mscData->cbw.dDataLength != 0) &&
      (mscData->bot_status == USBD_BOT_STATUS_NORMAL))
    usbdLowLevelStallEP (usbdHandle, MSC_EPOUT_ADDR);

  usbdLowLevelStallEP (usbdHandle, MSC_EPIN_ADDR);

  if (mscData->bot_status == USBD_BOT_STATUS_ERROR)
    usbdLowLevelPrepareReceive (usbdHandle, MSC_EPOUT_ADDR, (uint8_t*)&mscData->cbw, USBD_BOT_CBW_LENGTH);
  }
//}}}
//{{{
void mscBotCplClrFeature (USBD_HandleTypeDef* usbdHandle, uint8_t endPointNum) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if (mscData->bot_status == USBD_BOT_STATUS_ERROR ) {
    // Bad CBW Signature
    usbdLowLevelStallEP (usbdHandle, MSC_EPIN_ADDR);
    mscData->bot_status = USBD_BOT_STATUS_NORMAL;
    }
  else if (((endPointNum & 0x80) == 0x80) && (mscData->bot_status != USBD_BOT_STATUS_RECOVERY))
    mscBotSendCsw (usbdHandle, USBD_CSW_CMD_FAILED);
  }
//}}}

// scsi cmds
//{{{
int8_t scsiTestUnitReady (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  // case 9 : Hi > D0
  if (mscData->cbw.dDataLength != 0) {
    scsiSenseCode (usbdHandle, mscData->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
    return -1;
    }

  if (!sdIsReady (lun)) {
    scsiSenseCode (usbdHandle, lun, NOT_READY, MEDIUM_NOT_PRESENT);
    mscData->bot_state = USBD_BOT_NO_DATA;
    return -1;
    }

  mscData->bot_data_length = 0;
  return 0;
  }
//}}}
//{{{
int8_t scsiRequestSense (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  for (uint8_t i = 0 ; i < REQUEST_SENSE_DATA_LEN ; i++)
    mscData->bot_data[i] = 0;

  mscData->bot_data[0] = 0x70;
  mscData->bot_data[7] = REQUEST_SENSE_DATA_LEN - 6;

  if ((mscData->scsi_sense_head != mscData->scsi_sense_tail)) {
    mscData->bot_data[2] = mscData->scsi_sense[mscData->scsi_sense_head].Skey;
    mscData->bot_data[12] = mscData->scsi_sense[mscData->scsi_sense_head].w.b.ASCQ;
    mscData->bot_data[13] = mscData->scsi_sense[mscData->scsi_sense_head].w.b.ASC;
    mscData->scsi_sense_head++;

    if (mscData->scsi_sense_head == SENSE_LIST_DEPTH)
      mscData->scsi_sense_head = 0;
    }
  mscData->bot_data_length = REQUEST_SENSE_DATA_LEN;

  if (params[4] <= REQUEST_SENSE_DATA_LEN)
    mscData->bot_data_length = params[4];

  return 0;
  }
//}}}
//{{{
int8_t scsiInquiry (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  const uint8_t* pPage;
  uint16_t len;
  if (params[1] & 0x01) {
    // Evpd
    pPage = kMscPage00InquiryData;
    len = sizeof (kMscPage00InquiryData);
    }
  else {
    pPage = kSdInquiryData;
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
int8_t scsiStartStopUnit (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;
  mscData->bot_data_length = 0;
  return 0;
  }
//}}}

//{{{
int8_t scsiModeSense6 (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  uint16_t len = 8 ;
  mscData->bot_data_length = len;
  while (len) {
    len--;
    mscData->bot_data[len] = kMscModeSense6Data[len];
    }

  return 0;
  }
//}}}
//{{{
int8_t scsiModeSense10 (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  uint16_t len = 8;
  mscData->bot_data_length = len;
  while (len) {
    len--;
    mscData->bot_data[len] = kMscModeSense10Data[len];
    }

  return 0;
  }
//}}}

//{{{
int8_t scsiReadCapacity10 (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if (sdGetCapacity (lun, mscData->scsiBlocknbr, mscData->scsiBlocksize)) {
    mscData->bot_data[0] = (uint8_t)((mscData->scsiBlocknbr-1) >> 24);
    mscData->bot_data[1] = (uint8_t)((mscData->scsiBlocknbr-1) >> 16);
    mscData->bot_data[2] = (uint8_t)((mscData->scsiBlocknbr-1) >> 8);
    mscData->bot_data[3] = (uint8_t)(mscData->scsiBlocknbr-1);
    mscData->bot_data[4] = (uint8_t)(mscData->scsiBlocksize >> 24);
    mscData->bot_data[5] = (uint8_t)(mscData->scsiBlocksize >> 16);
    mscData->bot_data[6] = (uint8_t)(mscData->scsiBlocksize >> 8);
    mscData->bot_data[7] = (uint8_t)mscData->scsiBlocksize;
    mscData->bot_data_length = 8;
    return 0;
    }

  scsiSenseCode (usbdHandle, lun, NOT_READY, MEDIUM_NOT_PRESENT);
  return -1;
  }
//}}}
//{{{
int8_t scsiReadFormatCapacity (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  uint32_t blk_nbr;
  uint16_t blk_size;
  if (sdGetCapacity (lun, blk_nbr, blk_size) != 0) {
    mscData->bot_data[0] = 0;
    mscData->bot_data[1] = 0;
    mscData->bot_data[2] = 0;
    mscData->bot_data[3] = 0x08;
    mscData->bot_data[4] = (uint8_t)((blk_nbr-1) >> 24);
    mscData->bot_data[5] = (uint8_t)((blk_nbr-1) >> 16);
    mscData->bot_data[6] = (uint8_t)((blk_nbr-1) >> 8);
    mscData->bot_data[7] = (uint8_t)(blk_nbr-1);
    mscData->bot_data[8] = 0x02;
    mscData->bot_data[9] = (uint8_t)(blk_size >> 16);
    mscData->bot_data[10] = (uint8_t)(blk_size >> 8);
    mscData->bot_data[11] = (uint8_t)blk_size;
    mscData->bot_data_length = 12;
    return 0;
    }

  scsiSenseCode (usbdHandle, lun, NOT_READY, MEDIUM_NOT_PRESENT);
  return -1;
  }
//}}}

//{{{
int8_t scsiRead10 (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if (mscData->bot_state == USBD_BOT_IDLE) {
    // case 10 : Ho <> Di
    if ((mscData->cbw.bmFlags & 0x80) != 0x80) {
      //{{{  error
      scsiSenseCode (usbdHandle, mscData->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
      return -1;
      }
      //}}}
    if (!sdIsReady (lun)) {
      //{{{  error
      scsiSenseCode (usbdHandle, lun, NOT_READY, MEDIUM_NOT_PRESENT);
      return -1;
      }
      //}}}

    // load addr,len check in range
    mscData->scsiBlockaddr = (params[2] << 24) | (params[3] << 16) | (params[4] << 8) | params[5];
    mscData->scsiBlocklen = (params[7] << 8) | params[8];
    if ((mscData->scsiBlockaddr + mscData->scsiBlocklen) > mscData->scsiBlocknbr) {
      //{{{  error
      scsiSenseCode (usbdHandle, lun, ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
      return -1;
      }
      //}}}

    // cases 4,5 : Hi <> Dn
    mscData->bot_state = USBD_BOT_DATA_IN;
    mscData->scsiBlockaddr *= mscData->scsiBlocksize;
    mscData->scsiBlocklen *= mscData->scsiBlocksize;
    if (mscData->cbw.dDataLength != mscData->scsiBlocklen) {
      //{{{  error
      scsiSenseCode (usbdHandle, mscData->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
      return -1;
      }
      //}}}
    }

  mscData->bot_data_length = kMscMediaPacket;
  uint32_t len = MIN(mscData->scsiBlocklen, kMscMediaPacket);
  if (!sdRead (lun, mscData->bot_data,
               mscData->scsiBlockaddr / mscData->scsiBlocksize, len / mscData->scsiBlocksize)) {
    //{{{  error
    scsiSenseCode (usbdHandle, lun, HARDWARE_ERROR, UNRECOVERED_READ_ERROR);
    return -1;
    }
    //}}}
  usbdLowLevelTransmit (usbdHandle, MSC_EPIN_ADDR, mscData->bot_data, len);

  // case 6 : Hi = Di
  mscData->scsiBlockaddr += len;
  mscData->scsiBlocklen -= len;
  mscData->csw.dDataResidue -= len;
  if (mscData->scsiBlocklen == 0)
    mscData->bot_state = USBD_BOT_LAST_DATA_IN;

  return 0;
  }
//}}}
//{{{
int8_t scsiWrite10 (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if (mscData->bot_state == USBD_BOT_IDLE) {
    // case 8 : Hi <> Do
    if ((mscData->cbw.bmFlags & 0x80) == 0x80) {
      //{{{  error
      scsiSenseCode (usbdHandle, mscData->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
      return -1;
      }
      //}}}
    if (!sdIsReady (lun)) {
      //{{{  error, media not ready
      scsiSenseCode (usbdHandle, lun, NOT_READY, MEDIUM_NOT_PRESENT);
      return -1;
      }
      //}}}
    if (sdIsWriteProtected (lun)) {
      //{{{  error, media write-protected
      scsiSenseCode (usbdHandle, lun, NOT_READY, WRITE_PROTECTED);
      return -1;
      }
      //}}}

    // load addr,len check in range
    mscData->scsiBlockaddr = (params[2] << 24) | (params[3] << 16) | (params[4] <<  8) | params[5];
    mscData->scsiBlocklen = (params[7] <<  8) | params[8];
    if ((mscData->scsiBlockaddr + mscData->scsiBlocklen) > mscData->scsiBlocknbr) {
      //{{{  error
      scsiSenseCode (usbdHandle, lun, ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
      return -1;
      }
      //}}}

    // cases 3,11,13 : Hn,Ho <> D0
    mscData->scsiBlockaddr *= mscData->scsiBlocksize;
    mscData->scsiBlocklen *= mscData->scsiBlocksize;
    if (mscData->cbw.dDataLength != mscData->scsiBlocklen) {
      //{{{  error
      scsiSenseCode (usbdHandle, mscData->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
      return -1;
      }
      //}}}

    // first data packet
    mscData->bot_state = USBD_BOT_DATA_OUT;
    }

  else {
    // Write ongoing
    uint32_t len = MIN(mscData->scsiBlocklen, kMscMediaPacket);
    if (!sdWrite (lun , mscData->bot_data,
                  mscData->scsiBlockaddr/mscData->scsiBlocksize, len/mscData->scsiBlocksize) < 0) {
      //{{{  error
      scsiSenseCode (usbdHandle, lun, HARDWARE_ERROR, WRITE_FAULT);
      return -1;
      }
      //}}}

    // case 12 : Ho = Do
    mscData->scsiBlockaddr += len;
    mscData->scsiBlocklen -= len;
    mscData->csw.dDataResidue -= len;
    if (mscData->scsiBlocklen == 0) {
      mscBotSendCsw (usbdHandle, USBD_CSW_CMD_PASSED);
      return 0;
      }
    }

  // prepare EP to rx packet
  usbdLowLevelPrepareReceive (usbdHandle, MSC_EPOUT_ADDR,
                              mscData->bot_data, MIN(mscData->scsiBlocklen, kMscMediaPacket));
  return 0;
  }
//}}}
//{{{
int8_t scsiVerify10 (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  if ((params[1] & 0x02) == 0x02) {
    // error, Verify Mode Not supported
    scsiSenseCode (usbdHandle, lun, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
    return -1;
    }

  mscData->bot_data_length = 0;
  return 0;
  }
//}}}

//{{{
int8_t scsiCmd (USBD_HandleTypeDef* usbdHandle, uint8_t lun, uint8_t* params) {

  switch (params[0]) {
    case SCSI_TEST_UNIT_READY:        return scsiTestUnitReady (usbdHandle, lun, params);
    case SCSI_REQUEST_SENSE:          return scsiRequestSense (usbdHandle, lun, params);
    case SCSI_INQUIRY:                return scsiInquiry (usbdHandle, lun, params);
    case SCSI_START_STOP_UNIT:        return scsiStartStopUnit (usbdHandle, lun, params);
    case SCSI_ALLOW_MEDIUM_REMOVAL:   return scsiStartStopUnit (usbdHandle, lun, params);
    case SCSI_MODE_SENSE6:            return scsiModeSense6 (usbdHandle, lun, params);
    case SCSI_MODE_SENSE10:           return scsiModeSense10 (usbdHandle, lun, params);
    case SCSI_READ_FORMAT_CAPACITIES: return scsiReadFormatCapacity (usbdHandle, lun, params);
    case SCSI_READ_CAPACITY10:        return scsiReadCapacity10 (usbdHandle, lun, params);
    case SCSI_READ10:                 return scsiRead10 (usbdHandle, lun, params);
    case SCSI_WRITE10:                return scsiWrite10 (usbdHandle, lun, params);
    case SCSI_VERIFY10:               return scsiVerify10 (usbdHandle, lun, params);
    default:                          scsiSenseCode (usbdHandle, lun, ILLEGAL_REQUEST, INVALID_CDB); return -1;
    }
  }
//}}}

//{{{
void mscBotCbwDecode (USBD_HandleTypeDef* usbdHandle) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  mscData->csw.dTag = mscData->cbw.dTag;
  mscData->csw.dDataResidue = mscData->cbw.dDataLength;

  if ((usbdLowLevelGetRxDataSize (usbdHandle ,MSC_EPOUT_ADDR) != USBD_BOT_CBW_LENGTH) ||
      (mscData->cbw.dSignature != USBD_BOT_CBW_SIGNATURE) ||
        (mscData->cbw.bLUN > 1) ||
          (mscData->cbw.bCBLength < 1) ||
            (mscData->cbw.bCBLength > 16)) {
    scsiSenseCode (usbdHandle, mscData->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
    mscData->bot_status = USBD_BOT_STATUS_ERROR;
    mscBotAbort (usbdHandle);
    }

  else {
    if (scsiCmd (usbdHandle, mscData->cbw.bLUN, &mscData->cbw.CB[0]) < 0) {
      if (mscData->bot_state == USBD_BOT_NO_DATA)
        mscBotSendCsw (usbdHandle, USBD_CSW_CMD_FAILED);
      else
        mscBotAbort (usbdHandle);
      }

    // Burst xfer handled internally
    else if ((mscData->bot_state != USBD_BOT_DATA_IN) &&
             (mscData->bot_state != USBD_BOT_DATA_OUT) &&
             (mscData->bot_state != USBD_BOT_LAST_DATA_IN)) {
      if (mscData->bot_data_length > 0) {
        auto mscData = (sMscData*)usbdHandle->pClassData;
        uint16_t len = MIN (mscData->cbw.dDataLength, mscData->bot_data_length);
        mscData->csw.dDataResidue -= len;
        mscData->csw.bStatus = USBD_CSW_CMD_PASSED;
        mscData->bot_state = USBD_BOT_SEND_DATA;
        usbdLowLevelTransmit (usbdHandle, MSC_EPIN_ADDR, mscData->bot_data, len);
        }
      else if (mscData->bot_data_length == 0)
        mscBotSendCsw (usbdHandle, USBD_CSW_CMD_PASSED);
      }
    }
  }
//}}}
//}}}
sMscData gMscData;
//{{{  msc class handlers
//{{{
uint8_t mscInit (USBD_HandleTypeDef* usbdHandle, uint8_t cfgidx) {

  if (usbdHandle->dev_speed == USBD_SPEED_HIGH) {
    usbdLowLevelOpenEP (usbdHandle, MSC_EPOUT_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_HS_PACKET);
    usbdLowLevelOpenEP (usbdHandle, MSC_EPIN_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_HS_PACKET);
    }
  else {
    usbdLowLevelOpenEP (usbdHandle, MSC_EPOUT_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_FS_PACKET);
    usbdLowLevelOpenEP (usbdHandle, MSC_EPIN_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_FS_PACKET);
    }

  usbdHandle->pClassData = &gMscData;
  usbdLowLevelFlushEP (usbdHandle, MSC_EPOUT_ADDR);
  usbdLowLevelFlushEP (usbdHandle, MSC_EPIN_ADDR);

  // Prapare EP to Receive First BOT Cmd
  auto mscData = (sMscData*)usbdHandle->pClassData;

  mscData->bot_state = USBD_BOT_IDLE;
  mscData->bot_status = USBD_BOT_STATUS_NORMAL;
  mscData->scsi_sense_tail = 0;
  mscData->scsi_sense_head = 0;
  usbdLowLevelPrepareReceive (usbdHandle, MSC_EPOUT_ADDR, (uint8_t*)&mscData->cbw, USBD_BOT_CBW_LENGTH);

  return 0;
  }
//}}}
//{{{
uint8_t mscDeInit (USBD_HandleTypeDef* usbdHandle, uint8_t cfgidx) {

  // Close MSC EPs
  usbdLowLevelCloseEP (usbdHandle, MSC_EPOUT_ADDR);
  usbdLowLevelCloseEP (usbdHandle, MSC_EPIN_ADDR);

  //free (usbdHandle->pClassData);
  //usbdHandle->pClassData = nullptr;

  return 0;
  }
//}}}

//{{{
uint8_t mscSetup (USBD_HandleTypeDef* usbdHandle, USBD_SetupReqTypedef* req) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    //{{{
    case USB_REQ_TYPE_CLASS :   // Class request
      switch (req->bRequest) {
        case BOT_GET_MAX_LUN :
          if ((req->wValue  == 0) &&
              (req->wLength == 1) &&
              ((req->bmRequest & 0x80) == 0x80))
            // maxLun = 0
            USBD_CtlSendData (usbdHandle, 0, 1);
          else {
            USBD_CtlError (usbdHandle, req);
            return USBD_FAIL;
            }
          break;

        case BOT_RESET :
          if ((req->wValue  == 0) &&
              (req->wLength == 0) &&
              ((req->bmRequest & 0x80) != 0x80)) {
            // Prepare EP to Receive First BOT Cmd
            auto mscData = (sMscData*)usbdHandle->pClassData;
            mscData->bot_state = USBD_BOT_IDLE;
            mscData->bot_status = USBD_BOT_STATUS_RECOVERY;
            usbdLowLevelPrepareReceive (usbdHandle, MSC_EPOUT_ADDR, (uint8_t*)&mscData->cbw, USBD_BOT_CBW_LENGTH);
            }
          else {
            USBD_CtlError (usbdHandle , req);
            return USBD_FAIL;
            }
          break;

        default:
          USBD_CtlError (usbdHandle , req);
          return USBD_FAIL;
        }

      break;
    //}}}
    //{{{
    case USB_REQ_TYPE_STANDARD: // Interface & Endpoint request
      switch (req->bRequest) {
        case USB_REQ_GET_INTERFACE :
          USBD_CtlSendData (usbdHandle, (uint8_t*)&mscData->interface, 1);
          break;

        case USB_REQ_SET_INTERFACE :
          mscData->interface = (uint8_t)(req->wValue);
          break;

        case USB_REQ_CLEAR_FEATURE:
          // Flush the FIFO and Clear the stall status
          usbdLowLevelFlushEP (usbdHandle, (uint8_t)req->wIndex);

          // Reactivate the EP
          usbdLowLevelCloseEP (usbdHandle , (uint8_t)req->wIndex);
          usbdLowLevelOpenEP (usbdHandle,
                              (((uint8_t)req->wIndex) & 0x80) == 0x80 ? MSC_EPIN_ADDR : MSC_EPOUT_ADDR,
                              USBD_EP_TYPE_BULK,
                              usbdHandle->dev_speed == USBD_SPEED_HIGH ? MSC_MAX_HS_PACKET : MSC_MAX_FS_PACKET);

          // Handle BOT error
          mscBotCplClrFeature (usbdHandle, (uint8_t)req->wIndex);
          break;
        }

      break;
    //}}}
    default:
      break;
    }

  return 0;
  }
//}}}
//{{{
uint8_t mscDataIn (USBD_HandleTypeDef* usbdHandle, uint8_t endPointNum) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  switch (mscData->bot_state) {
    case USBD_BOT_DATA_IN:
      if (scsiCmd (usbdHandle, mscData->cbw.bLUN, &mscData->cbw.CB[0]) < 0)
        mscBotSendCsw (usbdHandle, USBD_CSW_CMD_FAILED);
      break;

    case USBD_BOT_SEND_DATA:
    case USBD_BOT_LAST_DATA_IN:
      mscBotSendCsw (usbdHandle, USBD_CSW_CMD_PASSED);
      break;

    default:
      break;
    }

  return 0;
  }
//}}}
//{{{
uint8_t mscDataOut (USBD_HandleTypeDef* usbdHandle, uint8_t endPointNum) {

  auto mscData = (sMscData*)usbdHandle->pClassData;

  switch (mscData->bot_state) {
    case USBD_BOT_IDLE:
      mscBotCbwDecode (usbdHandle);
      break;

    case USBD_BOT_DATA_OUT:
      if (scsiCmd (usbdHandle, mscData->cbw.bLUN, &mscData->cbw.CB[0]) < 0)
        mscBotSendCsw (usbdHandle, USBD_CSW_CMD_FAILED);
      break;

    default:
      break;
    }

  return 0;
  }
//}}}

//{{{
uint8_t* mscGetHighSpeedConfigDesc (uint16_t* length) {

  *length = sizeof (kMscHighSpeedConfigDesc);
  return (uint8_t*)kMscHighSpeedConfigDesc;
  }
//}}}
//{{{
uint8_t* mscGetFullSpeedConfigDesc (uint16_t* length) {

  *length = sizeof (kMscFullSpeedConfigDesc);
  return (uint8_t*)kMscFullSpeedConfigDesc;
  }
//}}}
//{{{
uint8_t* mscGetOtherSpeedConfigDesc (uint16_t* length) {

  *length = sizeof (kMscOtherSpeedConfigDesc);
  return (uint8_t*)kMscOtherSpeedConfigDesc;
  }
//}}}
//{{{
uint8_t* mscGetDeviceQualifierDescriptor (uint16_t* length) {

  *length = sizeof (kMscDeviceQualifierDesc);
  return (uint8_t*)kMscDeviceQualifierDesc;
  }
//}}}

USBD_ClassTypeDef kMscHandlers = {
  mscInit,
  mscDeInit,
  mscSetup,
  NULL, /*EP0_TxSent*/
  NULL, /*EP0_RxReady*/
  mscDataIn,
  mscDataOut,
  NULL, /*SOF */
  NULL,
  NULL,
  mscGetHighSpeedConfigDesc,
  mscGetFullSpeedConfigDesc,
  mscGetOtherSpeedConfigDesc,
  mscGetDeviceQualifierDescriptor,
  };
//}}}

// HAL pcd
//{{{
void HAL_PCD_MspInit (PCD_HandleTypeDef* pcdHandle) {

   __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  if (kUsbFullSpeed) {
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);

    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
    HAL_NVIC_SetPriority (OTG_FS_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ (OTG_FS_IRQn);
    }
  else {
    // highSpeed init
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;

    // CLK, D0
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_3;
    HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);

    // D1 D2 D3 D4 D5 D6 D7
    GPIO_InitStruct.Pin = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_5 |
                          GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
    HAL_GPIO_Init (GPIOB, &GPIO_InitStruct);

    // STP, DIR
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_2;
    HAL_GPIO_Init (GPIOC, &GPIO_InitStruct);

    // NXT
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    HAL_GPIO_Init (GPIOH, &GPIO_InitStruct);

    __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();
    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
    HAL_NVIC_SetPriority (OTG_HS_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ (OTG_HS_IRQn);
    }
  }
//}}}
//{{{
void HAL_PCD_MspDeInit (PCD_HandleTypeDef* pcdHandle) {

  if (kUsbFullSpeed) {
    // Disable USB1 FS Clock
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
    __HAL_RCC_SYSCFG_CLK_DISABLE();
    }
  else {
    // Disable USB2 HS Clocks
    __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
    __HAL_RCC_SYSCFG_CLK_DISABLE();
    }
  }
//}}}
//{{{
void HAL_PCD_SetupStageCallback (PCD_HandleTypeDef* pcdHandle) {
  usbdLowLevelSetupStage ((USBD_HandleTypeDef*)pcdHandle->pData, (uint8_t*)pcdHandle->Setup);
  }
//}}}
//{{{
void HAL_PCD_DataOutStageCallback (PCD_HandleTypeDef* pcdHandle, uint8_t endPointNum) {
  usbdLowLevelDataOutStage ((USBD_HandleTypeDef*)pcdHandle->pData, endPointNum, pcdHandle->OUT_ep[endPointNum].xfer_buff);
  }
//}}}
//{{{
void HAL_PCD_DataInStageCallback (PCD_HandleTypeDef* pcdHandle, uint8_t endPointNum) {
  usbdLowLevelDataInStage ((USBD_HandleTypeDef*)pcdHandle->pData, endPointNum, pcdHandle->IN_ep[endPointNum].xfer_buff);
  }
//}}}
//{{{
void HAL_PCD_SOFCallback (PCD_HandleTypeDef* pcdHandle) {
  usbdLowLevelSOF ((USBD_HandleTypeDef*)pcdHandle->pData);
  }
//}}}
//{{{
void HAL_PCD_ResetCallback (PCD_HandleTypeDef* pcdHandle) {

  usbdLowLevelReset ((USBD_HandleTypeDef*)pcdHandle->pData);
  usbdLowLevelSetSpeed ((USBD_HandleTypeDef*)pcdHandle->pData,
                        (pcdHandle->Init.speed == PCD_SPEED_HIGH) ? USBD_SPEED_HIGH : USBD_SPEED_FULL);
  }
//}}}
//{{{
void HAL_PCD_SuspendCallback (PCD_HandleTypeDef* pcdHandle) {
  usbdLowLevelSuspend ((USBD_HandleTypeDef*)pcdHandle->pData);
  }
//}}}
//{{{
void HAL_PCD_ResumeCallback (PCD_HandleTypeDef* pcdHandle) {
  usbdLowLevelResume ((USBD_HandleTypeDef*)pcdHandle->pData);
  }
//}}}
//{{{
void HAL_PCD_ISOOUTIncompleteCallback (PCD_HandleTypeDef* pcdHandle, uint8_t endPointNum) {
  usbdLowLevelIsoOUTIncomplete ((USBD_HandleTypeDef*)pcdHandle->pData, endPointNum);
  }
//}}}
//{{{
void HAL_PCD_ISOINIncompleteCallback (PCD_HandleTypeDef* pcdHandle, uint8_t endPointNum) {
  usbdLowLevelIsoINIncomplete ((USBD_HandleTypeDef*)pcdHandle->pData, endPointNum);
  }
//}}}
//{{{
void HAL_PCD_ConnectCallback (PCD_HandleTypeDef* pcdHandle) {
  usbdLowLevelDevConnected ((USBD_HandleTypeDef*)pcdHandle->pData);
  }
//}}}
//{{{
void HAL_PCD_DisconnectCallback (PCD_HandleTypeDef* pcdHandle) {
  usbdLowLevelDevDisconnected ((USBD_HandleTypeDef*)pcdHandle->pData);
  }
//}}}

// usbd lowLevel
//{{{
USBD_StatusTypeDef usbdLowLevelInit (USBD_HandleTypeDef* usbdHandle) {

  gPcdHandle.Init.use_dedicated_ep1 = 0;
  gPcdHandle.Init.ep0_mps = 0x40;
  gPcdHandle.Init.dma_enable = 0;
  gPcdHandle.Init.low_power_enable = 0;
  gPcdHandle.Init.lpm_enable = 0;
  gPcdHandle.Init.Sof_enable = 0;
  gPcdHandle.pData = usbdHandle;
  usbdHandle->pData = &gPcdHandle;

  if (kUsbFullSpeed) {
    // usb1 fullSpeed
    gPcdHandle.Instance = USB_OTG_FS;
    gPcdHandle.Init.dev_endpoints = 4;
    gPcdHandle.Init.phy_itface = PCD_PHY_EMBEDDED;
    gPcdHandle.Init.speed = PCD_SPEED_FULL;
    gPcdHandle.Init.vbus_sensing_enable = 0;

    HAL_PCD_Init (&gPcdHandle);
    HAL_PCDEx_SetRxFiFo (&gPcdHandle, 0x80);
    HAL_PCDEx_SetTxFiFo (&gPcdHandle, 0, 0x40);
    HAL_PCDEx_SetTxFiFo (&gPcdHandle, 1, 0x80);
    }

  else {
    // usb2 highSpeed
    gPcdHandle.Instance = USB_OTG_HS;
    gPcdHandle.Init.dev_endpoints = 6;
    gPcdHandle.Init.phy_itface = PCD_PHY_ULPI;
    gPcdHandle.Init.speed = PCD_SPEED_HIGH;
    gPcdHandle.Init.vbus_sensing_enable = 1;

    HAL_PCD_Init (&gPcdHandle);
    HAL_PCDEx_SetRxFiFo (&gPcdHandle, 0x200);
    HAL_PCDEx_SetTxFiFo (&gPcdHandle, 0, 0x80);
    HAL_PCDEx_SetTxFiFo (&gPcdHandle, 1, 0x174);
    }

  return USBD_OK;
  }
//}}}

//{{{
void mscInit (cLcd* lcd) {

  gLcd = lcd;
  BSP_SD_Init();

  BSP_SD_CardInfo cardInfo;
  BSP_SD_GetCardInfo (&cardInfo);
  gLcd->debug (LCD_COLOR_YELLOW, "num:%d size:%d", cardInfo.LogBlockNbr, cardInfo.LogBlockSize);
  auto max = cardInfo.LogBlockNbr / kSectorScale;
  //for (auto i = 0; i < 480 * 272; i++)
  //  gSectors[i] = i > max ? 0 : 0x40404040;
  }
//}}}
//{{{
void mscStart() {

  USBD_Init (&gUsbDevice, &kMscDescriptors, 0);
  USBD_RegisterClass (&gUsbDevice, &kMscHandlers);
  USBD_Start (&gUsbDevice);
  }
//}}}
//{{{
//uint32_t* mscGetSectors() {
  //return gSectors;
  //}
//}}}
