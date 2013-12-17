#include "lwip/opt.h"

#include "os_cpu.h"
#include "os_cfg.h"
#include "ucos_ii.h"
#include "time.h"


static struct {
    int s; /**< Time at last call to time_set in seconds since 1970. */
    int us;  /**< Fraction of second at last call of time_set in seconds since 1970. */
    int last_sync_time; /**< Tick count at which time_set was last called. */
} current_time;


void time_init(void)
{
    time_set(0, 0);
}

int time_get_s(void)
{
    int s;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    s = current_time.s;
    s += (OSTimeGet() - current_time.last_sync_time) / OS_TICKS_PER_SEC;
    OS_EXIT_CRITICAL();
    return s;
}

int time_get_us(void)
{
    int us;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    us = (1000000/OS_TICKS_PER_SEC) * (OSTimeGet()-current_time.last_sync_time);
    us += current_time.us;
    us = us % 1000000;
    OS_EXIT_CRITICAL();
    return us;
}

void time_set(int s, int us)
{
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    current_time.us = us;
    current_time.s = s;
    current_time.last_sync_time = OSTimeGet();
    OS_EXIT_CRITICAL();
}
