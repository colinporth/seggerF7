// cLcd.cpp
//{{{  includes
#include "cLcd.h"

#include "../common/system.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_sdram.h"

#include "font.h"
extern const sFONT Font16;
//}}}
//{{{  defines
#define POLY_X(Z)  ((int32_t)((Points + Z)->X))
#define POLY_Y(Z)  ((int32_t)((Points + Z)->Y))
#define ABS(X)     ((X) > 0 ? (X) : -(X))

#define LCD_DISP_PIN                    GPIO_PIN_12
#define LCD_DISP_GPIO_PORT              GPIOI
#define LCD_DISP_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOI_CLK_ENABLE()
#define LCD_DISP_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOI_CLK_DISABLE()

#define LCD_BL_CTRL_PIN                  GPIO_PIN_3
#define LCD_BL_CTRL_GPIO_PORT            GPIOK
#define LCD_BL_CTRL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOK_CLK_ENABLE()
#define LCD_BL_CTRL_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOK_CLK_DISABLE()
//}}}

LTDC_HandleTypeDef hLtdcHandler;
DMA2D_HandleTypeDef hDma2dHandler;
cLcd* cLcd::mLcd = nullptr;

extern "C" {
  //{{{
  void LTDC_IRQHandler() {

    // line Interrupt
    if (__HAL_LTDC_GET_FLAG (&hLtdcHandler, LTDC_FLAG_LI) != RESET)
      if (__HAL_LTDC_GET_IT_SOURCE (&hLtdcHandler, LTDC_IT_LI) != RESET) {
        __HAL_LTDC_DISABLE_IT (&hLtdcHandler, LTDC_IT_LI);
        __HAL_LTDC_CLEAR_FLAG (&hLtdcHandler, LTDC_FLAG_LI);
        }

    // register reload Interrupt
    if (__HAL_LTDC_GET_FLAG (&hLtdcHandler, LTDC_FLAG_RR) != RESET)
      if (__HAL_LTDC_GET_IT_SOURCE (&hLtdcHandler, LTDC_IT_RR) != RESET) {
        //__HAL_LTDC_DISABLE_IT (&hLtdcHandler, LTDC_IT_RR);
        __HAL_LTDC_CLEAR_FLAG (&hLtdcHandler, LTDC_FLAG_RR);

        cLcd::mLcd->debug (LCD_COLOR_YELLOW, "frame");
        // Register reload interrupt Callback
        //HAL_LTDC_ReloadEventCallback (&hLtdcHandler);
        }
    }
  //}}}
  //{{{
  void LTDC_ER_IRQHandler() {

    // transfer Error Interrupt
    if (__HAL_LTDC_GET_FLAG (&hLtdcHandler, LTDC_FLAG_TE) != RESET)
      if (__HAL_LTDC_GET_IT_SOURCE (&hLtdcHandler, LTDC_IT_TE) != RESET) {
        __HAL_LTDC_DISABLE_IT (&hLtdcHandler, LTDC_IT_TE);
        __HAL_LTDC_CLEAR_FLAG (&hLtdcHandler, LTDC_FLAG_TE);
        }

    // FIFO underrun Interrupt
    if (__HAL_LTDC_GET_FLAG (&hLtdcHandler, LTDC_FLAG_FU) != RESET)
      if (__HAL_LTDC_GET_IT_SOURCE (&hLtdcHandler, LTDC_IT_FU) != RESET) {
        __HAL_LTDC_DISABLE_IT (&hLtdcHandler, LTDC_IT_FU);
        __HAL_LTDC_CLEAR_FLAG (&hLtdcHandler, LTDC_FLAG_FU);
        }
    }
  //}}}
  }

