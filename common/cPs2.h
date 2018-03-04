// cPs2.h
// keyboard brown  - touchpad red    - V - +5v
// keyboard yellow - touchpad yellow - C - A3 - PF8 - clock
// keyboard red    - touchpad green  - O - A2 - PF9 - data
// keyboard black  - touchpad black  - G - ground
#pragma once
//{{{  includes
#include "system.h"
#include "cLcd.h"
//}}}

class cPs2 {
public:
  //{{{
  cPs2 (cLcd* lcd) : mLcd(lcd) {

    initGpio();
    mModBits = 0;
    mRxExpCode = false;
    mRxReleaseCode = false;
    mPauseCode = false;
    }
  //}}}

  //{{{
  void initKeyboard() {

    sendChar (0xFF);
    if (getRawChar() != 0xAA)
      mLcd->debug (LCD_COLOR_RED, "initPs2keyboard - missing 0xAA reset");
    resetCode();

    // send getId
    sendChar (0xF2);
    mLcd->debug (LCD_COLOR_YELLOW, "keyboard id %x %x", getRawChar(), getRawChar());

    resetChar();
    }
  //}}}
  //{{{
  void initTouchpad() {

    // reset
    sendChar (0xFF);

    // reset reply
    auto reply = getRawChar();
    if (reply != 0xAA)
      mLcd->debug (LCD_COLOR_RED, "initTouchpad - no 0xAA reply", reply);

    reply = getRawChar();
    if (reply != 0x00)
      mLcd->debug (LCD_COLOR_RED, "initTouchpad - missing 0x00 reset");
    mLcd->debug (LCD_COLOR_YELLOW, "initTouchpad - reset");

    sendTouchpadSpecialCommand (0x00);
    sendChar (0xE9); // touchpad statusRequest prefixed by specialCommand
    auto minor = getRawChar();
    getRawChar();  // 0x47
    auto major = getRawChar();
    mLcd->debug (LCD_COLOR_YELLOW, "Identify %x.%x.%x", major & 0x0F, minor, major >> 4);

    sendTouchpadSpecialCommand (0x02);
    sendChar (0xE9); // touchpad statusRequest prefixed by specialCommand
    auto capMsb = getRawChar();
    auto mid = getRawChar();  // 0x47
    auto capLsb = getRawChar();
    mLcd->debug (LCD_COLOR_YELLOW, "Capabilities %x %x", mid, (capMsb << 8) | capLsb);

    sendTouchpadSpecialCommand (0x03);
    sendChar (0xE9); // touchpad statusRequest prefixed by specialCommand
    auto modelId1 = getRawChar();
    auto modelId2 = getRawChar();
    auto modelId3 = getRawChar();
    mLcd->debug (LCD_COLOR_YELLOW, "ModelId %x %x %x", modelId1, modelId2, modelId3);

    sendTouchpadSpecialCommand (0x80);
    sendChar (0xF3); // touchpad setSampleRate prefixed by specialCommand
    sendChar (0x14); // - setSampleRate = 20
    sendChar (0xF4); // touchpad enable streaming

    mStream = true;
    }
  //}}}

  bool hasChar() { return mInPtr != mOutPtr; }
  bool hasRawChar() { return mInRawPtr != mOutRawPtr; }
  //{{{
  uint16_t getChar() {

    while (!hasChar()) { HAL_Delay (1); }
    uint16_t ch = mRxData[mOutPtr];
    mOutPtr = (mOutPtr + 1) % kMaxRing;
    return ch;
    }
  //}}}
  //{{{
  uint16_t getRawChar() {

    while (!hasRawChar()) { HAL_Delay (1); }
    uint16_t ch = mRxRawData[mOutRawPtr];
    mOutRawPtr = (mOutRawPtr + 1) % kMaxRing;
    return ch;
    }
  //}}}
  //{{{
  bool getTouch (int& touch, int& x, int& y, int& z) {

    touch = mTouchZ > 0;
    x = mTouchX;
    y = mTouchY;
    z = mTouchZ;
    return mTouchZ > 0;
    }
  //}}}

