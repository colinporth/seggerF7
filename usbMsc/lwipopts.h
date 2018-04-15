#pragma once

#define LWIP_NETCONN         1  // LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)

#define LWIP_SOCKET          0  // LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
#define LWIP_NETIF_HOSTNAME  1

// NO_SYS==1: Provides VERY minimal functionality. Otherwise use lwIP facilities.
#define NO_SYS               0

/* ---------- Memory options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT           4

// MEM_SIZE: the size of the heap memory
#define MEM_SIZE                (30*1024)
// MEMP_NUM_PBUF: the number of memp struct pbufs.
#define MEMP_NUM_PBUF           10
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One per active UDP "connection". */
#define MEMP_NUM_UDP_PCB        6
/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections. */
#define MEMP_NUM_TCP_PCB        10
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 5
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments. */
#define MEMP_NUM_TCP_SEG        16
/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active timeouts. */
#define MEMP_NUM_SYS_TIMEOUT    10

/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE          8
/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE       1524

#define LWIP_IPV4               1
#define LWIP_TCP                1
#define TCP_TTL                 255

/* Controls if TCP should queue segments that arrive out of order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         1

/* TCP Maximum segment size. */
#define TCP_MSS                 (1500 - 40)   /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF             (4*TCP_MSS)

/*  TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
  as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. */
#define TCP_SND_QUEUELEN        16

/* TCP receive window. */
#define TCP_WND                 (2*TCP_MSS)

#define LWIP_ICMP               1
#define LWIP_DHCP               1
#define LWIP_UDP                1
#define UDP_TTL                 255

#define LWIP_STATS 0

/* LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface whenever the link changes (i.e., link down) */
#define LWIP_NETIF_LINK_CALLBACK        1

// The STM32F4x7 allows computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
// - To use this feature let the following define uncommented.
// - To disable it and process by CPU comment the  the checksum.
#define CHECKSUM_BY_HARDWARE
#define CHECKSUM_GEN_IP                 0
#define CHECKSUM_GEN_UDP                0
#define CHECKSUM_GEN_TCP                0
#define CHECKSUM_CHECK_IP               0
#define CHECKSUM_CHECK_UDP              0
#define CHECKSUM_CHECK_TCP              0
#define CHECKSUM_GEN_ICMP               0

#define TCPIP_THREAD_NAME              "TCP/IP"
#define TCPIP_THREAD_STACKSIZE          1000
#define TCPIP_MBOX_SIZE                 6
#define DEFAULT_UDP_RECVMBOX_SIZE       6
#define DEFAULT_TCP_RECVMBOX_SIZE       6
#define DEFAULT_ACCEPTMBOX_SIZE         6
#define DEFAULT_THREAD_STACKSIZE        500
#define TCPIP_THREAD_PRIO               osPriorityHigh
