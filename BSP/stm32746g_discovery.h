#pragma once
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}
#include "stm32f7xx_hal.h"

//{{{  defines
typedef enum { LED1 = 0, LED_GREEN = LED1 } Led_TypeDef;
typedef enum { BUTTON_WAKEUP = 0, BUTTON_TAMPER = 1, BUTTON_KEY = 2 } Button_TypeDef;
typedef enum { BUTTON_MODE_GPIO = 0, BUTTON_MODE_EXTI = 1 } ButtonMode_TypeDef;
typedef enum { COM1 = 0, COM2 = 1 } COM_TypeDef;

#if !defined (USE_STM32746G_DISCO)
 #define USE_STM32746G_DISCO
#endif

#define LEDn                             ((uint8_t)1)

#define LED1_GPIO_PORT                      GPIOI
#define LED1_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOI_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()             __HAL_RCC_GPIOI_CLK_DISABLE()
#define LED1_PIN                            GPIO_PIN_1

#define BUTTONn                             ((uint8_t)3)

#define WAKEUP_BUTTON_PIN                   GPIO_PIN_11
#define WAKEUP_BUTTON_GPIO_PORT             GPIOI
#define WAKEUP_BUTTON_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOI_CLK_ENABLE()
#define WAKEUP_BUTTON_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOI_CLK_DISABLE()
#define WAKEUP_BUTTON_EXTI_IRQn             EXTI15_10_IRQn

#define TAMPER_BUTTON_PIN                    GPIO_PIN_11
#define TAMPER_BUTTON_GPIO_PORT              GPIOI
#define TAMPER_BUTTON_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOI_CLK_ENABLE()
#define TAMPER_BUTTON_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOI_CLK_DISABLE()
#define TAMPER_BUTTON_EXTI_IRQn              EXTI15_10_IRQn

#define KEY_BUTTON_PIN                       GPIO_PIN_11
#define KEY_BUTTON_GPIO_PORT                 GPIOI
#define KEY_BUTTON_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOI_CLK_ENABLE()
#define KEY_BUTTON_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOI_CLK_DISABLE()
#define KEY_BUTTON_EXTI_IRQn                 EXTI15_10_IRQn

#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == 0) WAKEUP_BUTTON_GPIO_CLK_ENABLE(); else\
                                                   if((__INDEX__) == 1) TAMPER_BUTTON_GPIO_CLK_ENABLE(); else\
                                                KEY_BUTTON_GPIO_CLK_ENABLE(); } while(0)

#define BUTTONx_GPIO_CLK_DISABLE(__INDEX__)    (((__INDEX__) == 0) ? WAKEUP_BUTTON_GPIO_CLK_DISABLE() :\
                                                ((__INDEX__) == 1) ? TAMPER_BUTTON_GPIO_CLK_DISABLE() : KEY_BUTTON_GPIO_CLK_DISABLE())

#define SIGNALn                             ((uint8_t)1)

#define SD_DETECT_PIN                        GPIO_PIN_13
#define SD_DETECT_GPIO_PORT                  GPIOC
#define SD_DETECT_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOC_CLK_ENABLE()
#define SD_DETECT_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOC_CLK_DISABLE()
#define SD_DETECT_EXTI_IRQn                  EXTI15_10_IRQn

#define TS_INT_PIN                           GPIO_PIN_13
#define TS_INT_GPIO_PORT                     GPIOI
#define TS_INT_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOI_CLK_ENABLE()
#define TS_INT_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOI_CLK_DISABLE()
#define TS_INT_EXTI_IRQn                     EXTI15_10_IRQn

#define COMn                             ((uint8_t)1)

#define DISCOVERY_COM1                          USART1
#define DISCOVERY_COM1_CLK_ENABLE()             __HAL_RCC_USART1_CLK_ENABLE()
#define DISCOVERY_COM1_CLK_DISABLE()            __HAL_RCC_USART1_CLK_DISABLE()

#define DISCOVERY_COM1_TX_PIN                   GPIO_PIN_9
#define DISCOVERY_COM1_TX_GPIO_PORT             GPIOA
#define DISCOVERY_COM1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define DISCOVERY_COM1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define DISCOVERY_COM1_TX_AF                    GPIO_AF7_USART1

#define DISCOVERY_COM1_RX_PIN                   GPIO_PIN_7
#define DISCOVERY_COM1_RX_GPIO_PORT             GPIOB
#define DISCOVERY_COM1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define DISCOVERY_COM1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()
#define DISCOVERY_COM1_RX_AF                    GPIO_AF7_USART1

#define DISCOVERY_COM1_IRQn                     USART1_IRQn

#define DISCOVERY_COMx_CLK_ENABLE(__INDEX__)            do { if((__INDEX__) == COM1) DISCOVERY_COM1_CLK_ENABLE(); } while(0)
#define DISCOVERY_COMx_CLK_DISABLE(__INDEX__)           (((__INDEX__) == 0) ? DISCOVERY_COM1_CLK_DISABLE() : 0)

