// cLcd.h
#pragma once
//{{{  includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include "cmsis_os.h"
//}}}
//{{{  colour defines
#define LCD_COLOR_BLUE          0x001F
#define LCD_COLOR_GREEN         0x07D0
#define LCD_COLOR_RED           0xF800
#define LCD_COLOR_YELLOW        0xFFD0
#define LCD_COLOR_WHITE         0xFFFF
#define LCD_COLOR_BLACK         0x0000
#define LCD_COLOR_MAGENTA       0xF81F
#define LCD_COLOR_CYAN          0x07FF
//}}}
  enum eTextAlign { CENTER_MODE, RIGHT_MODE, LEFT_MODE };

class cLcd {
public:
  //{{{  Point
  typedef struct {
    int16_t X;
    int16_t Y;
    } Point, *pPoint;
  //}}}

  cLcd (int lines);
  void init();

  static uint32_t getWidth() { return 480; }
  static uint32_t getHeight() { return 272; }

  uint16_t GetTextHeight();
  uint32_t GetTextColor() { return mTextColor; }
  uint32_t GetBackColor() { return mBackColor; }
  void SetTextColor (uint32_t color) { mTextColor = color; }
  void SetBackColor (uint32_t color) { mBackColor = color; }

  // flipped display
  void start();
  void start (uint16_t* src, uint16_t srcXsize, uint16_t srcYsize, bool unity);
  void drawInfo (uint32_t color, uint16_t column, const char* format, ... );
  void drawDebug();
  void present();

  int getScrollScale();
  unsigned getScrollLines();
  void incScrollValue (int inc);
  void incScrollIndex (int inc);

  void debug (uint32_t colour, const char* format, ... );

  // drawing
  void SelectLayer (uint32_t LayerIndex);
  void SetTransparency (uint32_t LayerIndex, uint8_t Transparency);
  void SetAddress (uint32_t LayerIndex, uint16_t* address, uint16_t* writeAddress);

  uint32_t ReadPixel (uint16_t x, uint16_t y);
  void DrawPixel (uint16_t x, uint16_t y, uint32_t color);
  void DrawBitmap (uint32_t Xpos, uint32_t Ypos, uint8_t* pbmp);

  void clearStringLine (uint32_t Line);
  void DisplayChar (uint16_t x, uint16_t y, uint8_t ascii);
  void DisplayStringAt (uint16_t x, uint16_t y, const char* text, eTextAlign mode);
  void DisplayStringAtLine (uint16_t line, const char* ptr);
  void DisplayStringAtLineColumn (uint16_t line, uint16_t column, const char* ptr);

  void clear (uint32_t Color);
  void DrawRect (uint16_t x, uint16_t y, uint16_t Width, uint16_t Height);
  void FillRect (uint16_t x, uint16_t y, uint16_t Width, uint16_t Height);
  void DrawCircle (uint16_t x, uint16_t y, uint16_t Radius);
  void FillCircle (uint16_t x, uint16_t y, uint16_t Radius);
  void DrawPolygon (pPoint Points, uint16_t PointCount);
  void FillPolygon (pPoint Points, uint16_t PointCount);
  void DrawEllipse (uint16_t xCentre, uint16_t yCentre, uint16_t XRadius, uint16_t YRadius);
  void FillEllipse (uint16_t xCentre, uint16_t yCentre, uint16_t XRadius, uint16_t YRadius);
  void DrawLine (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

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
  void layerInit (uint16_t LayerIndex, uint32_t FrameBuffer);

  void FillBuffer (uint32_t layer, uint32_t dst, uint32_t xsize, uint32_t ysize, uint32_t OffLine, uint32_t color);
  void FillTriangle (uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3);
  void ConvertLineToARGB8888 (void* src, void* dst, uint32_t xSize, uint32_t ColorMode);

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

  uint32_t mCurLayer = 0;
  uint32_t mTextColor = LCD_COLOR_WHITE;
  uint32_t mBackColor = LCD_COLOR_BLACK;

  bool mFlip = false;

  int mDisplayLines;
  unsigned mDebugLine = 0;
  cDebugItem mLines[kDebugMaxLines];
  int mScroll = 0;
  };
