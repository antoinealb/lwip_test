
#include <stdio.h>
#include "includes.h"

#include <lwip/tcpip.h>
#include <lwip/ip.h>
#include <netif/slipif.h>

#define   TASK_STACKSIZE          2048
#define   IPINIT_TASK_PRIORITY      20

/* Stack for the init task. */
OS_STK    ipinit_task_stk[TASK_STACKSIZE];

/* This is a shared semaphore to signal when lwIP init is done. */
OS_EVENT* lwip_init_done;

/* Serial net interface */
struct netif netif;

/** Callback for lwIP init completion. */
void ipinit_done_cb(void *a) {
    OSSemPost(lwip_init_done);
}



void ipinit_task(void* pdata) {
    struct netif *n;
    INT8U err;

    /* Netif configuration */
    static ip_addr_t ipaddr, netmask, gw;
    IP4_ADDR(&gw, 192,168,0,1);
    IP4_ADDR(&ipaddr, 192,168,0,9);
    IP4_ADDR(&netmask, 255,255,255,0);

    /* Creates the "Init done" semaphore. */
    lwip_init_done = OSSemCreate(0);


    /* We start the init of the IP stack. */
    tcpip_init(ipinit_done_cb, NULL);

    /* We wait for the IP stack to be fully initialized. */
    printf("Waiting for LWIP init...\n");
    OSSemPend(lwip_init_done, 0, &err);
    printf("LWIP init complete\n");

    /* Adds the serial interface to the list of network interfaces and makes it the default route. */ 
    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, slipif_init, tcpip_input);
    netif_set_default(&netif);
    netif_set_up(&netif);

    /* Lists every network interface and shows its IP. */
    printf("Listing network interfaces...\n");
    for(n=netif_list;n !=NULL;n=n->next) {
        /* Converts the IP adress to a human readable format. */
        char buf[16+1];
        ipaddr_ntoa_r(&n->ip_addr, buf, 17);
        printf("%s%d: %s\n", n->name, n->num, buf);
    }

    /* Creates a simple demo app. */
    ping_init();

    /* We delete the init task before returning. */
    OSTaskDel(IPINIT_TASK_PRIORITY);
}

int main(void)
{
    printf("==== Boot ====\n");
    sys_init();
    OSTaskCreateExt(ipinit_task,
                    NULL,
                    &ipinit_task_stk[TASK_STACKSIZE-1],
                    IPINIT_TASK_PRIORITY,
                    IPINIT_TASK_PRIORITY,
                    &ipinit_task_stk[0],
                    TASK_STACKSIZE,
                    NULL, NULL);
    OSStart();

    /* We should never get here because OSSStart() never returns. */
    for(;;);
    return 0;
}
