#include "timestamp.h"
#include <stdio.h>

static struct {
    int s;
    int us;
} time;

void time_set(int s, int us)
{
    printf("seconds : %d\n", s);
    printf("useconds : %d\n", us);
    time.s = s;
    time.us = us;
}
