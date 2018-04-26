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
  enum eTextAlign { eTextLeft, eTextCentreBox, eTextCentre, eTextBottomRight, eTextRight };

  static uint32_t getWidth() { return 480; }
  static uint32_t getHeight() { return 272; }
  static cPoint getSize() { return cPoint(480,272); }
  static cRect getRect() { return cRect (0,0,480,272); }

  cLcd (uint16_t displayLines);
  void init();

  uint16_t getCharWidth();
  uint16_t getTextHeight();

  // flipped display
  void drawInfo (uint16_t color, eTextAlign textAlign, const char* format, ... );
  void drawDebug();
  void present();

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
  void incScrollIndex (int inc) {
    incScrollValue (inc * getTextHeight() / getScrollScale());
    }
  //}}}
  void incScrollValue (int inc);

  //{{{
  void clearDebug() {

    mDebugLine = 0;
    }
  //}}}
  void debug (uint32_t colour, const char* format, ... );

  // drawing
  uint16_t readPix (cPoint p);

  void zoom565 (uint16_t* src, cPoint srcPos, cPoint srcSize, cRect dstRect, float zoomx, float zoomy);
  void rgb888to565 (uint8_t* src, uint16_t* dst, uint16_t xsize, uint16_t ysize);
  void rgb888to565cpu (uint8_t* src, uint16_t* dst, uint16_t xsize, uint16_t ysize);
  void convertFrameYuv (uint8_t* src, uint16_t srcXsize, uint16_t srcYsize,
                        uint8_t* dst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize);

  void displayChar (uint16_t color, cPoint p, uint8_t ascii);
  void displayString (uint16_t color, cPoint p, const char* str, eTextAlign textAlign);
  void displayStringShadow (uint16_t color, cPoint p, const char* str, eTextAlign textAlign);

  //{{{
  void displayStringLine (uint16_t color, uint16_t line, const char* str) {
    displayString (color, cPoint(0, line * getTextHeight()), str, eTextLeft);
    }
  //}}}
  //{{{
  void displayStringColumnLine (uint16_t color, uint16_t column, uint16_t line, const char* str) {
    displayString (color, cPoint(column * getCharWidth(), line * getTextHeight()), str, cLcd::eTextLeft);
    }
  //}}}
  //{{{
  void clearStringLine (uint16_t color, uint16_t line) {
    fillRect (color, cRect (0, line * getTextHeight(), getWidth(), (line + 1) * getTextHeight()));
    }
  //}}}

  void fillRect (uint16_t color, const cRect& rect);
  void fillRectCpu (uint16_t color, const cRect& rect);
  //{{{
  void fillRect (uint16_t color, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    fillRect (color, cRect (x, y, x+width, y+height));
    }
  //}}}
  //{{{
  void clear (uint16_t color) {
    fillRect (color, cRect (getSize()));
    }
  //}}}

  void drawRect (uint16_t color, const cRect& rect, uint16_t thickness);

  void drawCircle (uint16_t color, cPoint centre, uint16_t radius);
  void fillCircle (uint16_t color, cPoint centre, uint16_t radius);
  void drawEllipse (uint16_t color, cPoint centre, cPoint radius);
  void fillEllipse (uint16_t color, cPoint centre, cPoint radius);
  void drawPolygon (uint16_t color, cPoint* points, uint16_t numPoints);
  void fillPolygon (uint16_t color, cPoint* points, uint16_t numPoints);
  void drawLine (uint16_t color, cPoint p1, cPoint p2);

  void displayOff();
  void displayOn();

  static cLcd* mLcd;
  static bool mFrameWait;
  static SemaphoreHandle_t mFrameSem;
  static bool mDma2dWait;
  static SemaphoreHandle_t mDma2dSem;

private:
  void layerInit();
  uint16_t* getWriteBuffer() { return mFrameBuf; }

  void ready();
  void drawPix (uint16_t color, uint16_t x, uint16_t y) { *(mFrameBuf + y*getWidth() + x) = color; }
  void fillTriangle (uint16_t color, cPoint p1, cPoint p2, cPoint p3);

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

  uint16_t* mFrameBuf;

  uint16_t mDisplayLines;
  unsigned mDebugLine = 0;
  cDebugItem mLines[kDebugMaxLines];
  int mScroll = 0;
  };
