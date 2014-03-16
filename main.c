#include <stdio.h>

#include <lwip/sys.h>
#include <lwip/tcpip.h>
#include <lwip/ip.h>

#include "sntp.h"
#include "unittest.h"


#ifdef __unix__
#include <netif/tapif.h>
#include <netif/slipif.h>
#else
#include <netif/slipif.h>

/* Needed for UART baudrate. */
#include <io.h>
#endif

#ifndef __unix__
#define   TASK_STACKSIZE          2048
#define   INIT_TASK_PRIORITY      20
#define   BLINK_TASK_PRIORITY      35

/** Stack for the init task. */
OS_STK    init_task_stk[TASK_STACKSIZE];
OS_STK    blink_task_stk[TASK_STACKSIZE];
#endif

/** Shared semaphore to signal when lwIP init is done. */
sys_sem_t lwip_init_done;

/** Global var to say if we are client of server. */
int is_server = 0;

/** Serial net interface */
struct netif slipf;

#ifndef __unix__

void cvra_set_uart_speed(int32_t *uart_adress, int baudrate)
{
    int32_t divisor;
    /* Formule tiree du Embedded IP User Guide page 7-4 */
    divisor = (int32_t)(((float)PIO_FREQ/(float)baudrate) + 0.5);
    IOWR(uart_adress, 0x04, divisor); // ecrit le diviseur dans le bon registre
}

void blink_task(void *pdata)
{
    OS_CPU_SR cpu_sr;
    int32_t led_val;
    while (1) {
        OS_ENTER_CRITICAL();
        led_val = IORD(LED_BASE, 0);
        led_val ^= (1 << 2);
        IOWR(LED_BASE, 0, led_val);
        OS_EXIT_CRITICAL();
        OSTimeDlyHMSM(0, 0, 0, 500);
    }
}
#endif

void list_netifs(void)
{
    struct netif *n; /* used for iteration. */
    for (n = netif_list; n != NULL; n = n->next) {
        /* Converts the IP adress to a human readable format. */
        char buf[16+1];
        ipaddr_ntoa_r(&n->ip_addr, buf, 17);
        printf("%s: %s\n", n->name, buf);
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
    sys_sem_signal(&lwip_init_done);
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
    /* Netif configuration */
    static ip_addr_t ipaddr, netmask, gw;

#ifdef __unix__
    /* for some reason 192.168.4.9 fails on my test station. */
    IP4_ADDR(&gw, 10,0,0,1);

    if (is_server)
        IP4_ADDR(&ipaddr, 10,0,0,2);
    else
        IP4_ADDR(&ipaddr, 10,0,0,3);

    IP4_ADDR(&netmask, 255,255,255,0);
#else
    IP4_ADDR(&gw, 192,168,0,1);
    IP4_ADDR(&ipaddr, 192,168,4,9);
    IP4_ADDR(&netmask, 255,255,255,0);
#endif

    /* Creates the "Init done" semaphore. */
    sys_sem_new(&lwip_init_done, 0);

    /* We start the init of the IP stack. */
    tcpip_init(ipinit_done_cb, NULL);

    /* We wait for the IP stack to be fully initialized. */
    printf("Waiting for LWIP init...\n");
    sys_sem_wait(&lwip_init_done);

    /* Deletes the init done semaphore. */
    sys_sem_free(&lwip_init_done);
    printf("LWIP init complete\n");

    /* Adds a tap pseudo interface for unix debugging. */
    netif_add(&slipf, &ipaddr, &netmask, &gw, &is_server, slipif_init, tcpip_input);

    netif_set_default(&slipf);
    netif_set_up(&slipf);

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

    /* Creates a simple 'echo' app. */
    if (is_server)
        tcpecho_init();
    else
        unit_test_run_all();


#ifndef __unix__
    /* Creates the heartbeat task. */
    OSTaskCreateExt(blink_task,
                    NULL,
                    &blink_task_stk[TASK_STACKSIZE-1],
                    BLINK_TASK_PRIORITY,
                    BLINK_TASK_PRIORITY,
                    &blink_task_stk[0],
                    TASK_STACKSIZE,
                    NULL, NULL);
#endif


#ifndef __unix__
    /* We delete the init task before returning. */
    OSTaskDel(INIT_TASK_PRIORITY);
#endif
}

int main(int argc, char **argv)
{
    printf("==== Boot ====\n");

#ifdef __unix__
    if (argc == 2)
        is_server = 1;
    else
        is_server = 0;
#else
    is_server = 1;
#endif



#ifdef __unix__
    init_task(NULL);
#else
    /* ComBT2 is the main link to the PC (TCP/IP) and then it needs to be fast. */
    cvra_set_uart_speed(COMBT2_BASE, 230400);
    cvra_set_uart_speed(COMBT1_BASE, 57600);
    cvra_set_uart_speed(COMPC_BASE, 57600);
    cvra_set_uart_speed(COMBEACON_BASE, 57600);

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
#endif

    /* We should never get here because OSSStart() never returns. */
    for(;;);
    return 0;
}
