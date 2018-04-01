#include "stm32746g_discovery.h"

#define __STM32746G_DISCO_BSP_VERSION_MAIN   (0x02) /*!< [31:24] main version */
#define __STM32746G_DISCO_BSP_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __STM32746G_DISCO_BSP_VERSION_SUB2   (0x02) /*!< [15:8]  sub2 version */
#define __STM32746G_DISCO_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32746G_DISCO_BSP_VERSION         ((__STM32746G_DISCO_BSP_VERSION_MAIN << 24)\
                                             |(__STM32746G_DISCO_BSP_VERSION_SUB1 << 16)\
                                             |(__STM32746G_DISCO_BSP_VERSION_SUB2 << 8 )\
                                             |(__STM32746G_DISCO_BSP_VERSION_RC))

const uint32_t GPIO_PIN[LEDn] = {LED1_PIN};

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {WAKEUP_BUTTON_GPIO_PORT,
                                      TAMPER_BUTTON_GPIO_PORT,
                                      KEY_BUTTON_GPIO_PORT};

const uint16_t BUTTON_PIN[BUTTONn] = {WAKEUP_BUTTON_PIN,
                                      TAMPER_BUTTON_PIN,
                                      KEY_BUTTON_PIN};

const uint16_t BUTTON_IRQn[BUTTONn] = {WAKEUP_BUTTON_EXTI_IRQn,
                                       TAMPER_BUTTON_EXTI_IRQn,
                                       KEY_BUTTON_EXTI_IRQn};

USART_TypeDef* COM_USART[COMn] = {DISCOVERY_COM1};
GPIO_TypeDef* COM_TX_PORT[COMn] = {DISCOVERY_COM1_TX_GPIO_PORT};
GPIO_TypeDef* COM_RX_PORT[COMn] = {DISCOVERY_COM1_RX_GPIO_PORT};
const uint16_t COM_TX_PIN[COMn] = {DISCOVERY_COM1_TX_PIN};
const uint16_t COM_RX_PIN[COMn] = {DISCOVERY_COM1_RX_PIN};
const uint16_t COM_TX_AF[COMn] = {DISCOVERY_COM1_TX_AF};
const uint16_t COM_RX_AF[COMn] = {DISCOVERY_COM1_RX_AF};

static I2C_HandleTypeDef hI2cAudioHandler = {0};
static I2C_HandleTypeDef hI2cExtHandler = {0};

