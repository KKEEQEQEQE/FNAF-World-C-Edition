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

#include "World_Byte_Note.h"
#include "UI.h"
#include <malloc.h>
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CHIPS_IN_QUEUE 10
#define CHIP_NOTE_SCREEN_TIME 5
static uint8_t chip_queue[MAX_CHIPS_IN_QUEUE] = {0};

static uint8_t wait_queue = 0;
static uint8_t current_queue = 0;
static UIElement current_chip_banner = {{0}, 0, -0.8f, 1};

// Turns a Unsigned Long into a Stack Allocated String
static char * __fastcall uintstr(uint64_t number) 
{
    uint64_t temp = number;
    uint64_t length = 1;
    while (temp > 9) temp /= 10, length++; // Checks number of digits and length of string
    char * string = alloca(length);
    temp = 0;
    while (temp < length) string[length-temp-1] = '0' + number - (number / 10) * 10, number/=10, temp++;
    string[temp] = '\0';
    char * stringLocation = string;
    return stringLocation;
}

void AddChipNoteQueue(uint8_t id)
{
    if (wait_queue == MAX_CHIPS_IN_QUEUE) return;
    chip_queue[wait_queue] = id;
    wait_queue++;
}

void LoadNewChipBanner(void)
{
    static uint8_t last_queue_check = 0;
    if (last_queue_check == current_queue) return;

    if (current_chip_banner.visual.type != UInotype) FreeUIVisual(&current_chip_banner.visual);

    char path[41] = "Assets/Overworld/UI/Chip_Banners/";

    strcat( path, 
            uintstr(current_queue));

    strcat(path, ".png");

    current_chip_banner.visual = CreateUIVisual_UIAnimation_V2( path, 
                                                                10, 
                                                                3, 
                                                                (Vector2) {0,0}, 
                                                                WHITE);
    last_queue_check = current_queue;


}

void RenderChipNoteBanner(void)
{
    static uint8_t cooldown = 0;

    if (cooldown - clock() >= CHIP_NOTE_SCREEN_TIME * CLOCKS_PER_SEC)
    {
        LoadNewChipBanner();
        cooldown = clock();
    }

    RenderUIElement(&current_chip_banner);
}