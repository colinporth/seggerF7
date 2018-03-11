// stm32746g_lcd.cpp
//{{{  includes
#include "stm32746g_lcd.h"
#include "font.h"

extern const sFONT Font16;
//}}}
//{{{  defines
#define POLY_X(Z)           ((int32_t)((Points + Z)->X))
#define POLY_Y(Z)           ((int32_t)((Points + Z)->Y))
#define ABS(X)              ((X) > 0 ? (X) : -(X))

#define  RK043FN48H_HSYNC   ((uint16_t)41)   // Horizontal synchronization
#define  RK043FN48H_HBP     ((uint16_t)13)   // Horizontal back porch
#define  RK043FN48H_HFP     ((uint16_t)32)   // Horizontal front porch
#define  RK043FN48H_VSYNC   ((uint16_t)10)   // Vertical synchronization
#define  RK043FN48H_VBP     ((uint16_t)2)    // Vertical back porch
#define  RK043FN48H_VFP     ((uint16_t)2)    // Vertical front porch

#define  RK043FN48H_FREQUENCY_DIVIDER    5            // LCD Frequency divider
//}}}

LTDC_HandleTypeDef hLtdcHandler;
DMA2D_HandleTypeDef hDma2dHandler;
uint32_t ActiveLayer = 0;
LCD_DrawPropTypeDef DrawProp[MAX_LAYER_NUMBER];

uint32_t TextColor = LCD_COLOR_WHITE;
uint32_t BackColor = LCD_COLOR_BLACK;
//{{{
void FillTriangle (uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0,
  curpixel = 0;

  deltax = ABS(x2 - x1);        // The difference between the x's
  deltay = ABS(y2 - y1);        // The difference between the y's
  x = x1;                       // Start x off at the first pixel
  y = y1;                       // Start y off at the first pixel

  if (x2 >= x1) {                // The x-values are increasing
    xinc1 = 1;
    xinc2 = 1;
    }
  else {                         // The x-values are decreasing
    xinc1 = -1;
    xinc2 = -1;
    }

  if (y2 >= y1) {               // The y-values are increasing
    yinc1 = 1;
    yinc2 = 1;
    }
  else {                         // The y-values are decreasing
    yinc1 = -1;
    yinc2 = -1;
    }

  if (deltax >= deltay) {        // There is at least one x-value for every y-value
    xinc1 = 0;                  // Don't change the x when numerator >= denominator
    yinc2 = 0;                  // Don't change the y for every iteration
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         // There are more x-values than y-values
    }
  else {                         // There is at least one y-value for every x-value
    xinc2 = 0;                  // Don't change the x for every iteration
    yinc1 = 0;                  // Don't change the y when numerator >= denominator
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         // There are more y-values than x-values
    }

  for (curpixel = 0; curpixel <= num_pixels; curpixel++) {
    BSP_LCD_DrawLine(x, y, x3, y3);

    num += num_add;              // Increase the numerator by the top of the fraction
    if (num >= den)  {           // Check if numerator >= denominator
      num -= den;               // Calculate the new numerator value
      x += xinc1;               // Change the x as appropriate
      y += yinc1;               // Change the y as appropriate
      }
    x += xinc2;                 // Change the x as appropriate
    y += yinc2;                 // Change the y as appropriate
    }
  }
