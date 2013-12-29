
#include <stdio.h>
#include "includes.h"

#include <lwip/tcpip.h>
#include <lwip/ip.h>
#include <netif/slipif.h>

#define   TASK_STACKSIZE          2048
#define   INIT_TASK_PRIORITY      20

/** Stack for the init task. */
OS_STK    init_task_stk[TASK_STACKSIZE];

/** Shared semaphore to signal when lwIP init is done. */
OS_EVENT* lwip_init_done;

/** Serial net interface */
struct netif slipf;

void list_netifs(void) 
{
    struct netif *n; /* used for iteration. */
    for(n=netif_list;n !=NULL;n=n->next) {
        /* Converts the IP adress to a human readable format. */
        char buf[16+1];
        ipaddr_ntoa_r(&n->ip_addr, buf, 17);
        printf("%s%d: %s\n", n->name, n->num, buf);
    }
}

/** @rief Callback for lwIP init completion.
 *
 * This callback is automatically called from the lwIP thread after the 
 * initialization is complete. It must then tell the main init task that it
 * can proceed. To do thism we use a semaphore that is posted from the lwIP
 * thread and on which the main init task is pending. */
void ipinit_done_cb(void *a)
{
    OSSemPost(lwip_init_done);
}


/** @brief Inits the IP stack and the network interfaces.
 *
 * This function is responsible for the following :
 * 1. Initialize the lwIP library.
 * 2. Wait for lwIP init to be complete.
 * 3. Create the SLIP interface and give it a static adress/netmask.
 * 4. Set the SLIP interface as default and create a gateway.
 * 5. List all network interfaces and their settings, for debug purposes.
 */
void ip_stack_init(void) {
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
    netif_add(&slipf, &ipaddr, &netmask, &gw, NULL, slipif_init, tcpip_input);
    netif_set_default(&slipf);
    netif_set_up(&slipf);

    /* Deletes the init done semaphore. */
    OSSemDel(lwip_init_done, OS_DEL_ALWAYS, &err);
}

/** @brief Init task.
 *
 * This task is reponsible to initialize the whole system and to create tasks
 * for other services to run. We do it in a task because we need some of UC/OS-II
 * functions that are only available after OSStart().
 */
void init_task(void *pdata)
{
    ip_stack_init();

    /* Lists every network interface and shows its IP. */
    printf("Listing network interfaces...\n");
    list_netifs();

    /* Creates a simple demo app. */
    ping_init();

    /* We delete the init task before returning. */
    OSTaskDel(INIT_TASK_PRIORITY);
}

int main(void)
{
    printf("==== Boot ====\n");

    /* We have to do all the init in a task because lwIP expects most
     * multi thread functionality to be available right away. */
    OSTaskCreateExt(init_task,
                    NULL,
                    &init_task_stk[TASK_STACKSIZE-1],
                    INIT_TASK_PRIORITY,
                    INIT_TASK_PRIORITY,
                    &init_task_stk[0],
                    TASK_STACKSIZE,
                    NULL, NULL);
    OSStart();

    /* We should never get here because OSSStart() never returns. */
    for(;;);
    return 0;
}
