#include "lwip/opt.h"

#include    "os_cpu.h"
#include    "os_cfg.h"
#include    "ucos_ii.h"

/** Returns the current time, rounded to closest second. */
int32_t time_get_s(void) {
    return OSTimeGet() / OS_TICKS_PER_SEC;
}

/** Returns the current second fraction, rounded to closest second. */
int32_t time_get_us(void) {
    return (1000000 * (OSTimeGet() % OS_TICKS_PER_SEC)) / OS_TICKS_PER_SEC;
}

void time_set(int32_t s, int32_t us) {
    OSTimeSet(OS_TICKS_PER_SEC * s + (OS_TICKS_PER_SEC * us) / 1000000);
}
