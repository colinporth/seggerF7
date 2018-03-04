/*****************************************************************************
 * Copyright (c) 2014 Rowley Associates Limited.                             *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *****************************************************************************/

.macro ISR_HANDLER name=
  .section .vectors, "ax"
  .word \name
  .section .init, "ax"
  .thumb_func
  .weak \name
\name:
1: b 1b /* endless loop */
.endm

.macro ISR_RESERVED
  .section .vectors, "ax"
  .word 0
.endm

  .syntax unified
  .global reset_handler

  .section .vectors, "ax"
  .code 16
  .global _vectors

.macro DEFAULT_ISR_HANDLER name=
  .thumb_func
  .weak \name
\name:
1: b 1b /* endless loop */
.endm

_vectors:
  .word __stack_end__
  .word reset_handler

ISR_HANDLER NMI_Handler
ISR_HANDLER HardFault_Handler
ISR_HANDLER MemManage_Handler // Populate if using MemManage (MPU)
ISR_HANDLER BusFault_Handler // Populate if using Bus fault
ISR_HANDLER UsageFault_Handler // Populate if using Usage fault
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_HANDLER SVC_Handler
ISR_RESERVED // Populate if using a debug monitor
ISR_RESERVED
ISR_HANDLER PendSV_Handler
ISR_HANDLER SysTick_Handler

/* External Interrupts */
ISR_HANDLER     WWDG_IRQHandler                   /* Window WatchDog              */
ISR_HANDLER     PVD_IRQHandler                    /* PVD through EXTI Line detection */
ISR_HANDLER     TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */
ISR_HANDLER     RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */
ISR_HANDLER     FLASH_IRQHandler                  /* FLASH                        */
ISR_HANDLER     RCC_IRQHandler                    /* RCC                          */
ISR_HANDLER     EXTI0_IRQHandler                  /* EXTI Line0                   */
ISR_HANDLER     EXTI1_IRQHandler                  /* EXTI Line1                   */
ISR_HANDLER     EXTI2_IRQHandler                  /* EXTI Line2                   */
ISR_HANDLER     EXTI3_IRQHandler                  /* EXTI Line3                   */
ISR_HANDLER     EXTI4_IRQHandler                  /* EXTI Line4                   */
ISR_HANDLER     DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                */
ISR_HANDLER     DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                */
ISR_HANDLER     DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                */
ISR_HANDLER     DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                */
ISR_HANDLER     DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                */
ISR_HANDLER     DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                */
ISR_HANDLER     DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                */
ISR_HANDLER     ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s         */
ISR_HANDLER     CAN1_TX_IRQHandler                /* CAN1 TX                      */
ISR_HANDLER     CAN1_RX0_IRQHandler               /* CAN1 RX0                     */
ISR_HANDLER     CAN1_RX1_IRQHandler               /* CAN1 RX1                     */
ISR_HANDLER     CAN1_SCE_IRQHandler               /* CAN1 SCE                     */

ISR_HANDLER     EXTI9_5_IRQHandler

