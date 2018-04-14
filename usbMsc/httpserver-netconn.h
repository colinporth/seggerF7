#pragma once
#include "lwip/api.h"

//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}
void http_server_netconn_init();
void DynWebPage (struct netconn *conn);
//{{{
#ifdef __cplusplus
}
#endif
//}}}
