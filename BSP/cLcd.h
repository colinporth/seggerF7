// cLcd.h
#pragma once
//{{{  includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
//}}}
//{{{
#ifdef __cplusplus
  extern "C" {
#endif
//}}}

#define RGB565
//{{{  colour defines
#ifdef RGB565

#define LCD_COLOR_BLUE          0x001F
#define LCD_COLOR_GREEN         0x07D0
#define LCD_COLOR_RED           0xF800
#define LCD_COLOR_YELLOW        0xFFD0
#define LCD_COLOR_WHITE         0xFFFF
#define LCD_COLOR_BLACK         0x0000
#define LCD_COLOR_MAGENTA       0x7D1F
#define LCD_COLOR_CYAN          0x07FF

#else

#define LCD_COLOR_BLUE          0xFF0000FF
#define LCD_COLOR_GREEN         0xFF00FF00
#define LCD_COLOR_RED           0xFFFF0000
#define LCD_COLOR_CYAN          0xFF00FFFF
#define LCD_COLOR_MAGENTA       0xFFFF00FF
#define LCD_COLOR_YELLOW        0xFFFFFF00
#define LCD_COLOR_LIGHTBLUE     0xFF8080FF
#define LCD_COLOR_LIGHTGREEN    0xFF80FF80
#define LCD_COLOR_LIGHTRED      0xFFFF8080
#define LCD_COLOR_LIGHTCYAN     0xFF80FFFF
#define LCD_COLOR_LIGHTMAGENTA  0xFFFF80FF
#define LCD_COLOR_LIGHTYELLOW   0xFFFFFF80
#define LCD_COLOR_DARKBLUE      0xFF000080
#define LCD_COLOR_DARKGREEN     0xFF008000
#define LCD_COLOR_DARKRED       0xFF800000
#define LCD_COLOR_DARKCYAN      0xFF008080
#define LCD_COLOR_DARKMAGENTA   0xFF800080
#define LCD_COLOR_DARKYELLOW    0xFF808000
#define LCD_COLOR_WHITE         0xFFFFFFFF
#define LCD_COLOR_LIGHTGRAY     0xFFD3D3D3
#define LCD_COLOR_GRAY          0xFF808080
#define LCD_COLOR_DARKGRAY      0xFF404040
#define LCD_COLOR_BLACK         0xFF000000
#define LCD_COLOR_BROWN         0xFFA52A2A
#define LCD_COLOR_ORANGE        0xFFFFA500
#define LCD_COLOR_TRANSPARENT   0xFF000000

#endif
//}}}

class cLcd {
public:
  //{{{  Point
  typedef struct {
    int16_t X;
    int16_t Y;
    } Point, *pPoint;
  //}}}
  typedef enum { CENTER_MODE = 0x01, RIGHT_MODE = 0x02, LEFT_MODE = 0x03 } Text_AlignModeTypdef;

  cLcd (int lines);
  void init();

  static uint32_t getWidth() { return 480; }
  static uint32_t getHeight() { return 272; }

  uint32_t getCameraBuffer();

  uint16_t GetTextHeight();
  uint32_t GetTextColor() { return mTextColor; }
  uint32_t GetBackColor() { return mBackColor; }
  void SetTextColor (uint32_t color) { mTextColor = color; }
  void SetBackColor (uint32_t color) { mBackColor = color; }

  // flipped display
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

  // drawing
  void SelectLayer (uint32_t LayerIndex);
  void SetTransparency (uint32_t LayerIndex, uint8_t Transparency);
  void SetAddress (uint32_t LayerIndex, uint32_t address, uint32_t writeAddress);

  uint32_t ReadPixel (uint16_t x, uint16_t y);
  void DrawPixel (uint16_t x, uint16_t y, uint32_t pixel);
  void DrawBitmap (uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp);

  void clearStringLine (uint32_t Line);
  void DisplayChar (uint16_t x, uint16_t y, uint8_t ascii);
  void DisplayStringAt (uint16_t x, uint16_t y, char* text, Text_AlignModeTypdef mode);
  void DisplayStringAtLine (uint16_t line, char* ptr);
  void DisplayStringAtLineColumn (uint16_t line, uint16_t column, char* ptr);

  void clear (uint32_t Color);
  void DrawRect (uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
  void FillRect (uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
  void DrawCircle (uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
  void FillCircle (uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
  void DrawPolygon (pPoint Points, uint16_t PointCount);
  void FillPolygon (pPoint Points, uint16_t PointCount);
  void DrawEllipse (int Xpos, int Ypos, int XRadius, int YRadius);
  void FillEllipse (int Xpos, int Ypos, int XRadius, int YRadius);
  void DrawLine (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

  void ConvertFrame (uint16_t* src, uint32_t dst, uint16_t xsize, uint16_t ysize);
  void ConvertFrameCpu (uint16_t* src, uint16_t srcXsize, uint16_t srcYsize,
                        uint32_t* dst, uint16_t xsize, uint16_t ysize);
  void ConvertFrameCpu1 (uint16_t* src, uint16_t srcXsize, uint16_t srcYsize,
                         uint32_t* dst, uint16_t xsize, uint16_t ysize);
  void ConvertFrameYuv (uint8_t* src, uint16_t srcXsize, uint16_t srcYsize,
                        uint8_t* dst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize);

  void DisplayOff();
  void DisplayOn();

  static cLcd* mLcd;

private:
  uint32_t* getBuffer();

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

  uint32_t mCurLayer = 0;
  uint32_t mTextColor = LCD_COLOR_WHITE;
  uint32_t mBackColor = LCD_COLOR_BLACK;

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
