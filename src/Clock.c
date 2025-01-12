#include "../Include/raylib.h"
#include <stdint.h>

typedef uint64_t ray_clock_t;
#define CLOCKS_PER_SECOND 1000

static ray_clock_t clock = 0;

ray_clock_t ray_clock(void)
{
    return clock;
}

void clock_update(void)
{
    clock += GetFrameTime()*CLOCKS_PER_SECOND;
}