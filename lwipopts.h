/*
 * lwipopts.h
 *
 *  Created on: Oct 31, 2013
 *      Author: Antoine
 */

#ifndef LWIPOPTS_H_

#define LWIPOPTS_H_


/* Network Time protocol config. */
#define SNTP_SERVER_ADDRESS  "213.161.194.93" /* pool.ntp.org */



#define LWIP_ARP 0
#define SYS_LIGHTWEIGHT_PROT 0
#define MEM_LIBC_MALLOC 1
#define MEMP_MEM_MALLOC 1
#define MEM_ALIGNMENT 4
#define IP_FORWARD 1
#define IP_REASSEMBLY 1
#define IP_FRAG 1
#define LWIP_BROADCAST_PING 1
#define LWIP_MULTICAST_PING 1
#define LWIP_DNS 1
#define LWIP_HAVE_LOOPIF 1
#define LWIP_NETIF_LOOPBACK 1

#if 0
#define PPP_SUPPORT 1
#define PPPOS_SUPPORT 1
#define NUM_PPP 1
#else
#define LWIP_HAVE_SLIPIF 1
#endif

//#define LWIP_DEBUG 1
#define  MEM_SIZE 10000

/* See README.md for priority list. */
#define TCPIP_THREAD_PRIO 30
#define SLIPIF_THREAD_PRIO 31

/* Should never be used. */
#define DEFAULT_THREAD_PRIO 29


#endif /* LWIPOPTS_H_ */
