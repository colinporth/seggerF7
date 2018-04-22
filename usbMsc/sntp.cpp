//{{{
// SNTP client module
/*
 * Copyright (c) 2007-2009 Frédéric Bernon, Simon Goldschmidt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Frédéric Bernon, Simon Goldschmidt
 */
//}}}
//{{{
#include "sntp.h"

#include "lwip/opt.h"
#include "lwip/timeouts.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "lwip/dhcp.h"

#include <string.h>
#include <time.h>

#include "cLcd.h"
//}}}
//{{{  options
// The maximum number of SNTP servers that can be set
#define SNTP_MAX_SERVERS  LWIP_DHCP_MAX_NTP_SERVERS

// Set this to 1 to implement the callback function called by dhcp when * NTP servers are received
#define SNTP_GET_SERVERS_FROM_DHCP LWIP_DHCP_GET_NTP_SRV

// Set this to 1 to support DNS names (or IP address strings) to set sntp servers
// One server address/name can be defined as default if SNTP_SERVER_DNS == 1:
//#define SNTP_SERVER_ADDRESS "pool.ntp.org"
#define SNTP_SERVER_DNS  1

// SNTP_DEBUG: Enable debugging for SNTP
#define SNTP_DEBUG  LWIP_DBG_OFF

// SNTP server port
#define SNTP_PORT 123

// Sanity check:
//  Define this to
//  - 0 to turn off sanity checks (default; smaller code)
//  - >= 1 to check address and port of the response packet to ensure the
//         response comes from the server we sent the request to.
//  - >= 2 to check returned Originate Timestamp against Transmit Timestamp
//         sent to the server (to ensure response to older request).
//  - >= 3 @todo: discard reply if any of the LI, Stratum, or Transmit Timestamp
//         fields is 0 or the Mode field is not 4 (unicast) or 5 (broadcast).
//  - >= 4 @todo: to check that the Root Delay and Root Dispersion fields are each
//         greater than or equal to 0 and less than infinity, where infinity is
//         currently a cozy number like one second. This check avoids using a
//         server whose synchronization source has expired for a very long time. */
#define SNTP_CHECK_RESPONSE   2

// According to the RFC, this shall be a random delay * between 1 and 5 minutes (in milliseconds) to prevent load peaks.
// This can be defined to a random generation function, * which must return the delay in milliseconds as u32_t.
#define SNTP_STARTUP_DELAY  0

// If you want the startup delay to be a function, define this
// to a function (including the brackets) and define SNTP_STARTUP_DELAY to 1. */
#define SNTP_STARTUP_DELAY_FUNC  SNTP_STARTUP_DELAY

// SNTP receive timeout - in milliseconds
// Also used as retry timeout - this shouldn't be too low. * Default is 3 seconds. */
#define SNTP_RECV_TIMEOUT  3000

// SNTP update delay - in milliseconds
// Default is 1 hour. Must not be beolw 15 seconds by specification (i.e. 15000) */
#define SNTP_UPDATE_DELAY  3600000

// Default retry msTimeout
#define SNTP_RETRY_TIMEOUT          SNTP_RECV_TIMEOUT
//}}}
//{{{  defines
// the various debug levels for this file
#define SNTP_DEBUG_TRACE        (SNTP_DEBUG | LWIP_DBG_TRACE)
#define SNTP_DEBUG_STATE        (SNTP_DEBUG | LWIP_DBG_STATE)
#define SNTP_DEBUG_WARN         (SNTP_DEBUG | LWIP_DBG_LEVEL_WARNING)
#define SNTP_DEBUG_WARN_STATE   (SNTP_DEBUG | LWIP_DBG_LEVEL_WARNING | LWIP_DBG_STATE)
#define SNTP_DEBUG_SERIOUS      (SNTP_DEBUG | LWIP_DBG_LEVEL_SERIOUS)

#define SNTP_ERR_KOD                1

// SNTP protocol defines
#define SNTP_MSG_LEN                48

