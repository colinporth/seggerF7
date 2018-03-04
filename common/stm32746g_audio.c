// stm32746g_audio.cpp
#include "stm32746g_audio.h"
//{{{  defines
// CODEC_SLOT0 Left | CODEC_SLOT1 Left | CODEC_SLOT0 Right  | CODEC_SLOT1 Right |

/* SAI OUT peripheral configuration defines */
#define AUDIO_OUT_SAIx                           SAI2_Block_A
#define AUDIO_OUT_SAIx_CLK_ENABLE()              __HAL_RCC_SAI2_CLK_ENABLE()
#define AUDIO_OUT_SAIx_CLK_DISABLE()             __HAL_RCC_SAI2_CLK_DISABLE()
#define AUDIO_OUT_SAIx_SCK_AF                    GPIO_AF10_SAI2
#define AUDIO_OUT_SAIx_FS_SD_MCLK_AF             GPIO_AF10_SAI2

#define AUDIO_OUT_SAIx_MCLK_ENABLE()             __HAL_RCC_GPIOI_CLK_ENABLE()
#define AUDIO_OUT_SAIx_MCLK_GPIO_PORT            GPIOI
#define AUDIO_OUT_SAIx_MCLK_PIN                  GPIO_PIN_4
#define AUDIO_OUT_SAIx_SCK_SD_ENABLE()           __HAL_RCC_GPIOI_CLK_ENABLE()
#define AUDIO_OUT_SAIx_SCK_SD_GPIO_PORT          GPIOI
#define AUDIO_OUT_SAIx_SCK_PIN                   GPIO_PIN_5
#define AUDIO_OUT_SAIx_SD_PIN                    GPIO_PIN_6
#define AUDIO_OUT_SAIx_FS_ENABLE()               __HAL_RCC_GPIOI_CLK_ENABLE()
#define AUDIO_OUT_SAIx_FS_GPIO_PORT              GPIOI
#define AUDIO_OUT_SAIx_FS_PIN                    GPIO_PIN_7

/* SAI DMA Stream definitions */
#define AUDIO_OUT_SAIx_DMAx_CLK_ENABLE()         __HAL_RCC_DMA2_CLK_ENABLE()
#define AUDIO_OUT_SAIx_DMAx_STREAM               DMA2_Stream4
#define AUDIO_OUT_SAIx_DMAx_CHANNEL              DMA_CHANNEL_3
#define AUDIO_OUT_SAIx_DMAx_IRQ                  DMA2_Stream4_IRQn
#define AUDIO_OUT_SAIx_DMAx_PERIPH_DATA_SIZE     DMA_PDATAALIGN_HALFWORD
#define AUDIO_OUT_SAIx_DMAx_MEM_DATA_SIZE        DMA_MDATAALIGN_HALFWORD
#define DMA_MAX_SZE                              ((uint16_t)0xFFFF)

#define AUDIO_OUT_SAIx_DMAx_IRQHandler           DMA2_Stream4_IRQHandler

/* Select the interrupt preemption priority for the DMA interrupt */
#define AUDIO_OUT_IRQ_PREPRIO                    ((uint32_t)0x0E)   /* Select the preemption priority level(0 is the highest) */

/* SAI IN peripheral configuration defines */
#define AUDIO_IN_SAIx                            SAI2_Block_B
#define AUDIO_IN_SAIx_CLK_ENABLE()               __HAL_RCC_SAI2_CLK_ENABLE()
#define AUDIO_IN_SAIx_CLK_DISABLE()              __HAL_RCC_SAI2_CLK_DISABLE()
#define AUDIO_IN_SAIx_SD_AF                      GPIO_AF10_SAI2

#define AUDIO_IN_SAIx_SD_ENABLE()                __HAL_RCC_GPIOG_CLK_ENABLE()
#define AUDIO_IN_SAIx_SD_GPIO_PORT               GPIOG
#define AUDIO_IN_SAIx_SD_PIN                     GPIO_PIN_10

#define AUDIO_IN_INT_GPIO_ENABLE()               __HAL_RCC_GPIOH_CLK_ENABLE()
#define AUDIO_IN_INT_GPIO_PORT                   GPIOH
#define AUDIO_IN_INT_GPIO_PIN                    GPIO_PIN_15
#define AUDIO_IN_INT_IRQ                         EXTI15_10_IRQn

/* SAI DMA Stream definitions */
#define AUDIO_IN_SAIx_DMAx_CLK_ENABLE()          __HAL_RCC_DMA2_CLK_ENABLE()
#define AUDIO_IN_SAIx_DMAx_STREAM                DMA2_Stream7
#define AUDIO_IN_SAIx_DMAx_CHANNEL               DMA_CHANNEL_0
#define AUDIO_IN_SAIx_DMAx_IRQ                   DMA2_Stream7_IRQn
#define AUDIO_IN_SAIx_DMAx_PERIPH_DATA_SIZE      DMA_PDATAALIGN_HALFWORD
#define AUDIO_IN_SAIx_DMAx_MEM_DATA_SIZE         DMA_MDATAALIGN_HALFWORD

#define AUDIO_IN_SAIx_DMAx_IRQHandler            DMA2_Stream7_IRQHandler
#define AUDIO_IN_INT_IRQHandler                  EXTI15_10_IRQHandler

/* Select the interrupt preemption priority and subpriority for the IT/DMA interrupt */
#define AUDIO_IN_IRQ_PREPRIO                     ((uint32_t)0x0F)   /* Select the preemption priority level(0 is the highest) */

