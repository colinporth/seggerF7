#pragma once
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}

#include <stdint.h>

struct font8_t {
  uint8_t fixedWidth;
  uint8_t height;
  uint8_t spaceWidth;
  uint8_t firstChar;
  uint8_t lastChar;
  uint8_t* glyphsBase;
  uint16_t* glyphOffsets;
  };

extern const struct font8_t kFont18;

//{{{
#ifdef __cplusplus
}
#endif
//}}}
