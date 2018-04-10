// cLcd.cpp
//{{{  includes
#include "cLcd.h"
//}}}

cLcd::cLcd (int lines) : mDisplayLines(lines) {}
//{{{
void cLcd::init() {

  BSP_LCD_Init();

  BSP_LCD_LayerDefaultInit (0, SDRAM_SCREEN0);
  BSP_LCD_Clear (LCD_COLOR_BLACK);
  BSP_LCD_DisplayOn();
  }
//}}}

uint32_t* cLcd::getBuffer() { return (uint32_t*)(mFlip ? SDRAM_SCREEN1_565 : SDRAM_SCREEN0); }

//{{{
void cLcd::start() {
  BSP_LCD_Clear (LCD_COLOR_BLACK);
  }
//}}}
//{{{
void cLcd::startBgnd (uint16_t* bgnd) {
  memcpy ((uint32_t*)(mFlip ? SDRAM_SCREEN1_565 : SDRAM_SCREEN0), bgnd, 480*272*2);
  }
//}}}
//{{{
void cLcd::startBgnd (uint16_t* src, uint16_t srcXsize, uint16_t srcYsize, bool zoom) {

  if (zoom)
    BSP_LCD_ConvertFrameCpu1 (src, srcXsize, srcYsize, getBuffer(), BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  else
    BSP_LCD_ConvertFrameCpu (src, srcXsize, srcYsize, getBuffer(), BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  }
//}}}
//{{{
void cLcd::drawTitle (const char* title) {

  char str1[40];
  sprintf (str1, "%s %d", title, (int)mTick);

  BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
  BSP_LCD_DisplayStringAtLine (0, str1);
  }
//}}}
//{{{
void cLcd::drawDebug() {

  if (!BSP_PB_GetState (BUTTON_KEY))
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
//{{{
void cLcd::present() {

  uint32_t wait = 20 - (HAL_GetTick() % 20);
  HAL_Delay (wait);
  mTick = HAL_GetTick();

  mFlip = !mFlip;
  BSP_LCD_SetAddress (0, mFlip ? SDRAM_SCREEN0 : SDRAM_SCREEN1_565, mFlip ? SDRAM_SCREEN1_565 : SDRAM_SCREEN0);
  }
//}}}

//{{{
int cLcd::getScrollScale() {
  return 4;
  }
//}}}
//{{{
unsigned cLcd::getScrollLines() {
  return mScroll / getScrollScale();
  }
//}}}
//{{{
void cLcd::incScrollValue (int inc) {

  mScroll += inc;

  if (mScroll < 0)
    mScroll = 0;
  else if (getScrollLines() >  mDebugLine - mDisplayLines)
    mScroll = (mDebugLine - mDisplayLines) * getScrollScale();
  }
//}}}
//{{{
void cLcd::incScrollIndex (int inc) {
  incScrollValue (inc * BSP_LCD_GetTextHeight() / getScrollScale());
  }
//}}}

//{{{
void cLcd::debug (uint32_t colour, const char* format, ... ) {

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
