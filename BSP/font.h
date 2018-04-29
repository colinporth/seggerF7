#pragma once
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}

#include <stdint.h>

typedef struct _tFont {
  const uint8_t* mTable;
  uint16_t mWidth;
  uint16_t mHeight;
  } sFONT;

extern const sFONT gFont16;

//{{{
#ifdef __cplusplus
}
#endif
//}}}
