#pragma once

#include "lwip/api.h"

//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}
void httpServerInit();
void DynWebPage (struct netconn *conn);
//{{{
#ifdef __cplusplus
}
#endif
//}}}
