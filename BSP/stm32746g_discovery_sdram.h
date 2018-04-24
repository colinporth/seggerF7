#pragma once
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}

#include "stm32f7xx_hal.h"

#define SDRAM_OK           ((uint8_t)0x00)
#define SDRAM_ERROR        ((uint8_t)0x01)

#define SDRAM_DEVICE_ADDR  ((uint32_t)0xC0000000)
#define SDRAM_DEVICE_SIZE  ((uint32_t)0x00800000)  // SDRAM device size in MBytes - 8m

#define SDRAM_MEMORY_WIDTH  FMC_SDRAM_MEM_BUS_WIDTH_16
#define SDCLOCK_PERIOD      FMC_SDRAM_CLOCK_PERIOD_2
#define REFRESH_COUNT       ((uint32_t)0x0603)     // SDRAM refresh counter (100Mhz SD clock)
#define SDRAM_TIMEOUT       ((uint32_t)0xFFFF)

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

uint8_t BSP_SDRAM_Init();

//{{{
#ifdef __cplusplus
}
#endif
//}}}