#define SNTP_OFFSET_LI_VN_MODE      0
#define SNTP_LI_MASK                0xC0
#define SNTP_LI_NO_WARNING          0x00
#define SNTP_LI_LAST_MINUTE_61_SEC  0x01
#define SNTP_LI_LAST_MINUTE_59_SEC  0x02
#define SNTP_LI_ALARM_CONDITION     0x03 // (clock not synchronized)

#define SNTP_VERSION_MASK           0x38
#define SNTP_VERSION                (4/* NTP Version 4*/<<3)

#define SNTP_MODE_MASK              0x07
#define SNTP_MODE_CLIENT            0x03
#define SNTP_MODE_SERVER            0x04
#define SNTP_MODE_BROADCAST         0x05

#define SNTP_OFFSET_STRATUM         1
#define SNTP_STRATUM_KOD            0x00

#define SNTP_OFFSET_ORIGINATE_TIME  24
#define SNTP_OFFSET_RECEIVE_TIME    32
#define SNTP_OFFSET_TRANSMIT_TIME   40

// number of seconds between 1900 and 1970 (MSB=1)
#define DIFF_SEC_1900_1970         (2208988800UL)
// number of seconds between 1970 and Feb 7, 2036 (6:28:16 UTC) (MSB=0)
#define DIFF_SEC_1970_2036         (2085978496UL)
//}}}

//{{{
// SNTP packet format (without optional fields)
// Timestamps are coded as 64 bits:
// - 32 bits seconds since Jan 01, 1970, 00:00
// - 32 bits seconds fraction (0-padded)
// For future use, if the MSB in the seconds part is set, seconds are based  on Feb 07, 2036, 06:28:16.
PACK_STRUCT_BEGIN struct sntp_msg {
  PACK_STRUCT_FLD_8 (u8_t li_vn_mode);
  PACK_STRUCT_FLD_8 (u8_t stratum);
  PACK_STRUCT_FLD_8 (u8_t poll);
  PACK_STRUCT_FLD_8 (u8_t precision);
  PACK_STRUCT_FIELD (u32_t root_delay);
  PACK_STRUCT_FIELD (u32_t root_dispersion);
  PACK_STRUCT_FIELD (u32_t reference_identifier);
  PACK_STRUCT_FIELD (u32_t reference_timestamp[2]);
  PACK_STRUCT_FIELD (u32_t originate_timestamp[2]);
  PACK_STRUCT_FIELD (u32_t receive_timestamp[2]);
  PACK_STRUCT_FIELD (u32_t transmit_timestamp[2]);
  } PACK_STRUCT_STRUCT;
PACK_STRUCT_END
//}}}
//{{{
struct sntp_server {
  char* name;
  ip_addr_t addr;
  };
//}}}

struct udp_pcb* sntp_pcb;
struct sntp_server sntp_servers[SNTP_MAX_SERVERS];

u8_t sntp_current_server;           // The currently used server (initialized to 0)
ip_addr_t sntp_last_server_address; // Saves the last server address to compare with response
u32_t sntp_last_timestamp_sent[2];  // Save last timestamp sent to compare against in response

void sntp_request (void* arg);
//{{{
void sntp_retry (void* arg) {

  cLcd::mLcd->debug (LCD_COLOR_YELLOW, "sntp_retry next request %d ms", SNTP_RETRY_TIMEOUT);
  sys_timeout (SNTP_RETRY_TIMEOUT, sntp_request, NULL);
  }
//}}}
//{{{
void sntp_initialize_request (struct sntp_msg* req) {

  memset (req, 0, SNTP_MSG_LEN);
  req->li_vn_mode = SNTP_LI_NO_WARNING | SNTP_VERSION | SNTP_MODE_CLIENT;

  // fill in transmit timestamp and save it in 'sntp_last_timestamp_sent'
  u32_t sntp_time_sec = 0;
  u32_t sntp_time_us = 0;

  sntp_last_timestamp_sent[0] = lwip_htonl (sntp_time_sec + DIFF_SEC_1900_1970);
  req->transmit_timestamp[0] = sntp_last_timestamp_sent[0];

  // we send/save us instead of fraction to be faster...
  sntp_last_timestamp_sent[1] = lwip_htonl (sntp_time_us);
  req->transmit_timestamp[1] = sntp_last_timestamp_sent[1];
  }
