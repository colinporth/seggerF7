#pragma once

#define LWIP_NETCONN         1  // LWIP_NETCONN =1: Enable Netconn API (require to use api_lib.c)
#define LWIP_SOCKET          0  // LWIP_SOCKET  =1: Enable Socket API (require to use sockets.c)
#define LWIP_NETIF_HOSTNAME  1

#define NO_SYS               0

// Memory options
#define MEM_ALIGNMENT           4
#define MEM_SIZE                (30*1024) // MEM_SIZE: the size of the heap memory
#define MEMP_NUM_PBUF           10        // MEMP_NUM_PBUF: the number of memp struct pbufs
#define MEMP_NUM_UDP_PCB        6         // MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One per active UDP "connection"
#define MEMP_NUM_TCP_PCB        10        // MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections
#define MEMP_NUM_TCP_PCB_LISTEN 5         // MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections
#define MEMP_NUM_TCP_SEG        16        // MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments
#define MEMP_NUM_SYS_TIMEOUT    10        // MEMP_NUM_SYS_TIMEOUT: the number of simulateously active timeouts

// Pbuf options 
#define PBUF_POOL_SIZE          8    // PBUF_POOL_SIZE: the number of buffers in the pbuf pool
#define PBUF_POOL_BUFSIZE       1524 // PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool

#define LWIP_IPV4               1
#define LWIP_TCP                1
#define TCP_TTL                 255

#define TCP_QUEUE_OOSEQ         1           // TCP should queue segments that arrive out of order, 0 to dsiable
#define TCP_MSS                 (1500 - 40) // TCP_MSS Maximum segment size = (Ethernet MTU - IP header size - TCP header size) */
#define TCP_SND_BUF             (4*TCP_MSS) // TCP sender buffer space (bytes).
#define TCP_SND_QUEUELEN        16          // TCP sender buffer space (pbufs),  >= (2 * TCP_SND_BUF/TCP_MSS) 
#define TCP_WND                 (2*TCP_MSS) // TCP receive window.

#define LWIP_ICMP               1
#define LWIP_DHCP               1
#define LWIP_UDP                1
#define UDP_TTL                 255
#define LWIP_STATS 0
#define LWIP_NETIF_LINK_CALLBACK        1

// The STM32F4x7 allows computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
// - To use this feature let the following define uncommented.
// - To disable it and process by CPU comment the the checksum.
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
#define TCPIP_THREAD_PRIO               osPriorityHigh

#define DEFAULT_UDP_RECVMBOX_SIZE       6
#define DEFAULT_TCP_RECVMBOX_SIZE       6
#define DEFAULT_ACCEPTMBOX_SIZE         6

#define DEFAULT_THREAD_STACKSIZE        500
