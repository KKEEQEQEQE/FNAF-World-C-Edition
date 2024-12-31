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

#include "World_Chip_Note.h"
#include "UI.h"
#include <malloc.h>
#include <stdint.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CHIPS_IN_QUEUE 10
#define CHIP_NOTE_SCREEN_TIME 2
static uint8_t chip_queue[MAX_CHIPS_IN_QUEUE] = {0};

static uint8_t wait_queue = 0;
static uint8_t current_queue = 0;
static UIElement current_chip_banner = {{0}, 0, -0.8f, 1};
static Sound note_sfx = {0};

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

uint8_t GetAvailableChipQueue(void)
{
    uint8_t index = 0;

    for (; chip_queue[index] && index < MAX_CHIPS_IN_QUEUE; index++);

    return index;
}

uint8_t GetUnavailableChipQueue(void)
{
    uint8_t index = 0;

    for (; !chip_queue[index] && index < MAX_CHIPS_IN_QUEUE; index++);

    return index;
}
    
void AddChipNoteQueue(uint8_t id)
{
    uint8_t index = GetAvailableChipQueue();

    if (index == MAX_CHIPS_IN_QUEUE) return;

    wait_queue++;

    chip_queue[index] = id + 1;
}

void LoadNewChipBanner(void)
{
    char path[42] = "Assets/Overworld/UI/Chip_Banners/";

    if (chip_queue[current_queue] == 0) return;
    
    if (!IsSoundValid(note_sfx)) note_sfx = LoadSound("Assets/Sound_Effects/New_Chip.wav");

    PlaySound(note_sfx);

    strcat( path, 
            uintstr(chip_queue[current_queue] - 1));

    strcat(path, ".png");

    printf("%s\n", path);

    current_chip_banner.visual = CreateUIVisual_UIAnimation_V2( path, 
                                                                10, 
                                                                6, 
                                                                (Vector2) {400, 100}, 
                                                                WHITE);
}

void UpdateChipNoteBanner(void)
{
    static clock_t cooldown = 0;
    static uint8_t last_queue = 0;

    if (!wait_queue) return;
    
    if (last_queue == 0 && wait_queue)
    {
        current_queue = GetUnavailableChipQueue();
        LoadNewChipBanner();
        cooldown = clock() + CHIP_NOTE_SCREEN_TIME * CLOCKS_PER_SEC;
        last_queue = wait_queue;
        return;
    }
    
    if (clock() + CHIP_NOTE_SCREEN_TIME * CLOCKS_PER_SEC - cooldown >= CHIP_NOTE_SCREEN_TIME * CLOCKS_PER_SEC)
    {
        chip_queue[current_queue] = 0;
        current_queue = GetUnavailableChipQueue();

        if (current_chip_banner.visual.type != UInotype) FreeUIVisual(&current_chip_banner.visual);
        memset(&current_chip_banner.visual, 0, sizeof(UIVisual));
        
        if (current_queue == MAX_CHIPS_IN_QUEUE) 
        {
            wait_queue = 0; 
            
            return;
        }
        LoadNewChipBanner();
        cooldown = clock() + CHIP_NOTE_SCREEN_TIME * CLOCKS_PER_SEC;
    }
    
    last_queue = wait_queue;
}

void PrintChipQueue(void)
{
    printf("{ ");

    for (uint8_t i = 0; i < MAX_CHIPS_IN_QUEUE; i++)
    {
        printf("%u, ", chip_queue[i]);
    }

    printf("}\n");
}

void RenderChipNoteBanner(void)
{
    UpdateChipNoteBanner();
    PrintChipQueue();
    RenderUIElement(&current_chip_banner);
}