// cLcd.h
#pragma once
//{{{  includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "../common/cPointRect.h"

#include "cmsis_os.h"
//}}}
//{{{  colour defines
#define LCD_COLOR_BLUE       0x001F
#define LCD_COLOR_GREEN      0x07D0
#define LCD_COLOR_RED        0xF800
#define LCD_COLOR_YELLOW     0xFFD0
#define LCD_COLOR_MAGENTA    0xF81F
#define LCD_COLOR_CYAN       0x07FF

#define LCD_COLOR_WHITE      0xFFFF
#define LCD_COLOR_GREY       0x7BEF
#define LCD_COLOR_LIGHT_GREY 0x9512
#define LCD_COLOR_BLACK      0x0000
//}}}

class cLcd {
public:
  enum eTextAlign { CENTER_MODE, RIGHT_MODE, LEFT_MODE };

  cLcd (uint16_t displayLines);
  void init();

  static uint32_t getWidth() { return 480; }
  static uint32_t getHeight() { return 272; }

  uint16_t GetTextHeight();

  // flipped display
  void start();
  void start (uint16_t* src, uint16_t srcXsize, uint16_t srcYsize, bool unity);
  void drawInfo (uint16_t color, uint16_t column, const char* format, ... );
  void drawDebug();
  void present();

  int getScrollScale();
  unsigned getScrollLines();
  void incScrollValue (int inc);
  void incScrollIndex (int inc);

  void debug (uint32_t colour, const char* format, ... );

  // drawing
  void SelectLayer (uint32_t layerIndex);
  void SetTransparency (uint32_t layerIndex, uint8_t Transparency);
  void SetAddress (uint32_t layerIndex, uint16_t* address, uint16_t* writeAddress);

  uint16_t readPixel (uint16_t x, uint16_t y);
  void drawPixel (uint16_t color, uint16_t x, uint16_t y);

  void clearStringLine (uint16_t color, uint32_t line);
  void displayChar (uint16_t color, cPoint pos, uint8_t ascii);
  void displayStringAt (uint16_t color, cPoint pos, const char* str, eTextAlign textAlign);
  void displayStringAtLine (uint16_t color, uint16_t line, const char* str);
  void displayStringAtColumnLine (uint16_t color, uint16_t column, uint16_t line, const char* str);

  void clear (uint16_t color);
  void drawRect (uint16_t color, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
  void fillRect (uint16_t color, cRect& rect);
  void fillRectCpu (uint16_t color, cRect& rect);
  void fillRect (uint16_t color, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
  void drawCircle (uint16_t color, uint16_t x, uint16_t y, uint16_t radius);
  void fillCircle (uint16_t color, uint16_t x, uint16_t y, uint16_t radius);
  void drawPolygon (uint16_t color, cPoint* points, uint16_t pointCount);
  void fillPolygon (uint16_t color, cPoint* points, uint16_t pPointCount);
  void drawEllipse (uint16_t color, uint16_t xCentre, uint16_t yCentre, uint16_t xRadius, uint16_t yRadius);
  void fillEllipse (uint16_t color, uint16_t xCentre, uint16_t yCentre, uint16_t xRadius, uint16_t yRadius);
  void drawLine (uint16_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

  void rgb888to565 (uint8_t* src, uint16_t* dst, uint16_t size);
  void rgb888to565cpu (uint8_t* src, uint16_t* dst, uint16_t size);
  void convertFrameYuv (uint8_t* src, uint16_t srcXsize, uint16_t srcYsize,
                        uint8_t* dst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize);

  void displayOff();
  void displayOn();

  static cLcd* mLcd;
  static bool mFrameWait;
  static SemaphoreHandle_t mFrameSem;

private:
  uint16_t* getBuffer();

  void setLayer (uint32_t layerIndex);
  void layerInit (uint16_t layerIndex, uint32_t FrameBuffer);

  void fillBuffer (uint16_t color, uint32_t layer, uint32_t dst, uint32_t xsize, uint32_t ysize, uint32_t OffLine);
  void fillTriangle (uint16_t color, uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3);

  static const int kMaxStrSize = 40;
  static const int kDebugMaxLines = 100;
  //{{{
  class cDebugItem {
  public:
    cDebugItem() {}
    ~cDebugItem() { free (mStr); }

    char* mStr = nullptr;
    uint32_t mTicks = 0;
    uint32_t mColour = 0;
    };
  //}}}

  uint32_t mLayer = 0;

  bool mFlip = false;

  uint16_t mDisplayLines;
  unsigned mDebugLine = 0;
  cDebugItem mLines[kDebugMaxLines];
  int mScroll = 0;
  };
