// wm8994.c
#include "wm8994.h"

static uint8_t ColdStartup = 1;
static uint32_t outputEnabled = 0;
static uint32_t inputEnabled = 0;

//{{{
static uint32_t SetOutputMode (uint16_t DeviceAddr, uint8_t Output) {

  switch (Output) {
    case OUTPUT_DEVICE_SPEAKER:
      // Enable DAC1 (Left), Enable DAC1 (Right), Disable DAC2 (Left), Disable DAC2 (Right)
      AUDIO_IO_Write (DeviceAddr, 0x05, 0x0C0C);
      // Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path
      AUDIO_IO_Write (DeviceAddr, 0x601, 0x0000);
      // Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path
      AUDIO_IO_Write (DeviceAddr, 0x602, 0x0000);
      // Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path
      AUDIO_IO_Write (DeviceAddr, 0x604, 0x0002);
      // Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path
      AUDIO_IO_Write (DeviceAddr, 0x605, 0x0002);
      break;

    case OUTPUT_DEVICE_HEADPHONE:
      // Disable DAC1 (Left), Disable DAC1 (Right), Enable DAC2 (Left), Enable DAC2 (Right)
      AUDIO_IO_Write (DeviceAddr, 0x05, 0x0303);
      // Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path
      AUDIO_IO_Write (DeviceAddr, 0x601, 0x0001);
      // Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path
      AUDIO_IO_Write (DeviceAddr, 0x602, 0x0001);
      // Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path
      AUDIO_IO_Write (DeviceAddr, 0x604, 0x0000);
      // Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path
      AUDIO_IO_Write (DeviceAddr, 0x605, 0x0000);
      break;

    case OUTPUT_DEVICE_BOTH:
      // Enable DAC1 (Left), Enable DAC1 (Right), also Enable DAC2 (Left), Enable DAC2 (Right)
      AUDIO_IO_Write (DeviceAddr, 0x05, 0x0303 | 0x0C0C);
      // Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path
      AUDIO_IO_Write (DeviceAddr, 0x601, 0x0001);
      // Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path
      AUDIO_IO_Write (DeviceAddr, 0x602, 0x0001);
      // Enable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path
      AUDIO_IO_Write (DeviceAddr, 0x604, 0x0002);
      // Enable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path
      AUDIO_IO_Write (DeviceAddr, 0x605, 0x0002);
      break;
    }

  return 0;
  }
//}}}
//{{{
static uint32_t SetFrequency (uint16_t DeviceAddr, uint32_t AudioFreq) {

  //  Clock Configurations
  switch (AudioFreq) {
    case 48000:
      // AIF1 Sample Rate = 48KHz, ratio = 256
      AUDIO_IO_Write (DeviceAddr, 0x210, 0x0083);
      break;

    case 32000:
      // AIF1 Sample Rate = 32KHz, ratio=256
      AUDIO_IO_Write (DeviceAddr, 0x210, 0x0063);
      break;

    case 16000:
      // AIF1 Sample Rate = 16KHz, ratio=256
      AUDIO_IO_Write (DeviceAddr, 0x210, 0x0033);
      break;

    case 44100:
      // AIF1 Sample Rate = 44.1KHz, ratio=256
      AUDIO_IO_Write (DeviceAddr, 0x210, 0x0073);
      break;
    }

  return 0;
  }
//}}}
//{{{
static uint32_t SetMute (uint16_t DeviceAddr, uint32_t Cmd) {

  if (outputEnabled != 0) {
    if (Cmd == AUDIO_MUTE_ON) {
      // Soft Mute the AIF1 Timeslot 0 DAC1 path L&R
      AUDIO_IO_Write (DeviceAddr, 0x420, 0x0200);
      // Soft Mute the AIF1 Timeslot 1 DAC2 path L&R
      AUDIO_IO_Write (DeviceAddr, 0x422, 0x0200);
      }
    else {
      // Unmute the AIF1 Timeslot 0 DAC1 path L&R
      AUDIO_IO_Write (DeviceAddr, 0x420, 0x0010);
      // Unmute the AIF1 Timeslot 1 DAC2 path L&R
      AUDIO_IO_Write (DeviceAddr, 0x422, 0x0010);
      }
    }

  return 0;
  }