#define DISCOVERY_COMx_TX_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == COM1) DISCOVERY_COM1_TX_GPIO_CLK_ENABLE(); } while(0)
#define DISCOVERY_COMx_TX_GPIO_CLK_DISABLE(__INDEX__)   (((__INDEX__) == 0) ? DISCOVERY_COM1_TX_GPIO_CLK_DISABLE() : 0)

#define DISCOVERY_COMx_RX_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == COM1) DISCOVERY_COM1_RX_GPIO_CLK_ENABLE(); } while(0)
#define DISCOVERY_COMx_RX_GPIO_CLK_DISABLE(__INDEX__)   (((__INDEX__) == 0) ? DISCOVERY_COM1_RX_GPIO_CLK_DISABLE() : 0)

#define LCD_I2C_ADDRESS                  ((uint16_t)0x70)
#define CAMERA_I2C_ADDRESS_OV9655        ((uint16_t)0x60)
#define AUDIO_I2C_ADDRESS                ((uint16_t)0x34)
#define EEPROM_I2C_ADDRESS_A01           ((uint16_t)0xA0)
#define EEPROM_I2C_ADDRESS_A02           ((uint16_t)0xA6)
#define TS_I2C_ADDRESS                   ((uint16_t)0x70)

#ifndef I2C_SPEED
 #define I2C_SPEED                       ((uint32_t)100000)
#endif /* I2C_SPEED */

#define DISCOVERY_AUDIO_I2Cx                             I2C3
#define DISCOVERY_AUDIO_I2Cx_CLK_ENABLE()                __HAL_RCC_I2C3_CLK_ENABLE()
#define DISCOVERY_AUDIO_DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()

#define DISCOVERY_AUDIO_I2Cx_FORCE_RESET()               __HAL_RCC_I2C3_FORCE_RESET()
#define DISCOVERY_AUDIO_I2Cx_RELEASE_RESET()             __HAL_RCC_I2C3_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define DISCOVERY_AUDIO_I2Cx_SCL_PIN                     GPIO_PIN_7
#define DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_PORT           GPIOH
#define DISCOVERY_AUDIO_I2Cx_SCL_SDA_AF                  GPIO_AF4_I2C3
#define DISCOVERY_AUDIO_I2Cx_SDA_PIN                     GPIO_PIN_8

/* I2C interrupt requests */
#define DISCOVERY_AUDIO_I2Cx_EV_IRQn                     I2C3_EV_IRQn
#define DISCOVERY_AUDIO_I2Cx_ER_IRQn                     I2C3_ER_IRQn

/* Definition for external, camera and Arduino connector I2Cx resources */
#define DISCOVERY_EXT_I2Cx                               I2C1
#define DISCOVERY_EXT_I2Cx_CLK_ENABLE()                  __HAL_RCC_I2C1_CLK_ENABLE()
#define DISCOVERY_EXT_DMAx_CLK_ENABLE()                  __HAL_RCC_DMA1_CLK_ENABLE()
#define DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define DISCOVERY_EXT_I2Cx_FORCE_RESET()                 __HAL_RCC_I2C1_FORCE_RESET()
#define DISCOVERY_EXT_I2Cx_RELEASE_RESET()               __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define DISCOVERY_EXT_I2Cx_SCL_PIN                       GPIO_PIN_8
#define DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_PORT             GPIOB
#define DISCOVERY_EXT_I2Cx_SCL_SDA_AF                    GPIO_AF4_I2C1
#define DISCOVERY_EXT_I2Cx_SDA_PIN                       GPIO_PIN_9

/* I2C interrupt requests */
#define DISCOVERY_EXT_I2Cx_EV_IRQn                       I2C1_EV_IRQn
#define DISCOVERY_EXT_I2Cx_ER_IRQn                       I2C1_ER_IRQn

#ifndef DISCOVERY_I2Cx_TIMING
#define DISCOVERY_I2Cx_TIMING                      ((uint32_t)0x40912732)
#endif /* DISCOVERY_I2Cx_TIMING */
//}}}

uint32_t BSP_GetVersion();

void BSP_LED_Init (Led_TypeDef Led);
void BSP_LED_DeInit (Led_TypeDef Led);
void BSP_LED_On (Led_TypeDef Led);
void BSP_LED_Off( Led_TypeDef Led);
void BSP_LED_Toggle( Led_TypeDef Led);

void BSP_PB_Init (Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
void BSP_PB_DeInit (Button_TypeDef Button);
uint32_t BSP_PB_GetState (Button_TypeDef Button);

void BSP_COM_Init (COM_TypeDef COM, UART_HandleTypeDef *husart);
void BSP_COM_DeInit (COM_TypeDef COM, UART_HandleTypeDef *huart);

void CAMERA_IO_Init();
void CAMERA_IO_Write (uint8_t Addr, uint8_t Reg, uint8_t Value);
void CAMERA_IO_Write16 (uint8_t Addr, uint8_t Reg, uint16_t Value);
uint8_t CAMERA_IO_Read (uint8_t Addr, uint8_t Reg);
void CAMERA_Delay (uint32_t Delay);
uint16_t CAMERA_IO_Read16 (uint8_t Addr, uint8_t Reg);

//{{{
#ifdef __cplusplus
}
#endif
//}}}