cLcd::cLcd (int lines) : mDisplayLines(lines) {}
//{{{
void cLcd::init() {

  mLcd = this;

  //{{{  gpio config
  // Enable GPIOs clock
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  LCD_DISP_GPIO_CLK_ENABLE();
  LCD_BL_CTRL_GPIO_CLK_ENABLE();

  // gpio configuration
  GPIO_InitTypeDef gpio_init_structure;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FAST;

  // GPIOG configuration
  gpio_init_structure.Pin       = GPIO_PIN_12;
  gpio_init_structure.Alternate = GPIO_AF9_LTDC;
  HAL_GPIO_Init (GPIOG, &gpio_init_structure);

  // GPIOI LTDC alternate configuration
  gpio_init_structure.Pin       = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init (GPIOI, &gpio_init_structure);

  gpio_init_structure.Pin       = GPIO_PIN_4;
  HAL_GPIO_Init (GPIOE, &gpio_init_structure);

  // GPIOJ configuration
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |
                                  GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |
                                  GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init (GPIOJ, &gpio_init_structure);

  // GPIOK configuration
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
                                  GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  HAL_GPIO_Init (GPIOK, &gpio_init_structure);

  // LCD_DISP GPIO configuration
  gpio_init_structure.Pin = LCD_DISP_PIN;     // LCD_DISP pin has to be manually controlled
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init (LCD_DISP_GPIO_PORT, &gpio_init_structure);

  // LCD_BL_CTRL GPIO configuration
  gpio_init_structure.Pin = LCD_BL_CTRL_PIN;  // LCD_BL_CTRL pin has to be manually controlled
  HAL_GPIO_Init (LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);
  //}}}
  BSP_SDRAM_Init();
  //{{{  init hLtdcHandler, ltdc
  //{{{  original
  // RK043FN48H LCD clock configuration
  // PLLSAI_VCO Input  = HSE_VALUE/PLLM              - 1 Mhz
  // PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN  - 192 Mhz
  // PLLLCDCLK         = PLLSAI_VCO_Output / PLLSAIR - 192 / 5 = 38.4 Mhz
  // LTDCclock         = PLLLCDCLK / PLLSAI_DIVR_4   - 38.4 / 4 = 9.6Mhz
  //#define  RK043FN48H_PLLSAIR 5
  //#define  RK043FN48H_WIDTH  480
  //#define  RK043FN48H_HEIGHT  272
  //
  // H - 480 + 41 + 13 + 32 = 566
  //#define  RK043FN48H_HSYNC   41  // Horizontal synchronization
  //#define  RK043FN48H_HBP     13  // Horizontal back porch
  //#define  RK043FN48H_HFP     32  // Horizontal front porch
  // V - 272 + 10 + 2 + 2 = 286
  //#define  RK043FN48H_VSYNC   10  // Vertical synchronization
  //#define  RK043FN48H_VBP     2   // Vertical back porch
  //#define  RK043FN48H_VFP     2   // Vertical front porch
  // 9.6Mhz / 566*286  = 59.30Hz
  //}}}
  // PLLSAI_VCO Input  = HSE_VALUE/PLLM              - 1Mhz
  // PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN  - 192Mhz
  // PLLLCDCLK         = PLLSAI_VCO_Output / PLLSAIR - 192 / 7 = 27.428Mhz
  // LTDCclock         = PLLLCDCLK / PLLSAI_DIVR_4   - 27.428 / 4 = 6Mhz
  #define  RK043FN48H_PLLSAIR 7

  // minH - 480 + 8 + 2 = 490
  #define  RK043FN48H_HSYNC   1  // Horizontal synchronization
  #define  RK043FN48H_HBP     7  // Horizontal back porch
  #define  RK043FN48H_HFP     2  // Horizontal front porch
  // minV - 272 + 2 + 1 = 275
  #define  RK043FN48H_VSYNC   1  // Vertical synchronization
  #define  RK043FN48H_VBP     1  // Vertical back porch
  #define  RK043FN48H_VFP     2  // Vertical front porch
  #define  RK043FN48H_VFP     2  // Vertical front porch
  // 27.428Mhz / 490*275 = 50.8Hz

  RCC_PeriphCLKInitTypeDef clockConfig;
  clockConfig.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  clockConfig.PLLSAI.PLLSAIN = 192;
  clockConfig.PLLSAI.PLLSAIR = RK043FN48H_PLLSAIR;
  clockConfig.PLLSAIDivR = RCC_PLLSAIDIVR_4;
  HAL_RCCEx_PeriphCLKConfig (&clockConfig);
  __HAL_RCC_LTDC_CLK_ENABLE();

  hLtdcHandler.Instance = LTDC;
  hLtdcHandler.LayerCfg->ImageWidth = getWidth();
  hLtdcHandler.LayerCfg->ImageHeight = getHeight();
  hLtdcHandler.Init.HorizontalSync = (RK043FN48H_HSYNC - 1);
  hLtdcHandler.Init.VerticalSync = (RK043FN48H_VSYNC - 1);
  hLtdcHandler.Init.AccumulatedHBP = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
  hLtdcHandler.Init.AccumulatedVBP = (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
  hLtdcHandler.Init.AccumulatedActiveH = (getHeight() + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
  hLtdcHandler.Init.AccumulatedActiveW = (getWidth() + RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
  hLtdcHandler.Init.TotalHeigh = (getHeight() + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);
  hLtdcHandler.Init.TotalWidth = (getWidth() + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1);
  hLtdcHandler.Init.Backcolor.Blue = 0;
  hLtdcHandler.Init.Backcolor.Green = 0;
  hLtdcHandler.Init.Backcolor.Red = 0;
  hLtdcHandler.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hLtdcHandler.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hLtdcHandler.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hLtdcHandler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

  // config the HS, VS, DE and PC polarity
  LTDC->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);
  LTDC->GCR |=  (uint32_t)(hLtdcHandler.Init.HSPolarity | hLtdcHandler.Init.VSPolarity |
                                            hLtdcHandler.Init.DEPolarity | hLtdcHandler.Init.PCPolarity);

  // set Synchronization size
  LTDC->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
  uint32_t tmp = (hLtdcHandler.Init.HorizontalSync << 16);
  LTDC->SSCR |= (tmp | hLtdcHandler.Init.VerticalSync);

  // set Accumulated Back porch
  LTDC->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
  tmp = (hLtdcHandler.Init.AccumulatedHBP << 16);
  LTDC->BPCR |= (tmp | hLtdcHandler.Init.AccumulatedVBP);

  // set Accumulated Active Width
  LTDC->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
  tmp = (hLtdcHandler.Init.AccumulatedActiveW << 16);
  LTDC->AWCR |= (tmp | hLtdcHandler.Init.AccumulatedActiveH);

  // set Total Width
  LTDC->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
  tmp = (hLtdcHandler.Init.TotalWidth << 16);
  LTDC->TWCR |= (tmp | hLtdcHandler.Init.TotalHeigh);

  // set background color value
  tmp = ((uint32_t)(hLtdcHandler.Init.Backcolor.Green) << 8);
  uint32_t tmp1 = ((uint32_t)(hLtdcHandler.Init.Backcolor.Red) << 16);
  LTDC->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);
  LTDC->BCCR |= (tmp1 | tmp | hLtdcHandler.Init.Backcolor.Blue);

  // enable transferError,fifoUnderrun interrupt
  __HAL_LTDC_ENABLE_IT (&hLtdcHandler, LTDC_IT_TE);
  __HAL_LTDC_ENABLE_IT (&hLtdcHandler, LTDC_IT_FU);
  //__HAL_LTDC_ENABLE_IT (&hLtdcHandler, LTDC_FLAG_RR);

  __HAL_LTDC_ENABLE (&hLtdcHandler);
  //}}}

  // turn on display,backlight pins
  HAL_GPIO_WritePin (LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin (LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);

  hDma2dHandler.Instance = DMA2D;
  __HAL_RCC_DMA2D_CLK_ENABLE();
  HAL_DMA2D_ConfigDeadTime (&hDma2dHandler, 10);
  HAL_DMA2D_EnableDeadTime (&hDma2dHandler);

  layerInit (0, SDRAM_DEVICE_ADDR);
  clear (LCD_COLOR_BLACK);
  displayOn();
  }
//}}}

uint16_t cLcd::GetTextHeight() { return Font16.mHeight; }

//{{{
void cLcd::start() {
  clear (LCD_COLOR_BLACK);
  }
//}}}
//{{{
void cLcd::startBgnd (uint16_t* bgnd) {
  memcpy (getBuffer(), bgnd, 480*272*2);
  }
//}}}
//{{{
void cLcd::startBgnd (uint16_t* src, uint16_t srcXsize, uint16_t srcYsize, bool zoom) {

  if (zoom)
    convertFrameCpu1 (src, srcXsize, srcYsize, getBuffer(), getWidth(), getHeight());
  else
    convertFrameCpu (src, srcXsize, srcYsize, getBuffer(), getWidth(), getHeight());
  }
//}}}
//{{{
void cLcd::drawTitle (const char* title) {

  char str1[40];
  sprintf (str1, "%s %d", title, (int)mTick);

  SetTextColor (LCD_COLOR_WHITE);
  DisplayStringAtLine (0, str1);
  }
//}}}
//{{{
void cLcd::drawInfo (uint16_t column, char* str) {

  SetTextColor (LCD_COLOR_YELLOW);
  DisplayStringAtLineColumn (0, column, str);
  }
//}}}
//{{{
void cLcd::drawDebug() {

  if (!BSP_PB_GetState (BUTTON_KEY))
    for (auto displayLine = 0u; (displayLine < mDebugLine) && ((int)displayLine < mDisplayLines); displayLine++) {
      int debugLine = ((int)mDebugLine < mDisplayLines) ?
        displayLine : (mDebugLine - mDisplayLines + displayLine - getScrollLines())  % kDebugMaxLines;

      SetTextColor (LCD_COLOR_WHITE);
      char tickStr[20];
      auto ticks = mLines[debugLine].mTicks;
      sprintf (tickStr, "%2d.%03d", (int)ticks / 1000, (int)ticks % 1000);
      DisplayStringAtLineColumn (1+displayLine, 0, tickStr);

      SetTextColor (mLines[debugLine].mColour);
      DisplayStringAtLineColumn (1+displayLine, 7, mLines[debugLine].mStr);
      }
  }
//}}}
//{{{
void cLcd::present() {

  uint32_t wait = 40 - (HAL_GetTick() % 40);
  HAL_Delay (wait);
  mTick = HAL_GetTick();

  auto buffer = getBuffer();
  mFlip = !mFlip;
  SetAddress (0, buffer, getBuffer());
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
  incScrollValue (inc * GetTextHeight() / getScrollScale());
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

//{{{
void cLcd::SelectLayer (uint32_t LayerIndex) {
  mCurLayer = LayerIndex;
  }
//}}}
//{{{
void cLcd::SetTransparency (uint32_t LayerIndex, uint8_t Transparency) {

  // change layer Alpha
  hLtdcHandler.LayerCfg[LayerIndex].Alpha = Transparency;
  setLayer (LayerIndex);
  LTDC->SRCR = LTDC_SRCR_VBR;
  }
//}}}
//{{{
void cLcd::SetAddress (uint32_t LayerIndex, uint32_t* address, uint32_t* writeAddress) {

  // change layer addresses
  hLtdcHandler.LayerCfg[LayerIndex].FBStartAdress = (uint32_t)address;
  hLtdcHandler.LayerCfg[LayerIndex].FBStartAdressWrite = (uint32_t)writeAddress;
  setLayer (LayerIndex);
  LTDC->SRCR = LTDC_SRCR_VBR;
  }
//}}}

//{{{
uint32_t cLcd::ReadPixel (uint16_t x, uint16_t y) {

  #ifdef RGB565
    return *(__IO uint16_t*)(hLtdcHandler.LayerCfg[mCurLayer].FBStartAdressWrite + (2*(y*getWidth() + x)));
  #else
    return *(__IO uint32_t*)(hLtdcHandler.LayerCfg[mCurLayer].FBStartAdressWrite + (4*(y*getWidth() + x)));
  #endif
  }
//}}}
//{{{
void cLcd::DrawPixel (uint16_t x, uint16_t y, uint32_t color) {
// Write data value to all SDRAM memory

  #ifdef RGB565
    *(__IO uint16_t*) (hLtdcHandler.LayerCfg[mCurLayer].FBStartAdressWrite + (2*(y*getWidth() + x))) = (uint16_t)color;
  #else
    *(__IO uint32_t*) (hLtdcHandler.LayerCfg[mCurLayer].FBStartAdressWrite + (4*(y*getWidth() + x))) = color;
  #endif
  }
//}}}
//{{{
void cLcd::DrawBitmap (uint32_t x, uint32_t y, uint8_t *pbmp) {

  // Get bitmap data address offset
  uint32_t index = pbmp[10] + (pbmp[11] << 8) + (pbmp[12] << 16)  + (pbmp[13] << 24);

  // Read bitmap width
  uint32_t width = pbmp[18] + (pbmp[19] << 8) + (pbmp[20] << 16)  + (pbmp[21] << 24);

  // Read bitmap height
  uint32_t height = pbmp[22] + (pbmp[23] << 8) + (pbmp[24] << 16)  + (pbmp[25] << 24);

  // Read bit/pixel
  uint32_t bit_pixel = pbmp[28] + (pbmp[29] << 8);

  // Set the address
  uint32_t address = hLtdcHandler.LayerCfg[mCurLayer].FBStartAdressWrite + (((getWidth()*y) + x)*(4));

  // Get the layer pixel format
  uint32_t input_color_mode;
  if ((bit_pixel/8) == 4)
    input_color_mode = CM_ARGB8888;
  else if ((bit_pixel/8) == 2)
    input_color_mode = CM_RGB565;
  else
    input_color_mode = CM_RGB888;

  // Bypass the bitmap header
  pbmp += (index + (width * (height - 1) * (bit_pixel/8)));

  // Convert picture to ARGB8888 pixel format
  for (index = 0; index < height; index++) {
    // Pixel format conversion
    ConvertLineToARGB8888 ((uint32_t*)pbmp, (uint32_t*)address, width, input_color_mode);

    // Increment the source and destination buffers
    address += getWidth() * 4;
    pbmp -= width* (bit_pixel / 8);
    }
  }
//}}}

//{{{
void cLcd::clearStringLine (uint32_t Line) {

  // Draw rectangle with background color
  uint32_t color_backup = mTextColor;
  mTextColor = mBackColor;
  FillRect (0, Line * Font16.mHeight, getWidth(), Font16.mHeight);
  mTextColor = color_backup;
  }
//}}}
//{{{
void cLcd::DisplayChar (uint16_t x, uint16_t y, uint8_t ascii) {

  const uint16_t width = Font16.mWidth;
  const uint16_t byteAlignedWidth = (width+7)/8;
  const uint16_t offset = 8*(byteAlignedWidth) - width-1;
  const uint8_t* fontChar = &Font16.mTable [(ascii-' ') * Font16.mHeight * byteAlignedWidth];

  #ifdef RGB565
    auto fbPtr = ((uint16_t*)hLtdcHandler.LayerCfg[mCurLayer].FBStartAdressWrite) + (y * getWidth()) + x;
  #else
    auto fbPtr = ((uint32_t*)hLtdcHandler.LayerCfg[mCurLayer].FBStartAdressWrite) + (y * getWidth()) + x;
  #endif

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
          *fbPtr = mTextColor;
        fbPtr++;
        bit >>= 1;
        }
      fbPtr += getWidth() - width;
      }
    else
      fbPtr += getWidth();
    }
  }