  //{{{
  void sendLeds (int i) {

    sendChar (0xED);
    sendChar (i);
    }
  //}}}
  //{{{
  void resetChar() {

    mInPtr = 0;
    mOutPtr = 0;

    mInRawPtr = 0;
    mOutRawPtr = 0;

    mCurCode = 0;
    mCurChar = 0;

    mModBits = 0;
    mRxExpCode = false;
    mRxReleaseCode = false;
    mPauseCode = false;
    }
  //}}}
  //{{{
  void resetCode() {

    mInPtr = 0;
    mOutPtr = 0;
    mModBits = 0;
    }
  //}}}

  //{{{
  void showTouch() {

    BSP_LCD_SetTextColor (LCD_COLOR_YELLOW);

    char str[8];
    sprintf (str, "%d", mTouchX);
    BSP_LCD_DisplayStringAtLineColumn (13, 0, str);

    sprintf (str, "%d", mTouchY);
    BSP_LCD_DisplayStringAtLineColumn (13, 6, str);

    sprintf (str, "%d", mTouchZ);
    BSP_LCD_DisplayStringAtLineColumn (13, 12, str);

    sprintf (str, "%d", mStreamCode);
    BSP_LCD_DisplayStringAtLineColumn (13, 18, str);

    sprintf (str, "%x", mStreamCodes[0]);
    BSP_LCD_DisplayStringAtLineColumn (13, 22, str);

    sprintf (str, "%x", mStreamCodes[1]);
    BSP_LCD_DisplayStringAtLineColumn (13, 25, str);

    sprintf (str, "%x", mStreamCodes[2]);
    BSP_LCD_DisplayStringAtLineColumn (13, 28, str);

    sprintf (str, "%x", mStreamCodes[3]);
    BSP_LCD_DisplayStringAtLineColumn (13, 31, str);

    sprintf (str, "%x", mStreamCodes[4]);
    BSP_LCD_DisplayStringAtLineColumn (13, 34, str);

    sprintf (str, "%x", mStreamCodes[5]);
    BSP_LCD_DisplayStringAtLineColumn (13, 37, str);
    }
  //}}}
  //{{{
  void showChars() {

    auto ch = mCurChar - kMaxChars + 1;
    for (auto i = 0u; i < kMaxChars; i++) {
      if (ch > 0) {
        char str[2];
        BSP_LCD_SetTextColor (mChars[ch % kMaxChars] & 0x100 ? LCD_COLOR_GREEN : LCD_COLOR_YELLOW);
        str[0] = mChars[ch % kMaxChars] & 0xFF;
        str[1] = 0;
        BSP_LCD_DisplayStringAtLineColumn (13, i, str);
        }
      ch++;
      }
    }
  //}}}
  //{{{
  void showCodes() {

    BSP_LCD_SetTextColor (LCD_COLOR_YELLOW);

    auto code = mCurCode - kMaxCodes + 1;
    for (auto i = 0u; i < kMaxCodes; i++) {
      if (code > 0) {
        char str[4];
        sprintf (str, "%02x", mCodes[code % kMaxCodes]);
        BSP_LCD_DisplayStringAtLineColumn (14, i*3, str);
        }
      code++;
      }
    }
  //}}}
  //{{{
  void showWave() {

    int bitHeight = 12;
    int clockHeight = 12;
    int lineHeight = 16;
    auto waveY = BSP_LCD_GetYSize() - 2*lineHeight;

    bool lastBit = false;
    auto sample = mSample - kMaxSamples + 1;
    for (auto i = 0u; i < kMaxSamples; i++) {
      if (sample > 0) {
        bool bit =  mBitArray[sample % kMaxSamples];
        int bitPos = mBitPosArray[sample % kMaxSamples];
        sample++;

        if (bit != lastBit) {
          // draw edge
          BSP_LCD_SetTextColor (bitPos == -1 ? LCD_COLOR_RED : LCD_COLOR_WHITE);
          BSP_LCD_FillRect (i*kBitWidth, waveY, 1, bitHeight);
          lastBit = bit;
          }

        // draw bit
        BSP_LCD_SetTextColor ((bitPos == -1) || (bitPos == 9) ? LCD_COLOR_RED : LCD_COLOR_WHITE);
        BSP_LCD_FillRect (i*kBitWidth, waveY + (bit ? 0 : bitHeight-2), kBitWidth, 2);
        }

      BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
      BSP_LCD_FillRect (i*kBitWidth + (kBitWidth/4), waveY + lineHeight, 1, clockHeight);
      BSP_LCD_FillRect (i*kBitWidth + (kBitWidth/4), waveY + lineHeight , kBitWidth/2, 1);
      BSP_LCD_FillRect (i*kBitWidth + (kBitWidth*3/4), waveY + lineHeight, 1, clockHeight);
      BSP_LCD_FillRect (i*kBitWidth + (kBitWidth*3/4), waveY + lineHeight + clockHeight, kBitWidth/2, 1);
      }
    }
  //}}}

