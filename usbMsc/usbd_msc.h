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
bool hasSdChanged();
uint32_t* mscGetSectors();

//{{{
#ifdef __cplusplus
}
#endif
//}}}
