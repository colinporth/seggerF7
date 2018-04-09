#pragma once
//{{{  includes
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sdram.h"
//}}}
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}
#define RGB565
//{{{  LCD_DrawPropTypeDef
typedef struct {
  uint32_t TextColor;
  uint32_t BackColor;
  } LCD_DrawPropTypeDef;
//}}}
//{{{  Point
typedef struct {
  int16_t X;
  int16_t Y;
  } Point, * pPoint;
//}}}
//{{{  Text_AlignModeTypdef
typedef enum {
  CENTER_MODE             = 0x01,    /* Center mode */
  RIGHT_MODE              = 0x02,    /* Right mode  */
  LEFT_MODE               = 0x03     /* Left mode   */
  }Text_AlignModeTypdef;
//}}}
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
//{{{  other defines
#define MAX_LAYER_NUMBER        ((uint32_t)2)
#define LCD_LayerCfgTypeDef     LTDC_LayerCfgTypeDef
#define LTDC_ACTIVE_LAYER       ((uint32_t)1) /* Layer 1 */

#define LCD_OK                  ((uint8_t)0x00)
#define LCD_ERROR               ((uint8_t)0x01)
#define LCD_TIMEOUT             ((uint8_t)0x02)

#define LCD_RELOAD_IMMEDIATE           ((uint32_t)LTDC_SRCR_IMR)
#define LCD_RELOAD_VERTICAL_BLANKING   ((uint32_t)LTDC_SRCR_VBR)

#define LCD_DISP_PIN                    GPIO_PIN_12
#define LCD_DISP_GPIO_PORT              GPIOI
#define LCD_DISP_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOI_CLK_ENABLE()
#define LCD_DISP_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOI_CLK_DISABLE()

#define LCD_BL_CTRL_PIN                  GPIO_PIN_3
#define LCD_BL_CTRL_GPIO_PORT            GPIOK
#define LCD_BL_CTRL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOK_CLK_ENABLE()
#define LCD_BL_CTRL_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOK_CLK_DISABLE()

#define  RK043FN48H_WIDTH   480
#define  RK043FN48H_HEIGHT  272
//}}}

uint8_t BSP_LCD_Init();
uint8_t BSP_LCD_DeInit();

inline uint32_t BSP_LCD_GetXSize() { return RK043FN48H_WIDTH; }
inline uint32_t BSP_LCD_GetYSize() { return RK043FN48H_HEIGHT; }

void BSP_LCD_LayerDefaultInit (uint16_t LayerIndex, uint32_t FrameBuffer);
void BSP_LCD_SetTransparency (uint32_t LayerIndex, uint8_t Transparency);
void BSP_LCD_SelectLayer (uint32_t LayerIndex);
void BSP_LCD_SetLayerVisible (uint32_t LayerIndex, FunctionalState State);
void BSP_LCD_Reload (uint32_t ReloadType);

uint16_t BSP_LCD_GetTextHeight();
uint32_t BSP_LCD_GetTextColor();
uint32_t BSP_LCD_GetBackColor();

void BSP_LCD_SetTextColor (uint32_t Color);
void BSP_LCD_SetBackColor (uint32_t Color);

uint32_t BSP_LCD_ReadPixel (uint16_t x, uint16_t y);
void BSP_LCD_DrawPixel (uint16_t x, uint16_t y, uint32_t pixel);

void BSP_LCD_Clear (uint32_t Color);
void BSP_LCD_ClearStringLine (uint32_t Line);
void BSP_LCD_DisplayChar (uint16_t x, uint16_t y, uint8_t ascii);
void BSP_LCD_DisplayStringAt (uint16_t x, uint16_t y, char* text, Text_AlignModeTypdef mode);
void BSP_LCD_DisplayStringAtLine (uint16_t line, char* ptr);
void BSP_LCD_DisplayStringAtLineColumn (uint16_t line, uint16_t column, char* ptr);

void BSP_LCD_DrawLine (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void BSP_LCD_DrawRect (uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void BSP_LCD_DrawCircle (uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void BSP_LCD_DrawPolygon (pPoint Points, uint16_t PointCount);
void BSP_LCD_DrawEllipse (int Xpos, int Ypos, int XRadius, int YRadius);
void BSP_LCD_DrawBitmap (uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp);

void BSP_LCD_FillRect (uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void BSP_LCD_FillCircle (uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void BSP_LCD_FillPolygon (pPoint Points, uint16_t PointCount);
void BSP_LCD_FillEllipse (int Xpos, int Ypos, int XRadius, int YRadius);

void BSP_LCD_ConvertFrame (uint16_t* src, uint32_t dst, uint16_t xsize, uint16_t ysize);
void BSP_LCD_ConvertFrameCpu (uint16_t* src, uint16_t srcXsize, uint16_t srcYsize,
                               uint32_t* dst, uint16_t xsize, uint16_t ysize);
void BSP_LCD_ConvertFrameCpu1 (uint16_t* src, uint16_t srcXsize, uint16_t srcYsize,
                               uint32_t* dst, uint16_t xsize, uint16_t ysize);
void BSP_LCD_ConvertFrameYuv (uint8_t* src, uint16_t srcXsize, uint16_t srcYsize,
                              uint8_t* dst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize);

void BSP_LCD_DisplayOff();
void BSP_LCD_DisplayOn();

void BSP_LCD_MspInit (LTDC_HandleTypeDef *hltdc, void *Params);
void BSP_LCD_MspDeInit (LTDC_HandleTypeDef *hltdc, void *Params);
void BSP_LCD_ClockConfig (LTDC_HandleTypeDef *hltdc, void *Params);

//{{{
#ifdef __cplusplus
}
#endif
//}}}