  //{{{
  void onIrq() {

    if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_8) != RESET) {
      __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_8);

      if (!mTx) {
        bool bit = (GPIOF->IDR & GPIO_PIN_9) != 0;

        mSample++;
        mBitArray[mSample % kMaxSamples] = bit;
        mBitPosArray[mSample % kMaxSamples] = mBitPos;

        switch (mBitPos) {
          case -1:
            //{{{  wait for lo start bit
            if (!bit) { // lo start bit
              mBitPos = 0;
              mCode = 0;
              }
            break;
            //}}}

          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7:
            mCode = mCode | (bit << mBitPos);
            mBitPos++;
            break;

          case 8: // parity bit
            // add to codes debug buffer
            mBitPos++;
            mCurCode++;
            mCodes[mCurCode % kMaxCodes] = mCode;
            if (mStream) {
              //{{{  add to touchpad stream
              if (mStreamCode == -1) {
                if ((mCode & 0xC0) == 0x80) {
                  mStreamCode = 0;
                  mStreamCodes[mStreamCode] = mCode;
                  }
                }

              else {
                mStreamCode++;
                if ((mStreamCode == 3) && ((mCode & 0xc0) != 0xc0))
                  mStreamCode = -1;
                else {
                  mStreamCodes[mStreamCode] = mCode;
                  if (mStreamCode == 5) {
                    mTouchX = ((mStreamCodes[3] & 0x10) << 8) | ((mStreamCodes[1] & 0x0F) << 8) | mStreamCodes[4];
                    mTouchY = ((mStreamCodes[3] & 0x20) << 7) | ((mStreamCodes[1] & 0xF0) << 4) | mStreamCodes[5];
                    mTouchZ = mStreamCodes[2];
                    mStreamCode = -1;
                    }
                  }
                }
              }
              //}}}
            else {
              //{{{
              // add char code
              mRxRawData[mInRawPtr] = mCode;
              mInRawPtr = (mInRawPtr + 1) % kMaxRing;

              if (mCode == 0xE0)
                mRxExpCode = true;
              else if (mCode == 0xE1)
                mPauseCode = true;
              else if (mCode == 0xF0)
                mRxReleaseCode = true;
              else {
                auto hidCode = mRxExpCode ? kPs2Key[mCode].mExpHidCode : kPs2Key[mCode].mHidCode;
                auto modBits = mRxExpCode ? kPs2Key[mCode].mExpModBits : kPs2Key[mCode].mModBits;

                if (modBits) {
                  if (mRxReleaseCode)
                    mModBits &= ~modBits;
                  else
                    mModBits |= modBits;
                  mRxReleaseCode = false;
                  }
                else {
                  if (!mRxReleaseCode) {
                    auto asciiChar = (mModBits & (kLeftShift + kRightShift)) ?
                                       kPs2Key[mCode].mAsciiShiftCode : kPs2Key[mCode].mAsciiCode;
                    if (asciiChar) {
                      // add to decoded chars debug buffer
                      mCurChar++;
                      mChars[mCurChar % kMaxChars] = asciiChar;
                      }
                    }
                  }

                // add to ring buffer
                mRxData[mInPtr] = (mModBits << 8) | (mRxReleaseCode ? 0 : hidCode);
                mInPtr = (mInPtr + 1) % kMaxRing;

                mRxExpCode = false;
                mRxReleaseCode = false;
                mPauseCode = false;
                }
              }
              //}}}
            break;

          case 9:
            //{{{  hi stop bit
            mBitPos = -1;
            if (!bit)
              mLcd->debug (LCD_COLOR_RED, "lo stop bit");
            break;
            //}}}
          }
        }
      }
    }
  //}}}