#define AUDIODATA_SIZE                      ((uint16_t)2)   /* 16-bits audio data size */

/* Audio status definition */
#define AUDIO_OK                            ((uint8_t)0)
#define AUDIO_ERROR                         ((uint8_t)1)
#define AUDIO_TIMEOUT                       ((uint8_t)2)

/* AudioFreq * DataSize (2 bytes) * NumChannels (Stereo: 2) */
#define DEFAULT_AUDIO_IN_FREQ               I2S_AUDIOFREQ_16K
#define DEFAULT_AUDIO_IN_BIT_RESOLUTION     ((uint8_t)16)
#define DEFAULT_AUDIO_IN_CHANNEL_NBR        ((uint8_t)2) /* Mono = 1, Stereo = 2 */
#define DEFAULT_AUDIO_IN_VOLUME             ((uint16_t)64)

/* Delay for the Codec to be correctly reset */
#define CODEC_RESET_DELAY                   ((uint8_t)5)

#define DMA_MAX(x)  (((x) <= DMA_MAX_SZE)? (x):DMA_MAX_SZE)
//}}}

AUDIO_DrvTypeDef* audio_drv;
uint16_t __IO AudioInVolume = DEFAULT_AUDIO_IN_VOLUME;

static SAI_HandleTypeDef haudio_out_sai = {0};
static SAI_HandleTypeDef haudio_in_sai = {0};

void DMA2_Stream4_IRQHandler() { HAL_DMA_IRQHandler (haudio_out_sai.hdmatx); }

//{{{
static void SAIx_Out_Init (uint32_t AudioFreq) {

  // Initialize the haudio_out_sai Instance parameter
  haudio_out_sai.Instance = AUDIO_OUT_SAIx;

  // Disable SAI peripheral to allow access to SAI internal registers
  __HAL_SAI_DISABLE(&haudio_out_sai);

  // Configure SAI_Block_x
  // LSBFirst: Disabled
  // DataSize: 16
  haudio_out_sai.Init.AudioFrequency = AudioFreq;
  haudio_out_sai.Init.AudioMode = SAI_MODEMASTER_TX;
  haudio_out_sai.Init.NoDivider = SAI_MASTERDIVIDER_ENABLED;
  haudio_out_sai.Init.Protocol = SAI_FREE_PROTOCOL;
  haudio_out_sai.Init.DataSize = SAI_DATASIZE_16;
  haudio_out_sai.Init.FirstBit = SAI_FIRSTBIT_MSB;
  haudio_out_sai.Init.ClockStrobing = SAI_CLOCKSTROBING_RISINGEDGE;
  haudio_out_sai.Init.Synchro = SAI_ASYNCHRONOUS;
  haudio_out_sai.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLED;
  haudio_out_sai.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;

  // Configure SAI_Block_x Frame
  // Frame Length: 64
  // Frame active Length: 32
  // FS Definition: Start frame + Channel Side identification
  // FS Polarity: FS active Low
  // FS Offset: FS asserted one bit before the first bit of slot 0
  haudio_out_sai.FrameInit.FrameLength = 64;
  haudio_out_sai.FrameInit.ActiveFrameLength = 32;
  haudio_out_sai.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
  haudio_out_sai.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  haudio_out_sai.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;

  // Configure SAI Block_x Slot
  // Slot First Bit Offset: 0
  // Slot Size  : 16
  // Slot Number: 4
  // Slot Active: All slot actives
  haudio_out_sai.SlotInit.FirstBitOffset = 0;
  haudio_out_sai.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  haudio_out_sai.SlotInit.SlotNumber = 4;
  haudio_out_sai.SlotInit.SlotActive = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1 | SAI_SLOTACTIVE_2 | SAI_SLOTACTIVE_3;

  HAL_SAI_Init(&haudio_out_sai);

  // Enable SAI peripheral to generate MCLK */
  __HAL_SAI_ENABLE(&haudio_out_sai);
  }
//}}}
//{{{
static void SAIx_Out_DeInit() {

  // Initialize the haudio_out_sai Instance parameter
  haudio_out_sai.Instance = AUDIO_OUT_SAIx;

  // Disable SAI peripheral
  __HAL_SAI_DISABLE (&haudio_out_sai);
  HAL_SAI_DeInit (&haudio_out_sai);
  }