static void I2Cx_MspInit (I2C_HandleTypeDef* i2c_handler);
static void I2Cx_Init (I2C_HandleTypeDef* i2c_handler);
static HAL_StatusTypeDef I2Cx_ReadMultiple (I2C_HandleTypeDef* i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef I2Cx_WriteMultiple (I2C_HandleTypeDef* i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef I2Cx_IsDeviceReady (I2C_HandleTypeDef* i2c_handler, uint16_t DevAddress, uint32_t Trials);
static void I2Cx_Error (I2C_HandleTypeDef* i2c_handler, uint8_t Addr);

//{{{
uint32_t BSP_GetVersion()
{
  return __STM32746G_DISCO_BSP_VERSION;
}
//}}}

//{{{
void BSP_LED_Init (Led_TypeDef Led) {

  if (Led == LED1) {
    GPIO_TypeDef* gpio_led = LED1_GPIO_PORT;
    /* Enable the GPIO_LED clock */
    LED1_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitTypeDef  gpio_init_structure;
    gpio_init_structure.Pin = GPIO_PIN[Led];
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(gpio_led, &gpio_init_structure);

    /* By default, turn off LED */
    HAL_GPIO_WritePin(gpio_led, GPIO_PIN[Led], GPIO_PIN_RESET);
    }
  }
//}}}
//{{{
void BSP_LED_DeInit (Led_TypeDef Led) {


  if (Led == LED1) {
    GPIO_TypeDef* gpio_led = LED1_GPIO_PORT;
    /* Turn off LED */
    HAL_GPIO_WritePin (gpio_led, GPIO_PIN[Led], GPIO_PIN_RESET);

    /* Configure the GPIO_LED pin */
    GPIO_InitTypeDef  gpio_init_structure;
    gpio_init_structure.Pin = GPIO_PIN[Led];
    HAL_GPIO_DeInit(gpio_led, gpio_init_structure.Pin);
    }
  }
//}}}
//{{{
void BSP_LED_On (Led_TypeDef Led) {

  if (Led == LED1)  {
    /* Switch On LED connected to GPIO */
    GPIO_TypeDef* gpio_led = LED1_GPIO_PORT;
    HAL_GPIO_WritePin(gpio_led, GPIO_PIN[Led], GPIO_PIN_SET);
    }
  }
//}}}
//{{{
void BSP_LED_Off (Led_TypeDef Led) {

  if (Led == LED1) {
    /* Switch Off LED connected to GPIO */
    GPIO_TypeDef* gpio_led = LED1_GPIO_PORT;
    HAL_GPIO_WritePin(gpio_led, GPIO_PIN[Led], GPIO_PIN_RESET);
    }
  }
//}}}
//{{{
void BSP_LED_Toggle (Led_TypeDef Led)
{
  GPIO_TypeDef*     gpio_led;

  if (Led == LED1)  /* Toggle LED connected to GPIO */
  {
    gpio_led = LED1_GPIO_PORT;
    HAL_GPIO_TogglePin(gpio_led, GPIO_PIN[Led]);
  }
}
//}}}

//{{{
void BSP_PB_Init (Button_TypeDef Button, ButtonMode_TypeDef ButtonMode) {

  /* Enable the BUTTON clock */
  BUTTONx_GPIO_CLK_ENABLE (Button);

  GPIO_InitTypeDef gpio_init_structure;
  gpio_init_structure.Speed = GPIO_SPEED_FAST;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Pin = BUTTON_PIN[Button];

  if (ButtonMode == BUTTON_MODE_GPIO) {
    /* Configure Button pin as input */
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init (BUTTON_PORT[Button], &gpio_init_structure);
    }

  if (ButtonMode == BUTTON_MODE_EXTI) {
    /* Configure Button pin as input with External interrupt */
    if (Button != BUTTON_WAKEUP)
      gpio_init_structure.Mode = GPIO_MODE_IT_FALLING;
    else
      gpio_init_structure.Mode = GPIO_MODE_IT_RISING;

    HAL_GPIO_Init (BUTTON_PORT[Button], &gpio_init_structure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority ((IRQn_Type)(BUTTON_IRQn[Button]), 0x0F, 0x00);
    HAL_NVIC_EnableIRQ ((IRQn_Type)(BUTTON_IRQn[Button]));
    }
  }
//}}}
//{{{
void BSP_PB_DeInit (Button_TypeDef Button) {

  GPIO_InitTypeDef gpio_init_structure;
  gpio_init_structure.Pin = BUTTON_PIN[Button];
  HAL_NVIC_DisableIRQ ((IRQn_Type)(BUTTON_IRQn[Button]));
  HAL_GPIO_DeInit (BUTTON_PORT[Button], gpio_init_structure.Pin);
  }
//}}}
//{{{
uint32_t BSP_PB_GetState (Button_TypeDef Button) {
  return HAL_GPIO_ReadPin (BUTTON_PORT[Button], BUTTON_PIN[Button]);
  }
//}}}

//{{{
static void I2Cx_MspInit (I2C_HandleTypeDef* i2c_handler) {

  GPIO_InitTypeDef  gpio_init_structure;

  if (i2c_handler == (I2C_HandleTypeDef*)(&hI2cAudioHandler)) {
    // AUDIO and LCD I2C MSP init
    DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_CLK_ENABLE();

    // Configure I2C Tx as alternate function
    gpio_init_structure.Pin = DISCOVERY_AUDIO_I2Cx_SCL_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_OD;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FAST;
    gpio_init_structure.Alternate = DISCOVERY_AUDIO_I2Cx_SCL_SDA_AF;
    HAL_GPIO_Init (DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_PORT, &gpio_init_structure);

    // Configure I2C Rx as alternate function
    gpio_init_structure.Pin = DISCOVERY_AUDIO_I2Cx_SDA_PIN;
    HAL_GPIO_Init (DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_PORT, &gpio_init_structure);

    DISCOVERY_AUDIO_I2Cx_CLK_ENABLE();
    DISCOVERY_AUDIO_I2Cx_FORCE_RESET();
    DISCOVERY_AUDIO_I2Cx_RELEASE_RESET();
    HAL_NVIC_SetPriority (DISCOVERY_AUDIO_I2Cx_EV_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ (DISCOVERY_AUDIO_I2Cx_EV_IRQn);
    HAL_NVIC_SetPriority (DISCOVERY_AUDIO_I2Cx_ER_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ (DISCOVERY_AUDIO_I2Cx_ER_IRQn);
    }
  else {
    //  External, camera and Arduino connector I2C MSP init
    DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_CLK_ENABLE();

    // Configure I2C Tx as alternate function
    gpio_init_structure.Pin = DISCOVERY_EXT_I2Cx_SCL_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_OD;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FAST;
    gpio_init_structure.Alternate = DISCOVERY_EXT_I2Cx_SCL_SDA_AF;
    HAL_GPIO_Init (DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_PORT, &gpio_init_structure);

    // Configure I2C Rx as alternate function
    gpio_init_structure.Pin = DISCOVERY_EXT_I2Cx_SDA_PIN;
    HAL_GPIO_Init (DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_PORT, &gpio_init_structure);

    // Enable I2C clock
    DISCOVERY_EXT_I2Cx_CLK_ENABLE();
    DISCOVERY_EXT_I2Cx_FORCE_RESET();
    DISCOVERY_EXT_I2Cx_RELEASE_RESET();
    HAL_NVIC_SetPriority (DISCOVERY_EXT_I2Cx_EV_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ (DISCOVERY_EXT_I2Cx_EV_IRQn);
    HAL_NVIC_SetPriority (DISCOVERY_EXT_I2Cx_ER_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ (DISCOVERY_EXT_I2Cx_ER_IRQn);
    }
  }
//}}}
//{{{
static void I2Cx_Init (I2C_HandleTypeDef* i2c_handler) {

  if(HAL_I2C_GetState(i2c_handler) == HAL_I2C_STATE_RESET) {
    if (i2c_handler == (I2C_HandleTypeDef*)(&hI2cAudioHandler)) /* Audio and LCD I2C configuration */
      i2c_handler->Instance = DISCOVERY_AUDIO_I2Cx;
    else /* External, camera and Arduino connector  I2C configuration */
      i2c_handler->Instance = DISCOVERY_EXT_I2Cx;

    i2c_handler->Init.Timing           = DISCOVERY_I2Cx_TIMING;
    i2c_handler->Init.OwnAddress1      = 0;
    i2c_handler->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    i2c_handler->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    i2c_handler->Init.OwnAddress2      = 0;
    i2c_handler->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    i2c_handler->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

    /* Init the I2C */
    I2Cx_MspInit (i2c_handler);
    HAL_I2C_Init (i2c_handler);
    }
  }
//}}}
//{{{
static HAL_StatusTypeDef I2Cx_ReadMultiple (I2C_HandleTypeDef *i2c_handler,
                                            uint8_t Addr, uint16_t Reg, uint16_t MemAddress,
                                            uint8_t *Buffer, uint16_t Length) {

  /* Check the communication status */
  HAL_StatusTypeDef status = HAL_I2C_Mem_Read (i2c_handler, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, 1000);
  if (status != HAL_OK) /* I2C error occurred */
    I2Cx_Error (i2c_handler, Addr);

  return status;
  }
//}}}
//{{{
static HAL_StatusTypeDef I2Cx_WriteMultiple (I2C_HandleTypeDef *i2c_handler,
                                             uint8_t Addr, uint16_t Reg, uint16_t MemAddress,
                                             uint8_t *Buffer, uint16_t Length) {

  /* Check the communication status */
  HAL_StatusTypeDef status = HAL_I2C_Mem_Write (i2c_handler, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, 1000);
  if (status != HAL_OK) /* Re-Initiaize the I2C Bus */
    I2Cx_Error (i2c_handler, Addr);

  return status;
  }
//}}}
//{{{
static HAL_StatusTypeDef I2Cx_IsDeviceReady(I2C_HandleTypeDef *i2c_handler, uint16_t DevAddress, uint32_t Trials) {

  return (HAL_I2C_IsDeviceReady (i2c_handler, DevAddress, Trials, 1000));
  }
//}}}
//{{{
static void I2Cx_Error(I2C_HandleTypeDef *i2c_handler, uint8_t Addr) {

  /* De-initialize the I2C communication bus */
  HAL_I2C_DeInit (i2c_handler);

  /* Re-Initialize the I2C communication bus */
  I2Cx_Init (i2c_handler);
  }
//}}}

//{{{
void AUDIO_IO_Init() {
  I2Cx_Init (&hI2cAudioHandler);
  }
//}}}
void AUDIO_IO_DeInit() {}
//{{{
void AUDIO_IO_Write (uint8_t Addr, uint16_t Reg, uint16_t Value) {

  uint16_t tmp = Value;
  Value = ((uint16_t)(tmp >> 8) & 0x00FF);
  Value |= ((uint16_t)(tmp << 8)& 0xFF00);
  I2Cx_WriteMultiple (&hI2cAudioHandler, Addr, Reg, I2C_MEMADD_SIZE_16BIT,(uint8_t*)&Value, 2);
  }
//}}}
//{{{
uint16_t AUDIO_IO_Read (uint8_t Addr, uint16_t Reg) {

  uint16_t read_value = 0, tmp = 0;

  I2Cx_ReadMultiple (&hI2cAudioHandler, Addr, Reg, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&read_value, 2);

  tmp = ((uint16_t)(read_value >> 8) & 0x00FF);
  tmp |= ((uint16_t)(read_value << 8)& 0xFF00);
  read_value = tmp;

  return read_value;
  }
//}}}
//{{{
void AUDIO_IO_Delay (uint32_t Delay) {
  HAL_Delay (Delay);
  }
//}}}

//{{{
void CAMERA_IO_Init() {
  I2Cx_Init (&hI2cExtHandler);
  }
//}}}
//{{{
void CAMERA_IO_Write (uint8_t Addr, uint8_t Reg, uint8_t Value) {

  I2Cx_WriteMultiple (&hI2cExtHandler, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT,(uint8_t*)&Value, 1);
  }
//}}}
//{{{
uint8_t CAMERA_IO_Read (uint8_t Addr, uint8_t Reg) {

  uint8_t read_value = 0;
  I2Cx_ReadMultiple (&hI2cExtHandler, Addr, Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&read_value, 1);
  return read_value;
  }
//}}}
//{{{
void CAMERA_Delay (uint32_t Delay) {
  HAL_Delay (Delay);
  }
//}}}

//{{{
void TS_IO_Init() {
  I2Cx_Init(&hI2cAudioHandler);
  }
//}}}
//{{{
void TS_IO_Write (uint8_t Addr, uint8_t Reg, uint8_t Value) {

  I2Cx_WriteMultiple (&hI2cAudioHandler, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT,(uint8_t*)&Value, 1);
  }
//}}}
//{{{
uint8_t TS_IO_Read (uint8_t Addr, uint8_t Reg) {

  uint8_t read_value = 0;
  I2Cx_ReadMultiple (&hI2cAudioHandler, Addr, Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&read_value, 1);
  return read_value;
  }
//}}}
//{{{
void TS_IO_Delay (uint32_t Delay) {
  HAL_Delay (Delay);
  }
//}}}

//{{{
void BSP_COM_Init (COM_TypeDef COM, UART_HandleTypeDef* huart) {

  GPIO_InitTypeDef gpio_init_structure;

  /* Enable GPIO clock */
  DISCOVERY_COMx_TX_GPIO_CLK_ENABLE(COM);
  DISCOVERY_COMx_RX_GPIO_CLK_ENABLE(COM);

  /* Enable USART clock */
  DISCOVERY_COMx_CLK_ENABLE(COM);

  /* Configure USART Tx as alternate function */
  gpio_init_structure.Pin = COM_TX_PIN[COM];
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Speed = GPIO_SPEED_FAST;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Alternate = COM_TX_AF[COM];
  HAL_GPIO_Init(COM_TX_PORT[COM], &gpio_init_structure);

  /* Configure USART Rx as alternate function */
  gpio_init_structure.Pin = COM_RX_PIN[COM];
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = COM_RX_AF[COM];
  HAL_GPIO_Init(COM_RX_PORT[COM], &gpio_init_structure);

  /* USART configuration */
  huart->Instance = COM_USART[COM];
  HAL_UART_Init(huart);
  }
//}}}
//{{{
void BSP_COM_DeInit (COM_TypeDef COM, UART_HandleTypeDef* huart) {

  /* USART configuration */
  huart->Instance = COM_USART[COM];
  HAL_UART_DeInit(huart);

  /* Enable USART clock */
  DISCOVERY_COMx_CLK_DISABLE(COM);

  /* DeInit GPIO pins can be done in the application
     (by surcharging this __weak function) */

  /* GPIO pins clock, DMA clock can be shut down in the application
     by surcharging this __weak function */
  }
//}}}

//{{{
void EEPROM_IO_Init()
{
  I2Cx_Init(&hI2cExtHandler);
}
//}}}
//{{{
HAL_StatusTypeDef EEPROM_IO_WriteData (uint16_t DevAddress, uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize)
{
  return (I2Cx_WriteMultiple(&hI2cExtHandler, DevAddress, MemAddress, I2C_MEMADD_SIZE_16BIT, pBuffer, BufferSize));
}
//}}}
//{{{
HAL_StatusTypeDef EEPROM_IO_ReadData (uint16_t DevAddress, uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize)
{
  return (I2Cx_ReadMultiple(&hI2cExtHandler, DevAddress, MemAddress, I2C_MEMADD_SIZE_16BIT, pBuffer, BufferSize));
}
//}}}
//{{{
HAL_StatusTypeDef EEPROM_IO_IsDeviceReady (uint16_t DevAddress, uint32_t Trials)
{
  return (I2Cx_IsDeviceReady(&hI2cExtHandler, DevAddress, Trials));
}
//}}}
