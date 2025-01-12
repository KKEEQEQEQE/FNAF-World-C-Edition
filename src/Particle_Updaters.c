/*
    Zlib License

    Copyright (c) 2024 SpyterDev

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
        misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#include "Particle.h"
#include <stdio.h>
#include "Clock.h"

void Updater_DeleteAfterQuarterSecond(UIParticle * particle)
{
    if (ray_clock() - particle -> startTime > CLOCKS_PER_SEC / 4) 
    {
        particle -> startTime = 0;
    }
}

void Updater_DeleteAfterHalfSecond(UIParticle * particle)
{
    if (ray_clock() - particle -> startTime > CLOCKS_PER_SEC / 2) 
    {
        particle -> startTime = 0;
    }
}

void Updater_DeleteAfterSecond(UIParticle * particle)
{
    if (ray_clock() - particle -> startTime > 1 * CLOCKS_PER_SEC) 
    {
        particle -> startTime = 0;
    }
}

void Updater_DeleteAfter2Seconds(UIParticle * particle)
{
    if (ray_clock() - particle -> startTime > 2 * CLOCKS_PER_SEC) 
    {
        particle -> startTime = 0;
    }
}

void Updater_DeleteAfter3Seconds(UIParticle * particle)
{
    if (ray_clock() - particle -> startTime > 3 * CLOCKS_PER_SEC) 
    {
        particle -> startTime = 0;
    }
}

void Updater_DeleteAfter5Seconds(UIParticle * particle)
{
    if (ray_clock() - particle -> startTime > 5 * CLOCKS_PER_SEC) 
    {
        particle -> startTime = 0;
    }
}