//}}}
//{{{
static uint32_t SetVolume (uint16_t DeviceAddr, uint8_t Volume) {

  uint8_t convertedvol = VOLUME_CONVERT(Volume);

  // Output volume
  if (outputEnabled != 0) {
    if (convertedvol > 0x3E) {
      // Unmute audio codec
      SetMute (DeviceAddr, AUDIO_MUTE_OFF);
      // Left Headphone Volume
      AUDIO_IO_Write (DeviceAddr, 0x1C, 0x3F | 0x140);
      // Right Headphone Volume
      AUDIO_IO_Write (DeviceAddr, 0x1D, 0x3F | 0x140);
      // Left Speaker Volume
      AUDIO_IO_Write (DeviceAddr, 0x26, 0x3F | 0x140);
      // Right Speaker Volume
      AUDIO_IO_Write (DeviceAddr, 0x27, 0x3F | 0x140);
      }
    else if (Volume == 0)
      // Mute audio codec
      SetMute (DeviceAddr, AUDIO_MUTE_ON);
    else {
      // Unmute audio codec
      SetMute (DeviceAddr, AUDIO_MUTE_OFF);
      // Left Headphone Volume
      AUDIO_IO_Write (DeviceAddr, 0x1C, convertedvol | 0x140);
      // Right Headphone Volume
      AUDIO_IO_Write (DeviceAddr, 0x1D, convertedvol | 0x140);
      // Left Speaker Volume
      AUDIO_IO_Write (DeviceAddr, 0x26, convertedvol | 0x140);
      // Right Speaker Volume
      AUDIO_IO_Write (DeviceAddr, 0x27, convertedvol | 0x140);
      }
    }

  // Input volume
  if (inputEnabled != 0) {
    convertedvol = VOLUME_IN_CONVERT (Volume);
    // Left AIF1 ADC1 volume
    AUDIO_IO_Write (DeviceAddr, 0x400, convertedvol | 0x100);
    //Right AIF1 ADC1 volume
    AUDIO_IO_Write (DeviceAddr, 0x401, convertedvol | 0x100);
    // Left AIF1 ADC2 volume
    AUDIO_IO_Write (DeviceAddr, 0x404, convertedvol | 0x100);
    // Right AIF1 ADC2 volume
    AUDIO_IO_Write (DeviceAddr, 0x405, convertedvol | 0x100);
    }

  return 0;
  }
//}}}

//{{{
static uint32_t ReadID (uint16_t DeviceAddr) {

  // initialize the Control interface of the Audio Codec
  AUDIO_IO_Init();
  return ((uint32_t)AUDIO_IO_Read (DeviceAddr, WM8994_CHIPID_ADDR));
  }
//}}}
//{{{
static uint32_t Play (uint16_t DeviceAddr, uint16_t* pBuffer, uint16_t Size) {

  //SetMute (DeviceAddr, AUDIO_MUTE_OFF);
  return 0;
  }
//}}}
//{{{
static uint32_t Pause (uint16_t DeviceAddr) {

  SetMute (DeviceAddr, AUDIO_MUTE_ON);
  AUDIO_IO_Write (DeviceAddr, 0x02, 0x01);
  return 0;
  }
//}}}
//{{{
static uint32_t Resume (uint16_t DeviceAddr) {

  SetMute(DeviceAddr, AUDIO_MUTE_OFF);
  return 0;
  }
//}}}
//{{{
// CODEC_PDWN_SW: only mutes the audio codec. When resuming from this
//                mode the codec keeps the previous initialization
//                (no need to re-Initialize the codec registers).
// CODEC_PDWN_HW: Physically power down the codec. When resuming from this
//                mode, the codec is set to default configuration
//                (user should re-Initialize the codec in order to
//                 play again the audio stream).
static uint32_t Stop (uint16_t DeviceAddr, uint32_t CodecPdwnMode) {

  if (outputEnabled != 0) {
    // Mute the output first
    SetMute(DeviceAddr, AUDIO_MUTE_ON);

    if (CodecPdwnMode == CODEC_PDWN_SW) {
      // Only output mute required
      }
    else { // CODEC_PDWN_HW
      // Mute the AIF1 Timeslot 0 DAC1 path
      AUDIO_IO_Write(DeviceAddr, 0x420, 0x0200);
      // Mute the AIF1 Timeslot 1 DAC2 path
      AUDIO_IO_Write(DeviceAddr, 0x422, 0x0200);
      // Disable DAC1L_TO_HPOUT1L
      AUDIO_IO_Write(DeviceAddr, 0x2D, 0x0000);
      // Disable DAC1R_TO_HPOUT1R
      AUDIO_IO_Write(DeviceAddr, 0x2E, 0x0000);
      // Disable DAC1 and DAC2
      AUDIO_IO_Write(DeviceAddr, 0x05, 0x0000);
      // Reset Codec by writing in 0x0000 address register
      AUDIO_IO_Write(DeviceAddr, 0x0000, 0x0000);
      outputEnabled = 0;
      }
    }

  return 0;
  }
