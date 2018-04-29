#pragma once
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}

#include "stm32f7xx_hal.h"

#define SDRAM_DEVICE_ADDR  ((uint32_t)0xC0000000)
#define SDRAM_DEVICE_SIZE  ((uint32_t)0x00800000)  // SDRAM device size in MBytes - 8m

bool BSP_SDRAM_Init();

//{{{
#ifdef __cplusplus
}
#endif
//}}}
