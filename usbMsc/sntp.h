#pragma once

#include "lwip/ip_addr.h"

void sntpSetServerName (u8_t idx, char* server);
void sntpInit();
