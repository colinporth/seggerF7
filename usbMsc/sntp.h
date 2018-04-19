#pragma once

#include "lwip/ip_addr.h"

void sntp_setservername (u8_t idx, char* server);
void sntp_init();
void sntp_stop();
