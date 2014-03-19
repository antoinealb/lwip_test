/*
 * lwipopts.h
 *
 *  Created on: Oct 31, 2013
 *      Author: Antoine
 */

#ifndef LWIPOPTS_H_
#define LWIPOPTS_H_



/* Network Time protocol config. */
#define SNTP_SERVER_ADDRESS  "77.245.18.26" /* pool.ntp.org */
#define SNTP_SET_SYSTEM_TIME_US(sec, us) time_set(sec, us)
#define SNTP_SOCKET 1
#define SNTP_UPDATE_DELAY (60*1000) /* sync every minute */


#define SYS_LIGHTWEIGHT_PROT 0
#define MEM_LIBC_MALLOC 1
#define MEMP_MEM_MALLOC 1
#define MEM_ALIGNMENT 4
#define IP_FORWARD 1
#define IP_REASSEMBLY 0
#define IP_FRAG 0
#define LWIP_BROADCAST_PING 1
#define LWIP_MULTICAST_PING 1
#define LWIP_DNS 0
#define LWIP_HAVE_LOOPIF 1
#define LWIP_NETIF_LOOPBACK 1


#ifdef __unix__
/* <sys/time.h> is included in cc.h! */
#define LWIP_TIMEVAL_PRIVATE 0
#define LWIP_HAVE_SLIPIF 1
#else
#define LWIP_ARP 0
#define LWIP_HAVE_SLIPIF 1
#endif

//#define LWIP_DEBUG 1
#define  MEM_SIZE 10000

/* See README.md for priority list. */
#define TCPIP_THREAD_PRIO 30
#define SLIPIF_THREAD_PRIO 31
#define SNTP_THREAD_PRIO 34

/* Should never be used. */
#define DEFAULT_THREAD_PRIO 29


#endif /* LWIPOPTS_H_ */
