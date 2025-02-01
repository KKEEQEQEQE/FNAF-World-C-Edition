#pragma once

#include <time.h>

#define RAYCLOCKS_PER_SEC 1000

clock_t Rayclock(void);
void UpdateRayclock(void);