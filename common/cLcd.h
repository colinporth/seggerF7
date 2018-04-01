// cLcd.h
#pragma once
//{{{  includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "stm32746g_lcd.h"
//}}}
//{{{
#ifdef __cplusplus
  extern "C" {
#endif
//}}}

class cLcd {
public:
  cLcd (int lines) : mDisplayLines(lines) {}
  //{{{
  void init() {

    BSP_LCD_Init();

    BSP_LCD_LayerDefaultInit (0, SDRAM_SCREEN0);
    BSP_LCD_SelectLayer(0);
    BSP_LCD_Clear (LCD_COLOR_BLACK);
    BSP_LCD_SetLayerVisible (0, ENABLE);
    BSP_LCD_SetTransparency (0, 255);

    BSP_LCD_LayerDefaultInit (1, SDRAM_SCREEN1);
    BSP_LCD_SelectLayer(1);
    BSP_LCD_Clear (LCD_COLOR_BLACK);
    BSP_LCD_SetLayerVisible (1, ENABLE);
    BSP_LCD_SetTransparency (1, 0);

    BSP_LCD_DisplayOn();
    }
  //}}}

  uint32_t* getBuffer() { return (uint32_t*)(mFlip ? SDRAM_SCREEN1 : SDRAM_SCREEN0); }

  //{{{
  void start (const char* title) {

    BSP_LCD_Clear (LCD_COLOR_BLACK);

    drawTitle (title);
    if (!BSP_PB_GetState (BUTTON_KEY))
      drawDebug();
    }
  //}}}
  //{{{
  void startCam (const char* title, uint16_t* bgnd) {

    //BSP_LCD_ConvertFrame (bgnd, mFlip ? (uint8_t*)SDRAM_SCREEN1 : (uint8_t*)SDRAM_SCREEN0, 480, 272);
    BSP_LCD_ConvertFrameCpu (bgnd, mFlip ? (uint8_t*)SDRAM_SCREEN1 : (uint8_t*)SDRAM_SCREEN0, 480, 272);

    drawTitle (title);
    if (!BSP_PB_GetState (BUTTON_KEY))
      drawDebug();
    }
  //}}}
  //{{{
  void startBgnd (const char* title, uint32_t* bgnd) {

    memcpy ((uint32_t*)(mFlip ? SDRAM_SCREEN1 : SDRAM_SCREEN0), bgnd, 480*272*4);

    drawTitle (title);
    if (!BSP_PB_GetState (BUTTON_KEY))
      drawDebug();
    }
  //}}}
  //{{{
  void present() {

    uint32_t wait = 20 - (HAL_GetTick() % 20);
    HAL_Delay (wait);
    mTick = HAL_GetTick();

    BSP_LCD_SetTransparency (mFlip, 255);
    BSP_LCD_SetTransparency (!mFlip, 0);
    mFlip = !mFlip;
    BSP_LCD_SelectLayer (mFlip);
    }
  //}}}

  //{{{
  int getScrollScale() {
    return 4;
    }
  //}}}
  //{{{
  unsigned getScrollLines() {
    return mScroll / getScrollScale();
    }
  //}}}
  //{{{
  void incScrollValue (int inc) {

    mScroll += inc;

    if (mScroll < 0)
      mScroll = 0;
    else if (getScrollLines() >  mDebugLine - mDisplayLines)
      mScroll = (mDebugLine - mDisplayLines) * getScrollScale();
    }
  //}}}
  //{{{
  void incScrollIndex (int inc) {
    incScrollValue (inc * BSP_LCD_GetTextHeight() / getScrollScale());
    }
  //}}}

  //{{{
  void debug (uint32_t colour, const char* format, ... ) {

    auto line = mDebugLine % kDebugMaxLines;

    va_list args;
    va_start (args, format);
    vsnprintf (mLines[line].mStr, kMaxStrSize-1, format, args);
    va_end (args);

    mLines[line].mTicks = HAL_GetTick();
    mLines[line].mColour = colour;
    mDebugLine++;
    }
  //}}}

private:
  static const int kMaxStrSize = 40;
  static const int kDebugMaxLines = 100;
  //{{{
  class cDebugItem {
  public:
    cDebugItem() {}
    cDebugItem (char* str, uint32_t ticks, uint32_t colour) : mStr(str), mTicks(ticks), mColour(colour) {}
    //{{{
    ~cDebugItem() {
      free (mStr);
      }
    //}}}

    char* mStr = (char*)malloc (kMaxStrSize);
    uint32_t mTicks = 0;
    uint32_t mColour = 0;
    };
  //}}}
  //{{{
  void drawTitle (const char* title) {

    char str1[40];
    sprintf (str1, "%s %d", title, (int)mTick);
    BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAtLine (0, str1);
    }
  //}}}
  //{{{
  void drawDebug() {

    for (auto displayLine = 0u; (displayLine < mDebugLine) && ((int)displayLine < mDisplayLines); displayLine++) {
      int debugLine = ((int)mDebugLine < mDisplayLines) ?
        displayLine : (mDebugLine - mDisplayLines + displayLine - getScrollLines())  % kDebugMaxLines;

      BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
      char tickStr[20];
      auto ticks = mLines[debugLine].mTicks;
      sprintf (tickStr, "%2d.%03d", (int)ticks / 1000, (int)ticks % 1000);
      BSP_LCD_DisplayStringAtLineColumn (1+displayLine, 0, tickStr);

      BSP_LCD_SetTextColor (mLines[debugLine].mColour);
      BSP_LCD_DisplayStringAtLineColumn (1+displayLine, 7, mLines[debugLine].mStr);
      }
    }
  //}}}

  bool mFlip = false;
  uint32_t mTick = 0;

  int mDisplayLines;
  unsigned mDebugLine = 0;
  cDebugItem mLines[kDebugMaxLines];
  int mScroll = 0;
  };

//{{{
#ifdef __cplusplus
}
#endif
//}}}
