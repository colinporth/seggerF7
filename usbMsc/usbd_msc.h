#pragma once
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}

#include "usbd.h"

class cLcd;
void mscInit (cLcd* lcd);
void mscStart();

//{{{
#ifdef __cplusplus
}
#endif
//}}}
