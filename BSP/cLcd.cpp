// cLcd.cpp
//{{{  includes
#include "cLcd.h"

#include "cmsis_os.h"
#include "semphr.h"

#include "../common/system.h"

#include "sdRam.h"

#include "font.h"
#include "fontSpec.h"
//}}}
//{{{  defines
#define POLY_X(Z)  ((int32_t)((points + Z)->x))
#define POLY_Y(Z)  ((int32_t)((points + Z)->y))
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

//{{{  cLcd static inits
cLcd* cLcd::mLcd = nullptr;

bool cLcd::mFrameWait = false;
SemaphoreHandle_t cLcd::mFrameSem;

cLcd::eDma2dWait cLcd::mDma2dWait = eWaitNone;
SemaphoreHandle_t cLcd::mDma2dSem;
//}}}

extern "C" {
  //{{{
  void LTDC_IRQHandler() {

    // line Interrupt
    if ((LTDC->ISR & LTDC_FLAG_LI) != RESET) {
      LTDC->ICR = LTDC_FLAG_LI;
      if (cLcd::mFrameWait) {
        portBASE_TYPE taskWoken = pdFALSE;
        if (xSemaphoreGiveFromISR (cLcd::mFrameSem, &taskWoken) == pdTRUE)
          portEND_SWITCHING_ISR (taskWoken);
        }
      cLcd::mFrameWait = false;
      }

    // register reload Interrupt
    if ((LTDC->ISR & LTDC_FLAG_RR) != RESET) {
      LTDC->ICR = LTDC_FLAG_RR;
      cLcd::mLcd->debug (LCD_COLOR_YELLOW, "ltdc reload IRQ");
      }
    }
  //}}}
  //{{{
  void LTDC_ER_IRQHandler() {

    // transfer Error Interrupt
    if ((LTDC->ISR &  LTDC_FLAG_TE) != RESET) {
      LTDC->ICR = LTDC_IT_TE;
      cLcd::mLcd->debug (LCD_COLOR_RED, "ltdc te IRQ");
      }

    // FIFO underrun Interrupt
    if ((LTDC->ISR &  LTDC_FLAG_FU) != RESET) {
      LTDC->ICR = LTDC_FLAG_FU;
      cLcd::mLcd->debug (LCD_COLOR_RED, "ltdc fifoUnderrun IRQ");
      }
    }
  //}}}
  //{{{
  void DMA2D_IRQHandler() {

    if (DMA2D->ISR & DMA2D_FLAG_TC) {
      DMA2D->IFCR = DMA2D_FLAG_TC;

      portBASE_TYPE taskWoken = pdFALSE;
      if (xSemaphoreGiveFromISR (cLcd::mDma2dSem, &taskWoken) == pdTRUE)
        portEND_SWITCHING_ISR (taskWoken);
      }
    }
  //}}}
  }

//{{{
cLcd::cLcd (uint16_t* frameBufBase, uint16_t displayLines)
  : mFrameBufBase(frameBufBase), mFrameBuf(frameBufBase), mDisplayLines(displayLines) {}
