// main.cpp - hid class usb
//{{{  includes
#include "../common/system.h"
#include "../common/cLcd.h"
#include "../common/cTouch.h"
#include "../common/stm32746g_discovery_sd.h"

#include "usbd_msc.h"
//}}}

PCD_HandleTypeDef hpcd;
USBD_HandleTypeDef USBD_Device;
extern SD_HandleTypeDef uSdHandle;

extern "C" {
  void OTG_HS_IRQHandler() { HAL_PCD_IRQHandler (&hpcd); }
  void BSP_SDMMC_IRQHandler() { HAL_SD_IRQHandler (&uSdHandle); }
  void BSP_SDMMC_DMA_Tx_IRQHandler() { HAL_DMA_IRQHandler (uSdHandle.hdmatx); }
  void BSP_SDMMC_DMA_Rx_IRQHandler() { HAL_DMA_IRQHandler (uSdHandle.hdmarx); }
  }

const char* kVersion = "USB Msc 5/3/18";

//{{{
class cApp : public cTouch {
public:
  cApp (int x, int y) : cTouch (x,y) {}
  cLcd* getLcd() { return mLcd; }

  void run (bool keyboard);

protected:
  virtual void onProx (int x, int y, int z);
  virtual void onPress (int x, int y);
  virtual void onMove (int x, int y, int z);
  virtual void onScroll (int x, int y, int z);
  virtual void onRelease (int x, int y);

private:
  cLcd* mLcd = nullptr;
  bool mButton = false;
  };
//}}}
cApp* gApp;

//{{{  usbd
//{{{
void HAL_PCD_MspInit (PCD_HandleTypeDef* hpcd) {

  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Configure USB FS GPIOs */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /* CLK */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* D0 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* D1 D2 D3 D4 D5 D6 D7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_5 |\
    GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* STP */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* NXT */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /* DIR */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();

  /* Enable USB HS Clocks */
  __HAL_RCC_USB_OTG_HS_CLK_ENABLE();

  /* Set USBHS Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(OTG_HS_IRQn, 7, 0);

  /* Enable USBHS Interrupt */
  HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
  }
//}}}
//{{{
void HAL_PCD_MspDeInit (PCD_HandleTypeDef* hpcd) {

  /* Disable USB HS Clocks */
  __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
  __HAL_RCC_SYSCFG_CLK_DISABLE();
  }
//}}}

void HAL_PCD_SetupStageCallback (PCD_HandleTypeDef* hpcd) {
  USBD_LL_SetupStage ((USBD_HandleTypeDef*)(hpcd->pData), (uint8_t*)hpcd->Setup); }
void HAL_PCD_DataOutStageCallback (PCD_HandleTypeDef* hpcd, uint8_t epnum) {
  USBD_LL_DataOutStage ((USBD_HandleTypeDef*)(hpcd->pData), epnum, hpcd->OUT_ep[epnum].xfer_buff); }
void HAL_PCD_DataInStageCallback (PCD_HandleTypeDef* hpcd, uint8_t epnum) {
  USBD_LL_DataInStage ((USBD_HandleTypeDef*)(hpcd->pData), epnum, hpcd->IN_ep[epnum].xfer_buff); }
void HAL_PCD_SOFCallback (PCD_HandleTypeDef* hpcd) { USBD_LL_SOF((USBD_HandleTypeDef*)(hpcd->pData)); }
//{{{
void HAL_PCD_ResetCallback (PCD_HandleTypeDef* hpcd) {

  USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

  /* Set USB Current Speed */
  switch (hpcd->Init.speed) {
    case PCD_SPEED_HIGH:
      speed = USBD_SPEED_HIGH;
      break;

    case PCD_SPEED_FULL:
      speed = USBD_SPEED_FULL;
      break;

    default:
      speed = USBD_SPEED_FULL;
      break;
    }

  /* Reset Device */
  USBD_LL_Reset ((USBD_HandleTypeDef*)(hpcd->pData));
  USBD_LL_SetSpeed ((USBD_HandleTypeDef*)(hpcd->pData), speed);
  }
//}}}
void HAL_PCD_SuspendCallback (PCD_HandleTypeDef* hpcd) { USBD_LL_Suspend((USBD_HandleTypeDef*)(hpcd->pData)); }
void HAL_PCD_ResumeCallback (PCD_HandleTypeDef* hpcd) { USBD_LL_Resume((USBD_HandleTypeDef*)(hpcd->pData)); }
void HAL_PCD_ISOOUTIncompleteCallback (PCD_HandleTypeDef* hpcd, uint8_t epnum) {
  USBD_LL_IsoOUTIncomplete ((USBD_HandleTypeDef*)(hpcd->pData), epnum); }