//}}}
//{{{
void cLcd::DisplayStringAt (uint16_t x, uint16_t y, char* text, Text_AlignModeTypdef mode) {

  uint16_t column = 1;
  switch (mode) {
    case CENTER_MODE:  {
      uint32_t xSize = getWidth() / Font16.mWidth;
      char* ptr = text;
      uint32_t size = 0;
      while (*ptr++)
        size++;
      column = x + ((xSize - size) * Font16.mWidth) / 2;
      break;
      }

    case RIGHT_MODE: {
      uint32_t xSize = getWidth() / Font16.mWidth;
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
  if ((column < 1) || (column >= getWidth()))
    column = 1;

  while (*text && (column + Font16.mWidth < getWidth())) {
    DisplayChar (column, y, *text++);
    column += Font16.mWidth;
    }
  }
//}}}
//{{{
void cLcd::DisplayStringAtLine (uint16_t line, char* ptr) {
  DisplayStringAt (0, line * Font16.mHeight, ptr, LEFT_MODE);
  }
//}}}
//{{{
void cLcd::DisplayStringAtLineColumn (uint16_t line, uint16_t column, char* ptr) {
  DisplayStringAt (column * Font16.mWidth, line * Font16.mHeight, ptr, LEFT_MODE);
  }
//}}}

//{{{
void cLcd::clear (uint32_t Color) {
  FillBuffer (mCurLayer, hLtdcHandler.LayerCfg[mCurLayer].FBStartAdressWrite, getWidth(), getHeight(), 0, Color);
  }
//}}}
//{{{
void cLcd::DrawRect (uint16_t x, uint16_t y, uint16_t Width, uint16_t Height) {

  // Draw horizontal lines
  FillRect (x, y, Width, 1);
  FillRect (x, (y+ Height), Width, 1);

  // Draw vertical lines
  FillRect (x, y, 1, Height);
  FillRect ((x + Width), y, 1, Height);
  }
//}}}
//{{{
void cLcd::FillRect (uint16_t x, uint16_t y, uint16_t Width, uint16_t Height) {

  FillBuffer (mCurLayer,
              hLtdcHandler.LayerCfg[mCurLayer].FBStartAdressWrite + 4*(getWidth()*y + x),
              Width, Height, (getWidth() - Width), mTextColor);
  }
//}}}
//{{{
void cLcd::DrawCircle (uint16_t x, uint16_t y, uint16_t Radius) {

  int32_t decision;    // Decision Variable
  uint32_t current_x;   // Current X Value
  uint32_t current_y;   // Current Y Value

  decision = 3 - (Radius << 1);
  current_x = 0;
  current_y = Radius;

  while (current_x <= current_y) {
    DrawPixel ((x + current_x), (y - current_y), mTextColor);
    DrawPixel ((x - current_x), (y - current_y), mTextColor);
    DrawPixel ((x + current_y), (y - current_x), mTextColor);
    DrawPixel ((x - current_y), (y - current_x), mTextColor);
    DrawPixel ((x + current_x), (y + current_y), mTextColor);
    DrawPixel ((x - current_x), (y + current_y), mTextColor);
    DrawPixel ((x + current_y), (y + current_x), mTextColor);
    DrawPixel ((x - current_y), (y + current_x), mTextColor);

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
void cLcd::FillCircle (uint16_t x, uint16_t y, uint16_t Radius) {

  int32_t decision = 3 - (Radius << 1);
  uint32_t current_x = 0;
  uint32_t current_y = Radius;

  while (current_x <= current_y) {
    if (current_y > 0) {
      FillRect (x - current_y, y + current_x, 1, 2*current_y);
      FillRect (x - current_y, y - current_x, 1, 2*current_y);
      }
    if (current_x > 0) {
      FillRect (x - current_x, y - current_y, 1, 2*current_x);
      FillRect (x - current_x, y + current_y, 1, 2*current_x);
      }
    if (decision < 0)
      decision += (current_x << 2) + 6;
    else {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
      }
    current_x++;
    }

  DrawCircle (x, y, Radius);
  }
//}}}
//{{{
void cLcd::DrawPolygon (pPoint Points, uint16_t PointCount) {

  int16_t x = 0, y = 0;

  if (PointCount < 2)
    return;

  DrawLine (Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);

  while(--PointCount) {
    x = Points->X;
    y = Points->Y;
    Points++;
    DrawLine (x, y, Points->X, Points->Y);
    }
  }
//}}}
//{{{
void cLcd::FillPolygon (pPoint Points, uint16_t PointCount) {

  int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0, Y_first = 0, pixelX = 0, pixelY = 0, counter = 0;
  uint16_t  image_left = 0, image_right = 0, image_top = 0, image_bottom = 0;

  image_left = image_right = Points->X;
  image_top= image_bottom = Points->Y;

  for (counter = 1; counter < PointCount; counter++) {
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

  if (PointCount < 2)
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
void cLcd::DrawEllipse (uint16_t xCentre, uint16_t yCentre, uint16_t XRadius, uint16_t YRadius) {

  int x = 0;
  int y = -YRadius;
  int err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;

  rad1 = XRadius;
  rad2 = YRadius;

  k = (float)(rad2/rad1);

  do {
    DrawPixel ((xCentre - (uint16_t)(x/k)), yCentre+y, mTextColor);
    DrawPixel ((xCentre + (uint16_t)(x/k)), yCentre+y, mTextColor);
    DrawPixel ((xCentre + (uint16_t)(x/k)), yCentre-y, mTextColor);
    DrawPixel ((xCentre - (uint16_t)(x/k)), yCentre-y, mTextColor);

    e2 = err;
    if (e2 <= x) {
      err += ++x*2+1;
      if (-y == x && e2 <= y)
        e2 = 0;
      }
    if (e2 > y)
      err += ++y*2+1;
    }

  while (y <= 0);
  }
//}}}
//{{{
void cLcd::FillEllipse (uint16_t xCentre, uint16_t yCentre, uint16_t XRadius, uint16_t YRadius) {

  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;

  rad1 = XRadius;
  rad2 = YRadius;

  k = (float)(rad2/rad1);
  do {
    FillRect((xCentre-(uint16_t)(x/k)), (yCentre+y), 1, (2*(uint16_t)(x/k) + 1));
    FillRect((xCentre-(uint16_t)(x/k)), (yCentre-y), 1, (2*(uint16_t)(x/k) + 1));

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
void cLcd::DrawLine (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {

  int16_t xinc1 = 0, xinc2 = 0, yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0;

  int16_t deltax = ABS(x2 - x1);        // The difference between the x's
  int16_t deltay = ABS(y2 - y1);        // The difference between the y's
  int16_t x = x1;                       // Start x off at the first pixel
  int16_t y = y1;                       // Start y off at the first pixel

  if (x2 >= x1) { // The x-values are increasing
    xinc1 = 1;
    xinc2 = 1;
    }
  else {          // The x-values are decreasing
    xinc1 = -1;
    xinc2 = -1;
    }

  if (y2 >= y1) { // The y-values are increasing
    yinc1 = 1;
    yinc2 = 1;
    }
  else {          // The y-values are decreasing
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

  for (int16_t curpixel = 0; curpixel <= num_pixels; curpixel++) {
    DrawPixel (x, y, mTextColor);
    num += num_add;     // Increase the numerator by the top of the fraction
    if (num >= den) {   // Check if numerator >= denominator
      num -= den;       // Calculate the new numerator value
      x += xinc1;       // Change the x as appropriate
      y += yinc1;       // Change the y as appropriate
      }
    x += xinc2;         // Change the x as appropriate
    y += yinc2;         // Change the y as appropriate
    }
  }
//}}}

//{{{
void cLcd::convertFrame (uint16_t* src, uint32_t dst, uint16_t xsize, uint16_t ysize) {

  hDma2dHandler.Init.Mode = DMA2D_M2M_PFC;
  hDma2dHandler.Init.ColorMode = DMA2D_ARGB8888;
  hDma2dHandler.Init.OutputOffset = 0;

  hDma2dHandler.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hDma2dHandler.LayerCfg[1].InputAlpha = 0xFF;
  hDma2dHandler.LayerCfg[1].InputColorMode = CM_RGB565;
  hDma2dHandler.LayerCfg[1].InputOffset = 0;

  HAL_DMA2D_Init (&hDma2dHandler);
  HAL_DMA2D_ConfigLayer (&hDma2dHandler, 1);
  HAL_DMA2D_Start (&hDma2dHandler, (uint32_t)src, dst, xsize, ysize);
  HAL_DMA2D_PollForTransfer (&hDma2dHandler, 10);
  }
//}}}
//{{{
void cLcd::convertFrameCpu (uint16_t* src, uint16_t srcXsize, uint16_t srcYsize,
                            uint32_t* dst, uint16_t xsize, uint16_t ysize) {

  int srcScale = (srcXsize / xsize) + 1;
  int xpad = (xsize - (srcXsize/srcScale)) / 2;

  if (srcYsize >= ysize)
    src += (((srcYsize/srcScale) - ysize) / 2) * srcXsize;
  else
    src += ((ysize - srcYsize) / 2) * srcXsize;

#ifdef RGB565

  auto dst565 = (uint16_t*)dst;
  for (uint16_t y = 0; y < ysize; y++) {
    for (auto x = 0; x < xpad; x++)
      *dst565++ = 0;
    for (auto x = 0; x < xsize - xpad - xpad; x++) {
      *dst565++ = *src;
      src += srcScale;
      }
    src += (srcScale - 1) * srcXsize;
    for (auto x = 0; x < xpad; x++)
      *dst565++ = 0;
    }

#else

  for (uint16_t y = 0; y < ysize; y++) {
    for (auto x = 0; x < (xsize - (srcXsize/srcScale)) / 2; x++)
      *dst++ = 0xFF000000;
    for (auto x = 0; x < srcXsize/2); x++) {
      *dst++ = 0xFF000000 | ((*src & 0xF800) << 8) | ((*src & 0x07E0) << 5) | ((*src & 0x001F) << 3);
      src += 2;
      }
    src += srcXsize;
    for (auto x = 0; x < (xsize - (srcXsize/srcScale)) / 2; x++)
      *dst++ = 0xFF000000;
    }

#endif
  }
//}}}
//{{{
void cLcd::convertFrameCpu1 (uint16_t* src, uint16_t srcXsize, uint16_t srcYsize,
                               uint32_t* dst, uint16_t xsize, uint16_t ysize) {

  src += ((srcXsize-xsize)/2) + (((srcYsize - ysize) / 2) * srcXsize);

#ifdef RGB565
  auto dst565 = (uint16_t*)dst;
  for (uint16_t y = 0; y < ysize; y++) {
    memcpy (dst565, src, xsize*2);
    src += srcXsize;
    dst565 += xsize;
    }
#else
  for (uint16_t y = 0; y < ysize; y++) {
    for (auto x = 0; x < xsize; x++) {
      *dst++ = 0xFF000000 | ((*src & 0xF800) << 8) | ((*src & 0x07E0) << 5) | ((*src & 0x001F) << 3);
      src++;
      }
    src += srcXsize - xsize;
    }
#endif
  }
//}}}
//{{{
void cLcd::convertFrameYuv (uint8_t* src, uint16_t srcXsize, uint16_t srcYsize,
                              uint8_t* dst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize) {

  //{{{
  static const uint32_t yTable [256] = {
    0x7FFFFFEDu,
    0x7FFFFFEFu,
    0x7FFFFFF0u,
    0x7FFFFFF1u,
    0x7FFFFFF2u,
    0x7FFFFFF3u,
    0x7FFFFFF4u,
    0x7FFFFFF6u,
    0x7FFFFFF7u,
    0x7FFFFFF8u,
    0x7FFFFFF9u,
    0x7FFFFFFAu,
    0x7FFFFFFBu,
    0x7FFFFFFDu,
    0x7FFFFFFEu,
    0x7FFFFFFFu,
    0x80000000u,
    0x80400801u,
    0x80A01002u,
    0x80E01803u,
    0x81202805u,
    0x81803006u,
    0x81C03807u,
    0x82004008u,
    0x82604809u,
    0x82A0500Au,
    0x82E0600Cu,
    0x8340680Du,
    0x8380700Eu,
    0x83C0780Fu,
    0x84208010u,
    0x84608811u,
    0x84A09813u,
    0x8500A014u,
    0x8540A815u,
    0x8580B016u,
    0x85E0B817u,
    0x8620C018u,
    0x8660D01Au,
    0x86C0D81Bu,
    0x8700E01Cu,
    0x8740E81Du,
    0x87A0F01Eu,
    0x87E0F81Fu,
    0x88210821u,
    0x88811022u,
    0x88C11823u,
    0x89012024u,
    0x89412825u,
    0x89A13026u,
    0x89E14028u,
    0x8A214829u,
    0x8A81502Au,
    0x8AC1582Bu,
    0x8B01602Cu,
    0x8B61682Du,
    0x8BA1782Fu,
    0x8BE18030u,
    0x8C418831u,
    0x8C819032u,
    0x8CC19833u,
    0x8D21A034u,
    0x8D61B036u,
    0x8DA1B837u,
    0x8E01C038u,
    0x8E41C839u,
    0x8E81D03Au,
    0x8EE1D83Bu,
    0x8F21E83Du,
    0x8F61F03Eu,
    0x8FC1F83Fu,
    0x90020040u,
    0x90420841u,
    0x90A21042u,
    0x90E22044u,
    0x91222845u,
    0x91823046u,
    0x91C23847u,
    0x92024048u,
    0x92624849u,
    0x92A2504Au,
    0x92E2604Cu,
    0x9342684Du,
    0x9382704Eu,
    0x93C2784Fu,
    0x94228050u,
    0x94628851u,
    0x94A29853u,
    0x9502A054u,
    0x9542A855u,
    0x9582B056u,
    0x95E2B857u,
    0x9622C058u,
    0x9662D05Au,
    0x96C2D85Bu,
    0x9702E05Cu,
    0x9742E85Du,
    0x97A2F05Eu,
    0x97E2F85Fu,
    0x98230861u,
    0x98831062u,
    0x98C31863u,
    0x99032064u,
    0x99632865u,
    0x99A33066u,
    0x99E34068u,
    0x9A434869u,
    0x9A83506Au,
    0x9AC3586Bu,
    0x9B23606Cu,
    0x9B63686Du,
    0x9BA3786Fu,
    0x9BE38070u,
    0x9C438871u,
    0x9C839072u,
    0x9CC39873u,
    0x9D23A074u,
    0x9D63B076u,
    0x9DA3B877u,
    0x9E03C078u,
    0x9E43C879u,
    0x9E83D07Au,
    0x9EE3D87Bu,
    0x9F23E87Du,
    0x9F63F07Eu,
    0x9FC3F87Fu,
    0xA0040080u,
    0xA0440881u,
    0xA0A41082u,
    0xA0E42084u,
    0xA1242885u,
    0xA1843086u,
    0xA1C43887u,
    0xA2044088u,
    0xA2644889u,
    0xA2A4588Bu,
    0xA2E4608Cu,
    0xA344688Du,
    0xA384708Eu,
    0xA3C4788Fu,
    0xA4248090u,
    0xA4649092u,
    0xA4A49893u,
    0xA504A094u,
    0xA544A895u,
    0xA584B096u,
    0xA5E4B897u,
    0xA624C098u,
    0xA664D09Au,
    0xA6C4D89Bu,
    0xA704E09Cu,
    0xA744E89Du,
    0xA7A4F09Eu,
    0xA7E4F89Fu,
    0xA82508A1u,
    0xA88510A2u,
    0xA8C518A3u,
    0xA90520A4u,
    0xA96528A5u,
    0xA9A530A6u,
    0xA9E540A8u,
    0xAA4548A9u,
    0xAA8550AAu,
    0xAAC558ABu,
    0xAB2560ACu,
    0xAB6568ADu,
    0xABA578AFu,
    0xAC0580B0u,
    0xAC4588B1u,
    0xAC8590B2u,
    0xACE598B3u,
    0xAD25A0B4u,
    0xAD65B0B6u,
    0xADA5B8B7u,
    0xAE05C0B8u,
    0xAE45C8B9u,
    0xAE85D0BAu,
    0xAEE5D8BBu,
    0xAF25E8BDu,
    0xAF65F0BEu,
    0xAFC5F8BFu,
    0xB00600C0u,
    0xB04608C1u,
    0xB0A610C2u,
    0xB0E620C4u,
    0xB12628C5u,
    0xB18630C6u,
    0xB1C638C7u,
    0xB20640C8u,
    0xB26648C9u,
    0xB2A658CBu,
    0xB2E660CCu,
    0xB34668CDu,
    0xB38670CEu,
    0xB3C678CFu,
    0xB42680D0u,
    0xB46690D2u,
    0xB4A698D3u,
    0xB506A0D4u,
    0xB546A8D5u,
    0xB586B0D6u,
    0xB5E6B8D7u,
    0xB626C8D9u,
    0xB666D0DAu,
    0xB6C6D8DBu,
    0xB706E0DCu,
    0xB746E8DDu,
    0xB7A6F0DEu,
    0xB7E6F8DFu,
    0xB82708E1u,
    0xB88710E2u,
    0xB8C718E3u,
    0xB90720E4u,
    0xB96728E5u,
    0xB9A730E6u,
    0xB9E740E8u,
    0xBA4748E9u,
    0xBA8750EAu,
    0xBAC758EBu,
    0xBB2760ECu,
    0xBB6768EDu,
    0xBBA778EFu,
    0xBC0780F0u,
    0xBC4788F1u,
    0xBC8790F2u,
    0xBCE798F3u,
    0xBD27A0F4u,
    0xBD67B0F6u,
    0xBDC7B8F7u,
    0xBE07C0F8u,
    0xBE47C8F9u,
    0xBEA7D0FAu,
    0xBEE7D8FBu,
    0xBF27E8FDu,
    0xBF87F0FEu,
    0xBFC7F8FFu,
    0xC0080100u,
    0xC0480901u,
    0xC0A81102u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    0xC0E82104u,
    };
  //}}}
  //{{{
  static const uint32_t uTable [256] = {
    0x0C400103u,
    0x0C200105u,
    0x0C200107u,
    0x0C000109u,
    0x0BE0010Bu,
    0x0BC0010Du,
    0x0BA0010Fu,
    0x0BA00111u,
    0x0B800113u,
    0x0B600115u,
    0x0B400117u,
    0x0B400119u,
    0x0B20011Bu,
    0x0B00011Du,
    0x0AE0011Fu,
    0x0AE00121u,
    0x0AC00123u,
    0x0AA00125u,
    0x0A800127u,
    0x0A600129u,
    0x0A60012Bu,
    0x0A40012Du,
    0x0A20012Fu,
    0x0A000131u,
    0x0A000132u,
    0x09E00134u,
    0x09C00136u,
    0x09A00138u,
    0x09A0013Au,
    0x0980013Cu,
    0x0960013Eu,
    0x09400140u,
    0x09400142u,
    0x09200144u,
    0x09000146u,
    0x08E00148u,
    0x08C0014Au,
    0x08C0014Cu,
    0x08A0014Eu,
    0x08800150u,
    0x08600152u,
    0x08600154u,
    0x08400156u,
    0x08200158u,
    0x0800015Au,
    0x0800015Cu,
    0x07E0015Eu,
    0x07C00160u,
    0x07A00162u,
    0x07A00164u,
    0x07800166u,
    0x07600168u,
    0x0740016Au,
    0x0720016Cu,
    0x0720016Eu,
    0x07000170u,
    0x06E00172u,
    0x06C00174u,
    0x06C00176u,
    0x06A00178u,
    0x0680017Au,
    0x0660017Cu,
    0x0660017Eu,
    0x06400180u,
    0x06200182u,
    0x06000184u,
    0x05E00185u,
    0x05E00187u,
    0x05C00189u,
    0x05A0018Bu,
    0x0580018Du,
    0x0580018Fu,
    0x05600191u,
    0x05400193u,
    0x05200195u,
    0x05200197u,
    0x05000199u,
    0x04E0019Bu,
    0x04C0019Du,
    0x04C0019Fu,
    0x04A001A1u,
    0x048001A3u,
    0x046001A5u,
    0x044001A7u,
    0x044001A9u,
    0x042001ABu,
    0x040001ADu,
    0x03E001AFu,
    0x03E001B1u,
    0x03C001B3u,
    0x03A001B5u,
    0x038001B7u,
    0x038001B9u,
    0x036001BBu,
    0x034001BDu,
    0x032001BFu,
    0x032001C1u,
    0x030001C3u,
    0x02E001C5u,
    0x02C001C7u,
    0x02A001C9u,
    0x02A001CBu,
    0x028001CDu,
    0x026001CFu,
    0x024001D1u,
    0x024001D3u,
    0x022001D5u,
    0x020001D7u,
    0x01E001D8u,
    0x01E001DAu,
    0x01C001DCu,
    0x01A001DEu,
    0x018001E0u,
    0x016001E2u,
    0x016001E4u,
    0x014001E6u,
    0x012001E8u,
    0x010001EAu,
    0x010001ECu,
    0x00E001EEu,
    0x00C001F0u,
    0x00A001F2u,
    0x00A001F4u,
    0x008001F6u,
    0x006001F8u,
    0x004001FAu,
    0x004001FCu,
    0x002001FEu,
    0x00000200u,
    0xFFE00202u,
    0xFFC00204u,
    0xFFC00206u,
    0xFFA00208u,
    0xFF80020Au,
    0xFF60020Cu,
    0xFF60020Eu,
    0xFF400210u,
    0xFF200212u,
    0xFF000214u,
    0xFF000216u,
    0xFEE00218u,
    0xFEC0021Au,
    0xFEA0021Cu,
    0xFEA0021Eu,
    0xFE800220u,
    0xFE600222u,
    0xFE400224u,
    0xFE200226u,
    0xFE200228u,
    0xFE000229u,
    0xFDE0022Bu,
    0xFDC0022Du,
    0xFDC0022Fu,
    0xFDA00231u,
    0xFD800233u,
    0xFD600235u,
    0xFD600237u,
    0xFD400239u,
    0xFD20023Bu,
    0xFD00023Du,
    0xFCE0023Fu,
    0xFCE00241u,
    0xFCC00243u,
    0xFCA00245u,
    0xFC800247u,
    0xFC800249u,
    0xFC60024Bu,
    0xFC40024Du,
    0xFC20024Fu,
    0xFC200251u,
    0xFC000253u,
    0xFBE00255u,
    0xFBC00257u,
    0xFBC00259u,
    0xFBA0025Bu,
    0xFB80025Du,
    0xFB60025Fu,
    0xFB400261u,
    0xFB400263u,
    0xFB200265u,
    0xFB000267u,
    0xFAE00269u,
    0xFAE0026Bu,
    0xFAC0026Du,
    0xFAA0026Fu,
    0xFA800271u,
    0xFA800273u,
    0xFA600275u,
    0xFA400277u,
    0xFA200279u,
    0xFA20027Bu,
    0xFA00027Cu,
    0xF9E0027Eu,
    0xF9C00280u,
    0xF9A00282u,
    0xF9A00284u,
    0xF9800286u,
    0xF9600288u,
    0xF940028Au,
    0xF940028Cu,
    0xF920028Eu,
    0xF9000290u,
    0xF8E00292u,
    0xF8E00294u,
    0xF8C00296u,
    0xF8A00298u,
    0xF880029Au,
    0xF860029Cu,
    0xF860029Eu,
    0xF84002A0u,
    0xF82002A2u,
    0xF80002A4u,
    0xF80002A6u,
    0xF7E002A8u,
    0xF7C002AAu,
    0xF7A002ACu,
    0xF7A002AEu,
    0xF78002B0u,
    0xF76002B2u,
    0xF74002B4u,
    0xF74002B6u,
    0xF72002B8u,
    0xF70002BAu,
    0xF6E002BCu,
    0xF6C002BEu,
    0xF6C002C0u,
    0xF6A002C2u,
    0xF68002C4u,
    0xF66002C6u,
    0xF66002C8u,
    0xF64002CAu,
    0xF62002CCu,
    0xF60002CEu,
    0xF60002CFu,
    0xF5E002D1u,
    0xF5C002D3u,
    0xF5A002D5u,
    0xF5A002D7u,
    0xF58002D9u,
    0xF56002DBu,
    0xF54002DDu,
    0xF52002DFu,
    0xF52002E1u,
    0xF50002E3u,
    0xF4E002E5u,
    0xF4C002E7u,
    0xF4C002E9u,
    0xF4A002EBu,
    0xF48002EDu,
    0xF46002EFu,
    0xF46002F1u,
    0xF44002F3u,
    0xF42002F5u,
    0xF40002F7u,
    0xF3E002F9u,
    0xF3E002FBu,
    };
  //}}}
  //{{{
  static const uint32_t vTable [256] = {
    0x1A09A000u,
    0x19E9A800u,
    0x19A9B800u,
    0x1969C800u,
    0x1949D000u,
    0x1909E000u,
    0x18C9E800u,
    0x18A9F800u,
    0x186A0000u,
    0x182A1000u,
    0x180A2000u,
    0x17CA2800u,
    0x17AA3800u,
    0x176A4000u,
    0x172A5000u,
    0x170A6000u,
    0x16CA6800u,
    0x168A7800u,
    0x166A8000u,
    0x162A9000u,
    0x160AA000u,
    0x15CAA800u,
    0x158AB800u,
    0x156AC000u,
    0x152AD000u,
    0x14EAE000u,
    0x14CAE800u,
    0x148AF800u,
    0x146B0000u,
    0x142B1000u,
    0x13EB2000u,
    0x13CB2800u,
    0x138B3800u,
    0x134B4000u,
    0x132B5000u,
    0x12EB6000u,
    0x12CB6800u,
    0x128B7800u,
    0x124B8000u,
    0x122B9000u,
    0x11EBA000u,
    0x11ABA800u,
    0x118BB800u,
    0x114BC000u,
    0x112BD000u,
    0x10EBE000u,
    0x10ABE800u,
    0x108BF800u,
    0x104C0000u,
    0x100C1000u,
    0x0FEC2000u,
    0x0FAC2800u,
    0x0F8C3800u,
    0x0F4C4000u,
    0x0F0C5000u,
    0x0EEC5800u,
    0x0EAC6800u,
    0x0E6C7800u,
    0x0E4C8000u,
    0x0E0C9000u,
    0x0DEC9800u,
    0x0DACA800u,
    0x0D6CB800u,
    0x0D4CC000u,
    0x0D0CD000u,
    0x0CCCD800u,
    0x0CACE800u,
    0x0C6CF800u,
    0x0C4D0000u,
    0x0C0D1000u,
    0x0BCD1800u,
    0x0BAD2800u,
    0x0B6D3800u,
    0x0B2D4000u,
    0x0B0D5000u,
    0x0ACD5800u,
    0x0AAD6800u,
    0x0A6D7800u,
    0x0A2D8000u,
    0x0A0D9000u,
    0x09CD9800u,
    0x098DA800u,
    0x096DB800u,
    0x092DC000u,
    0x090DD000u,
    0x08CDD800u,
    0x088DE800u,
    0x086DF800u,
    0x082E0000u,
    0x07EE1000u,
    0x07CE1800u,
    0x078E2800u,
    0x076E3800u,
    0x072E4000u,
    0x06EE5000u,
    0x06CE5800u,
    0x068E6800u,
    0x064E7800u,
    0x062E8000u,
    0x05EE9000u,
    0x05CE9800u,
    0x058EA800u,
    0x054EB800u,
    0x052EC000u,
    0x04EED000u,
    0x04AED800u,
    0x048EE800u,
    0x044EF000u,
    0x042F0000u,
    0x03EF1000u,
    0x03AF1800u,
    0x038F2800u,
    0x034F3000u,
    0x030F4000u,
    0x02EF5000u,
    0x02AF5800u,
    0x028F6800u,
    0x024F7000u,
    0x020F8000u,
    0x01EF9000u,
    0x01AF9800u,
    0x016FA800u,
    0x014FB000u,
    0x010FC000u,
    0x00EFD000u,
    0x00AFD800u,
    0x006FE800u,
    0x004FF000u,
    0x00100000u,
    0xFFD01000u,
    0xFFB01800u,
    0xFF702800u,
    0xFF303000u,
    0xFF104000u,
    0xFED05000u,
    0xFEB05800u,
    0xFE706800u,
    0xFE307000u,
    0xFE108000u,
    0xFDD09000u,
    0xFD909800u,
    0xFD70A800u,
    0xFD30B000u,
    0xFD10C000u,
    0xFCD0D000u,
    0xFC90D800u,
    0xFC70E800u,
    0xFC30F000u,
    0xFBF10000u,
    0xFBD11000u,
    0xFB911800u,
    0xFB712800u,
    0xFB313000u,
    0xFAF14000u,
    0xFAD14800u,
    0xFA915800u,
    0xFA516800u,
    0xFA317000u,
    0xF9F18000u,
    0xF9D18800u,
    0xF9919800u,
    0xF951A800u,
    0xF931B000u,
    0xF8F1C000u,
    0xF8B1C800u,
    0xF891D800u,
    0xF851E800u,
    0xF831F000u,
    0xF7F20000u,
    0xF7B20800u,
    0xF7921800u,
    0xF7522800u,
    0xF7123000u,
    0xF6F24000u,
    0xF6B24800u,
    0xF6925800u,
    0xF6526800u,
    0xF6127000u,
    0xF5F28000u,
    0xF5B28800u,
    0xF5729800u,
    0xF552A800u,
    0xF512B000u,
    0xF4F2C000u,
    0xF4B2C800u,
    0xF472D800u,
    0xF452E800u,
    0xF412F000u,
    0xF3D30000u,
    0xF3B30800u,
    0xF3731800u,
    0xF3532800u,
    0xF3133000u,
    0xF2D34000u,
    0xF2B34800u,
    0xF2735800u,
    0xF2336800u,
    0xF2137000u,
    0xF1D38000u,
    0xF1B38800u,
    0xF1739800u,
    0xF133A800u,
    0xF113B000u,
    0xF0D3C000u,
    0xF093C800u,
    0xF073D800u,
    0xF033E000u,
    0xF013F000u,
    0xEFD40000u,
    0xEF940800u,
    0xEF741800u,
    0xEF342000u,
    0xEEF43000u,
    0xEED44000u,
    0xEE944800u,
    0xEE745800u,
    0xEE346000u,
    0xEDF47000u,
    0xEDD48000u,
    0xED948800u,
    0xED549800u,
    0xED34A000u,
    0xECF4B000u,
    0xECD4C000u,
    0xEC94C800u,
    0xEC54D800u,
    0xEC34E000u,
    0xEBF4F000u,
    0xEBB50000u,
    0xEB950800u,
    0xEB551800u,
    0xEB352000u,
    0xEAF53000u,
    0xEAB54000u,
    0xEA954800u,
    0xEA555800u,
    0xEA156000u,
    0xE9F57000u,
    0xE9B58000u,
    0xE9958800u,
    0xE9559800u,
    0xE915A000u,
    0xE8F5B000u,
    0xE8B5C000u,
    0xE875C800u,
    0xE855D800u,
    0xE815E000u,
    0xE7F5F000u,
    0xE7B60000u,
    0xE7760800u,
    0xE7561800u,
    0xE7162000u,
    0xE6D63000u,
    0xE6B64000u,
    0xE6764800u,
    0xE6365800U
    };
  //}}}

  dst += ((y * xsize) + x) * 4;

  for (y = 0; y < srcYsize; y++) {
    for (x = 0; x < srcXsize/2; x++) {
      // read yuv pair
      uint8_t y1 = *src++;
      uint8_t u = *src++;
      uint8_t y2 = *src++;
      uint8_t v = *src++;

      uint32_t uv = uTable[u] | vTable[v];

      uint32_t yuv = yTable[y1] + uv;
      int tmp = yuv & 0x40080100;
      if (tmp) {
        tmp -= tmp >> 8;
        yuv |= tmp;
        tmp = 0x40080100 & ~(yuv >> 1);
        yuv += tmp >> 8;
        }
      *dst++ = yuv;
      *dst++ = yuv >> 22;
      *dst++ = yuv >> 11;
      *dst++ = 0xFF;

      yuv = yTable[y2] + uv;
      tmp = yuv & 0x40080100;
      if (tmp) {
        tmp -= tmp >> 8;
        yuv |= tmp;
        tmp = 0x40080100 & ~(yuv >> 1);
        yuv += tmp >> 8;
        }
      *dst++ = yuv;
      *dst++ = yuv >> 22;
      *dst++ = yuv >> 11;
      *dst++ = 0xFF;
      }
    dst += (xsize - srcXsize) * 4;
    }
  }
//}}}

//{{{
void cLcd::displayOn() {

  __HAL_LTDC_ENABLE (&hLtdcHandler);
  HAL_GPIO_WritePin (LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);        // Assert LCD_DISP pin
  HAL_GPIO_WritePin (LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);  // Assert LCD_BL_CTRL pin
  }
//}}}
//{{{
void cLcd::displayOff() {

  __HAL_LTDC_DISABLE (&hLtdcHandler);
  HAL_GPIO_WritePin (LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_RESET);      // De-assert LCD_DISP pin
  HAL_GPIO_WritePin (LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_RESET);// De-assert LCD_BL_CTRL pin
  }
//}}}

// private
//{{{
uint32_t* cLcd::getBuffer() {
  return (uint32_t*)(mFlip ? SDRAM_DEVICE_ADDR + 0x40000 : SDRAM_DEVICE_ADDR);
  }
//}}}
//{{{
void cLcd::setLayer (uint32_t layerIndex) {

  auto layerConfig = &hLtdcHandler.LayerCfg[layerIndex];

  // Config horizontal start and stop position
  uint32_t tmp = (layerConfig->WindowX1 + ((LTDC->BPCR & LTDC_BPCR_AHBP) >> 16)) << 16;
  LTDC_LAYER (&hLtdcHandler, layerIndex)->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
  LTDC_LAYER (&hLtdcHandler, layerIndex)->WHPCR = ((layerConfig->WindowX0 + ((LTDC->BPCR & LTDC_BPCR_AHBP) >> 16) + 1) | tmp);

  // config vertical start and stop position
  tmp = (layerConfig->WindowY1 + (LTDC->BPCR & LTDC_BPCR_AVBP)) << 16;
  LTDC_LAYER (&hLtdcHandler, layerIndex)->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
  LTDC_LAYER (&hLtdcHandler, layerIndex)->WVPCR  = ((layerConfig->WindowY0 + (LTDC->BPCR & LTDC_BPCR_AVBP) + 1) | tmp);

  // pixel format
  LTDC_LAYER (&hLtdcHandler, layerIndex)->PFCR &= ~(LTDC_LxPFCR_PF);
  LTDC_LAYER (&hLtdcHandler, layerIndex)->PFCR = (layerConfig->PixelFormat);

  // config default color values
  tmp = ((uint32_t)(layerConfig->Backcolor.Green) << 8);
  uint32_t tmp1 = ((uint32_t)(layerConfig->Backcolor.Red) << 16);
  uint32_t tmp2 = (layerConfig->Alpha0 << 24);
  LTDC_LAYER (&hLtdcHandler, layerIndex)->DCCR &= ~(LTDC_LxDCCR_DCBLUE | LTDC_LxDCCR_DCGREEN | LTDC_LxDCCR_DCRED | LTDC_LxDCCR_DCALPHA);
  LTDC_LAYER (&hLtdcHandler, layerIndex)->DCCR = (layerConfig->Backcolor.Blue | tmp | tmp1 | tmp2);

  // constant alpha value
  LTDC_LAYER (&hLtdcHandler, layerIndex)->CACR &= ~(LTDC_LxCACR_CONSTA);
  LTDC_LAYER (&hLtdcHandler, layerIndex)->CACR = (layerConfig->Alpha);

  // Sblending factors
  LTDC_LAYER (&hLtdcHandler, layerIndex)->BFCR &= ~(LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
  LTDC_LAYER (&hLtdcHandler, layerIndex)->BFCR = (layerConfig->BlendingFactor1 | layerConfig->BlendingFactor2);

  // config  color frame buffer start address
  LTDC_LAYER (&hLtdcHandler, layerIndex)->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
  LTDC_LAYER (&hLtdcHandler, layerIndex)->CFBAR = (layerConfig->FBStartAdress);

  // config  color frame buffer pitch in byte
  tmp = (layerConfig->PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888) ? 4 : 2;
  LTDC_LAYER (&hLtdcHandler, layerIndex)->CFBLR &= ~(LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
  LTDC_LAYER (&hLtdcHandler, layerIndex)->CFBLR = ((layerConfig->ImageWidth * tmp) << 16) |
                                                   (((layerConfig->WindowX1 - layerConfig->WindowX0) * tmp)  + 3);

  // config  frame buffer line number
  LTDC_LAYER (&hLtdcHandler, layerIndex)->CFBLNR &= ~(LTDC_LxCFBLNR_CFBLNBR);
  LTDC_LAYER (&hLtdcHandler, layerIndex)->CFBLNR = (layerConfig->ImageHeight);

  // Enable LTDC_Layer by setting LEN bit
  LTDC_LAYER (&hLtdcHandler, layerIndex)->CR |= (uint32_t)LTDC_LxCR_LEN;
  }
//}}}
//{{{
void cLcd::layerInit (uint16_t LayerIndex, uint32_t FB_Address) {

  hLtdcHandler.LayerCfg[LayerIndex].FBStartAdress = FB_Address;
  hLtdcHandler.LayerCfg[LayerIndex].FBStartAdressWrite = FB_Address;
  hLtdcHandler.LayerCfg[LayerIndex].PixelFormat = LTDC_PIXEL_FORMAT_RGB565;  // LTDC_PIXEL_FORMAT_ARGB8888;

  hLtdcHandler.LayerCfg[LayerIndex].ImageWidth = getWidth();
  hLtdcHandler.LayerCfg[LayerIndex].ImageHeight = getHeight();

  hLtdcHandler.LayerCfg[LayerIndex].WindowX0 = 0;
  hLtdcHandler.LayerCfg[LayerIndex].WindowX1 = getWidth();
  hLtdcHandler.LayerCfg[LayerIndex].WindowY0 = 0;
  hLtdcHandler.LayerCfg[LayerIndex].WindowY1 = getHeight();

  hLtdcHandler.LayerCfg[LayerIndex].Alpha = 255;
  hLtdcHandler.LayerCfg[LayerIndex].Alpha0 = 0;

  hLtdcHandler.LayerCfg[LayerIndex].Backcolor.Blue = 0;
  hLtdcHandler.LayerCfg[LayerIndex].Backcolor.Green = 0;
  hLtdcHandler.LayerCfg[LayerIndex].Backcolor.Red = 0;

  hLtdcHandler.LayerCfg[LayerIndex].BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  hLtdcHandler.LayerCfg[LayerIndex].BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;

  // Configure the LTDC Layer
  setLayer (LayerIndex);

  // Sets the Reload type
  LTDC->SRCR = LTDC_SRCR_IMR;

  __HAL_LTDC_LAYER_ENABLE (&hLtdcHandler, LayerIndex);
  __HAL_LTDC_RELOAD_CONFIG (&hLtdcHandler);

  mCurLayer = LayerIndex;
  }
//}}}

//{{{
void cLcd::FillBuffer (uint32_t layer, uint32_t dst, uint32_t xsize, uint32_t ysize, uint32_t OffLine, uint32_t color) {

  hDma2dHandler.Init.Mode = DMA2D_R2M;
  #ifdef RGB565
    hDma2dHandler.Init.ColorMode = DMA2D_RGB565;
  #else
    hDma2dHandler.Init.ColorMode = DMA2D_ARGB8888;
  #endif
  hDma2dHandler.Init.OutputOffset = OffLine;

  HAL_DMA2D_Init (&hDma2dHandler);
  HAL_DMA2D_ConfigLayer (&hDma2dHandler, layer);
  HAL_DMA2D_Start (&hDma2dHandler, color, dst, xsize, ysize);
  HAL_DMA2D_PollForTransfer (&hDma2dHandler, 10);
  }
//}}}
//{{{
void cLcd::FillTriangle (uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3) {

  int16_t deltax = ABS(x2 - x1);        // The difference between the x's
  int16_t deltay = ABS(y2 - y1);        // The difference between the y's
  int16_t x = x1;                       // Start x off at the first pixel
  int16_t y = y1;                       // Start y off at the first pixel

  int16_t xinc1 = 0, xinc2 = 0;
  int16_t yinc1 = 0, yinc2 = 0;
  if (x2 >= x1) {
    // The x-values are increasing
    xinc1 = 1;
    xinc2 = 1;
    }
  else {
    // The x-values are decreasing
    xinc1 = -1;
    xinc2 = -1;
    }

  if (y2 >= y1) {
    // The y-values are increasing
    yinc1 = 1;
    yinc2 = 1;
    }
  else {
    // The y-values are decreasing
    yinc1 = -1;
    yinc2 = -1;
    }

  int16_t den = 0, num = 0, num_add = 0, num_pixels = 0;
  if (deltax >= deltay) {
    // There is at least one x-value for every y-value
    xinc1 = 0;           // Don't change the x when numerator >= denominator
    yinc2 = 0;           // Don't change the y for every iteration
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;  // There are more x-values than y-values
    }
  else {
    // There is at least one y-value for every x-value
    xinc2 = 0;           // Don't change the x for every iteration
    yinc1 = 0;           // Don't change the y when numerator >= denominator
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         // There are more y-values than x-values
    }

  for (int16_t curpixel = 0; curpixel <= num_pixels; curpixel++) {
    DrawLine (x, y, x3, y3);
    num += num_add;     // Increase the numerator by the top of the fraction
    if (num >= den)  {  // Check if numerator >= denominator
      num -= den;       // Calculate the new numerator value
      x += xinc1;       // Change the x as appropriate
      y += yinc1;       // Change the y as appropriate
      }
    x += xinc2;         // Change the x as appropriate
    y += yinc2;         // Change the y as appropriate
    }
  }
//}}}
//{{{
void cLcd::ConvertLineToARGB8888 (void* src, void* dst, uint32_t xSize, uint32_t ColorMode) {

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
      if (HAL_DMA2D_Start (&hDma2dHandler, (uint32_t)src, (uint32_t)dst, xSize, 1) == HAL_OK)
        HAL_DMA2D_PollForTransfer (&hDma2dHandler, 10);
  }
//}}}