//}}}
//{{{
static void SAIx_In_Init (uint32_t SaiOutMode, uint32_t SlotActive, uint32_t AudioFreq) {

  // Initialize SAI2 block A in MASTER RX
  // Initialize the haudio_out_sai Instance parameter
  haudio_out_sai.Instance = AUDIO_OUT_SAIx;

  // Disable SAI peripheral to allow access to SAI internal registers
  __HAL_SAI_DISABLE(&haudio_out_sai);

  // Configure SAI_Block_x
  // LSBFirst: Disabled
  // DataSize: 16
  haudio_out_sai.Init.AudioFrequency = AudioFreq;
  haudio_out_sai.Init.AudioMode = SaiOutMode;
  haudio_out_sai.Init.NoDivider = SAI_MASTERDIVIDER_ENABLED;
  haudio_out_sai.Init.Protocol = SAI_FREE_PROTOCOL;
  haudio_out_sai.Init.DataSize = SAI_DATASIZE_16;
  haudio_out_sai.Init.FirstBit = SAI_FIRSTBIT_MSB;
  haudio_out_sai.Init.ClockStrobing = SAI_CLOCKSTROBING_RISINGEDGE;
  haudio_out_sai.Init.Synchro = SAI_ASYNCHRONOUS;
  haudio_out_sai.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLED;
  haudio_out_sai.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;

  // Configure SAI_Block_x Frame
  // Frame Length: 64
  // Frame active Length: 32
  // FS Definition: Start frame + Channel Side identification
  // FS Polarity: FS active Low
  // FS Offset: FS asserted one bit before the first bit of slot 0
  haudio_out_sai.FrameInit.FrameLength = 64;
  haudio_out_sai.FrameInit.ActiveFrameLength = 32;
  haudio_out_sai.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
  haudio_out_sai.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  haudio_out_sai.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;

  // Configure SAI Block_x Slot
  // Slot First Bit Offset: 0
  // Slot Size  : 16
  // Slot Number: 4
  // Slot Active: All slot actives
  haudio_out_sai.SlotInit.FirstBitOffset = 0;
  haudio_out_sai.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  haudio_out_sai.SlotInit.SlotNumber = 4;
  haudio_out_sai.SlotInit.SlotActive = SlotActive;

  HAL_SAI_Init(&haudio_out_sai);

  // Initialize SAI2 block B in SLAVE RX synchronous from SAI2 block A
  // Initialize the haudio_in_sai Instance parameter
  haudio_in_sai.Instance = AUDIO_IN_SAIx;

  // Disable SAI peripheral to allow access to SAI internal registers
  __HAL_SAI_DISABLE(&haudio_in_sai);

  // Configure SAI_Block_x LSBFirst: Disabled DataSize: 16
  haudio_in_sai.Init.AudioFrequency = AudioFreq;
  haudio_in_sai.Init.AudioMode = SAI_MODESLAVE_RX;
  haudio_in_sai.Init.NoDivider = SAI_MASTERDIVIDER_ENABLED;
  haudio_in_sai.Init.Protocol = SAI_FREE_PROTOCOL;
  haudio_in_sai.Init.DataSize = SAI_DATASIZE_16;
  haudio_in_sai.Init.FirstBit = SAI_FIRSTBIT_MSB;
  haudio_in_sai.Init.ClockStrobing = SAI_CLOCKSTROBING_RISINGEDGE;
  haudio_in_sai.Init.Synchro = SAI_SYNCHRONOUS;
  haudio_in_sai.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLED;
  haudio_in_sai.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;

  // Configure SAI_Block_x Frame
  // Frame Length: 64
  // Frame active Length: 32
  // FS Definition: Start frame + Channel Side identification
  // FS Polarity: FS active Low
  // FS Offset: FS asserted one bit before the first bit of slot 0
  haudio_in_sai.FrameInit.FrameLength = 64;
  haudio_in_sai.FrameInit.ActiveFrameLength = 32;
  haudio_in_sai.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
  haudio_in_sai.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  haudio_in_sai.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;

  // Configure SAI Block_x Slot
  // Slot First Bit Offset: 0
  // Slot Size  : 16
  // Slot Number: 4
  // Slot Active: All slot active
  haudio_in_sai.SlotInit.FirstBitOffset = 0;
  haudio_in_sai.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  haudio_in_sai.SlotInit.SlotNumber = 4;
  haudio_in_sai.SlotInit.SlotActive = SlotActive;

  HAL_SAI_Init(&haudio_in_sai);

  // Enable SAI peripheral to generate MCLK
  __HAL_SAI_ENABLE(&haudio_out_sai);

  // Enable SAI peripheral
  __HAL_SAI_ENABLE(&haudio_in_sai);
  }
//}}}
//{{{
static void SAIx_In_DeInit() {

  // Initialize the haudio_in_sai Instance parameter
  haudio_in_sai.Instance = AUDIO_IN_SAIx;

  // Disable SAI peripheral
  __HAL_SAI_DISABLE(&haudio_in_sai);
  HAL_SAI_DeInit(&haudio_in_sai);
  }
//}}}

//{{{
void HAL_SAI_TxCpltCallback (SAI_HandleTypeDef *hsai) {
  BSP_AUDIO_OUT_TransferComplete_CallBack();
  }
//}}}
//{{{
void HAL_SAI_TxHalfCpltCallback (SAI_HandleTypeDef *hsai) {
  BSP_AUDIO_OUT_HalfTransfer_CallBack();
  }
//}}}
//{{{
void HAL_SAI_RxCpltCallback (SAI_HandleTypeDef *hsai) {
  BSP_AUDIO_IN_TransferComplete_CallBack();
  }
//}}}
//{{{
void HAL_SAI_RxHalfCpltCallback (SAI_HandleTypeDef *hsai) {
  BSP_AUDIO_IN_HalfTransfer_CallBack();
  }
//}}}
//{{{
void HAL_SAI_ErrorCallback (SAI_HandleTypeDef *hsai) {

  HAL_SAI_StateTypeDef audio_out_state;
  HAL_SAI_StateTypeDef audio_in_state;

  audio_out_state = HAL_SAI_GetState(&haudio_out_sai);
  audio_in_state = HAL_SAI_GetState(&haudio_in_sai);

  // Determines if it is an audio out or audio in error
  if ((audio_out_state == HAL_SAI_STATE_BUSY) || (audio_out_state == HAL_SAI_STATE_BUSY_TX))
    BSP_AUDIO_OUT_Error_CallBack();

  if ((audio_in_state == HAL_SAI_STATE_BUSY) || (audio_in_state == HAL_SAI_STATE_BUSY_RX))
    BSP_AUDIO_IN_Error_CallBack();
  }