private:
  //{{{  struct tHidLookup
  const uint8_t kLeftControl  = 0x01;
  const uint8_t kLeftShift    = 0x02;
  const uint8_t kLeftAlt      = 0x04;
  const uint8_t kLeftMeta     = 0x08;
  const uint8_t kRightControl = 0x10;
  const uint8_t kRightShift   = 0x20;
  const uint8_t kRightAlt     = 0x40;
  const uint8_t kRightMeta    = 0x80;

  typedef struct tHidLookup {
    uint8_t mHidCode;
    uint8_t mModBits;
    uint8_t mExpHidCode;
    uint8_t mExpModBits;
    uint8_t mAsciiCode;
    uint8_t mAsciiShiftCode;
    } tHidLookup;

  const tHidLookup kPs2Key[0x84] = {
   //{{{  0x00
   {0,0,    0,0, 0,0},
   {0x42,0, 0,0, 0,0},  // F9
   {0,0,    0,0, 0,0},
   {0x3e,0, 0,0, 0,0},  // F5

   // 0x04
   {0x3c,0, 0,0, 0,0},  // F3
   {0x3a,0, 0,0, 0,0},  // F1
   {0x3b,0, 0,0, 0,0},  // F2
   {0x45,0, 0,0, 0,0},  // F12

   // 0x08
   {0,0,    0,0, 0,0},
   {0x43,0, 0,0, 0,0},  // F10
   {0x41,0, 0,0, 0,0},  // F8
   {0x3f,0, 0,0, 0,0},  // F6

   // 0x0c
   {0x3d,0, 0,0, 0,0},        // F4
   {0x2b,0, 0,0, 0x08,0x08},  // tab
   {0x35,0, 0,0, '`','~'},    // ` ~
   {0,0,    0,0, 0,0},
   //}}}
   //{{{  0x10
   {0,0, 0,0, 0,0},
   {0xe2,kLeftAlt, 0xe6,kRightAlt, 0,0},
   {0xe1,kLeftShift, 0,0, 0,0},
   {0,0, 0,0, 0,0},

   // 0x14
   {0xe0,kLeftControl, 0xe4,kRightControl, 0,0},
   {0x14,0, 0,0, 'q','Q'},
   {0x1e,0, 0,0, '1','!'},
   {0,0, 0,0, 0,0},

   // 0x18
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},
   {0x1d,0, 0,0, 'z','Z'},
   {0x16,0, 0,0, 's','S'},

   // 0x1c
   {0x04,0, 0,0, 'a','A'},
   {0x1a,0, 0,0, 'w','W'},
   {0x1f,0, 0,0, '2','"'},
   {0,0, 0xe3,kLeftMeta, 0,0},
   //}}}
   //{{{  0x20
   {0,0, 0,0, 0,0},
   {0x06,0, 0,0, 'c','C'},
   {0x1b,0, 0,0, 'x','X'},
   {0x07,0, 0,0, 'd','D'},

   // 0x24
   {0x08,0, 0,0, 'e','E'},
   {0x21,0, 0,0, '4','$'},
   {0x20,0, 0,0, '3','#'},
   {0,0, 0xe7,kRightMeta, 0,0},

   // 0x28
   {0,0, 0,0, 0,0},
   {0x2c,0, 0,0, ' ',' '},
   {0x19,0, 0,0, 'v','V'},
   {0x09,0, 0,0, 'f','F'},

   // 0x2C
   {0x17,0, 0,0, 't',0},
   {0x15,0, 0,0, 'r',0},
   {0x22,0, 0,0, '%',0},
   {0,0, 0,0, 0,0},
   //}}}
   //{{{  0x30
   {0,0, 0,0, 0,0},
   {0x11,0, 0,0, 'n','N'},
   {0x05,0, 0,0, 'b','B'},
   {0x0b,0, 0,0, 'h','H'},

   // 0x34
   {0x0a,0, 0,0, 'g','G'},
   {0x1c,0, 0,0, 'y','Y'},
   {0x23,0, 0,0, '^',0},
   {0,0, 0,0, 0,0},

   // 0x38
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},
   {0x10,0, 0,0, 'm','M'},
   {0x0d,0, 0,0, 'j','J'},

   // 0x3C
   {0x18,0, 0,0, 'u','U'},
   {0x24,0, 0,0, '&',0},
   {0x25,0, 0,0, '*',0},
   {0,0, 0,0, 0,0},
   //}}}
   //{{{  0x40
   {0,0, 0,0, 0,0},
   {0x36,0, 0,0, '<',0},
   {0x0e,0, 0,0, 'k',0},
   {0x0c,0, 0,0, 'i',0},

   // 0x44
   {0x12,0, 0,0, 'o',0},
   {0x27,0, 0,0, ')',0},
   {0x26,0, 0,0, '(',0},
   {0,0,    0,0, 0,0},

   // 0x48
   {0,0, 0,0,       0,0},
   {0x37,0, 0,0,    '>',0},
   {0x38,0, 0x38,0, '?',0},
   {0x0f,0, 0,0,    'l',0},

   // 0x4C
   {0x33,0, 0,0, ':',0},
   {0x13,0, 0,0, 'p',0},
   {0x2d,0, 0,0, '_',0},
   {0,0, 0,0, 0,0},
   //}}}
   //{{{  0x50
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},
   {0x34,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},

   // 0x54
   {0x2f,0, 0,0, '[',0},
   {0x2e,0, 0,0, '=',0},
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},

   // 0x58
   {0x39,0, 0,0, 0,0},
   {0xe5,kRightShift, 0,0, 0,0},
   {0x28,0, 0x58,0, 0x0d,0},
   {0x30,0, 0,0, ']',0},

   // 0x5C
   {0,0, 0,0, 0,0},
   {0x31,0, 0,0, '\'',0},
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},
   //}}}
   //{{{  0x60
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},

   // 0x64
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},
   {0x2a,0, 0,0, 0x08,0},
   {0,0, 0,0, 0,0},

   // 0x68
   {0,0, 0,0, 0,0},
   {0x59,0, 0,0, '1',0},
   {0,0, 0,0, 0,0},
   {0x5c,0, 0x50,0, '4','4'},

   // 0x6C
   {0x5f,0, 0x4a,0, '7','7'},
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},
   //}}}
   //{{{  0x70
   {0x62,0, 0x49,0, '0',0},
   {0x63,0, 0x4c,0, '.',0},
   {0x5A,0, 0x51,0, '2','2'},
   {0x5d,0, 0,0,    '5','5'},

   // 0x74
   {0x5e,0, 0x4f,0, '6','6'},
   {0x60,0, 0x52,0, '8','8'},
   {0x29,0, 0,0,    0x1b,0},
   {0x53,0, 0,0,    0,0},

   // 0x78
   {0x44,0, 0,0,    0,0},
   {0x57,0, 0,0,    '+','+'},
   {0x5b,0, 0x4e,0, '3','3'},
   {0x56,0, 0,0,    '-','-'},

   // 0x7C
   {0x55,0, 0x46,0, '*','*'},
   {0x61,0, 0x4b,0, '9','9'},
   {0x47,0, 0,0,    0,0},
   {0,0,    0,0,    0,0},
   //}}}
   //{{{  0x80
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},
   {0,0, 0,0, 0,0},
   {0x40,0, 0,0, 0,0},
   //}}}
   };
  //}}}

  static const int kBitWidth = 8;
  static const int kMaxSamples = 480 / kBitWidth;
  static const int kMaxCodes = 14;
  static const int kMaxChars = 43;
  static const int kMaxRing = 32;
  //{{{
  void initGpio() {

    __HAL_RCC_GPIOF_CLK_ENABLE();

    // init PS2 clock
    GPIO_InitTypeDef GPIO_Init_Structure;
    GPIO_Init_Structure.Mode = GPIO_MODE_IT_RISING;
    GPIO_Init_Structure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_Init_Structure.Pull = GPIO_PULLUP;
    GPIO_Init_Structure.Pin = GPIO_PIN_8;
    HAL_GPIO_Init (GPIOF, &GPIO_Init_Structure);

    // init PS2 data
    GPIO_Init_Structure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Init_Structure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_Init_Structure.Pull = GPIO_PULLUP;
    GPIO_Init_Structure.Pin = GPIO_PIN_9;
    HAL_GPIO_Init (GPIOF, &GPIO_Init_Structure);

    // enable and set EXTI line 8 Interrupt to the lowest priority
    HAL_NVIC_SetPriority (EXTI9_5_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ (EXTI9_5_IRQn);

    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_8, GPIO_PIN_SET);
    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
    HAL_Delay (100);
    }
  //}}}

  //{{{
  void sendChar (uint8_t value)  {

    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_8, GPIO_PIN_RESET); // set clock lo, release inhibit, if necessary
    HAL_Delay (2); // Wait out any final clock pulse, 100us

    mTx = true;
    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_9, GPIO_PIN_RESET); // set data lo, start bit
    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_8, GPIO_PIN_SET);   // set clock hi, float

    uint8_t parity = 1;
    for (int bit = 0; bit < 8; bit++) {
      while (HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for rising edge
      HAL_GPIO_WritePin (GPIOF, GPIO_PIN_9, (value & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); // set data to i’th data bit
      parity = parity + value;  // Accumulate parity
      value = value >> 1;       // Shift right to get next bit
      while (!HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for falling edge
      }

    while (HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for rising edge
    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_9, (parity & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); // set data to parity bit
    while (!HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for falling edge

    while (HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for rising edge
    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_9, GPIO_PIN_SET); // set data hi, stop bit
    while (!HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for falling edge
    mTx = false;

    while (HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for rising edge
    while (!HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for falling edge

    getRawChar();
    resetCode();
    }
  //}}}
  //{{{
  void sendTouchpadSpecialCommand (uint8_t arg) {
  // send touchpad special command sequence

    for (auto i = 0u; i < 4; i++) {
      sendChar (0xE8);
      sendChar ((arg >> (6-2*i)) & 3);
      }
    }
  //}}}
  //{{{  vars
  cLcd* mLcd;

  // bits
  bool mTx = false;
  int mBitPos = -1;
  uint16_t mCode = 0;

  // keyboard
  volatile int mInRawPtr = 0;
  volatile int mOutRawPtr = 0;
  int mRxRawData[kMaxRing];

  volatile int mInPtr = 0;
  volatile int mOutPtr = 0;
  int mRxData[kMaxRing];

  int mCurCode = 0;
  int mCodes[kMaxCodes];
  int mCurChar = 0;
  int mChars[kMaxChars];

  uint8_t mModBits = 0;
  bool mRxExpCode = false;
  bool mRxReleaseCode = false;
  bool mPauseCode = false;

  // touchpad
  bool mStream = false;
  int mStreamCode = -1;
  uint8_t mStreamCodes[6];
  int mTouchX = 0;
  int mTouchY = 0;
  int mTouchZ = 0;

  // waveform
  uint16_t mSample = 0;
  bool mBitArray[kMaxSamples];
  int mBitPosArray[kMaxSamples];
  //}}}
  };
