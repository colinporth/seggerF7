#pragma once
#include "stm32746g_discovery.h"
#include "wm8994.h"
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}

#define OUTPUT_DEVICE_HEADPHONE1 OUTPUT_DEVICE_HEADPHONE
#define OUTPUT_DEVICE_HEADPHONE2 OUTPUT_DEVICE_SPEAKER

extern __IO uint16_t AudioInVolume;

uint8_t BSP_AUDIO_OUT_Init (uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq);
uint8_t BSP_AUDIO_OUT_Play (uint16_t* pBuffer, uint32_t Size);
void BSP_AUDIO_OUT_ChangeBuffer (uint16_t *pData, uint16_t Size);
uint8_t BSP_AUDIO_OUT_Pause();
uint8_t BSP_AUDIO_OUT_Resume();
uint8_t BSP_AUDIO_OUT_Stop (uint32_t Option);
uint8_t BSP_AUDIO_OUT_SetVolume (uint8_t Volume);
void BSP_AUDIO_OUT_SetFrequency (uint32_t AudioFreq);
void BSP_AUDIO_OUT_SetAudioFrameSlot (uint32_t AudioFrameSlot);
uint8_t BSP_AUDIO_OUT_SetMute (uint32_t Cmd);
uint8_t BSP_AUDIO_OUT_SetOutputMode (uint8_t Output);
void BSP_AUDIO_OUT_DeInit();

void BSP_AUDIO_OUT_TransferComplete_CallBack();
void BSP_AUDIO_OUT_HalfTransfer_CallBack();
void BSP_AUDIO_OUT_Error_CallBack();

void BSP_AUDIO_OUT_ClockConfig (SAI_HandleTypeDef *hsai, uint32_t AudioFreq, void *Params);
void BSP_AUDIO_OUT_MspInit (SAI_HandleTypeDef *hsai, void *Params);
void BSP_AUDIO_OUT_MspDeInit (SAI_HandleTypeDef *hsai, void *Params);

uint8_t BSP_AUDIO_IN_Init (uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr);
uint8_t BSP_AUDIO_IN_InitEx (uint16_t InputDevice, uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr);
uint8_t BSP_AUDIO_IN_OUT_Init (uint16_t InputDevice, uint16_t OutputDevice, uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr);
uint8_t BSP_AUDIO_IN_Record (uint16_t *pData, uint32_t Size);
uint8_t BSP_AUDIO_IN_Stop (uint32_t Option);
uint8_t BSP_AUDIO_IN_Pause();
uint8_t BSP_AUDIO_IN_Resume();
uint8_t BSP_AUDIO_IN_SetVolume (uint8_t Volume);
void BSP_AUDIO_IN_DeInit();
void BSP_AUDIO_IN_TransferComplete_CallBack();
void BSP_AUDIO_IN_HalfTransfer_CallBack();
void BSP_AUDIO_IN_Error_CallBack();

void BSP_AUDIO_IN_MspInit (SAI_HandleTypeDef *hsai, void *Params);
void BSP_AUDIO_IN_MspDeInit (SAI_HandleTypeDef *hsai, void *Params);

//{{{
#ifdef __cplusplus
  }
#endif
//}}}