//}}}

// out
//{{{
uint8_t BSP_AUDIO_OUT_Init (uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq) {

  uint8_t ret = AUDIO_ERROR;
  uint32_t deviceid = 0x00;

  // Disable SAI
  SAIx_Out_DeInit();

  // PLL clock is set depending on the AudioFreq (44.1khz vs 48khz groups)
  BSP_AUDIO_OUT_ClockConfig (&haudio_out_sai, AudioFreq, NULL);

  // SAI data transfer preparation:
  // Prepare the Media to be used for the audio transfer from memory to SAI peripheral
  haudio_out_sai.Instance = AUDIO_OUT_SAIx;
  if (HAL_SAI_GetState(&haudio_out_sai) == HAL_SAI_STATE_RESET)
    // Init the SAI MSP: this __weak function can be redefined by the application
    BSP_AUDIO_OUT_MspInit(&haudio_out_sai, NULL);
  SAIx_Out_Init(AudioFreq);

  // wm8994 codec initialization
  deviceid = wm8994_drv.ReadID(AUDIO_I2C_ADDRESS);
  if ((deviceid) == WM8994_ID) {
    // Reset the Codec Registers
    wm8994_drv.Reset(AUDIO_I2C_ADDRESS);
    // Initialize the audio driver structure
    audio_drv = &wm8994_drv;
    ret = AUDIO_OK;
    }
  else
    ret = AUDIO_ERROR;

  if (ret == AUDIO_OK)
    // Initialize the codec internal registers
    audio_drv->Init(AUDIO_I2C_ADDRESS, OutputDevice, Volume, AudioFreq);

  return ret;
  }
//}}}
//{{{
void BSP_AUDIO_OUT_DeInit() {

  SAIx_Out_DeInit();
  BSP_AUDIO_OUT_MspDeInit(&haudio_out_sai, NULL);
  }
//}}}

//{{{
uint8_t BSP_AUDIO_OUT_Play (uint16_t* pBuffer, uint32_t Size) {

  audio_drv->Play (AUDIO_I2C_ADDRESS, pBuffer, Size);
  HAL_SAI_Transmit_DMA (&haudio_out_sai, (uint8_t*) pBuffer, DMA_MAX(Size / AUDIODATA_SIZE));
  return AUDIO_OK;
  }
//}}}
//{{{
void BSP_AUDIO_OUT_ChangeBuffer (uint16_t *pData, uint16_t Size) {
  HAL_SAI_Transmit_DMA (&haudio_out_sai, (uint8_t*) pData, Size);
  }
//}}}
//{{{
uint8_t BSP_AUDIO_OUT_Pause() {

  audio_drv->Pause (AUDIO_I2C_ADDRESS);
  HAL_SAI_DMAPause (&haudio_out_sai);
  return AUDIO_OK;
  }
//}}}
//{{{
uint8_t BSP_AUDIO_OUT_Resume() {

  audio_drv->Resume (AUDIO_I2C_ADDRESS);
  HAL_SAI_DMAResume (&haudio_out_sai);
  return AUDIO_OK;
  }
//}}}
//{{{
uint8_t BSP_AUDIO_OUT_Stop (uint32_t Option) {

  HAL_SAI_DMAStop (&haudio_out_sai);

  audio_drv->Stop (AUDIO_I2C_ADDRESS, Option);
  if (Option == CODEC_PDWN_HW)
    // Wait at least 100us
    HAL_Delay(1);

  return AUDIO_OK;
  }
//}}}
//{{{
uint8_t BSP_AUDIO_OUT_SetVolume (uint8_t Volume) {

  audio_drv->SetVolume(AUDIO_I2C_ADDRESS, Volume);
  return AUDIO_OK;
  }
//}}}
//{{{
uint8_t BSP_AUDIO_OUT_SetMute (uint32_t Cmd) {

  audio_drv->SetMute (AUDIO_I2C_ADDRESS, Cmd);
  return AUDIO_OK;
  }
//}}}
//{{{
uint8_t BSP_AUDIO_OUT_SetOutputMode (uint8_t Output) {

  audio_drv->SetOutputMode (AUDIO_I2C_ADDRESS, Output);
  return AUDIO_OK;
  }
//}}}
//{{{
void BSP_AUDIO_OUT_SetFrequency (uint32_t AudioFreq) {

  // PLL clock is set depending by the AudioFreq (44.1khz vs 48khz groups)
  BSP_AUDIO_OUT_ClockConfig(&haudio_out_sai, AudioFreq, NULL);

  // Disable SAI peripheral to allow access to SAI internal registers
  __HAL_SAI_DISABLE(&haudio_out_sai);

  // Update the SAI audio frequency configuration
  haudio_out_sai.Init.AudioFrequency = AudioFreq;
  HAL_SAI_Init(&haudio_out_sai);

  // Enable SAI peripheral to generate MCLK
  __HAL_SAI_ENABLE(&haudio_out_sai);
  }
//}}}
//{{{
void BSP_AUDIO_OUT_SetAudioFrameSlot (uint32_t AudioFrameSlot) {

  // Disable SAI peripheral to allow access to SAI internal registers
  __HAL_SAI_DISABLE (&haudio_out_sai);

  // Update the SAI audio frame slot configuration
  haudio_out_sai.SlotInit.SlotActive = AudioFrameSlot;
  HAL_SAI_Init (&haudio_out_sai);

  // Enable SAI peripheral to generate MCLK
  __HAL_SAI_ENABLE (&haudio_out_sai);
  }
