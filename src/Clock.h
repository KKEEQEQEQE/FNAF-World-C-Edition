#include "../Include/raylib.h"
#include <stdint.h>

#pragma once

typedef uint64_t ray_clock_t;
#define CLOCKS_PER_SEC 1000

extern uint64_t ray_clock(void);
extern void clock_update(void);