//}}}
//{{{
void sntp_try_next_server (void* arg) {

  auto old_server = sntp_current_server;
  for (u8_t i = 0; i < SNTP_MAX_SERVERS - 1; i++) {
    sntp_current_server++;
    if (sntp_current_server >= SNTP_MAX_SERVERS) {
      sntp_current_server = 0;
      }

    if (!ip_addr_isany(&sntp_servers[sntp_current_server].addr) || (sntp_servers[sntp_current_server].name != NULL)) {
      cLcd::mLcd->debug (LCD_COLOR_YELLOW, "sntp_try_next_server %d", (u16_t)sntp_current_server);

      // instantly send a request to the next server
      sntp_request (NULL);
      return;
      }
    }

  // no other valid server found
  sntp_current_server = old_server;
  sntp_retry (NULL);
  }
//}}}
//{{{
void sntp_send_request (const ip_addr_t* server_addr) {

  auto p = pbuf_alloc (PBUF_TRANSPORT, SNTP_MSG_LEN, PBUF_RAM);
  if (p != NULL) {
    // initialize request message
    auto sntpmsg = (struct sntp_msg*)p->payload;
    sntp_initialize_request (sntpmsg);

    // send request
    udp_sendto (sntp_pcb, p, server_addr, SNTP_PORT);

    // free the pbuf after sending it
    pbuf_free (p);

    // set up receive timeout: try next server or retry on timeout
    sys_timeout ((u32_t)SNTP_RECV_TIMEOUT, sntp_try_next_server, NULL);

    // save server address to verify it in sntp_recv
    ip_addr_set (&sntp_last_server_address, server_addr);
    }
  else {
    cLcd::mLcd->debug (LCD_COLOR_RED, "sntp_send_request - no mem");
    sys_timeout ((u32_t)SNTP_RETRY_TIMEOUT, sntp_request, NULL);
    }
  }
//}}}
//{{{
void sntp_process (u32_t receive_timestamp) {

  // convert SNTP time (1900-based) to unix GMT time (1970-based) * if MSB is 0, SNTP time is 2036-based!
  u32_t rx_secs = lwip_ntohl (receive_timestamp);
  int is_1900_based = ((rx_secs & 0x80000000) != 0);
  u32_t t = is_1900_based ? (rx_secs - DIFF_SEC_1900_1970) : (rx_secs + DIFF_SEC_1970_2036);
  time_t tim = t;

  // display local time from GMT time
  cLcd::mLcd->debug (LCD_COLOR_GREEN, "sntp %s", ctime(&tim));
  }
