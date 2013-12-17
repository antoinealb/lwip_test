#include "lwip/opt.h"

#include    "os_cpu.h"
#include    "os_cfg.h"
#include    "ucos_ii.h"

int time_get_s(void) {
    return OSTimeGet() / OS_TICKS_PER_SEC;
}

int time_get_us(void) {
    return (1000000 * (OSTimeGet() % OS_TICKS_PER_SEC)) / OS_TICKS_PER_SEC;
}

void time_set(int s, int us) {
    OSTimeSet(OS_TICKS_PER_SEC * s + (OS_TICKS_PER_SEC * us) / 1000000);
}
