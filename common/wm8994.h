#pragma once
#include <stdint.h>

#define CODEC_STANDARD  0x04
#define I2S_STANDARD    I2S_STANDARD_PHILIPS

#define WM8994_ID           0x8994
#define WM8994_CHIPID_ADDR  0x00

//{{{  AUDIO_DrvTypeDef
typedef struct {
  uint32_t  (*Init)(uint16_t, uint16_t, uint8_t, uint32_t);
  void      (*DeInit)(void);
  uint32_t  (*ReadID)(uint16_t);
  uint32_t  (*Play)(uint16_t, uint16_t*, uint16_t);
  uint32_t  (*Pause)(uint16_t);
  uint32_t  (*Resume)(uint16_t);
  uint32_t  (*Stop)(uint16_t, uint32_t);
  uint32_t  (*SetFrequency)(uint16_t, uint32_t);
  uint32_t  (*SetVolume)(uint16_t, uint8_t);
  uint32_t  (*SetMute)(uint16_t, uint32_t);
  uint32_t  (*SetOutputMode)(uint16_t, uint8_t);
  uint32_t  (*Reset)(uint16_t);
  } AUDIO_DrvTypeDef;
//}}}

#define OUTPUT_DEVICE_SPEAKER   ((uint16_t)0x0001)
#define OUTPUT_DEVICE_HEADPHONE ((uint16_t)0x0002)
#define OUTPUT_DEVICE_BOTH      ((uint16_t)0x0003)

#define INPUT_DEVICE_DIGITAL_MICROPHONE_1 ((uint16_t)0x0100)
#define INPUT_DEVICE_DIGITAL_MICROPHONE_2 ((uint16_t)0x0200)
#define INPUT_DEVICE_INPUT_LINE_1         ((uint16_t)0x0300)
#define INPUT_DEVICE_INPUT_LINE_2         ((uint16_t)0x0400)
#define INPUT_DEVICE_DIGITAL_MIC1_MIC2    ((uint16_t)0x0800)

#define DEFAULT_VOLMIN  0x00
#define DEFAULT_VOLMAX  0xFF
#define DEFAULT_VOLSTEP 0x04

#define AUDIO_PAUSE     0
#define AUDIO_RESUME    1

#define CODEC_PDWN_HW   1
#define CODEC_PDWN_SW   2

#define AUDIO_MUTE_ON   1
#define AUDIO_MUTE_OFF  0

#define VOLUME_CONVERT(Volume)    (((Volume) > 100)? 100:((uint8_t)(((Volume) * 63) / 100)))
#define VOLUME_IN_CONVERT(Volume) (((Volume) >= 100)? 239:((uint8_t)(((Volume) * 240) / 100)))

void AUDIO_IO_Init();
void AUDIO_IO_DeInit();
void AUDIO_IO_Write (uint8_t Addr, uint16_t Reg, uint16_t Value);
uint8_t AUDIO_IO_Read (uint8_t Addr, uint16_t Reg);
void AUDIO_IO_Delay (uint32_t Delay);

extern AUDIO_DrvTypeDef  wm8994_drv;
