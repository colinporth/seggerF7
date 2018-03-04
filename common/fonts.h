#ifndef __FONTS_H
#define __FONTS_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

typedef struct _tFont
{
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;

} sFONT;

extern sFONT Font16;

#define LINE(x) ((x) * (((sFONT *)BSP_LCD_GetFont())->Height))

#ifdef __cplusplus
}
#endif

#endif /* __FONTS_H */