//}}}

//{{{
__weak void BSP_AUDIO_OUT_MspInit (SAI_HandleTypeDef *hsai, void *Params) {

  static DMA_HandleTypeDef hdma_sai_tx;
  GPIO_InitTypeDef  gpio_init_structure;

  // Enable SAI clock
  AUDIO_OUT_SAIx_CLK_ENABLE();

  // Enable GPIO clock
  AUDIO_OUT_SAIx_MCLK_ENABLE();
  AUDIO_OUT_SAIx_SCK_SD_ENABLE();
  AUDIO_OUT_SAIx_FS_ENABLE();

  // CODEC_SAI pins configuration: FS, SCK, MCK and SD pins
  gpio_init_structure.Pin = AUDIO_OUT_SAIx_FS_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  gpio_init_structure.Alternate = AUDIO_OUT_SAIx_FS_SD_MCLK_AF;
  HAL_GPIO_Init(AUDIO_OUT_SAIx_FS_GPIO_PORT, &gpio_init_structure);

  gpio_init_structure.Pin = AUDIO_OUT_SAIx_SCK_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  gpio_init_structure.Alternate = AUDIO_OUT_SAIx_SCK_AF;
  HAL_GPIO_Init(AUDIO_OUT_SAIx_SCK_SD_GPIO_PORT, &gpio_init_structure);

  gpio_init_structure.Pin =  AUDIO_OUT_SAIx_SD_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  gpio_init_structure.Alternate = AUDIO_OUT_SAIx_FS_SD_MCLK_AF;
  HAL_GPIO_Init(AUDIO_OUT_SAIx_SCK_SD_GPIO_PORT, &gpio_init_structure);

  gpio_init_structure.Pin = AUDIO_OUT_SAIx_MCLK_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  gpio_init_structure.Alternate = AUDIO_OUT_SAIx_FS_SD_MCLK_AF;
  HAL_GPIO_Init(AUDIO_OUT_SAIx_MCLK_GPIO_PORT, &gpio_init_structure);

  // Enable the DMA clock */
  AUDIO_OUT_SAIx_DMAx_CLK_ENABLE();

  if (hsai->Instance == AUDIO_OUT_SAIx) {
    // Configure the hdma_saiTx handle parameters
    hdma_sai_tx.Init.Channel             = AUDIO_OUT_SAIx_DMAx_CHANNEL;
    hdma_sai_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_sai_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_sai_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_sai_tx.Init.PeriphDataAlignment = AUDIO_OUT_SAIx_DMAx_PERIPH_DATA_SIZE;
    hdma_sai_tx.Init.MemDataAlignment    = AUDIO_OUT_SAIx_DMAx_MEM_DATA_SIZE;
    hdma_sai_tx.Init.Mode                = DMA_CIRCULAR;
    hdma_sai_tx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_sai_tx.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    hdma_sai_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_sai_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
    hdma_sai_tx.Init.PeriphBurst         = DMA_PBURST_SINGLE;
    hdma_sai_tx.Instance = AUDIO_OUT_SAIx_DMAx_STREAM;

    // Associate the DMA handle */
    __HAL_LINKDMA (hsai, hdmatx, hdma_sai_tx);
    // Deinitialize the Stream for new transfer
    HAL_DMA_DeInit (&hdma_sai_tx);
    // Configure the DMA Stream
    HAL_DMA_Init (&hdma_sai_tx);
    }

  // SAI DMA IRQ Channel configuration
  HAL_NVIC_SetPriority (AUDIO_OUT_SAIx_DMAx_IRQ, AUDIO_OUT_IRQ_PREPRIO, 0);
  HAL_NVIC_EnableIRQ (AUDIO_OUT_SAIx_DMAx_IRQ);
  }
//}}}
//{{{
__weak void BSP_AUDIO_OUT_MspDeInit (SAI_HandleTypeDef *hsai, void *Params) {

  GPIO_InitTypeDef  gpio_init_structure;

  /* SAI DMA IRQ Channel deactivation */
  HAL_NVIC_DisableIRQ(AUDIO_OUT_SAIx_DMAx_IRQ);

  if(hsai->Instance == AUDIO_OUT_SAIx)
    /* Deinitialize the DMA stream */
    HAL_DMA_DeInit(hsai->hdmatx);

  /* Disable SAI peripheral */
  __HAL_SAI_DISABLE(hsai);

  /* Deactives CODEC_SAI pins FS, SCK, MCK and SD by putting them in input mode */
  gpio_init_structure.Pin = AUDIO_OUT_SAIx_FS_PIN;
  HAL_GPIO_DeInit(AUDIO_OUT_SAIx_FS_GPIO_PORT, gpio_init_structure.Pin);

  gpio_init_structure.Pin = AUDIO_OUT_SAIx_SCK_PIN;
  HAL_GPIO_DeInit(AUDIO_OUT_SAIx_SCK_SD_GPIO_PORT, gpio_init_structure.Pin);

  gpio_init_structure.Pin =  AUDIO_OUT_SAIx_SD_PIN;
  HAL_GPIO_DeInit(AUDIO_OUT_SAIx_SCK_SD_GPIO_PORT, gpio_init_structure.Pin);

  gpio_init_structure.Pin = AUDIO_OUT_SAIx_MCLK_PIN;
  HAL_GPIO_DeInit(AUDIO_OUT_SAIx_MCLK_GPIO_PORT, gpio_init_structure.Pin);

  /* Disable SAI clock */
  AUDIO_OUT_SAIx_CLK_DISABLE();

  /* GPIO pins clock and DMA clock can be shut down in the application
     by surcharging this __weak function */
  }