void HAL_PCD_ISOINIncompleteCallback( PCD_HandleTypeDef* hpcd, uint8_t epnum) {
  USBD_LL_IsoINIncomplete ((USBD_HandleTypeDef*)(hpcd->pData), epnum); }
void HAL_PCD_ConnectCallback (PCD_HandleTypeDef* hpcd) { USBD_LL_DevConnected ((USBD_HandleTypeDef*)(hpcd->pData)); }
void HAL_PCD_DisconnectCallback (PCD_HandleTypeDef* hpcd) { USBD_LL_DevDisconnected ((USBD_HandleTypeDef*)(hpcd->pData)); }

//{{{
USBD_StatusTypeDef USBD_LL_Init (USBD_HandleTypeDef* pdev) {

  /* Set LL Driver parameters */
  hpcd.Instance = USB_OTG_HS;
  hpcd.Init.dev_endpoints = 6;
  hpcd.Init.use_dedicated_ep1 = 0;
  hpcd.Init.ep0_mps = 0x40;

  // Be aware that enabling DMA mode will result in data being sent only by
  //multiple of 4 packet sizes. This is due to the fact that USB DMA does
  //not allow sending data from non word-aligned addresses.
  //For this specific application, it is advised to not enable this option unless required
  hpcd.Init.dma_enable = 0;
  hpcd.Init.low_power_enable = 0;
  hpcd.Init.lpm_enable = 0;
  hpcd.Init.phy_itface = PCD_PHY_ULPI;
  hpcd.Init.Sof_enable = 0;
  hpcd.Init.speed = PCD_SPEED_HIGH;
  hpcd.Init.vbus_sensing_enable = 1;

  /* Link The driver to the stack */
  hpcd.pData = pdev;
  pdev->pData = &hpcd;

  /* Initialize LL Driver */
  HAL_PCD_Init (&hpcd);
  HAL_PCDEx_SetRxFiFo( &hpcd, 0x200);
  HAL_PCDEx_SetTxFiFo (&hpcd, 0, 0x80);
  HAL_PCDEx_SetTxFiFo (&hpcd, 1, 0x174);

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_DeInit (USBD_HandleTypeDef* pdev) {
  HAL_PCD_DeInit((PCD_HandleTypeDef*)(pdev->pData));
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_Start (USBD_HandleTypeDef* pdev) {
  HAL_PCD_Start((PCD_HandleTypeDef*)(pdev->pData));
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_Stop( USBD_HandleTypeDef* pdev) {
  HAL_PCD_Stop((PCD_HandleTypeDef*)(pdev->pData));
  return USBD_OK;
  }
//}}}

//{{{
USBD_StatusTypeDef USBD_LL_OpenEP( USBD_HandleTypeDef* pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps) {
  HAL_PCD_EP_Open((PCD_HandleTypeDef*)(pdev->pData), ep_addr, ep_mps, ep_type);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_CloseEP (USBD_HandleTypeDef* pdev, uint8_t ep_addr) {
  HAL_PCD_EP_Close((PCD_HandleTypeDef*)(pdev->pData), ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_FlushEP (USBD_HandleTypeDef* pdev, uint8_t ep_addr) {
  HAL_PCD_EP_Flush((PCD_HandleTypeDef*)(pdev->pData), ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_StallEP (USBD_HandleTypeDef* pdev, uint8_t ep_addr) {
  HAL_PCD_EP_SetStall((PCD_HandleTypeDef*)(pdev->pData), ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_ClearStallEP (USBD_HandleTypeDef* pdev, uint8_t ep_addr) {
  HAL_PCD_EP_ClrStall((PCD_HandleTypeDef*)(pdev->pData), ep_addr);
  return USBD_OK;
  }
//}}}

//{{{
uint8_t USBD_LL_IsStallEP (USBD_HandleTypeDef* pdev, uint8_t ep_addr) {

  PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef*)(pdev->pData);
  if ((ep_addr & 0x80) == 0x80)
    return hpcd->IN_ep[ep_addr & 0xF].is_stall;
  else
    return hpcd->OUT_ep[ep_addr & 0xF].is_stall;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_SetUSBAddress (USBD_HandleTypeDef* pdev, uint8_t dev_addr) {
  HAL_PCD_SetAddress((PCD_HandleTypeDef*)(pdev->pData), dev_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_Transmit (USBD_HandleTypeDef* pdev, uint8_t ep_addr, uint8_t* pbuf, uint16_t size) {
  HAL_PCD_EP_Transmit((PCD_HandleTypeDef*)(pdev->pData), ep_addr, pbuf, size);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_PrepareReceive (USBD_HandleTypeDef* pdev, uint8_t ep_addr, uint8_t* pbuf, uint16_t size) {
  HAL_PCD_EP_Receive((PCD_HandleTypeDef*)(pdev->pData), ep_addr, pbuf, size);
  return USBD_OK;
  }
//}}}

//{{{
uint32_t USBD_LL_GetRxDataSize (USBD_HandleTypeDef* pdev, uint8_t ep_addr) {
  return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef*)(pdev->pData), ep_addr);
  }
//}}}

void USBD_LL_Delay (uint32_t Delay) { HAL_Delay(Delay); }
//}}}
__IO uint32_t writestatus, readstatus = 0;
//{{{  sd card
//{{{
void BSP_SD_MspInit (SD_HandleTypeDef* hsd, void* Params) {

  static DMA_HandleTypeDef dma_rx_handle;
  static DMA_HandleTypeDef dma_tx_handle;

  GPIO_InitTypeDef gpio_init_structure;

  __HAL_RCC_SDMMC1_CLK_ENABLE();
  __DMAx_TxRx_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* Common GPIO configuration */
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
  gpio_init_structure.Alternate = GPIO_AF12_SDMMC1;

  /* GPIOC configuration */
  gpio_init_structure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
  HAL_GPIO_Init(GPIOC, &gpio_init_structure);

  /* GPIOD configuration */
  gpio_init_structure.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  /* NVIC configuration for SDIO interrupts */
  HAL_NVIC_SetPriority(SDMMC1_IRQn, 0x05, 0);
  HAL_NVIC_EnableIRQ(SDMMC1_IRQn);

  /* Configure DMA Rx parameters */
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

  /* Configure DMA Tx parameters */
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

  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SD_DMAx_Rx_IRQn, 0x06, 0);
  HAL_NVIC_EnableIRQ(SD_DMAx_Rx_IRQn);

  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SD_DMAx_Tx_IRQn, 0x06, 0);
  HAL_NVIC_EnableIRQ(SD_DMAx_Tx_IRQn);
  }
//}}}
void BSP_SD_WriteCpltCallback() { writestatus = 1; }
void BSP_SD_ReadCpltCallback() { readstatus = 1; }

//{{{
int8_t init (uint8_t lun) {

  gApp->getLcd()->debug (LCD_COLOR_WHITE, "sd init");

  BSP_SD_Init();
  return 0;
  }
//}}}
//{{{
int8_t getCapacity (uint8_t lun, uint32_t* block_num, uint16_t* block_size) {

  if (BSP_SD_IsDetected() != SD_NOT_PRESENT) {
    HAL_SD_CardInfoTypeDef info;
    BSP_SD_GetCardInfo (&info);
    *block_num = info.LogBlockNbr - 1;
    *block_size = info.LogBlockSize;
    //gApp->getLcd()->debug (LCD_COLOR_WHITE, "sd getCapacity %d %d", (int)block_num, (int)block_size);
    return 0;
    }
  else
    gApp->getLcd()->debug (LCD_COLOR_WHITE, "sd getCapacity SD_NOT_PRESENT");

  return -1;
  }
//}}}
//{{{
int8_t isReady (uint8_t lun) {

  static int8_t prev_status = 0;
  if (BSP_SD_IsDetected() != SD_NOT_PRESENT) {
    if (prev_status < 0) {
      BSP_SD_Init();
      prev_status = 0;
      }
    if (BSP_SD_GetCardState() == SD_TRANSFER_OK)
      return 0;
    }
  else if (prev_status == 0)
    prev_status = -1;

  return -1;
  }
//}}}
int8_t isWriteProtected (uint8_t lun) { return 0; }

//{{{
int8_t read (uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len) {

  if (BSP_SD_IsDetected() != SD_NOT_PRESENT) {
    BSP_SD_ReadBlocks ((uint32_t *)buf, blk_addr, blk_len, 100);
    //while (!readstatus) {}
    //readstatus = 0;

    while (BSP_SD_GetCardState() != SD_TRANSFER_OK) {}

    //gApp->getLcd()->debug (LCD_COLOR_WHITE, "read %x", blk_addr);
    printf ("read %d\n", blk_addr);
    return 0;
    }

  return -1;
  }
//}}}
//{{{
int8_t write (uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len) {

  if (BSP_SD_IsDetected() != SD_NOT_PRESENT) {
    BSP_SD_WriteBlocks ((uint32_t *)buf, blk_addr, blk_len, 100);
    //while (!writestatus) {}
    //writestatus = 0;

    // Wait until SD card is ready to use for new operation
    while (BSP_SD_GetCardState() != SD_TRANSFER_OK) {}

    gApp->getLcd()->debug (LCD_COLOR_WHITE, "write buf");
    return 0;
    }

  return -1;
  }
//}}}

int8_t getMaxLun() { return 0; }

//{{{
//  USB Mass storage Standard Inquiry Data
const uint8_t kInquirydata[] = {
  // 36 bytes
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

USBD_StorageTypeDef USBD_DISK_fops = {
  init,
  getCapacity,
  isReady,
  isWriteProtected,
  read,
  write,
  getMaxLun,
  (int8_t*)kInquirydata,
  };
//}}}

// init usbDevice library
//{{{
void cApp::run (bool keyboard) {

  mButton = BSP_PB_GetState (BUTTON_KEY);

  // init lcd
  mLcd = new cLcd (12);
  mLcd->init();

  USBD_Init (&USBD_Device, &MSC_Desc, 0);
  USBD_RegisterClass (&USBD_Device, USBD_MSC_CLASS);
  USBD_MSC_RegisterStorage (&USBD_Device, &USBD_DISK_fops);
  USBD_Start (&USBD_Device);

  while (true) {
    pollTouch();
    mLcd->show (kVersion);
    mLcd->flip();
    }
  }
//}}}
//{{{
void cApp::onProx (int x, int y, int z) {

  if (x || y) {
    //uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 0,(uint8_t)x,(uint8_t)y,0 };
    // hidSendReport (&gUsbDevice, HID_Buffer);
    mLcd->debug (LCD_COLOR_MAGENTA, "onProx %d %d %d", x, y, z);
    }
  }
//}}}
//{{{
void cApp::onPress (int x, int y) {

  //uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 1,0,0,0 };
  //hidSendReport (&gUsbDevice, HID_Buffer);
  mLcd->debug (LCD_COLOR_GREEN, "onPress %d %d", x, y);
  }
//}}}
//{{{
void cApp::onMove (int x, int y, int z) {

  if (x || y) {
    //uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 1,(uint8_t)x,(uint8_t)y,0 };
    //hidSendReport (&gUsbDevice, HID_Buffer);
    mLcd->debug (LCD_COLOR_GREEN, "onMove %d %d %d", x, y, z);
    }
  }
//}}}
//{{{
void cApp::onScroll (int x, int y, int z) {
  mLcd->incScrollValue (y);
  }
//}}}
//{{{
void cApp::onRelease (int x, int y) {

  //uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 0,0,0,0 };
  //hidSendReport (&gUsbDevice, HID_Buffer);
  mLcd->debug (LCD_COLOR_GREEN, "onRelease %d %d", x, y);
  }
//}}}

//{{{
int main() {

  SCB_EnableICache();
  SCB_EnableDCache();
  HAL_Init();
  //{{{  config system clock
  // System Clock source            = PLL (HSE)
  // SYSCLK(Hz)                     = 216000000
  // HCLK(Hz)                       = 216000000
  // AHB Prescaler                  = 1
  // APB1 Prescaler                 = 4
  // APB2 Prescaler                 = 2
  // HSE Frequency(Hz)              = 25000000
  // PLL_M                          = 25
  // PLL_N                          = 432
  // PLL_P                          = 2
  // PLLSAI_N                       = 384
  // PLLSAI_P                       = 8
  // VDD(V)                         = 3.3
  // Main regulator output voltage  = Scale1 mode
  // Flash Latency(WS)              = 7

  // Enable HSE Oscillator and activate PLL with HSE as source
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK)
    while (true) {}

  // Activate the OverDrive to reach the 216 Mhz Frequency
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    while (true) {}

  // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    while (true) {}
  //}}}
  BSP_PB_Init (BUTTON_KEY, BUTTON_MODE_GPIO);

  gApp = new cApp (BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  gApp->run (true);
  }
//}}}