ISR_HANDLER     TIM1_BRK_TIM9_IRQHandler          /* TIM1 Break and TIM9          */
ISR_HANDLER     TIM1_UP_TIM10_IRQHandler          /* TIM1 Update and TIM10        */
ISR_HANDLER     TIM1_TRG_COM_TIM11_IRQHandler     /* TIM1 Trigger and Commutation and TIM11 */
ISR_HANDLER     TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */
ISR_HANDLER     TIM2_IRQHandler                   /* TIM2                         */
ISR_HANDLER     TIM3_IRQHandler                   /* TIM3                         */
ISR_HANDLER     TIM4_IRQHandler                   /* TIM4                         */
ISR_HANDLER     I2C1_EV_IRQHandler                /* I2C1 Event                   */
ISR_HANDLER     I2C1_ER_IRQHandler                /* I2C1 Error                   */
ISR_HANDLER     I2C2_EV_IRQHandler                /* I2C2 Event                   */
ISR_HANDLER     I2C2_ER_IRQHandler                /* I2C2 Error                   */
ISR_HANDLER     SPI1_IRQHandler                   /* SPI1                         */
ISR_HANDLER     SPI2_IRQHandler                   /* SPI2                         */
ISR_HANDLER     USART1_IRQHandler                 /* USART1                       */
ISR_HANDLER     USART2_IRQHandler                 /* USART2                       */
ISR_HANDLER     USART3_IRQHandler                 /* USART3                       */
ISR_HANDLER     EXTI15_10_IRQHandler              /* External Line[15:10]s        */
ISR_HANDLER     RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */
ISR_HANDLER     OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI line */
ISR_HANDLER     TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12         */
ISR_HANDLER     TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13        */
ISR_HANDLER     TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
ISR_HANDLER     TIM8_CC_IRQHandler                /* TIM8 Capture Compare         */
ISR_HANDLER     DMA1_Stream7_IRQHandler           /* DMA1 Stream7                 */
ISR_HANDLER     FMC_IRQHandler                    /* FMC                          */
ISR_HANDLER     SDMMC1_IRQHandler                 /* SDMMC1                       */
ISR_HANDLER     TIM5_IRQHandler                   /* TIM5                         */
ISR_HANDLER     SPI3_IRQHandler                   /* SPI3                         */
ISR_HANDLER     UART4_IRQHandler                  /* UART4                        */
ISR_HANDLER     UART5_IRQHandler                  /* UART5                        */
ISR_HANDLER     TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */
ISR_HANDLER     TIM7_IRQHandler                   /* TIM7                         */
ISR_HANDLER     DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                */
ISR_HANDLER     DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                */
ISR_HANDLER     DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                */
ISR_HANDLER     DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                */
ISR_HANDLER     DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                */
ISR_HANDLER     ETH_IRQHandler                    /* Ethernet                     */
ISR_HANDLER     ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */
ISR_HANDLER     CAN2_TX_IRQHandler                /* CAN2 TX                      */
ISR_HANDLER     CAN2_RX0_IRQHandler               /* CAN2 RX0                     */
ISR_HANDLER     CAN2_RX1_IRQHandler               /* CAN2 RX1                     */
ISR_HANDLER     CAN2_SCE_IRQHandler               /* CAN2 SCE                     */
ISR_HANDLER     OTG_FS_IRQHandler                 /* USB OTG FS                   */
ISR_HANDLER     DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                */
ISR_HANDLER     DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                */
ISR_HANDLER     DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                */
ISR_HANDLER     USART6_IRQHandler                 /* USART6                       */
ISR_HANDLER     I2C3_EV_IRQHandler                /* I2C3 event                   */
ISR_HANDLER     I2C3_ER_IRQHandler                /* I2C3 error                   */
ISR_HANDLER     OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out   */
ISR_HANDLER     OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In    */
ISR_HANDLER     OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */
ISR_HANDLER     OTG_HS_IRQHandler                 /* USB OTG HS                   */
ISR_HANDLER     DCMI_IRQHandler                   /* DCMI                         */
ISR_RESERVED                                              /* Reserved                     */
ISR_HANDLER     RNG_IRQHandler                    /* Rng                          */
ISR_HANDLER     FPU_IRQHandler                    /* FPU                          */
ISR_HANDLER     UART7_IRQHandler                  /* UART7                        */
ISR_HANDLER     UART8_IRQHandler                  /* UART8                        */
ISR_HANDLER     SPI4_IRQHandler                   /* SPI4                         */
ISR_HANDLER     SPI5_IRQHandler                   /* SPI5                                 */
ISR_HANDLER     SPI6_IRQHandler                   /* SPI6                                     */
ISR_HANDLER     SAI1_IRQHandler                   /* SAI1                                             */
ISR_HANDLER     LTDC_IRQHandler                   /* LTDC                                         */
ISR_HANDLER     LTDC_ER_IRQHandler                /* LTDC error                                       */
ISR_HANDLER     DMA2D_IRQHandler                  /* DMA2D                                        */
ISR_HANDLER     SAI2_IRQHandler                   /* SAI2                         */
ISR_HANDLER     QUADSPI_IRQHandler                /* QUADSPI                      */
ISR_HANDLER     LPTIM1_IRQHandler                 /* LPTIM1                       */
ISR_HANDLER     CEC_IRQHandler                    /* HDMI_CEC                     */
ISR_HANDLER     I2C4_EV_IRQHandler                /* I2C4 Event                   */
ISR_HANDLER     I2C4_ER_IRQHandler                /* I2C4 Error                   */
ISR_HANDLER     SPDIF_RX_IRQHandler               /* SPDIF_RX                     */

  .section .vectors, "ax"
_vectors_end:

  .section .init, "ax"
  .thumb_func

  reset_handler:

#ifndef __NO_SYSTEM_INIT
  ldr r0, =__SRAM_segment_end__
  mov sp, r0
  bl SystemInit
#endif

#if !defined(__SOFTFP__)
  // Enable CP11 and CP10 with CPACR |= (0xf<<20)
  movw r0, 0xED88
  movt r0, 0xE000
  ldr r1, [r0]
  orrs r1, r1, #(0xf << 20)
  str r1, [r0]
#endif

  b _start

#ifndef __NO_SYSTEM_INIT
  .thumb_func
  .weak SystemInit
SystemInit:
  bx lr
#endif
