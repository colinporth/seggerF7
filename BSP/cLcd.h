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
  cLcd (int lines);
  void init();

  uint32_t* getBuffer();

  void start();
  void startBgnd (uint16_t* bgnd);
  void startBgnd (uint16_t* src, uint16_t srcXsize, uint16_t srcYsize, bool zoom);
  void drawTitle (const char* title);
  void drawDebug();
  void present();

  int getScrollScale();
  unsigned getScrollLines();
  void incScrollValue (int inc);
  void incScrollIndex (int inc);                                       

  void debug (uint32_t colour, const char* format, ... );

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
