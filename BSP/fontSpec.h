#pragma once
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}

#include <stdint.h>

struct fontChar_t {
  uint8_t width;
  uint8_t height;
  int8_t  left;
  int8_t  top;
  uint8_t advance;
  };

struct fontSpec_t {
  uint8_t fixedWidth;
  uint8_t height;
  uint8_t spaceWidth;
  uint8_t firstChar;
  uint8_t lastChar;
  uint8_t* glyphsBase;
  uint16_t* glyphOffsets;
  };

extern const struct fontSpec_t kFont16;

//{{{
#ifdef __cplusplus
}
#endif
//}}}
