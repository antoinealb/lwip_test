
#include <stdio.h>
#include "includes.h"

#include <lwip/tcpip.h>
#include <lwip/ip.h>

#define   TASK_STACKSIZE          2048
#define   IPINIT_TASK_PRIORITY      22

/* Stack for the init task. */
OS_STK    ipinit_task_stk[TASK_STACKSIZE];

/* We allow the use of a shared variable as a IPC mecanism because it is only
 * written to from one thread.
 */
static int ip_init_done;

void ipinit_done_cb(void *a) {
    ip_init_done = 1;
}

void ipinit_task(void* pdata)
{
    /* We reset the flag. */
    ip_init_done = 0;

    /* We start the init of the IP stack. */
    tcpip_init(ipinit_done_cb, NULL);

    printf("Waiting for init complete...\n");

    /* We wait for the IP stack to be fully initialized. */
    while(!ip_init_done);

    printf("IP Stack init complete\n");

    printf("Listing ifs...\n");
    struct netif *n;
    for(n=netif_list;n !=NULL;n=n->next) {
        printf("%s: %p\n", n->name, n->ip_addr.addr);

    }

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
