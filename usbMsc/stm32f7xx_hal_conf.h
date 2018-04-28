#pragma once
//{{{
#ifdef __cplusplus
  extern "C" {
#endif
//}}}

//{{{  module enabled defines
#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_DMA2D_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_SDRAM_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_I2S_MODULE_ENABLED
#define HAL_LTDC_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_SD_MODULE_ENABLED
#define HAL_SAI_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_PCD_MODULE_ENABLED
#define HAL_ETH_MODULE_ENABLED
//}}}

#define HSE_VALUE            25000000U  // Value of the External oscillator in Hz */
#define HSE_STARTUP_TIMEOUT  100U       // Time out for HSE start up, in ms */
#define HSI_VALUE            16000000U  // Value of the Internal oscillator in Hz*/
#define LSI_VALUE            32000U     // LSI Typical Value in Hz*/
#define LSE_VALUE            32768U     // Value of the External Low Speed oscillator in Hz */
#define LSE_STARTUP_TIMEOUT  5000U      // Time out for LSE start up, in ms */
#define EXTERNAL_CLOCK_VALUE 12288000U  // Value of the Internal oscillator in Hz*/

#define VDD_VALUE                    3300U /*!< Value of VDD in mv */
#define TICK_INT_PRIORITY            0U /*!< tick interrupt priority */
#define USE_RTOS                     0U
#define PREFETCH_ENABLE              1U
#define ART_ACCLERATOR_ENABLE        1U /* To enable instruction cache and prefetch */

//{{{  old ethernet defines
//#define MAC_ADDR0   2U
//#define MAC_ADDR1   0U
//#define MAC_ADDR2   0U
//#define MAC_ADDR3   0U
//#define MAC_ADDR4   0U
//#define MAC_ADDR5   0U

///* Definition of the Ethernet driver buffers size and count */
//#define ETH_RX_BUF_SIZE                ETH_MAX_PACKET_SIZE /* buffer size for receive               */
//#define ETH_TX_BUF_SIZE                ETH_MAX_PACKET_SIZE /* buffer size for transmit              */
//#define ETH_RXBUFNB                    4U       /* 4 Rx buffers of size ETH_RX_BUF_SIZE  */
//#define ETH_TXBUFNB                    4U       /* 4 Tx buffers of size ETH_TX_BUF_SIZE  */

///* DP83848 PHY Address*/
//#define DP83848_PHY_ADDRESS             0x01U
///* PHY Reset delay these values are based on a 1 ms Systick interrupt*/
//#define PHY_RESET_DELAY                 0x000000FFU
///* PHY Configuration delay */
//#define PHY_CONFIG_DELAY                0x00000FFFU

//#define PHY_READ_TO                     0x0000FFFFU
//#define PHY_WRITE_TO                    0x0000FFFFU

//#define PHY_BCR                         ((uint16_t)0x00U)    /*!< Transceiver Basic Control Register   */
//#define PHY_BSR                         ((uint16_t)0x01U)    /*!< Transceiver Basic Status Register    */

//#define PHY_RESET                       ((uint16_t)0x8000U)  /*!< PHY Reset */
//#define PHY_LOOPBACK                    ((uint16_t)0x4000U)  /*!< Select loop-back mode */
//#define PHY_FULLDUPLEX_100M             ((uint16_t)0x2100U)  /*!< Set the full-duplex mode at 100 Mb/s */
//#define PHY_HALFDUPLEX_100M             ((uint16_t)0x2000U)  /*!< Set the half-duplex mode at 100 Mb/s */
//#define PHY_FULLDUPLEX_10M              ((uint16_t)0x0100U)  /*!< Set the full-duplex mode at 10 Mb/s  */
//#define PHY_HALFDUPLEX_10M              ((uint16_t)0x0000U)  /*!< Set the half-duplex mode at 10 Mb/s  */
//#define PHY_AUTONEGOTIATION             ((uint16_t)0x1000U)  /*!< Enable auto-negotiation function     */
//#define PHY_RESTART_AUTONEGOTIATION     ((uint16_t)0x0200U)  /*!< Restart auto-negotiation function    */
//#define PHY_POWERDOWN                   ((uint16_t)0x0800U)  /*!< Select the power down mode           */
//#define PHY_ISOLATE                     ((uint16_t)0x0400U)  /*!< Isolate PHY from MII                 */

//#define PHY_AUTONEGO_COMPLETE           ((uint16_t)0x0020U)  /*!< Auto-Negotiation process completed   */
//#define PHY_LINKED_STATUS               ((uint16_t)0x0004U)  /*!< Valid link established               */
//#define PHY_JABBER_DETECTION            ((uint16_t)0x0002U)  /*!< Jabber condition detected            */

///* Section 4: Extended PHY Registers */
//#define PHY_SR                          ((uint16_t)0x10U)    /*!< PHY status register Offset                      */
//#define PHY_MICR                        ((uint16_t)0x11U)    /*!< MII Interrupt Control Register                  */
//#define PHY_MISR                        ((uint16_t)0x12U)    /*!< MII Interrupt Status and Misc. Control Register */

//#define PHY_LINK_STATUS                 ((uint16_t)0x0001U)  /*!< PHY Link mask                                   */
//#define PHY_SPEED_STATUS                ((uint16_t)0x0002U)  /*!< PHY Speed mask                                  */
//#define PHY_DUPLEX_STATUS               ((uint16_t)0x0004U)  /*!< PHY Duplex mask                                 */

//#define PHY_MICR_INT_EN                 ((uint16_t)0x0002U)  /*!< PHY Enable interrupts                           */
//#define PHY_MICR_INT_OE                 ((uint16_t)0x0001U)  /*!< PHY Enable output interrupt events              */

//#define PHY_MISR_LINK_INT_EN            ((uint16_t)0x0020U)  /*!< Enable Interrupt on change of link status       */
//#define PHY_LINK_INTERRUPT              ((uint16_t)0x2000U)  /*!< PHY link status interrupt mask                  */
//}}}
//{{{  ethernet defines
/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   2U
#define MAC_ADDR1   0U
#define MAC_ADDR2   0U
#define MAC_ADDR3   0U
#define MAC_ADDR4   0U
#define MAC_ADDR5   0U

/* Definition of the Ethernet driver buffers size and count */
#define ETH_RX_BUF_SIZE                ETH_MAX_PACKET_SIZE /* buffer size for receive               */
#define ETH_TX_BUF_SIZE                ETH_MAX_PACKET_SIZE /* buffer size for transmit              */
#define ETH_RXBUFNB                    ((uint32_t)4U)       /* 4 Rx buffers of size ETH_RX_BUF_SIZE  */
#define ETH_TXBUFNB                    ((uint32_t)4U)       /* 4 Tx buffers of size ETH_TX_BUF_SIZE  */

/* Section 2: PHY configuration section */
/* LAN8742A PHY Address*/
#define LAN8742A_PHY_ADDRESS            0x00U
/* PHY Reset delay these values are based on a 1 ms Systick interrupt*/
#define PHY_RESET_DELAY                 ((uint32_t)0x00000FFFU)
/* PHY Configuration delay */
#define PHY_CONFIG_DELAY                ((uint32_t)0x00000FFFU)

#define PHY_READ_TO                     ((uint32_t)0x0000FFFFU)
#define PHY_WRITE_TO                    ((uint32_t)0x0000FFFFU)

/* Section 3: Common PHY Registers */
#define PHY_BCR                         ((uint16_t)0x00U)    /*!< Transceiver Basic Control Register   */
#define PHY_BSR                         ((uint16_t)0x01U)    /*!< Transceiver Basic Status Register    */
#define PHY_RESET                       ((uint16_t)0x8000U)  /*!< PHY Reset */
#define PHY_LOOPBACK                    ((uint16_t)0x4000U)  /*!< Select loop-back mode */
#define PHY_FULLDUPLEX_100M             ((uint16_t)0x2100U)  /*!< Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M             ((uint16_t)0x2000U)  /*!< Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M              ((uint16_t)0x0100U)  /*!< Set the full-duplex mode at 10 Mb/s  */
#define PHY_HALFDUPLEX_10M              ((uint16_t)0x0000U)  /*!< Set the half-duplex mode at 10 Mb/s  */
#define PHY_AUTONEGOTIATION             ((uint16_t)0x1000U)  /*!< Enable auto-negotiation function     */
#define PHY_RESTART_AUTONEGOTIATION     ((uint16_t)0x0200U)  /*!< Restart auto-negotiation function    */
#define PHY_POWERDOWN                   ((uint16_t)0x0800U)  /*!< Select the power down mode           */
#define PHY_ISOLATE                     ((uint16_t)0x0400U)  /*!< Isolate PHY from MII                 */
#define PHY_AUTONEGO_COMPLETE           ((uint16_t)0x0020U)  /*!< Auto-Negotiation process completed   */
#define PHY_LINKED_STATUS               ((uint16_t)0x0004U)  /*!< Valid link established               */
#define PHY_JABBER_DETECTION            ((uint16_t)0x0002U)  /*!< Jabber condition detected            */

/* Section 4: Extended PHY Registers */
#define PHY_SR                          ((uint16_t)0x1FU)    /*!< PHY special control/ status register Offset     */
#define PHY_SPEED_STATUS                ((uint16_t)0x0004U)  /*!< PHY Speed mask                                  */
#define PHY_DUPLEX_STATUS               ((uint16_t)0x0010U)  /*!< PHY Duplex mask                                 */
#define PHY_ISFR                        ((uint16_t)0x1DU)    /*!< PHY Interrupt Source Flag register Offset       */
#define PHY_ISFR_INT4                   ((uint16_t)0x0010U)  /*!< PHY Link down inturrupt                         */
//}}}

#define USE_SPI_CRC                  1U

#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_cortex.h"
#include "stm32f7xx_hal_dma2d.h"
#include "stm32f7xx_hal_eth.h"
#include "stm32f7xx_hal_flash.h"
#include "stm32f7xx_hal_sdram.h"
#include "stm32f7xx_hal_i2c.h"
#include "stm32f7xx_hal_i2s.h"
#include "stm32f7xx_hal_ltdc.h"
#include "stm32f7xx_hal_pwr.h"
#include "stm32f7xx_hal_sai.h"
#include "stm32f7xx_hal_sd.h"
#include "stm32f7xx_hal_spi.h"
#include "stm32f7xx_hal_tim.h"
#include "stm32f7xx_hal_uart.h"
#include "stm32f7xx_hal_usart.h"
#include "stm32f7xx_hal_pcd.h"
#include "stm32f7xx_hal_hcd.h"
#include "stm32f7xx_hal_adc.h"
//{{{  unused
//#include "stm32f7xx_hal_adc.h"
//#include "stm32f7xx_hal_can.h"
//#include "stm32f7xx_hal_can_legacy.h"
//#include "stm32f7xx_hal_cec.h"
//#include "stm32f7xx_hal_crc.h"
//#include "stm32f7xx_hal_cryp.h"
//#include "stm32f7xx_hal_dac.h"
//#include "stm32f7xx_hal_sram.h"
//#include "stm32f7xx_hal_nor.h"
//#include "stm32f7xx_hal_nand.h"
//#include "stm32f7xx_hal_hash.h"
//#include "stm32f7xx_hal_iwdg.h"
//#include "stm32f7xx_hal_lptim.h"
//#include "stm32f7xx_hal_qspi.h"
//#include "stm32f7xx_hal_rng.h"
//#include "stm32f7xx_hal_rtc.h"
//#include "stm32f7xx_hal_spdifrx.h"
//#include "stm32f7xx_hal_irda.h"
//#include "stm32f7xx_hal_smartcard.h"
//#include "stm32f7xx_hal_wwdg.h"
//#include "stm32f7xx_hal_dfsdm.h"
//#include "stm32f7xx_hal_dsi.h"
//#include "stm32f7xx_hal_jpeg.h"
//#include "stm32f7xx_hal_mdios.h"
//#include "stm32f7xx_hal_smbus.h"
//#include "stm32f7xx_hal_mmc.h"
//}}}

#ifdef  USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

//{{{
#ifdef __cplusplus
  }
#endif
//}}}
