
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
    ip_init_done = 0;

    tcpip_init(ipinit_done_cb, NULL);                            //call this routine if you use an OS

    while(!ip_init_done);
    printf("IP Stack init complete\n");


    OSTaskDel(IPINIT_TASK_PRIORITY);
}

int main(void)
{
    //printf("--> boot biatch\n");
    sys_init();
    printf("booted in theory mofo\n");
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