//}}}
//{{{
void FillBuffer (uint32_t LayerIndex, void* pDst, uint32_t xSize, uint32_t ySize,
                    uint32_t OffLine, uint32_t ColorIndex) {

  hDma2dHandler.Init.Mode = DMA2D_R2M;
  if (hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
    hDma2dHandler.Init.ColorMode = DMA2D_RGB565;
  else
    hDma2dHandler.Init.ColorMode = DMA2D_ARGB8888;
  hDma2dHandler.Init.OutputOffset = OffLine;

  if (HAL_DMA2D_Init (&hDma2dHandler) == HAL_OK)
    if (HAL_DMA2D_ConfigLayer (&hDma2dHandler, LayerIndex) == HAL_OK)
      if (HAL_DMA2D_Start (&hDma2dHandler, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK)
        HAL_DMA2D_PollForTransfer (&hDma2dHandler, 10);
}
//}}}
//{{{
void ConvertLineToARGB8888 (void* pSrc, void* pDst, uint32_t xSize, uint32_t ColorMode)
{
  hDma2dHandler.Init.Mode = DMA2D_M2M_PFC;
  hDma2dHandler.Init.ColorMode = DMA2D_ARGB8888;
  hDma2dHandler.Init.OutputOffset = 0;

  // Foreground Configuration
  hDma2dHandler.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hDma2dHandler.LayerCfg[1].InputAlpha = 0xFF;
  hDma2dHandler.LayerCfg[1].InputColorMode = ColorMode;
  hDma2dHandler.LayerCfg[1].InputOffset = 0;

  if (HAL_DMA2D_Init (&hDma2dHandler) == HAL_OK)
    if (HAL_DMA2D_ConfigLayer (&hDma2dHandler, 1) == HAL_OK)
      if (HAL_DMA2D_Start (&hDma2dHandler, (uint32_t)pSrc, (uint32_t)pDst, xSize, 1) == HAL_OK)
        HAL_DMA2D_PollForTransfer (&hDma2dHandler, 10);
  }
//}}}

//{{{
uint8_t BSP_LCD_Init() {

  // The RK043FN48H LCD 480x272 is selected
  hLtdcHandler.Init.HorizontalSync = (RK043FN48H_HSYNC - 1);
  hLtdcHandler.Init.VerticalSync = (RK043FN48H_VSYNC - 1);
  hLtdcHandler.Init.AccumulatedHBP = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
  hLtdcHandler.Init.AccumulatedVBP = (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
  hLtdcHandler.Init.AccumulatedActiveH = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
  hLtdcHandler.Init.AccumulatedActiveW = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
  hLtdcHandler.Init.TotalHeigh = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);
  hLtdcHandler.Init.TotalWidth = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1);

  // LCD clock configuration
  BSP_LCD_ClockConfig (&hLtdcHandler, NULL);

  // Initialize the LCD pixel width and pixel height
  hLtdcHandler.LayerCfg->ImageWidth  = RK043FN48H_WIDTH;
  hLtdcHandler.LayerCfg->ImageHeight = RK043FN48H_HEIGHT;

  // Background value
  hLtdcHandler.Init.Backcolor.Blue = 0;
  hLtdcHandler.Init.Backcolor.Green = 0;
  hLtdcHandler.Init.Backcolor.Red = 0;

  // Polarity
  hLtdcHandler.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hLtdcHandler.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hLtdcHandler.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hLtdcHandler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hLtdcHandler.Instance = LTDC;

  if(HAL_LTDC_GetState(&hLtdcHandler) == HAL_LTDC_STATE_RESET)
    BSP_LCD_MspInit (&hLtdcHandler, NULL);
  HAL_LTDC_Init (&hLtdcHandler);

  // Assert display enable LCD_DISP pin
  HAL_GPIO_WritePin (LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);

  // Assert backlight LCD_BL_CTRL pin
  HAL_GPIO_WritePin (LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);

  BSP_SDRAM_Init();

  hDma2dHandler.Instance = DMA2D;

  return LCD_OK;
  }
//}}}
//{{{
uint8_t BSP_LCD_DeInit() {

  // Initialize the hLtdcHandler Instance parameter
  hLtdcHandler.Instance = LTDC;

 // Disable LTDC block //
  __HAL_LTDC_DISABLE (&hLtdcHandler);

  // DeInit the LTDC
  HAL_LTDC_DeInit (&hLtdcHandler);

  // DeInit the LTDC MSP : this __weak function can be rewritten by the application
  BSP_LCD_MspDeInit (&hLtdcHandler, NULL);

  return LCD_OK;
  }
//}}}
//{{{
void BSP_LCD_LayerDefaultInit (uint16_t LayerIndex, uint32_t FB_Address) {

  LCD_LayerCfgTypeDef  layer_cfg;

  // Layer Init
  layer_cfg.WindowX0 = 0;
  layer_cfg.WindowX1 = BSP_LCD_GetXSize();
  layer_cfg.WindowY0 = 0;
  layer_cfg.WindowY1 = BSP_LCD_GetYSize();
  layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  layer_cfg.FBStartAdress = FB_Address;
  layer_cfg.Alpha = 255;
  layer_cfg.Alpha0 = 0;
  layer_cfg.Backcolor.Blue = 0;
  layer_cfg.Backcolor.Green = 0;
  layer_cfg.Backcolor.Red = 0;
  layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  layer_cfg.ImageWidth = BSP_LCD_GetXSize();
  layer_cfg.ImageHeight = BSP_LCD_GetYSize();

  HAL_LTDC_ConfigLayer(&hLtdcHandler, &layer_cfg, LayerIndex);
  }
//}}}

void BSP_LCD_SelectLayer (uint32_t LayerIndex) { ActiveLayer = LayerIndex; }
//{{{
void BSP_LCD_SetLayerVisible (uint32_t LayerIndex, FunctionalState State)
{
  if(State == ENABLE)
    __HAL_LTDC_LAYER_ENABLE(&hLtdcHandler, LayerIndex);
  else
    __HAL_LTDC_LAYER_DISABLE(&hLtdcHandler, LayerIndex);
  __HAL_LTDC_RELOAD_CONFIG(&hLtdcHandler);
}
//}}}
//{{{
void BSP_LCD_SetLayerVisible_NoReload (uint32_t LayerIndex, FunctionalState State)
{
  if(State == ENABLE)
    __HAL_LTDC_LAYER_ENABLE(&hLtdcHandler, LayerIndex);
  else
    __HAL_LTDC_LAYER_DISABLE(&hLtdcHandler, LayerIndex);
  // Do not Sets the Reload
}
//}}}
//{{{
void BSP_LCD_SetTransparency (uint32_t LayerIndex, uint8_t Transparency) {
  HAL_LTDC_SetAlpha(&hLtdcHandler, Transparency, LayerIndex);
  }
//}}}
//{{{
void BSP_LCD_SetTransparency_NoReload (uint32_t LayerIndex, uint8_t Transparency) {
  HAL_LTDC_SetAlpha_NoReload(&hLtdcHandler, Transparency, LayerIndex);
  }
//}}}
//{{{
void BSP_LCD_SetLayerAddress (uint32_t LayerIndex, uint32_t Address)
{
  HAL_LTDC_SetAddress(&hLtdcHandler, Address, LayerIndex);
}
//}}}
//{{{
void BSP_LCD_SetLayerAddress_NoReload (uint32_t LayerIndex, uint32_t Address) {
  HAL_LTDC_SetAddress_NoReload(&hLtdcHandler, Address, LayerIndex);
  }

//}}}
void BSP_LCD_Reload (uint32_t ReloadType) { HAL_LTDC_Reload (&hLtdcHandler, ReloadType); }

uint32_t BSP_LCD_GetTextColor() { return TextColor; }
uint32_t BSP_LCD_GetBackColor() { return BackColor; }
void BSP_LCD_SetTextColor (uint32_t Color) { TextColor = Color; }
void BSP_LCD_SetBackColor (uint32_t Color) { BackColor = Color; }

uint16_t BSP_LCD_GetTextHeight() { return Font16.mHeight; }

//{{{
uint32_t BSP_LCD_ReadPixel (uint16_t Xpos, uint16_t Ypos) {

  uint32_t ret = 0;

  if (hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
    // Read data value from SDRAM memory
    ret = *(__IO uint32_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (4*(Ypos*BSP_LCD_GetXSize() + Xpos)));
  else if (hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
    // Read data value from SDRAM memory
    ret = (*(__IO uint32_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (4*(Ypos*BSP_LCD_GetXSize() + Xpos))) & 0x00FFFFFF);
  else if ((hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
           (hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
           (hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))
    // Read data value from SDRAM memory
    ret = *(__IO uint16_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*BSP_LCD_GetXSize() + Xpos)));
  else
    // Read data value from SDRAM memory
    ret = *(__IO uint8_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*BSP_LCD_GetXSize() + Xpos)));

  return ret;
  }
//}}}

//{{{
void BSP_LCD_ClearStringLine (uint32_t Line) {

  // Draw rectangle with background color
  uint32_t color_backup = TextColor;
  TextColor = BackColor;
  BSP_LCD_FillRect (0, Line * Font16.mHeight, BSP_LCD_GetXSize(), Font16.mHeight);
  TextColor = color_backup;
  }
//}}}
//{{{
void BSP_LCD_DisplayChar (uint16_t x, uint16_t y, uint8_t ascii) {

  const uint16_t width = Font16.mWidth;
  const uint16_t byteAlignedWidth = (width+7)/8;
  const uint16_t offset = 8*(byteAlignedWidth) - width-1;
  const uint8_t* fontChar = &Font16.mTable [(ascii-' ') * Font16.mHeight * byteAlignedWidth];
  auto fbPtr = ((uint32_t*)hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress) + (y * BSP_LCD_GetXSize()) + x;

  for (auto fontLine = 0u; fontLine < Font16.mHeight; fontLine++) {
    auto fontPtr = (uint8_t*)fontChar + byteAlignedWidth * fontLine;
    uint16_t fontLineBits = *fontPtr++;
    if (byteAlignedWidth == 2)
      fontLineBits = (fontLineBits << 8) | *fontPtr;
    if (fontLineBits) {
      uint16_t bit = 1 << (width + offset);
      auto endPtr = fbPtr + width;
      while (fbPtr != endPtr) {
        if (fontLineBits & bit)
          *fbPtr = TextColor;
        fbPtr++;
        bit >>= 1;
        }
      fbPtr += BSP_LCD_GetXSize() - width;
      }
    else
      fbPtr += BSP_LCD_GetXSize();
    }
  }
//}}}
//{{{
void BSP_LCD_DisplayStringAt (uint16_t x, uint16_t y, char* text, Text_AlignModeTypdef mode) {

  uint16_t column = 1;
  switch (mode) {
    case CENTER_MODE:  {
      uint32_t xSize = BSP_LCD_GetXSize() / Font16.mWidth;
      char* ptr = text;
      uint32_t size = 0;
      while (*ptr++)
        size++;
      column = x + ((xSize - size) * Font16.mWidth) / 2;
      break;
      }

    case RIGHT_MODE: {
      uint32_t xSize = BSP_LCD_GetXSize() / Font16.mWidth;
      char* ptr = text;
      uint32_t size = 0;
      while (*ptr++)
        size++;
      column = -x + ((xSize - size) * Font16.mWidth);
      break;
      }

    case LEFT_MODE:
      column = x;
      break;
    }

  // Check that the start x is on screen
  if ((column < 1) || (column >= BSP_LCD_GetXSize()))
    column = 1;

  while (*text && (column + Font16.mWidth < BSP_LCD_GetXSize())) {
    BSP_LCD_DisplayChar (column, y, *text++);
    column += Font16.mWidth;
    }
  }
//}}}
//{{{
void BSP_LCD_DisplayStringAtLine (uint16_t line, char* ptr) {
  BSP_LCD_DisplayStringAt (0, line * Font16.mHeight, ptr, LEFT_MODE);
  }
//}}}
//{{{
void BSP_LCD_DisplayStringAtLineColumn (uint16_t line, uint16_t column, char* ptr) {
  BSP_LCD_DisplayStringAt (column * Font16.mWidth, line * Font16.mHeight, ptr, LEFT_MODE);
  }
//}}}

//{{{
void BSP_LCD_DrawHLine (uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t  Xaddress = 0;

  // Get the line address
  if(hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
    Xaddress = (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(BSP_LCD_GetXSize()*Ypos + Xpos);
  else
    Xaddress = (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress) + 4*(BSP_LCD_GetXSize()*Ypos + Xpos);

  // Write line
  FillBuffer  (ActiveLayer, (uint32_t *)Xaddress, Length, 1, 0, TextColor);
  }
//}}}
//{{{
void BSP_LCD_DrawVLine (uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t  Xaddress = 0;

  // Get the line address
  if (hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
    Xaddress = (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(BSP_LCD_GetXSize()*Ypos + Xpos);
  else
    Xaddress = (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress) + 4*(BSP_LCD_GetXSize()*Ypos + Xpos);

  // Write line
  FillBuffer(ActiveLayer, (uint32_t *)Xaddress, 1, Length, (BSP_LCD_GetXSize() - 1), TextColor);
}
//}}}
//{{{
void BSP_LCD_DrawLine (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0,
  curpixel = 0;

  deltax = ABS(x2 - x1);        // The difference between the x's
  deltay = ABS(y2 - y1);        // The difference between the y's
  x = x1;                       // Start x off at the first pixel
  y = y1;                       // Start y off at the first pixel

  if (x2 >= x1)                 // The x-values are increasing
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          // The x-values are decreasing
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1)                 // The y-values are increasing
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          // The y-values are decreasing
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)         // There is at least one x-value for every y-value
  {
    xinc1 = 0;                  // Don't change the x when numerator >= denominator
    yinc2 = 0;                  // Don't change the y for every iteration
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         // There are more x-values than y-values
  }
  else                          // There is at least one y-value for every x-value
  {
    xinc2 = 0;                  // Don't change the x for every iteration
    yinc1 = 0;                  // Don't change the y when numerator >= denominator
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         // There are more y-values than x-values
  }

  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
  {
    BSP_LCD_DrawPixel(x, y, TextColor);   // Draw the current pixel
    num += num_add;                            // Increase the numerator by the top of the fraction
    if (num >= den)                           // Check if numerator >= denominator
    {
      num -= den;                             // Calculate the new numerator value
      x += xinc1;                             // Change the x as appropriate
      y += yinc1;                             // Change the y as appropriate
    }
    x += xinc2;                               // Change the x as appropriate
    y += yinc2;                               // Change the y as appropriate
  }
}
//}}}
//{{{
void BSP_LCD_DrawRect (uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  // Draw horizontal lines
  BSP_LCD_DrawHLine(Xpos, Ypos, Width);
  BSP_LCD_DrawHLine(Xpos, (Ypos+ Height), Width);

  // Draw vertical lines
  BSP_LCD_DrawVLine(Xpos, Ypos, Height);
  BSP_LCD_DrawVLine((Xpos + Width), Ypos, Height);
}
//}}}
//{{{
void BSP_LCD_DrawCircle (uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t   decision;    // Decision Variable
  uint32_t  current_x;   // Current X Value
  uint32_t  current_y;   // Current Y Value

  decision = 3 - (Radius << 1);
  current_x = 0;
  current_y = Radius;

  while (current_x <= current_y) {
    BSP_LCD_DrawPixel ((Xpos + current_x), (Ypos - current_y), TextColor);
    BSP_LCD_DrawPixel ((Xpos - current_x), (Ypos - current_y), TextColor);
    BSP_LCD_DrawPixel ((Xpos + current_y), (Ypos - current_x), TextColor);
    BSP_LCD_DrawPixel ((Xpos - current_y), (Ypos - current_x), TextColor);
    BSP_LCD_DrawPixel ((Xpos + current_x), (Ypos + current_y), TextColor);
    BSP_LCD_DrawPixel ((Xpos - current_x), (Ypos + current_y), TextColor);
    BSP_LCD_DrawPixel ((Xpos + current_y), (Ypos + current_x), TextColor);
    BSP_LCD_DrawPixel ((Xpos - current_y), (Ypos + current_x), TextColor);

    if (decision < 0)
      decision += (current_x << 2) + 6;
    else {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
      }
    current_x++;
  }
}
//}}}
//{{{
void BSP_LCD_DrawPolygon (pPoint Points, uint16_t PointCount)
{
  int16_t x = 0, y = 0;

  if (PointCount < 2)
    return;

  BSP_LCD_DrawLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);

  while(--PointCount)
  {
    x = Points->X;
    y = Points->Y;
    Points++;
    BSP_LCD_DrawLine(x, y, Points->X, Points->Y);
  }
}
//}}}
//{{{
void BSP_LCD_DrawEllipse (int Xpos, int Ypos, int XRadius, int YRadius)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;

  rad1 = XRadius;
  rad2 = YRadius;

  k = (float)(rad2/rad1);

  do {
    BSP_LCD_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos+y), TextColor);
    BSP_LCD_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos+y), TextColor);
    BSP_LCD_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos-y), TextColor);
    BSP_LCD_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos-y), TextColor);

    e2 = err;
    if (e2 <= x) {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;
  }
  while (y <= 0);
}
//}}}
//{{{
void BSP_LCD_DrawPixel (uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code) {
  // Write data value to all SDRAM memory
  if (hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
    *(__IO uint16_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*BSP_LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;
  else
    *(__IO uint32_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (4*(Ypos*BSP_LCD_GetXSize() + Xpos))) = RGB_Code;
  }
//}}}
//{{{
void BSP_LCD_DrawBitmap (uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp)
{
  uint32_t index = 0, width = 0, height = 0, bit_pixel = 0;
  uint32_t address;
  uint32_t input_color_mode = 0;

  // Get bitmap data address offset
  index = pbmp[10] + (pbmp[11] << 8) + (pbmp[12] << 16)  + (pbmp[13] << 24);

  // Read bitmap width
  width = pbmp[18] + (pbmp[19] << 8) + (pbmp[20] << 16)  + (pbmp[21] << 24);

  // Read bitmap height
  height = pbmp[22] + (pbmp[23] << 8) + (pbmp[24] << 16)  + (pbmp[25] << 24);

  // Read bit/pixel
  bit_pixel = pbmp[28] + (pbmp[29] << 8);

  // Set the address
  address = hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (((BSP_LCD_GetXSize()*Ypos) + Xpos)*(4));

  // Get the layer pixel format
  if ((bit_pixel/8) == 4)
    input_color_mode = CM_ARGB8888;
  else if ((bit_pixel/8) == 2)
    input_color_mode = CM_RGB565;
  else
    input_color_mode = CM_RGB888;

  // Bypass the bitmap header
  pbmp += (index + (width * (height - 1) * (bit_pixel/8)));

  // Convert picture to ARGB8888 pixel format
  for(index=0; index < height; index++)
  {
    // Pixel format conversion
    ConvertLineToARGB8888((uint32_t *)pbmp, (uint32_t *)address, width, input_color_mode);

    // Increment the source and destination buffers
    address+=  (BSP_LCD_GetXSize()*4);
    pbmp -= width*(bit_pixel/8);
  }
}
//}}}

//{{{
void BSP_LCD_Clear (uint32_t Color) {
  FillBuffer (ActiveLayer, (uint32_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress), BSP_LCD_GetXSize(), BSP_LCD_GetYSize(), 0, Color);
  }
//}}}
//{{{
void BSP_LCD_FillRect (uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  uint32_t  x_address = 0;

  // Get the rectangle start address
  if (hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
    x_address = (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(BSP_LCD_GetXSize()*Ypos + Xpos);
  else
    x_address = (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress) + 4*(BSP_LCD_GetXSize()*Ypos + Xpos);

  // Fill the rectangle
  FillBuffer (ActiveLayer, (uint32_t *)x_address, Width, Height, (BSP_LCD_GetXSize() - Width), TextColor);
}
//}}}
//{{{
void BSP_LCD_FillCircle (uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  decision;     // Decision Variable
  uint32_t  current_x;   // Current X Value
  uint32_t  current_y;   // Current Y Value

  decision = 3 - (Radius << 1);

  current_x = 0;
  current_y = Radius;

  while (current_x <= current_y) {
    if(current_y > 0) {
      BSP_LCD_DrawHLine(Xpos - current_y, Ypos + current_x, 2*current_y);
      BSP_LCD_DrawHLine(Xpos - current_y, Ypos - current_x, 2*current_y);
    }

    if(current_x > 0) {
      BSP_LCD_DrawHLine(Xpos - current_x, Ypos - current_y, 2*current_x);
      BSP_LCD_DrawHLine(Xpos - current_x, Ypos + current_y, 2*current_x);
    }
    if (decision < 0)
      decision += (current_x << 2) + 6;
    else {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  }

  BSP_LCD_DrawCircle(Xpos, Ypos, Radius);
}
//}}}
//{{{
void BSP_LCD_FillPolygon (pPoint Points, uint16_t PointCount)
{
  int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0, Y_first = 0, pixelX = 0, pixelY = 0, counter = 0;
  uint16_t  image_left = 0, image_right = 0, image_top = 0, image_bottom = 0;

  image_left = image_right = Points->X;
  image_top= image_bottom = Points->Y;

  for(counter = 1; counter < PointCount; counter++) {
    pixelX = POLY_X(counter);
    if(pixelX < image_left)
      image_left = pixelX;
    if(pixelX > image_right)
      image_right = pixelX;

    pixelY = POLY_Y(counter);
    if(pixelY < image_top)
      image_top = pixelY;
    if(pixelY > image_bottom)
      image_bottom = pixelY;
  }

  if(PointCount < 2)
    return;

  X_center = (image_left + image_right)/2;
  Y_center = (image_bottom + image_top)/2;

  X_first = Points->X;
  Y_first = Points->Y;

  while(--PointCount) {
    X = Points->X;
    Y = Points->Y;
    Points++;
    X2 = Points->X;
    Y2 = Points->Y;

    FillTriangle(X, X2, X_center, Y, Y2, Y_center);
    FillTriangle(X, X_center, X2, Y, Y_center, Y2);
    FillTriangle(X_center, X2, X, Y_center, Y2, Y);
    }

  FillTriangle(X_first, X2, X_center, Y_first, Y2, Y_center);
  FillTriangle(X_first, X_center, X2, Y_first, Y_center, Y2);
  FillTriangle(X_center, X2, X_first, Y_center, Y2, Y_first);
  }
//}}}
//{{{
void BSP_LCD_FillEllipse (int Xpos, int Ypos, int XRadius, int YRadius) {

  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;

  rad1 = XRadius;
  rad2 = YRadius;

  k = (float)(rad2/rad1);
  do {
    BSP_LCD_DrawHLine((Xpos-(uint16_t)(x/k)), (Ypos+y), (2*(uint16_t)(x/k) + 1));
    BSP_LCD_DrawHLine((Xpos-(uint16_t)(x/k)), (Ypos-y), (2*(uint16_t)(x/k) + 1));

    e2 = err;
    if (e2 <= x) {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
      }
    if (e2 > y) err += ++y*2+1;
    }

  while (y <= 0);
  }
//}}}

//{{{
void BSP_LCD_DisplayOn() {

  // Display On
  __HAL_LTDC_ENABLE(&hLtdcHandler);
  HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);        // Assert LCD_DISP pin
  HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);  // Assert LCD_BL_CTRL pin
}
//}}}
//{{{
void BSP_LCD_DisplayOff()
{
  // Display Off
  __HAL_LTDC_DISABLE(&hLtdcHandler);
  HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_RESET);      // De-assert LCD_DISP pin
  HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_RESET);// De-assert LCD_BL_CTRL pin
}
//}}}

//{{{
__weak void BSP_LCD_MspInit (LTDC_HandleTypeDef* hltdc, void* Params) {

  GPIO_InitTypeDef gpio_init_structure;

  // Enable the LTDC and DMA2D clocks
  __HAL_RCC_LTDC_CLK_ENABLE();
  __HAL_RCC_DMA2D_CLK_ENABLE();

  // Enable GPIOs clock
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  LCD_DISP_GPIO_CLK_ENABLE();
  LCD_BL_CTRL_GPIO_CLK_ENABLE();

  // LTDC Pins configuration **
  // GPIOE configuration
  gpio_init_structure.Pin       = GPIO_PIN_4;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  // GPIOG configuration
  gpio_init_structure.Pin       = GPIO_PIN_12;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF9_LTDC;
  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

  // GPIOI LTDC alternate configuration
  gpio_init_structure.Pin       = GPIO_PIN_9 | GPIO_PIN_10 | \
                                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOI, &gpio_init_structure);

  // GPIOJ configuration
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                                  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | \
                                  GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOJ, &gpio_init_structure);

  // GPIOK configuration
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
                                  GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOK, &gpio_init_structure);

  // LCD_DISP GPIO configuration
  gpio_init_structure.Pin       = LCD_DISP_PIN;     // LCD_DISP pin has to be manually controlled
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(LCD_DISP_GPIO_PORT, &gpio_init_structure);

  // LCD_BL_CTRL GPIO configuration
  gpio_init_structure.Pin       = LCD_BL_CTRL_PIN;  // LCD_BL_CTRL pin has to be manually controlled
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);
  }