//}}}
//{{{
void sntp_recv (void* arg, struct udp_pcb* pcb, struct pbuf* p, const ip_addr_t* addr, u16_t port) {

  // packet received: stop retry timeout
  sys_untimeout (sntp_try_next_server, NULL);
  sys_untimeout (sntp_request, NULL);

  // check server address and port
  u8_t mode;
  err_t err = ERR_ARG;
  u32_t receive_timestamp;
  if (ip_addr_cmp (addr, &sntp_last_server_address) && (port == SNTP_PORT)) {
    // process the response
    if (p->tot_len == SNTP_MSG_LEN) {
      pbuf_copy_partial (p, &mode, 1, SNTP_OFFSET_LI_VN_MODE);
      mode &= SNTP_MODE_MASK;
      // if this is a SNTP response...
      if (mode == SNTP_MODE_SERVER) {
        u8_t stratum;
        pbuf_copy_partial (p, &stratum, 1, SNTP_OFFSET_STRATUM);
        if (stratum == SNTP_STRATUM_KOD) {
          // Kiss-of-death packet. Use another server or increase UPDATE_DELAY
          err = SNTP_ERR_KOD;
          cLcd::mLcd->debug (LCD_COLOR_RED, "sntp_recv - Kiss-of-Death");
          }
        else {
          // check originate_timetamp against sntp_last_timestamp_sent
          u32_t originate_timestamp[2];
          pbuf_copy_partial (p, &originate_timestamp, 8, SNTP_OFFSET_ORIGINATE_TIME);
          if ((originate_timestamp[0] != sntp_last_timestamp_sent[0]) ||
              (originate_timestamp[1] != sntp_last_timestamp_sent[1]))
            cLcd::mLcd->debug (LCD_COLOR_RED, "sntp_recv - invalid originate timestamp");
          else { // correct answer
            err = ERR_OK;
            pbuf_copy_partial (p, &receive_timestamp, 4, SNTP_OFFSET_TRANSMIT_TIME);
            }
          }
        }
      else { // wait for correct response
        cLcd::mLcd->debug (LCD_COLOR_RED, "sntp_recv - invalid mode %d", (u16_t)mode);
        err = ERR_TIMEOUT;
        }
      }
    else
      cLcd::mLcd->debug (LCD_COLOR_RED, "sntp_recv - nvalid packetLen %d", p->tot_len);
    }
  else // packet from wrong remote address or port, wait for correct response
    err = ERR_TIMEOUT;
  pbuf_free (p);

  if (err == ERR_OK) {
    sntp_process (receive_timestamp);
    // Set up timeout for next request
    sys_timeout ((u32_t)SNTP_UPDATE_DELAY, sntp_request, NULL);
    }
  else if (err != ERR_TIMEOUT) {
    // Errors are only processed in case of an explicit poll response
    if (err == SNTP_ERR_KOD) // Kiss-of-death packet Use another server or increase UPDATE_DELAY
      sntp_try_next_server (NULL);
    else // another error, try the same server again
      sntp_retry (NULL);
    }
  }
//}}}
//{{{
void sntp_dns_found (const char* hostname, const ip_addr_t* ipaddr, void* arg) {

  if (ipaddr != NULL)
    sntp_send_request (ipaddr);
  else {
    // DNS resolving failed -> try another server
    cLcd::mLcd->debug (LCD_COLOR_RED, "sntp_dns_found - failed to resolve");
    sntp_try_next_server (NULL);
    }
  }
//}}}
//{{{
void sntp_request (void* arg) {

  // initialize SNTP server address
  err_t err;
  ip_addr_t sntp_server_address;
  if (sntp_servers[sntp_current_server].name) {
    // always resolve the name and rely on dns-internal caching & timeout
    ip_addr_set_zero (&sntp_servers[sntp_current_server].addr);
    err = dns_gethostbyname (sntp_servers[sntp_current_server].name, &sntp_server_address, sntp_dns_found, NULL);
    if (err == ERR_INPROGRESS)
      return;
    else if (err == ERR_OK)
      sntp_servers[sntp_current_server].addr = sntp_server_address;
    }
  else {
    sntp_server_address = sntp_servers[sntp_current_server].addr;
    err = (ip_addr_isany_val (sntp_server_address)) ? ERR_ARG : ERR_OK;
    }

  if (err == ERR_OK)
    sntp_send_request (&sntp_server_address);
  else {
    // address conversion failed, try another server
    cLcd::mLcd->debug (LCD_COLOR_YELLOW, "sntp_request - retry in %ds", SNTP_RETRY_TIMEOUT/1000);
    sys_timeout ((u32_t)SNTP_RETRY_TIMEOUT, sntp_try_next_server, NULL);
    }
  }
//}}}

// init
//{{{
void sntpSetServerName (u8_t idx, char* server) {

  if (idx < SNTP_MAX_SERVERS)
    sntp_servers[idx].name = server;
  }
//}}}
//{{{
void sntpInit() {

  if (sntp_pcb == NULL) {
    sntp_pcb = udp_new_ip_type (IPADDR_TYPE_ANY);
    if (sntp_pcb != NULL) {
      udp_recv (sntp_pcb, sntp_recv, NULL);
      sntp_request (NULL);
      }
    }
  }
//}}}