//}}}
//{{{
__weak void BSP_AUDIO_OUT_ClockConfig (SAI_HandleTypeDef *hsai, uint32_t AudioFreq, void *Params) {

  RCC_PeriphCLKInitTypeDef rcc_ex_clk_init_struct;
  HAL_RCCEx_GetPeriphCLKConfig(&rcc_ex_clk_init_struct);

  // I2S clock config
  //PLLI2S_VCO: VCO_344M
  //I2S_CLK(first level) = PLLI2S_VCO/PLLI2SQ = 344/7 = 49.142 Mhz
  //I2S_CLK_x = I2S_CLK(first level)/PLLI2SDIVQ = 49.142/1 = 49.142 Mhz */
  rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
  rcc_ex_clk_init_struct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
  rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 344;
  rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 7;
  rcc_ex_clk_init_struct.PLLI2SDivQ = 1;

  HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);
  }
//}}}
__weak void BSP_AUDIO_OUT_TransferComplete_CallBack() {}
__weak void BSP_AUDIO_OUT_HalfTransfer_CallBack() {}
__weak void BSP_AUDIO_OUT_Error_CallBack() {}

// in
//{{{
/**
  * @brief  Initializes wave recording.
  * @param  AudioFreq: Audio frequency to be configured for the SAI peripheral.
  * @param  BitRes: Audio frequency to be configured.
  * @param  ChnlNbr: Channel number.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_IN_Init (uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr)
{
  return BSP_AUDIO_IN_InitEx(INPUT_DEVICE_DIGITAL_MICROPHONE_2, AudioFreq, BitRes, ChnlNbr);
}
//}}}
//{{{
/**
  * @brief  Initializes wave recording.
  * @param  InputDevice: INPUT_DEVICE_DIGITAL_MICROPHONE_2 or INPUT_DEVICE_INPUT_LINE_1
  * @param  AudioFreq: Audio frequency to be configured for the SAI peripheral.
  * @param  BitRes: Audio frequency to be configured.
  * @param  ChnlNbr: Channel number.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_IN_InitEx (uint16_t InputDevice, uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr)
{
  uint8_t ret = AUDIO_ERROR;
  uint32_t deviceid = 0x00;
  uint32_t slot_active;

  if ((InputDevice != INPUT_DEVICE_INPUT_LINE_1) &&       /* Only INPUT_LINE_1 and MICROPHONE_2 inputs supported */
      (InputDevice != INPUT_DEVICE_DIGITAL_MICROPHONE_2))
  {
    ret = AUDIO_ERROR;
  }
  else
  {
    /* Disable SAI */
    SAIx_In_DeInit();

    /* PLL clock is set depending on the AudioFreq (44.1khz vs 48khz groups) */
    BSP_AUDIO_OUT_ClockConfig(&haudio_in_sai, AudioFreq, NULL); /* Clock config is shared between AUDIO IN and OUT */

    /* SAI data transfer preparation:
    Prepare the Media to be used for the audio transfer from SAI peripheral to memory */
    haudio_in_sai.Instance = AUDIO_IN_SAIx;
    if(HAL_SAI_GetState(&haudio_in_sai) == HAL_SAI_STATE_RESET)
    {
      /* Init the SAI MSP: this __weak function can be redefined by the application*/
      BSP_AUDIO_OUT_MspInit(&haudio_in_sai, NULL);  /* Initialize GPIOs for SAI2 block A Master signals */
      BSP_AUDIO_IN_MspInit(&haudio_in_sai, NULL);
    }

    /* Configure SAI in master RX mode :
     *   - SAI2_block_A in master RX mode
     *   - SAI2_block_B in slave RX mode synchronous from SAI2_block_A
     */
    if (InputDevice == INPUT_DEVICE_DIGITAL_MICROPHONE_2)
    {
      slot_active = SAI_SLOTACTIVE_1 | SAI_SLOTACTIVE_3;
    }
    else
    {
      slot_active = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_2;
    }
    SAIx_In_Init(SAI_MODEMASTER_RX, slot_active, AudioFreq);

    /* wm8994 codec initialization */
    deviceid = wm8994_drv.ReadID(AUDIO_I2C_ADDRESS);

    if((deviceid) == WM8994_ID)
    {
      /* Reset the Codec Registers */
      wm8994_drv.Reset(AUDIO_I2C_ADDRESS);
      /* Initialize the audio driver structure */
      audio_drv = &wm8994_drv;
      ret = AUDIO_OK;
    }
    else
    {
      ret = AUDIO_ERROR;
    }

    if(ret == AUDIO_OK)
    {
      /* Initialize the codec internal registers */
      audio_drv->Init(AUDIO_I2C_ADDRESS, InputDevice, 100, AudioFreq);
    }
  }
  return ret;
}
//}}}
//{{{
/**
  * @brief  Initializes wave recording and playback in parallel.
  * @param  InputDevice: INPUT_DEVICE_DIGITAL_MICROPHONE_2
  * @param  OutputDevice: OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
  *                       or OUTPUT_DEVICE_BOTH.
  * @param  AudioFreq: Audio frequency to be configured for the SAI peripheral.
  * @param  BitRes: Audio frequency to be configured.
  * @param  ChnlNbr: Channel number.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_IN_OUT_Init (uint16_t InputDevice, uint16_t OutputDevice, uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr)
{
  uint8_t ret = AUDIO_ERROR;
  uint32_t deviceid = 0x00;
  uint32_t slot_active;

  if (InputDevice != INPUT_DEVICE_DIGITAL_MICROPHONE_2)  /* Only MICROPHONE_2 input supported */
  {
    ret = AUDIO_ERROR;
  }
  else
  {
    /* Disable SAI */
    SAIx_In_DeInit();
    SAIx_Out_DeInit();

    /* PLL clock is set depending on the AudioFreq (44.1khz vs 48khz groups) */
    BSP_AUDIO_OUT_ClockConfig(&haudio_in_sai, AudioFreq, NULL); /* Clock config is shared between AUDIO IN and OUT */

    /* SAI data transfer preparation:
    Prepare the Media to be used for the audio transfer from SAI peripheral to memory */
    haudio_in_sai.Instance = AUDIO_IN_SAIx;
    if(HAL_SAI_GetState(&haudio_in_sai) == HAL_SAI_STATE_RESET)
    {
      /* Init the SAI MSP: this __weak function can be redefined by the application*/
      BSP_AUDIO_IN_MspInit(&haudio_in_sai, NULL);
    }

    /* SAI data transfer preparation:
    Prepare the Media to be used for the audio transfer from memory to SAI peripheral */
    haudio_out_sai.Instance = AUDIO_OUT_SAIx;
    if(HAL_SAI_GetState(&haudio_out_sai) == HAL_SAI_STATE_RESET)
    {
      /* Init the SAI MSP: this __weak function can be redefined by the application*/
      BSP_AUDIO_OUT_MspInit(&haudio_out_sai, NULL);
    }

    /* Configure SAI in master mode :
     *   - SAI2_block_A in master TX mode
     *   - SAI2_block_B in slave RX mode synchronous from SAI2_block_A
     */
    if (InputDevice == INPUT_DEVICE_DIGITAL_MICROPHONE_2)
    {
      slot_active = SAI_SLOTACTIVE_1 | SAI_SLOTACTIVE_3;
    }
    else
    {
      slot_active = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_2;
    }
    SAIx_In_Init(SAI_MODEMASTER_TX, slot_active, AudioFreq);

    /* wm8994 codec initialization */
    deviceid = wm8994_drv.ReadID(AUDIO_I2C_ADDRESS);

    if((deviceid) == WM8994_ID)
    {
      /* Reset the Codec Registers */
      wm8994_drv.Reset(AUDIO_I2C_ADDRESS);
      /* Initialize the audio driver structure */
      audio_drv = &wm8994_drv;
      ret = AUDIO_OK;
    }
    else
    {
      ret = AUDIO_ERROR;
    }

    if(ret == AUDIO_OK)
    {
      /* Initialize the codec internal registers */
      audio_drv->Init(AUDIO_I2C_ADDRESS, InputDevice | OutputDevice, 100, AudioFreq);
    }
  }
  return ret;
}
//}}}
//{{{
/**
  * @brief  Deinit the audio IN peripherals.
  * @retval None
  */