//}}}
//{{{
__weak void BSP_LCD_MspDeInit (LTDC_HandleTypeDef *hltdc, void *Params) {

  GPIO_InitTypeDef  gpio_init_structure;

  // Disable LTDC block
  __HAL_LTDC_DISABLE (hltdc);

  // LTDC Pins deactivation

  // GPIOE deactivation
  gpio_init_structure.Pin       = GPIO_PIN_4;
  HAL_GPIO_DeInit (GPIOE, gpio_init_structure.Pin);

  // GPIOG deactivation
  gpio_init_structure.Pin       = GPIO_PIN_12;
  HAL_GPIO_DeInit (GPIOG, gpio_init_structure.Pin);

  // GPIOI deactivation
  gpio_init_structure.Pin       = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_12 | \
                                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_DeInit (GPIOI, gpio_init_structure.Pin);

  // GPIOJ deactivation
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                                  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | \
                                  GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_DeInit (GPIOJ, gpio_init_structure.Pin);

  // GPIOK deactivation
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
                                  GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  HAL_GPIO_DeInit (GPIOK, gpio_init_structure.Pin);

  // Disable LTDC clock
  __HAL_RCC_LTDC_CLK_DISABLE();

  // GPIO pins clock can be shut down in the application  by surcharging this __weak function
  }
//}}}
//{{{
__weak void BSP_LCD_ClockConfig (LTDC_HandleTypeDef *hltdc, void *Params) {

  static RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;

  // RK043FN48H LCD clock configuration
  // PLLSAI_VCO Input = HSE_VALUE/PLLM = 1 Mhz
  // PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz
  // PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/5 = 38.4 Mhz
  // LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_4 = 38.4/4 = 9.6Mhz
  periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  periph_clk_init_struct.PLLSAI.PLLSAIN = 192;
  periph_clk_init_struct.PLLSAI.PLLSAIR = RK043FN48H_FREQUENCY_DIVIDER;
  periph_clk_init_struct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
  HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);
  }
//}}}
