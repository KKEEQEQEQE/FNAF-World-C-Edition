#include "../Include/raylib.h"
#include <time.h>
#include "rayclock.h"

clock_t raytime_clock = 0;

clock_t Rayclock(void) 
{
    return raytime_clock;
}

void UpdateRayclock(void)
{
    raytime_clock += GetFrameTime() * CLOCKS_PER_SEC;
}