void BSP_AUDIO_IN_DeInit()
{
  SAIx_In_DeInit();
  /* DeInit the SAI MSP : this __weak function can be rewritten by the application */
  BSP_AUDIO_IN_MspDeInit(&haudio_in_sai, NULL);
}
//}}}
//{{{
/**
  * @brief  Starts audio recording.
  * @param  pbuf: Main buffer pointer for the recorded data storing
  * @param  size: size of the recorded buffer in number of elements (typically number of half-words)
  *               Be careful that it is not the same unit than BSP_AUDIO_OUT_Play function
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_IN_Record (uint16_t* pbuf, uint32_t size)
{
  uint32_t ret = AUDIO_ERROR;

  /* Start the process receive DMA */
  HAL_SAI_Receive_DMA(&haudio_in_sai, (uint8_t*)pbuf, size);

  /* Return AUDIO_OK when all operations are correctly done */
  ret = AUDIO_OK;

  return ret;
}
//}}}
//{{{
/**
  * @brief  Stops audio recording.
  * @param  Option: could be one of the following parameters
  *           - CODEC_PDWN_SW: for software power off (by writing registers).
  *                            Then no need to reconfigure the Codec after power on.
  *           - CODEC_PDWN_HW: completely shut down the codec (physically).
  *                            Then need to reconfigure the Codec after power on.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_IN_Stop (uint32_t Option)
{
  /* Call the Media layer stop function */
  HAL_SAI_DMAStop(&haudio_in_sai);

  /* Call Audio Codec Stop function */
  if(audio_drv->Stop(AUDIO_I2C_ADDRESS, Option) != 0)
  {
    return AUDIO_ERROR;
  }
  else
  {
    if(Option == CODEC_PDWN_HW)
    {
      /* Wait at least 100us */
      HAL_Delay(1);
    }
    /* Return AUDIO_OK when all operations are correctly done */
    return AUDIO_OK;
  }
}
//}}}
//{{{
/**
  * @brief  Pauses the audio file stream.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_IN_Pause()
{
  /* Call the Media layer pause function */
  HAL_SAI_DMAPause(&haudio_in_sai);
  /* Return AUDIO_OK when all operations are correctly done */
  return AUDIO_OK;
}
//}}}
//{{{
/**
  * @brief  Resumes the audio file stream.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_IN_Resume()
{
  /* Call the Media layer pause/resume function */
  HAL_SAI_DMAResume(&haudio_in_sai);
  /* Return AUDIO_OK when all operations are correctly done */
  return AUDIO_OK;
}
//}}}
//{{{
/**
  * @brief  Controls the audio in volume level.
  * @param  Volume: Volume level in range 0(Mute)..80(+0dB)..100(+17.625dB)
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_IN_SetVolume (uint8_t Volume)
{
  /* Call the codec volume control function with converted volume value */
  if(audio_drv->SetVolume(AUDIO_I2C_ADDRESS, Volume) != 0)
  {
    return AUDIO_ERROR;
  }
  else
  {
    /* Set the Global variable AudioInVolume  */
    AudioInVolume = Volume;
    /* Return AUDIO_OK when all operations are correctly done */
    return AUDIO_OK;
  }
}
//}}}