//}}}
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
  //{{{  ltdc init
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

  // config the HS, VS, DE and PC polarity
  LTDC->GCR = LTDC_HSPOLARITY_AL | LTDC_VSPOLARITY_AL | LTDC_DEPOLARITY_AL | LTDC_PCPOLARITY_IPC;

  // set Synchronization size
  LTDC->SSCR = ((RK043FN48H_HSYNC - 1) << 16) |
                (RK043FN48H_VSYNC - 1);

  // set Accumulated Back porch
  LTDC->BPCR = ((RK043FN48H_HSYNC + RK043FN48H_HBP - 1) << 16) |
                (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);

  // set Accumulated Active Width
  LTDC->AWCR = ((getWidth() + RK043FN48H_HSYNC + RK043FN48H_HBP - 1) << 16) |
                (getHeight() + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);

  // set Total Width
  LTDC->TWCR = ((getWidth() + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1) << 16) |
                (getHeight() + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);

  // set background color value
  LTDC->BCCR = 0;

  // set line interupt line number
  LTDC->LIPCR = 0;

  // clear interrupts
  LTDC->IER = LTDC_IT_TE | LTDC_IT_FU | LTDC_IT_LI;

  mFrameWait = false;
  vSemaphoreCreateBinary (mFrameSem);

  HAL_NVIC_SetPriority (LTDC_IRQn, 0xE, 0);
  HAL_NVIC_EnableIRQ (LTDC_IRQn);
  //}}}
  //{{{  ltdc layer 1 init
  // config color frame buffer start address
  LTDC_Layer1->CFBAR = (uint32_t)mFrameBuf;

  // pixel format
  LTDC_Layer1->PFCR = LTDC_PIXEL_FORMAT_RGB565;

  // Config horizontal start and stop position
  LTDC_Layer1->WHPCR = (((LTDC->BPCR & LTDC_BPCR_AHBP) >> 16) + 1) |
                       ((getWidth() + ((LTDC->BPCR & LTDC_BPCR_AHBP) >> 16)) << 16);

  // config vertical start and stop position
  LTDC_Layer1->WVPCR  = ((LTDC->BPCR & LTDC_BPCR_AVBP) + 1) |
                        ((getHeight() + (LTDC->BPCR & LTDC_BPCR_AVBP)) << 16);

  // config default color values
  LTDC_Layer1->DCCR = 0;

  // constant alpha value
  LTDC_Layer1->CACR = 255;

  // Sblending factors
  LTDC_Layer1->BFCR = LTDC_BLENDING_FACTOR1_PAxCA | LTDC_BLENDING_FACTOR2_PAxCA;

  // config color frame buffer pitch in byte
  LTDC_Layer1->CFBLR = ((getWidth() * 2) << 16) |
                       ((getWidth() * 2) + 3);

  // config frame buffer line number
  LTDC_Layer1->CFBLNR = getHeight();

  // Enable LTDC_Layer by setting LEN bit
  LTDC_Layer1->CR |= (uint32_t)LTDC_LxCR_LEN;

  // Sets the Reload type
  LTDC->SRCR = LTDC_SRCR_IMR;
  //}}}

  // turn on display,backlight pins
  HAL_GPIO_WritePin (LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin (LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);

  // dma2d init
  __HAL_RCC_DMA2D_CLK_ENABLE();

  DMA2D->BGPFCCR = DMA2D_RGB565;
  DMA2D->OPFCCR = DMA2D_RGB565;

  const int kDeadTime = 2;
  DMA2D->AMTCR = (kDeadTime << 8) | 0x0001;

  vSemaphoreCreateBinary (mDma2dSem);
  HAL_NVIC_SetPriority (DMA2D_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ (DMA2D_IRQn);

  displayOn();
  }
//}}}

//{{{
void cLcd::drawInfo (uint16_t color, eTextAlign textAlign, const char* format, ... ) {

  char str[kMaxStrSize];

  va_list args;
  va_start (args, format);
  vsnprintf (str, kMaxStrSize-1, format, args);
  va_end (args);

  displayString (color, getRect().getTR(), str, textAlign);

  }
//}}}
//{{{
void cLcd::drawDebug() {

  auto numWidth = getCharWidth ('0');
  for (auto displayLine = 0u; (displayLine < mDebugLine) && ((int)displayLine < mDisplayLines); displayLine++) {
    int debugLine = ((int)mDebugLine < mDisplayLines) ?
      displayLine : (mDebugLine - mDisplayLines + displayLine - getScrollLines())  % kDebugMaxLines;

    auto ticks = mLines[debugLine].mTicks;
    auto secs = (int)ticks / 1000;

    std::string str = dec(secs) + "." + dec (ticks % 1000, 3);
    displayString (LCD_COLOR_WHITE, cPoint(0, (displayLine+1) * getTextHeight()), str, eTextLeft);

    auto offset = 5 * numWidth;
    while (secs > 9) {
      offset += numWidth;
      secs / 10;
      }
    displayString (mLines[debugLine].mColour, cPoint(offset, (displayLine+1) * getTextHeight()), mLines[debugLine].mStr, eTextLeft);
    }
  }
//}}}
//{{{
void cLcd::present() {
// show frameBuffer, frame wait, flip it

  LTDC_Layer1->CFBAR = (uint32_t)mFrameBuf;
  LTDC->SRCR = LTDC_SRCR_VBR;

  mFrameWait = true;
  xSemaphoreTake (mFrameSem, 100);

  mFrameBuf = (uint16_t*)(((uint32_t)mFrameBuf == SDRAM_DEVICE_ADDR) ? SDRAM_DEVICE_ADDR + 0x40000 : SDRAM_DEVICE_ADDR);
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
void cLcd::debug (uint32_t colour, const std::string& str) {

  auto line = mDebugLine % kDebugMaxLines;

  if (!mLines[line].mStr)
    mLines[line].mStr = (char*)malloc (kMaxStrSize);

  int strSize = str.size() < kMaxStrSize-1 ? str.size() : kMaxStrSize-1;
  memcpy (mLines[line].mStr, str.c_str(), strSize);
  mLines[line].mStr[strSize] = 0;

  mLines[line].mTicks = HAL_GetTick();
  mLines[line].mColour = colour;
  mDebugLine++;
  }
//}}}
//{{{
void cLcd::debug (uint32_t colour, const char* format, ... ) {

  auto line = mDebugLine % kDebugMaxLines;

  if (!mLines[line].mStr)
    mLines[line].mStr = (char*)malloc (kMaxStrSize);

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
uint16_t cLcd::readPix (cPoint p) {

  return *(mFrameBuf + p.y*getWidth() + p.x);
  }
//}}}

//{{{
void cLcd::zoom565 (uint16_t* src, cPoint srcPos, cPoint srcSize, cRect dstRect, float zoomx, float zoomy) {

  uint32_t srcPitch = srcSize.x;
  int32_t srcSize16x = srcSize.x << 16;
  int32_t srcSize16y = srcSize.y << 16;

  int32_t inc16x = int32_t (0x10000 / zoomx);
  int32_t src16x = -(srcPos.x << 16) + (srcSize.x << 15) - ((dstRect.getWidth() * inc16x) >> 1);

  int32_t inc16y = int32_t (0x10000 / zoomy);
  int32_t src16y = -(srcPos.y << 16) + (srcSize.y << 15) - ((dstRect.getHeight() * inc16y) >> 1);

  // set dma2d common regs
  ready();
  DMA2D->OCOLR = 0;
  DMA2D->OOR = getWidth() - dstRect.getWidth();

  int32_t dsty = 0;
  uint16_t* dst = mFrameBuf + (dstRect.top * getWidth()) + dstRect.left;

  if (src16y < 0) {
    //{{{  before valid src
    dsty = 1 - src16y/inc16y;

    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->NLR = (dstRect.getWidth() << 16) |  dsty;
    DMA2D->CR = DMA2D_CR_START | DMA2D_R2M | DMA2D_CR_TCIE;
    mDma2dWait = eWaitIrq;

    src16y += dsty * inc16y;
    dst += dsty * getWidth();

    ready();
    }
    //}}}

  if (src16y < srcSize16y) {
    // valid src
    int16_t lastSrcDsty = dsty + (srcSize16y / inc16y);
    if (lastSrcDsty > dstRect.getHeight())
      lastSrcDsty = dstRect.getHeight();

    // draw valid src lines
    while (dsty < lastSrcDsty) {
      uint16_t* srcPtr = src + ((src16y >> 16) * srcPitch);
      int32_t x16 = src16x;
      for (uint16_t dstx = 0; dstx < dstRect.getWidth(); dstx++) {
        *dst++ = ((x16 >= 0) && (x16 < srcSize16x)) ? *(srcPtr + (x16 >> 16)) : 0;
        x16 += inc16x;
        }
      dst += getWidth() - dstRect.getWidth();
      src16y += inc16y;
      dsty++;
      }
    }

  if (dsty < dstRect.getHeight()) {
    //{{{  after valid src
    int trail = dstRect.getHeight() - dsty;

    ready();
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->NLR = (dstRect.getWidth() << 16) |  trail;
    DMA2D->CR = DMA2D_CR_START | DMA2D_R2M | DMA2D_CR_TCIE;
    mDma2dWait = eWaitIrq;
    }
    //}}}
  }
//}}}
//{{{
void cLcd::rgb888to565 (uint8_t* src, uint16_t* dst, uint16_t xsize, uint16_t ysize) {

  ready();

  DMA2D->FGPFCCR = DMA2D_INPUT_RGB888;
  DMA2D->FGOR = 0;
  DMA2D->FGMAR = (uint32_t)src;
  DMA2D->OMAR = (uint32_t)dst;
  DMA2D->NLR = (xsize << 16) | ysize;
  DMA2D->OOR = getWidth() - xsize;

  // start transfer
  DMA2D->CR = DMA2D_CR_START | DMA2D_M2M_PFC | DMA2D_CR_TCIE;
  mDma2dWait = eWaitIrq;
  }
//}}}
//{{{
void cLcd::rgb888to565cpu (uint8_t* src, uint16_t* dst, uint16_t xsize, uint16_t ysize) {

  ready();
  for (uint16_t x = 0; x < xsize; x++) {
    uint8_t b = (*src++) & 0xF8;
    uint8_t g = (*src++) & 0xFC;
    uint8_t r = (*src++) & 0xF8;
    *dst++ = (r << 8) | (g << 3) | (b >> 3);
    }
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

  ready();
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

uint16_t cLcd::getTextHeight() { return kFont16.height; }
//{{{
int16_t cLcd::getCharWidth (uint8_t ascii) {

  if ((ascii >= kFont16.firstChar) && (ascii <= kFont16.lastChar)) {
    auto char8 = (uint8_t*)(kFont16.glyphsBase + kFont16.glyphOffsets[ascii - kFont16.firstChar]);
    return char8[4];
    }

  return kFont16.spaceWidth;
  }
//}}}
//{{{
void cLcd::displayString (uint16_t color, cPoint p, const std::string& str, eTextAlign textAlign) {

  alignPos (p, str, textAlign);

  uint32_t color32 = ((color & 0xf800) << 8) | ((color & 0x07d0) << 5) | ((color & 0x001f) << 3);
  for (auto ch : str) {
    p.x += displayChar8 (color32, p, ch);
    if (p.x > getWidth())
      break;
    }
  }
//}}}
//{{{
void cLcd::displayStringShadow (uint16_t color, cPoint p, const std::string& str, eTextAlign textAlign) {

  alignPos (p, str, textAlign);
  cPoint p1 = p + cPoint (1,1);

  for (auto ch : str) {
    p1.x += displayChar8 (0, p1, ch);
    if (p1.x > getWidth())
      break;
    }

  uint32_t color32 = ((color & 0xf800) << 8) | ((color & 0x07d0) << 5) | ((color & 0x001f) << 3);  //a:r:g:b
  for (auto ch : str) {
    p.x += displayChar8 (color32, p, ch);
    if (p.x > getWidth())
      break;
    }
  }
//}}}

//{{{
void cLcd::fillRect (uint16_t color, const cRect& rect) {

  ready();

  DMA2D->OCOLR = color;
  DMA2D->OOR = getWidth() - rect.getWidth();
  DMA2D->OMAR = (uint32_t)(mFrameBuf + rect.top*getWidth() + rect.left);
  DMA2D->NLR = (rect.getWidth() << 16) |  rect.getHeight();

  // start transfer
  DMA2D->CR = DMA2D_CR_START | DMA2D_R2M | DMA2D_CR_TCIE;
  mDma2dWait = eWaitIrq;
  }
//}}}
//{{{
void cLcd::fillRectCpu (uint16_t color, const cRect& rect) {
// dma2d hogs bandwidth

  ready();

  auto pitch = getWidth() - rect.getWidth();
  auto dst = mFrameBuf + rect.top*getWidth() + rect.left;

  for (auto y = 0; y < rect.getHeight(); y++) {
    for (auto x = 0; x < rect.getWidth(); x++)
      *dst++ = color;
    dst += pitch;
    }
  }
//}}}
//{{{
void cLcd::drawRect (uint16_t color, const cRect& rect, uint16_t thickness) {

  fillRect (color, cRect (rect.left, rect.top, rect.right, rect.top + thickness));
  fillRect (color, cRect (rect.left, rect.bottom - thickness, rect.right, rect.bottom));
  fillRect (color, cRect (rect.left, rect.top, rect.left + thickness, rect.bottom));
  fillRect (color, cRect (rect.right - thickness, rect.top, rect.right, rect.bottom));
  }
//}}}

//{{{
void cLcd::drawCircle (uint16_t color, cPoint centre, uint16_t radius) {

  int32_t decision = 3 - (radius << 1);
  cPoint p = {0, radius};

  ready();
  while (p.x <= p.y) {
    drawPix (color, centre.x + p.x, centre.y - p.y);
    drawPix (color, centre.x - p.x, centre.y - p.y);
    drawPix (color, centre.x + p.y, centre.y - p.x);
    drawPix (color, centre.x - p.y, centre.y - p.x);
    drawPix (color, centre.x + p.x, centre.y + p.y);
    drawPix (color, centre.x - p.x, centre.y + p.y);
    drawPix (color, centre.x + p.y, centre.y + p.x);
    drawPix (color, centre.x - p.y, centre.y + p.x);

    if (decision < 0)
      decision += (p.x << 2) + 6;
    else {
      decision += ((p.x - p.y) << 2) + 10;
      p.y--;
      }

    p.x++;
    }
  }
//}}}
//{{{
void cLcd::fillCircle (uint16_t color, cPoint centre, uint16_t radius) {

  int32_t decision = 3 - (radius << 1);
  uint32_t current_x = 0;
  uint32_t current_y = radius;

  while (current_x <= current_y) {
    if (current_y > 0) {
      fillRect (color, centre.x - current_y, centre.y + current_x, 1, 2*current_y);
      fillRect (color, centre.x - current_y, centre.y - current_x, 1, 2*current_y);
      }
    if (current_x > 0) {
      fillRect (color, centre.x - current_x, centre.y - current_y, 1, 2*current_x);
      fillRect (color, centre.x - current_x, centre.y + current_y, 1, 2*current_x);
      }
    if (decision < 0)
      decision += (current_x << 2) + 6;
    else {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
      }
    current_x++;
    }

  drawCircle (color, centre, radius);
  }
//}}}
//{{{
void cLcd::drawEllipse (uint16_t color, cPoint centre, cPoint radius) {

  int x = 0;
  int y = -radius.y;

  int err = 2 - 2 * radius.x;
  float k = (float)radius.y / (float)radius.x;

  ready();
  do {
    drawPix (color, (centre.x - (int16_t)(x / k)), centre.y + y);
    drawPix (color, (centre.x + (int16_t)(x / k)), centre.y + y);
    drawPix (color, (centre.x + (int16_t)(x / k)), centre.y - y);
    drawPix (color, (centre.x - (int16_t)(x / k)), centre.y - y);

    int e2 = err;
    if (e2 <= x) {
      err += ++x * 2+ 1 ;
      if (-y == x && e2 <= y)
        e2 = 0;
      }
    if (e2 > y)
      err += ++y *2 + 1;
    } while (y <= 0);
  }
//}}}
//{{{
void cLcd::fillEllipse (uint16_t color, cPoint centre, cPoint radius) {

  int x = 0;
  int y = -radius.y;
  int err = 2 - 2 * radius.x;
  float k = (float)radius.y / (float)radius.x;

  do {
    fillRect (color, (centre.x - (int16_t)(x/k)), (centre.y + y), 1, (2 * (int16_t)(x / k) + 1));
    fillRect (color, (centre.x - (int16_t)(x/k)), (centre.y - y), 1, (2 * (int16_t)(x / k) + 1));

    int e2 = err;
    if (e2 <= x) {
      err += ++x * 2 + 1;
      if (-y == x && e2 <= y)
        e2 = 0;
      }
    if (e2 > y)
      err += ++y*2+1;
    } while (y <= 0);
  }
//}}}
//{{{
void cLcd::drawPolygon (uint16_t color, cPoint* points, uint16_t numPoints) {

  int16_t x = 0, y = 0;

  if (numPoints < 2)
    return;

  drawLine (color, *points, *(points + numPoints-1));

  while (--numPoints) {
    cPoint point = *points++;
    drawLine (color, point, *points);
    }
  }
//}}}
//{{{
void cLcd::fillPolygon (uint16_t color, cPoint* points, uint16_t numPoints) {

  cPoint tl = *points;
  cPoint br = *points;

  cPoint pixel;
  for (int16_t counter = 1; counter < numPoints; counter++) {
    pixel.x = POLY_X (counter);
    if (pixel.x < tl.x)
      tl.x = pixel.x;
    if (pixel.x > br.x)
      br.x = pixel.x;

    pixel.y = POLY_Y(counter);
    if (pixel.y < tl.y)
      tl.y = pixel.y;
    if (pixel.y > br.y)
      br.y = pixel.y;
    }

  if (numPoints < 2)
    return;

  cPoint centre = (tl + br) / 2;
  cPoint first = *points;

  ready();
  cPoint p2;
  while (--numPoints) {
    cPoint p1 = *points;
    points++;
    p2 = *points;

    fillTriangle (color, p1, p2, centre);
    fillTriangle (color, p1, centre, p2);
    fillTriangle (color, centre, p2, p1);
    }

  fillTriangle (color, first, p2, centre);
  fillTriangle (color, first, centre, p2);
  fillTriangle (color, centre, p2, first);
  }
//}}}
//{{{
void cLcd::drawLine (uint16_t color, cPoint p1, cPoint p2) {

  int16_t xinc1 = 0, xinc2 = 0, yinc1 = 0, yinc2 = 0;
  int16_t den = 0, num = 0, num_add = 0, num_pixels = 0;

  int16_t deltax = ABS(p2.x - p1.x); // The difference between the x's
  int16_t deltay = ABS(p2.y - p1.y); // The difference between the y's
  int16_t x = p1.x;                       // Start x off at the first pixel
  int16_t y = p1.y;                       // Start y off at the first pixel

  if (p2.x >= p1.x) {
    // The x-values are increasing
    xinc1 = 1;
    xinc2 = 1;
    }
  else {
    // The x-values are decreasing
    xinc1 = -1;
    xinc2 = -1;
    }

  if (p2.y >= p1.y) {
    // The y-values are increasing
    yinc1 = 1;
    yinc2 = 1;
    }
  else {
    // The y-values are decreasing
    yinc1 = -1;
    yinc2 = -1;
    }

  if (deltax >= deltay) { // There is at least one x-value for every y-value
    xinc1 = 0;            // Don't change the x when numerator >= denominator
    yinc2 = 0;            // Don't change the y for every iteration
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;  // There are more x-values than y-values
    }
  else {                  // There is at least one y-value for every x-value
    xinc2 = 0;            // Don't change the x for every iteration
    yinc1 = 0;            // Don't change the y when numerator >= denominator
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;  // There are more y-values than x-values
    }

  ready();
  for (int16_t curpixel = 0; curpixel <= num_pixels; curpixel++) {
    drawPix (color, x, y);
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
void cLcd::displayOn() {

  LTDC->GCR |= LTDC_GCR_LTDCEN;
  HAL_GPIO_WritePin (LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);        // Assert LCD_DISP pin
  HAL_GPIO_WritePin (LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);  // Assert LCD_BL_CTRL pin
  }
//}}}
//{{{
void cLcd::displayOff() {

  LTDC->GCR &= ~LTDC_GCR_LTDCEN;
  HAL_GPIO_WritePin (LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_RESET);      // De-assert LCD_DISP pin
  HAL_GPIO_WritePin (LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_RESET);// De-assert LCD_BL_CTRL pin
  }
//}}}

// private
//{{{
void cLcd::alignPos (cPoint& p, const std::string& str, eTextAlign textAlign) {

  switch (textAlign) {
    case eTextCentreBox:
      p.y -= kFont16.height/2;

    case eTextCentre:  {
      uint16_t size = 0;
      for (auto ch : str)
        size += getCharWidth (ch);
      p.x -= size/2;
      }
      break;

    case eTextBottomRight :
      p.y -= kFont16.height;
    case eTextRight: {
      uint16_t size = 0;
      for (auto ch : str)
        size += getCharWidth (ch);
      p.x -= size;
      }
      break;
    }

  if (p.x < 0)
    p.x = 0;
  else if (p.x >= getWidth())
    p.x = 0;
  }
//}}}
//{{{
int16_t cLcd::displayChar (uint16_t color, cPoint p, uint8_t ascii) {

  if ((ascii >= 0x20) && (ascii <= 0x7f)) {
    auto width = gFont16.mWidth;
    auto byteAlignedWidth = (width + 7) / 8;
    auto offset = (8 * byteAlignedWidth) - width - 1;
    auto fontChar = &gFont16.mTable [(ascii - ' ') * gFont16.mHeight * byteAlignedWidth];

    auto dst = mFrameBuf + (p.y * getWidth()) + p.x;

    ready();
    for (auto fontLine = 0u; fontLine < gFont16.mHeight; fontLine++) {
      auto fontPtr = (uint8_t*)fontChar + byteAlignedWidth * fontLine;
      uint16_t fontLineBits = *fontPtr++;
      if (byteAlignedWidth == 2)
        fontLineBits = (fontLineBits << 8) | *fontPtr;
      if (fontLineBits) {
        uint16_t bit = 1 << (width + offset);
        auto endPtr = dst + width;
        while (dst != endPtr) {
          if (fontLineBits & bit)
            *dst = color;
          dst++;
          bit >>= 1;
          }
        dst += getWidth() - width;
        }
      else
        dst += getWidth();
      }
    }

  return gFont16.mWidth;
  }
//}}}
//{{{
int16_t cLcd::displayChar8 (uint32_t color, cPoint p, uint8_t ascii) {

  if ((ascii >= kFont16.firstChar) && (ascii <= kFont16.lastChar)) {
    auto fontChar = (fontChar_t*)(kFont16.glyphsBase + kFont16.glyphOffsets[ascii - kFont16.firstChar]);
    uint32_t dstStride = getWidth() - fontChar->width;
    uint32_t nlr = (fontChar->width << 16) | fontChar->height;
    uint32_t dst = uint32_t(mFrameBuf + ((p.y + 14 - fontChar->top) * getWidth()) + p.x + fontChar->left);
    uint32_t src = (uint32_t)(fontChar) + sizeof (fontChar_t);

    ready();
    DMA2D->FGPFCCR = DMA2D_INPUT_A8; // fgnd PFC
    DMA2D->FGMAR   = src;            // fgnd start address
    DMA2D->FGOR    = 0;
    DMA2D->FGCOLR  = color;
    DMA2D->BGMAR   = dst;            // output start address
    DMA2D->OMAR    = dst;            // output start address
    DMA2D->BGOR    = dstStride;      // output stride
    DMA2D->OOR     = dstStride;      // output stride
    DMA2D->NLR     = nlr;            // width:height
    DMA2D->CR      = DMA2D_CR_START | DMA2D_M2M_BLEND | DMA2D_CR_TCIE;;
    mDma2dWait = eWaitIrq;

    return fontChar->advance;
    }

  return kFont16.spaceWidth;
  }
//}}}

//{{{
void cLcd::fillTriangle (uint16_t color, cPoint p1, cPoint p2, cPoint p3) {

  cPoint inc1;
  cPoint inc2;

  if (p2.x >= p1.x) {
    //{{{  x increasing
    inc1.x = 1;
    inc2.x = 1;
    }
    //}}}
  else {
    //{{{  x decreasing
    inc1.x = -1;
    inc2.x = -1;
    }
    //}}}

  if (p2.y >= p1.y) {
    //{{{  y increasing
    inc1.y = 1;
    inc2.y = 1;
    }
    //}}}
  else {
    //{{{  y decreasing
    inc1.y = -1;
    inc2.y = -1;
    }
    //}}}

  int16_t den;
  int16_t num;
  int16_t num_add;
  int16_t num_pixels;

  int16_t deltax = ABS (p2.x - p1.x);  // The difference between the x's
  int16_t deltay = ABS (p2.y - p1.y);  // The difference between the y's
  if (deltax >= deltay) {
    //{{{  at least one x-value for every y-value
    inc1.x = 0;           // Don't change the x when numerator >= denominator
    inc2.y = 0;           // Don't change the y for every iteration

    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;  // There are more x-values than y-values
    }
    //}}}
  else {
    //{{{  at least one y-value for every x-value
    inc2.x = 0;           // Don't change the x for every iteration
    inc1.y = 0;           // Don't change the y when numerator >= denominator

    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay; // There are more y-values than x-values
    }
    //}}}

  cPoint p = p1;
  for (int16_t curpixel = 0; curpixel <= num_pixels; curpixel++) {
    drawLine (color, p, p3);
    num += num_add;     // Increase the numerator by the top of the fraction
    if (num >= den)  {  // Check if numerator >= denominator
      num -= den;       // Calculate the new numerator value
      p += inc1;       // Change the x as appropriate
      }

    p += inc2;         // Change the x as appropriate
    }
  }
//}}}
