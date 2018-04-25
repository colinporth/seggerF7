// cLcd.h
//{{{  includes
#pragma once

#include "../common/cPointRect.h"

#include "cmsis_os.h"
//}}}
//{{{  color
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
  enum eTextAlign { eTextLeft, eTextCentre, eTextRight };

  static uint32_t getWidth() { return 480; }
  static uint32_t getHeight() { return 272; }
  static cPoint getSize() { return cPoint(480,272); }

  cLcd (uint16_t displayLines);
  void init();

  uint16_t GetTextHeight();

  // flipped display
  void drawInfo (uint16_t color, uint16_t column, const char* format, ... );
  void drawDebug();
  void present();

  int getScrollScale();
  unsigned getScrollLines();
  void incScrollValue (int inc);
  void incScrollIndex (int inc);

  void clearDebug();
  void debug (uint32_t colour, const char* format, ... );

  // drawing
  uint16_t readPix (uint16_t x, uint16_t y);
  void drawPix (uint16_t color, uint16_t x, uint16_t y);

  void zoom565 (uint16_t* src, cPoint srcPos, cPoint srcSize, cRect dstRect, float zoomx, float zoomy);
  void rgb888to565 (uint8_t* src, uint16_t* dst, uint16_t xsize, uint16_t ysize);
  void rgb888to565cpu (uint8_t* src, uint16_t* dst, uint16_t size);
  void convertFrameYuv (uint8_t* src, uint16_t srcXsize, uint16_t srcYsize,
                        uint8_t* dst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize);

  void displayChar (uint16_t color, cPoint pos, uint8_t ascii);
  void displayString (uint16_t color, cPoint pos, const char* str, eTextAlign textAlign);
  void displayStringLine (uint16_t color, uint16_t line, const char* str);
  void displayStringColumnLine (uint16_t color, uint16_t column, uint16_t line, const char* str);
  void clearStringLine (uint16_t color, uint16_t line);

  void clear (uint16_t color);
  void drawRect (uint16_t color, cRect& rect, uint16_t thickness);
  void drawRect (uint16_t color, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t thickness);
  void fillRect (uint16_t color, cRect& rect);
  void fillRectCpu (uint16_t color, cRect& rect);
  void fillRect (uint16_t color, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

  void drawCircle (uint16_t color, uint16_t x, uint16_t y, uint16_t radius);
  void fillCircle (uint16_t color, uint16_t x, uint16_t y, uint16_t radius);
  void drawEllipse (uint16_t color, uint16_t xCentre, uint16_t yCentre, uint16_t xRadius, uint16_t yRadius);
  void fillEllipse (uint16_t color, uint16_t xCentre, uint16_t yCentre, uint16_t xRadius, uint16_t yRadius);
  void drawPolygon (uint16_t color, cPoint* points, uint16_t pointCount);
  void fillPolygon (uint16_t color, cPoint* points, uint16_t pPointCount);
  void drawLine (uint16_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

  void displayOff();
  void displayOn();

  static cLcd* mLcd;

  static bool mFrameWait;
  static SemaphoreHandle_t mFrameSem;
  static SemaphoreHandle_t mDma2dSem;

private:
  void layerInit();

  uint16_t* getWriteBuffer();

  void ready();
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

  uint16_t mDisplayLines;
  unsigned mDebugLine = 0;
  cDebugItem mLines[kDebugMaxLines];
  int mScroll = 0;
  };
