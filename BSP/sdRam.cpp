// sdram.c
#include "sdRam.h"
//{{{  defines
#define SDRAM_MEMORY_WIDTH  FMC_SDRAM_MEM_BUS_WIDTH_16
#define SDCLOCK_PERIOD      FMC_SDRAM_CLOCK_PERIOD_2
#define REFRESH_COUNT       ((uint32_t)0x0603)     // SDRAM refresh counter (100Mhz SD clock)

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
//}}}

bool BSP_SDRAM_Init() {

  //{{{  config clocks
  __HAL_RCC_FMC_CLK_ENABLE();

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  //}}}
  //{{{  config GPIO
  // Common GPIO configuration
  GPIO_InitTypeDef gpio_init_structure;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FAST;
  gpio_init_structure.Alternate = GPIO_AF12_FMC;

  // GPIOC configuration
  gpio_init_structure.Pin = GPIO_PIN_3;
  HAL_GPIO_Init (GPIOC, &gpio_init_structure);

  // GPIOD configuration
  gpio_init_structure.Pin = GPIO_PIN_0  | GPIO_PIN_1  |
                            GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                            GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init (GPIOD, &gpio_init_structure);

  // GPIOE configuration
  gpio_init_structure.Pin = GPIO_PIN_0  | GPIO_PIN_1  |
                            GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 |
                            GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init (GPIOE, &gpio_init_structure);

  // GPIOF configuration
  gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4  |
                            GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init (GPIOF, &gpio_init_structure);

  // GPIOG configuration
  gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 |
                            GPIO_PIN_4| GPIO_PIN_5 |
                            GPIO_PIN_8 |
                            GPIO_PIN_15;
  HAL_GPIO_Init (GPIOG, &gpio_init_structure);

  // GPIOH configuration
  gpio_init_structure.Pin = GPIO_PIN_3 | GPIO_PIN_5;
  HAL_GPIO_Init (GPIOH, &gpio_init_structure);
  //}}}

  SDRAM_HandleTypeDef sdramHandle;
  sdramHandle.Instance = FMC_SDRAM_DEVICE;
  sdramHandle.Init.SDBank = FMC_SDRAM_BANK1;
  sdramHandle.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  sdramHandle.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  sdramHandle.Init.MemoryDataWidth = SDRAM_MEMORY_WIDTH;
  sdramHandle.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  sdramHandle.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
  sdramHandle.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  sdramHandle.Init.SDClockPeriod = SDCLOCK_PERIOD;
  sdramHandle.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  sdramHandle.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  FMC_SDRAM_Init (FMC_SDRAM_DEVICE, &(sdramHandle.Init));

  // configuration for 100Mhz as SD clock frequency (System clock is up to 200Mhz)
  FMC_SDRAM_TimingTypeDef Timing;
  Timing.LoadToActiveDelay    = 2;
  Timing.ExitSelfRefreshDelay = 7;
  Timing.SelfRefreshTime      = 4;
  Timing.RowCycleDelay        = 7;
  Timing.WriteRecoveryTime    = 2;
  Timing.RPDelay              = 2;
  Timing.RCDDelay             = 2;
  FMC_SDRAM_Timing_Init (FMC_SDRAM_DEVICE, &Timing, FMC_SDRAM_BANK1);

  //{{{  config a clock configuration enable command
  FMC_SDRAM_CommandTypeDef Command;
  Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = 0;
  FMC_SDRAM_SendCommand (FMC_SDRAM_DEVICE, &Command, 0xFFFF);
  //}}}
  HAL_Delay (1);  // 100us minimum delay

  //{{{  config a PALL (precharge all) command
  Command.CommandMode = FMC_SDRAM_CMD_PALL;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = 0;
  FMC_SDRAM_SendCommand (FMC_SDRAM_DEVICE, &Command, 0xFFFF);
  //}}}
  //{{{  config an Auto Refresh command
  Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber = 8;
  Command.ModeRegisterDefinition = 0;
  FMC_SDRAM_SendCommand (FMC_SDRAM_DEVICE, &Command, 0xFFFF);
  //}}}
  //{{{  config external memory mode register
  Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = SDRAM_MODEREG_BURST_LENGTH_8          |
                                   SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                                   SDRAM_MODEREG_CAS_LATENCY_2           |
                                   SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                                   SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  FMC_SDRAM_SendCommand (FMC_SDRAM_DEVICE, &Command, 0xFFFF);
  //}}}
  FMC_SDRAM_ProgramRefreshRate (FMC_SDRAM_DEVICE ,REFRESH_COUNT);

  return true;
  }
