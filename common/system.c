// system.c
//{{{  includes
#include "system.h"
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
//}}}

// common handlers
void NMI_Handler() {}
void SVC_Handler() {}
void PendSV_Handler() {}
void DebugMon_Handler() {}

void BusFault_Handler() { while (1) {} }
//void HardFault_Handler() { while (1) {} }
void MemManage_Handler() { while (1) {} }
void UsageFault_Handler() { while (1) {} }

void SysTick_Handler() { HAL_IncTick(); }

const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
uint32_t SystemCoreClock = 16000000;

//{{{
void SystemInit() {

  SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */

  /* Reset the RCC clock configuration to the default reset state Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x24003010;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;

  SCB->VTOR = FLASH_BASE; /* Vector Table Relocation in Internal FLASH */
  }
//}}}
//{{{
// Update SystemCoreClock variable according to Clock Register Values.
// The SystemCoreClock variable contains the core clock (HCLK), it can
//  be used by the user application to setup the SysTick timer or configure other parameters.
// Each time the core clock (HCLK) changes, this function must be called
// to update SystemCoreClock variable value. Otherwise, any configuration
// based on this variable will be incorrect.
// - The system frequency computed by this function is not the real
//   frequency in the chip. It is calculated based on the predefined
//   constant and the selected clock source:
//    - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
//    - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
//    - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
//      or HSI_VALUE(*) multiplied/divided by the PLL factors.
//  (*) HSI_VALUE is a constant defined in stm32f7xx.h file (default value
//      16 MHz) but the real value may vary depending on the variations
//      in voltage and temperature.
//  (**) HSE_VALUE is a constant defined in stm32f7xx.h file (default value
//       25 MHz), user has to ensure that HSE_VALUE is same as the real
//       frequency of the crystal used. Otherwise, this function may
//       have wrong result.
// - The result of this function could be not correct when using fractional value for HSE crystal.
void SystemCoreClockUpdate() {

  uint32_t pllvco = 0;
  uint32_t pllp = 2;
  uint32_t pllsource = 0;
  uint32_t pllm = 2;

  /* Get SYSCLK source -------------------------------------------------------*/
  uint32_t tmp = RCC->CFGR & RCC_CFGR_SWS;
  switch (tmp) {
    case 0x00:  // HSI used as system clock source
      SystemCoreClock = HSI_VALUE; break;

    case 0x04:  // HSE used as system clock source
      SystemCoreClock = HSE_VALUE;
      break;

    case 0x08:  // PLL used as system clock source
      // PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N SYSCLK = PLL_VCO / PLL_P */
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
      pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
      if (pllsource != 0) // HSE used as PLL clock source
        pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      else // HSI used as PLL clock source
        pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
      SystemCoreClock = pllvco/pllp;
      break;

    default:
      SystemCoreClock = HSI_VALUE;
      break;
    }

  // Compute HCLK frequency, get HCLK prescaler
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  // HCLK frequency
  SystemCoreClock >>= tmp;
  }
//}}}