//}}}
//{{{
static uint32_t Reset (uint16_t DeviceAddr) {

  AUDIO_IO_Write (DeviceAddr, 0x0000, 0x0000);
  outputEnabled = 0;
  inputEnabled = 0;
  return 0;
  }
//}}}

//{{{
static uint32_t Init (uint16_t DeviceAddr, uint16_t OutputInputDevice, uint8_t Volume, uint32_t AudioFreq) {

  uint16_t output_device = OutputInputDevice & 0xFF;
  uint16_t input_device = OutputInputDevice & 0xFF00;
  uint16_t power_mgnt_reg_1 = 0;

  // Initialize the Control interface of the Audio Codec
  AUDIO_IO_Init();

  // wm8994 Errata workaounds
  AUDIO_IO_Write (DeviceAddr, 0x102, 0x0003);
  AUDIO_IO_Write (DeviceAddr, 0x817, 0x0000);
  AUDIO_IO_Write (DeviceAddr, 0x102, 0x0000);

  // Enable VMID soft start (fast), Start-up Bias Current Enabled
  AUDIO_IO_Write (DeviceAddr, 0x39, 0x006C);

  // Enable bias generator, Enable VMID
  if (input_device > 0)
    AUDIO_IO_Write (DeviceAddr, 0x01, 0x0013);
  else
    AUDIO_IO_Write (DeviceAddr, 0x01, 0x0003);
  AUDIO_IO_Delay (50);

  if (output_device > 0) {
    outputEnabled = 1;
    SetOutputMode (DeviceAddr, output_device);
    }
  else
    outputEnabled = 0;

  if (input_device > 0) {
    inputEnabled = 1;
    switch (input_device) {
      //{{{
      case INPUT_DEVICE_DIGITAL_MICROPHONE_2 :
        // Enable AIF1ADC2 (Left), Enable AIF1ADC2 (Right)
        // Enable DMICDAT2 (Left), Enable DMICDAT2 (Right)
        // Enable Left ADC, Enable Right ADC
        AUDIO_IO_Write(DeviceAddr, 0x04, 0x0C30);
        // Enable AIF1 DRC2 Signal Detect & DRC in AIF1ADC2 Left/Right Timeslot 1
        AUDIO_IO_Write(DeviceAddr, 0x450, 0x00DB);
        // Disable IN1L, IN1R, IN2L, IN2R, Enable Thermal sensor & shutdown
        AUDIO_IO_Write(DeviceAddr, 0x02, 0x6000);
        // Enable the DMIC2(Left) to AIF1 Timeslot 1 (Left) mixer path
        AUDIO_IO_Write(DeviceAddr, 0x608, 0x0002);
        // Enable the DMIC2(Right) to AIF1 Timeslot 1 (Right) mixer path
        AUDIO_IO_Write(DeviceAddr, 0x609, 0x0002);
        // GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC2 signal detect
        AUDIO_IO_Write(DeviceAddr, 0x700, 0x000E);
        break;
      //}}}
      //{{{
      case INPUT_DEVICE_INPUT_LINE_1 :
        // IN1LN_TO_IN1L, IN1LP_TO_VMID, IN1RN_TO_IN1R, IN1RP_TO_VMID
        AUDIO_IO_Write(DeviceAddr, 0x28, 0x0011);
        // Disable mute on IN1L_TO_MIXINL and +30dB on IN1L PGA output
        AUDIO_IO_Write(DeviceAddr, 0x29, 0x0035);
        // Disable mute on IN1R_TO_MIXINL, Gain = +30dB
        AUDIO_IO_Write(DeviceAddr, 0x2A, 0x0035);
        // Enable AIF1ADC1 (Left), Enable AIF1ADC1 (Right)
        //  Enable Left ADC, Enable Right ADC
        AUDIO_IO_Write(DeviceAddr, 0x04, 0x0303);
        // Enable AIF1 DRC1 Signal Detect & DRC in AIF1ADC1 Left/Right Timeslot 0
        AUDIO_IO_Write(DeviceAddr, 0x440, 0x00DB);
        // Enable IN1L and IN1R, Disable IN2L and IN2R, Enable Thermal sensor & shutdown
        AUDIO_IO_Write(DeviceAddr, 0x02, 0x6350);
        // Enable the ADCL(Left) to AIF1 Timeslot 0 (Left) mixer path
        AUDIO_IO_Write(DeviceAddr, 0x606, 0x0002);
        // Enable the ADCR(Right) to AIF1 Timeslot 0 (Right) mixer path
        AUDIO_IO_Write(DeviceAddr, 0x607, 0x0002);
        // GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC1 signal detect
        AUDIO_IO_Write(DeviceAddr, 0x700, 0x000D);
        break;
      //}}}
      //{{{
      case INPUT_DEVICE_DIGITAL_MICROPHONE_1 :
        // Enable AIF1ADC1 (Left), Enable AIF1ADC1 (Right)
        // Enable DMICDAT1 (Left), Enable DMICDAT1 (Right)
        // Enable Left ADC, Enable Right ADC
        AUDIO_IO_Write(DeviceAddr, 0x04, 0x030C);
        // Enable AIF1 DRC2 Signal Detect & DRC in AIF1ADC1 Left/Right Timeslot 0
        AUDIO_IO_Write(DeviceAddr, 0x440, 0x00DB);
        // Disable IN1L, IN1R, IN2L, IN2R, Enable Thermal sensor & shutdown
        AUDIO_IO_Write(DeviceAddr, 0x02, 0x6350);
        // Enable the DMIC2(Left) to AIF1 Timeslot 0 (Left) mixer path
        AUDIO_IO_Write(DeviceAddr, 0x606, 0x0002);
        // Enable the DMIC2(Right) to AIF1 Timeslot 0 (Right) mixer path
        AUDIO_IO_Write(DeviceAddr, 0x607, 0x0002);
        // GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC1 signal detect
        AUDIO_IO_Write(DeviceAddr, 0x700, 0x000D);
        break;
      //}}}
      //{{{
      case INPUT_DEVICE_DIGITAL_MIC1_MIC2 :
        // Enable AIF1ADC1 (Left), Enable AIF1ADC1 (Right)
        // Enable DMICDAT1 (Left), Enable DMICDAT1 (Right)
        // Enable Left ADC, Enable Right ADC
        AUDIO_IO_Write(DeviceAddr, 0x04, 0x0F3C);
        // Enable AIF1 DRC2 Signal Detect & DRC in AIF1ADC2 Left/Right Timeslot 1
        AUDIO_IO_Write(DeviceAddr, 0x450, 0x00DB);
        // Enable AIF1 DRC2 Signal Detect & DRC in AIF1ADC1 Left/Right Timeslot 0
        AUDIO_IO_Write(DeviceAddr, 0x440, 0x00DB);
        // Disable IN1L, IN1R, Enable IN2L, IN2R, Thermal sensor & shutdown
        AUDIO_IO_Write(DeviceAddr, 0x02, 0x63A0);
        // Enable the DMIC2(Left) to AIF1 Timeslot 0 (Left) mixer path
        AUDIO_IO_Write(DeviceAddr, 0x606, 0x0002);
        // Enable the DMIC2(Right) to AIF1 Timeslot 0 (Right) mixer path
        AUDIO_IO_Write(DeviceAddr, 0x607, 0x0002);
        // Enable the DMIC2(Left) to AIF1 Timeslot 1 (Left) mixer path
        AUDIO_IO_Write(DeviceAddr, 0x608, 0x0002);
        // Enable the DMIC2(Right) to AIF1 Timeslot 1 (Right) mixer path
        AUDIO_IO_Write(DeviceAddr, 0x609, 0x0002);
        // GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC1 signal detect
        AUDIO_IO_Write(DeviceAddr, 0x700, 0x000D);
        break;
      //}}}
      //{{{
      case INPUT_DEVICE_INPUT_LINE_2 :
      default:
        // Actually, no other input devices supported
        break;
      //}}}
      }
    }
  else
    inputEnabled = 0;

  SetFrequency (DeviceAddr, AudioFreq);

  if (input_device == INPUT_DEVICE_DIGITAL_MIC1_MIC2)
    // AIF1 Word Length = 16-bits, AIF1 Format = DSP mode
    AUDIO_IO_Write (DeviceAddr, 0x300, 0x4018);
  else
    // AIF1 Word Length = 16-bits, AIF1 Format = I2S (Default Register Value)
    AUDIO_IO_Write (DeviceAddr, 0x300, 0x4010);

  // slave mode
  AUDIO_IO_Write (DeviceAddr, 0x302, 0x0000);

  // Enable the DSP processing clock for AIF1, Enable the core clock
  AUDIO_IO_Write (DeviceAddr, 0x208, 0x000A);

  // Enable AIF1 Clock, AIF1 Clock Source = MCLK1 pin
  AUDIO_IO_Write (DeviceAddr, 0x200, 0x0001);

  if (output_device > 0) {
    //{{{  Audio output selected
    if (output_device == OUTPUT_DEVICE_HEADPHONE) {
      // Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path
      AUDIO_IO_Write(DeviceAddr, 0x2D, 0x0100);
      // Select DAC1 (Right) to Right Headphone Output PGA (HPOUT1RVOL) path
      AUDIO_IO_Write(DeviceAddr, 0x2E, 0x0100);
      // Startup sequence for Headphone
      if (ColdStartup) {
        AUDIO_IO_Write (DeviceAddr, 0x110, 0x8100);
        ColdStartup=0;
        // Add Delay
        AUDIO_IO_Delay (300);
        }
      else {
        // Headphone Warm Start-Up
        AUDIO_IO_Write (DeviceAddr,0x110,0x8108);
        // Add Delay
        AUDIO_IO_Delay (50);
        }

      // Soft un-Mute the AIF1 Timeslot 0 DAC1 path L&R
      AUDIO_IO_Write(DeviceAddr, 0x420, 0x0000);
      }
    else {
      // Enable SPKRVOL PGA, Enable SPKMIXR, Enable SPKLVOL PGA, Enable SPKMIXL
      AUDIO_IO_Write(DeviceAddr, 0x03, 0x0300);
      // Left Speaker Mixer Volume = 0dB
      AUDIO_IO_Write(DeviceAddr, 0x22, 0x0000);
      // Speaker output mode = Class D, Right Speaker Mixer Volume = 0dB ((0x23, 0x0100) = class AB)
      AUDIO_IO_Write(DeviceAddr, 0x23, 0x0000);
      // Unmute DAC2 (Left) to Left Speaker Mixer (SPKMIXL) path,
      //Unmute DAC2 (Right) to Right Speaker Mixer (SPKMIXR) path
      AUDIO_IO_Write(DeviceAddr, 0x36, 0x0300);
      // Enable bias generator, Enable VMID, Enable SPKOUTL, Enable SPKOUTR
      AUDIO_IO_Write(DeviceAddr, 0x01, 0x3003);

      // Headphone/Speaker Enable
      if (input_device == INPUT_DEVICE_DIGITAL_MIC1_MIC2)
        // Enable Class W, Class W Envelope Tracking = AIF1 Timeslots 0 and 1
        AUDIO_IO_Write(DeviceAddr, 0x51, 0x0205);
      else
        // Enable Class W, Class W Envelope Tracking = AIF1 Timeslot 0
        AUDIO_IO_Write(DeviceAddr, 0x51, 0x0005);

      // Enable bias generator, Enable VMID, Enable HPOUT1 (Left) and Enable HPOUT1 (Right) input stages
      // idem for Speaker
      power_mgnt_reg_1 |= 0x0303 | 0x3003;
      AUDIO_IO_Write(DeviceAddr, 0x01, power_mgnt_reg_1);

      // Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate stages
      AUDIO_IO_Write(DeviceAddr, 0x60, 0x0022);
      // Enable Charge Pump
      AUDIO_IO_Write(DeviceAddr, 0x4C, 0x9F25);
      // Add Delay
      AUDIO_IO_Delay(15);
      // Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path
      AUDIO_IO_Write(DeviceAddr, 0x2D, 0x0001);
      // Select DAC1 (Right) to Right Headphone Output PGA (HPOUT1RVOL) path
      AUDIO_IO_Write(DeviceAddr, 0x2E, 0x0001);
      // Enable Left Output Mixer (MIXOUTL), Enable Right Output Mixer (MIXOUTR)
      // idem for SPKOUTL and SPKOUTR
      AUDIO_IO_Write(DeviceAddr, 0x03, 0x0030 | 0x0300);
      // Enable DC Servo and trigger start-up mode on left and right channels
      AUDIO_IO_Write(DeviceAddr, 0x54, 0x0033);

      // Add Delay
      AUDIO_IO_Delay(257);

      // Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate and output stages. Remove clamps
      AUDIO_IO_Write(DeviceAddr, 0x60, 0x00EE);
      }

    // Unmute DAC 1 (Left)
    AUDIO_IO_Write(DeviceAddr, 0x610, 0x00C0);
    // Unmute DAC 1 (Right)
    AUDIO_IO_Write(DeviceAddr, 0x611, 0x00C0);
    // Unmute the AIF1 Timeslot 0 DAC path
    AUDIO_IO_Write(DeviceAddr, 0x420, 0x0010);
    // Unmute DAC 2 (Left)
    AUDIO_IO_Write(DeviceAddr, 0x612, 0x00C0);
    // Unmute DAC 2 (Right)
    AUDIO_IO_Write(DeviceAddr, 0x613, 0x00C0);
    // Unmute the AIF1 Timeslot 1 DAC2 path
    AUDIO_IO_Write(DeviceAddr, 0x422, 0x0010);
    // Volume Control
    SetVolume(DeviceAddr, Volume);
    }
    //}}}
  if (input_device > 0) {
    //{{{  Audio input selected
    if ((input_device == INPUT_DEVICE_DIGITAL_MICROPHONE_1) || (input_device == INPUT_DEVICE_DIGITAL_MICROPHONE_2)) {
      // Enable Microphone bias 1 generator, Enable VMID
      power_mgnt_reg_1 |= 0x0013;
      AUDIO_IO_Write(DeviceAddr, 0x01, power_mgnt_reg_1);
      // ADC oversample enable
      AUDIO_IO_Write(DeviceAddr, 0x620, 0x0002);
      // AIF ADC2 HPF enable, HPF cut = voice mode 1 fc=127Hz at fs=8kHz
      AUDIO_IO_Write(DeviceAddr, 0x411, 0x3800); }
    else if(input_device == INPUT_DEVICE_DIGITAL_MIC1_MIC2) {
      // Enable Microphone bias 1 generator, Enable VMID
      power_mgnt_reg_1 |= 0x0013;
      AUDIO_IO_Write(DeviceAddr, 0x01, power_mgnt_reg_1);
      // ADC oversample enable
      AUDIO_IO_Write(DeviceAddr, 0x620, 0x0002);
      // AIF ADC1 HPF enable, HPF cut = voice mode 1 fc=127Hz at fs=8kHz
      AUDIO_IO_Write(DeviceAddr, 0x410, 0x1800);
      // AIF ADC2 HPF enable, HPF cut = voice mode 1 fc=127Hz at fs=8kHz
      AUDIO_IO_Write(DeviceAddr, 0x411, 0x1800);
      }
    else if ((input_device == INPUT_DEVICE_INPUT_LINE_1) || (input_device == INPUT_DEVICE_INPUT_LINE_2)) {
      // Disable mute on IN1L, IN1L Volume = +0dB
      AUDIO_IO_Write(DeviceAddr, 0x18, 0x000B);
      // Disable mute on IN1R, IN1R Volume = +0dB
      AUDIO_IO_Write(DeviceAddr, 0x1A, 0x000B);
      // AIF ADC1 HPF enable, HPF cut = hifi mode fc=4Hz at fs=48kHz
      AUDIO_IO_Write(DeviceAddr, 0x410, 0x1800);
      }

    // Volume Control
    SetVolume(DeviceAddr, Volume);
    }
    //}}}

  return 0;
  }
//}}}
//{{{
static void DeInit() {
  AUDIO_IO_DeInit();
  }
//}}}

AUDIO_DrvTypeDef wm8994_drv = {
  Init,
  DeInit,
  ReadID,
  Play,
  Pause,
  Resume,
  Stop,
  SetFrequency,
  SetVolume,
  SetMute,
  SetOutputMode,
  Reset
  };
