// cLcd.h
//{{{  includes
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <math.h>

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
//{{{
class cPoint {
public:
  //{{{
  cPoint()  {
    x = 0;
    y = 0;
    }
  //}}}
  //{{{
  cPoint (uint16_t x, uint16_t y) {
    this->x = x;
    this->y = y;
    }
  //}}}

  //{{{
  cPoint operator - (const cPoint& point) const {
    return cPoint (x - point.x, y - point.y);
    }
  //}}}
  //{{{
  cPoint operator + (const cPoint& point) const {
    return cPoint (x + point.x, y + point.y);
    }
  //}}}
  //{{{
  cPoint operator * (const cPoint& point) const {
    return cPoint (x * point.x, y * point.y);
    }
  //}}}

  //{{{
  const cPoint& operator += (const cPoint& point)  {
    x += point.x;
    y += point.y;
    return *this;
    }
  //}}}
  //{{{
  const cPoint& operator -= (const cPoint& point)  {
    x -= point.x;
    y -= point.y;
    return *this;
    }
  //}}}

  //{{{
  bool inside (const cPoint& pos) const {
  // return pos inside rect formed by us as size
    return pos.x >= 0 && pos.x < x && pos.y >= 0 && pos.y < y;
    }
  //}}}
  //{{{
  float magnitude() const {
  // return magnitude of point as vector
    return sqrt (float(x*x) + float(y*y));
    }
  //}}}

  int16_t x;
  int16_t y;
  };
//}}}
//{{{
class cRect {
public:
  //{{{
  cRect()  {
    left = 0;
    bottom = 0;
    right = 0;
    bottom = 0;
    }
  //}}}
  //{{{
  cRect (const cPoint& size)  {
  left = 0;
  top = 0;
  right = size.x;
  bottom = size.y;
  }
  //}}}
  //{{{
  cRect (const cPoint& topLeft, const cPoint& bottomRight)  {
   left = topLeft.x;
   top = topLeft.y;
   right = bottomRight.x;
   bottom = bottomRight.y;
   }
  //}}}
  //{{{
  cRect (uint16_t l, uint16_t t, uint16_t r, uint16_t b) {
    left = l;
    top = t;
    right = r;
    bottom = b;
    }
  //}}}

  //{{{
  cRect operator + (const cPoint& point) const {
    return cRect (left + point.x, top + point.y, right + point.x, bottom + point.y);
    }
  //}}}

  int getWidth() const { return right - left; }
  int getHeight() const { return bottom - top; }
  int getWidthInt() const { return int(right - left); }
  int getHeightInt() const { return int(bottom - top); }

  cPoint getTL() const { return cPoint(left, top); }
  cPoint getTL (int offset) const { return cPoint(left+offset, top+offset); }
  cPoint getTR() const { return cPoint(right, top); }
  cPoint getBL() const { return cPoint(left, bottom); }
  cPoint getBR() const { return cPoint(right, bottom); }

  cPoint getSize() const { return cPoint(right-left, bottom-top); }
  cPoint getCentre() const { return cPoint(getCentreX(), getCentreY()); }
  int getCentreX() const { return (left + right)/2.f; }
  int getCentreY() const { return (top + bottom)/2.f; }
  //{{{
  bool inside (const cPoint& pos) const {
  // return pos inside rect
    return (pos.x >= left) && (pos.x < right) && (pos.y >= top) && (pos.y < bottom);
    }
  //}}}

  int16_t left;
  int16_t right;
  int16_t top;
  int16_t bottom;
  };
//}}}

class cLcd {
public:
  enum eTextAlign { eTextLeft, eTextCentre, eTextRight };

  static uint32_t getWidth() { return 480; }
  static uint32_t getHeight() { return 272; }

  cLcd (uint16_t displayLines);
  void init();

  uint16_t GetTextHeight();

  // flipped display
  void start();
  void start (uint16_t* src, uint16_t srcXsize, uint16_t srcYsize, int zoom, cPoint zoomCentre);
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
  void SetTransparency (uint8_t Transparency);
  void SetAddress (uint16_t* address, uint16_t* writeAddress);

  uint16_t readPix (uint16_t x, uint16_t y);
  void drawPix (uint16_t color, uint16_t x, uint16_t y);

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

  void fillBuffer (uint16_t color, uint16_t* dst, uint16_t xsize, uint16_t ysize, uint32_t OffLine);
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

  bool mFlip = false;

  uint16_t mDisplayLines;
  unsigned mDebugLine = 0;
  cDebugItem mLines[kDebugMaxLines];
  int mScroll = 0;
  };