//{{{
/**
  * @brief  Initializes BSP_AUDIO_IN MSP.
  * @param  hsai: SAI handle
  * @param  Params
  * @retval None
  */
__weak void BSP_AUDIO_IN_MspInit (SAI_HandleTypeDef *hsai, void *Params)
{
  static DMA_HandleTypeDef hdma_sai_rx;
  GPIO_InitTypeDef  gpio_init_structure;

  /* Enable SAI clock */
  AUDIO_IN_SAIx_CLK_ENABLE();

  /* Enable SD GPIO clock */
  AUDIO_IN_SAIx_SD_ENABLE();
  /* CODEC_SAI pin configuration: SD pin */
  gpio_init_structure.Pin = AUDIO_IN_SAIx_SD_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FAST;
  gpio_init_structure.Alternate = AUDIO_IN_SAIx_SD_AF;
  HAL_GPIO_Init(AUDIO_IN_SAIx_SD_GPIO_PORT, &gpio_init_structure);

  /* Enable Audio INT GPIO clock */
  AUDIO_IN_INT_GPIO_ENABLE();
  /* Audio INT pin configuration: input */
  gpio_init_structure.Pin = AUDIO_IN_INT_GPIO_PIN;
  gpio_init_structure.Mode = GPIO_MODE_INPUT;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(AUDIO_IN_INT_GPIO_PORT, &gpio_init_structure);

  /* Enable the DMA clock */
  AUDIO_IN_SAIx_DMAx_CLK_ENABLE();

  if(hsai->Instance == AUDIO_IN_SAIx)
  {
    /* Configure the hdma_sai_rx handle parameters */
    hdma_sai_rx.Init.Channel             = AUDIO_IN_SAIx_DMAx_CHANNEL;
    hdma_sai_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_sai_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_sai_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_sai_rx.Init.PeriphDataAlignment = AUDIO_IN_SAIx_DMAx_PERIPH_DATA_SIZE;
    hdma_sai_rx.Init.MemDataAlignment    = AUDIO_IN_SAIx_DMAx_MEM_DATA_SIZE;
    hdma_sai_rx.Init.Mode                = DMA_CIRCULAR;
    hdma_sai_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_sai_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_sai_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_sai_rx.Init.MemBurst            = DMA_MBURST_SINGLE;
    hdma_sai_rx.Init.PeriphBurst         = DMA_MBURST_SINGLE;

    hdma_sai_rx.Instance = AUDIO_IN_SAIx_DMAx_STREAM;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmarx, hdma_sai_rx);

    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hdma_sai_rx);

    /* Configure the DMA Stream */
    HAL_DMA_Init(&hdma_sai_rx);
  }

  /* SAI DMA IRQ Channel configuration */
  HAL_NVIC_SetPriority(AUDIO_IN_SAIx_DMAx_IRQ, AUDIO_IN_IRQ_PREPRIO, 0);
  HAL_NVIC_EnableIRQ(AUDIO_IN_SAIx_DMAx_IRQ);

  /* Audio INT IRQ Channel configuration */
  HAL_NVIC_SetPriority(AUDIO_IN_INT_IRQ, AUDIO_IN_IRQ_PREPRIO, 0);
  HAL_NVIC_EnableIRQ(AUDIO_IN_INT_IRQ);
}
//}}}
//{{{
/**
  * @brief  DeInitializes BSP_AUDIO_IN MSP.
  * @param  hsai: SAI handle
  * @param  Params
  * @retval None
  */
__weak void BSP_AUDIO_IN_MspDeInit (SAI_HandleTypeDef *hsai, void *Params)
{
  GPIO_InitTypeDef  gpio_init_structure;

  static DMA_HandleTypeDef hdma_sai_rx;

  /* SAI IN DMA IRQ Channel deactivation */
  HAL_NVIC_DisableIRQ(AUDIO_IN_SAIx_DMAx_IRQ);

  if(hsai->Instance == AUDIO_IN_SAIx)
  {
    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hdma_sai_rx);
  }

 /* Disable SAI block */
  __HAL_SAI_DISABLE(hsai);

  /* Disable pin: SD pin */
  gpio_init_structure.Pin = AUDIO_IN_SAIx_SD_PIN;
  HAL_GPIO_DeInit(AUDIO_IN_SAIx_SD_GPIO_PORT, gpio_init_structure.Pin);

  /* Disable SAI clock */
  AUDIO_IN_SAIx_CLK_DISABLE();

  /* GPIO pins clock and DMA clock can be shut down in the application
     by surcharging this __weak function */
}
//}}}
__weak void BSP_AUDIO_IN_TransferComplete_CallBack() {}
__weak void BSP_AUDIO_IN_HalfTransfer_CallBack() {}
__weak void BSP_AUDIO_IN_Error_CallBack() {}
