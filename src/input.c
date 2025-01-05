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


#include "input.h"
#include "UI.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_INPUT_POINTS 11

static uint8_t refresh = 0;

static enum Input_Types current_input_style = KEYBOARD;

// Gets the most recent input method
enum Input_Types GetInputType(void)
{
    return current_input_style;
}

// GetInputType is refreshed and on next GetInputTap, a refresh occurs
void RefreshInput(void)
{
    if (GetKeyPressed() != 0) current_input_style = KEYBOARD;
    else if (GetTouchPointCount() != 0) current_input_style = TOUCH;
    refresh ^= 1;
}

// Returns a Vector2[MAX_INPUT_POINTS] with all input points on screen (finger touching screen or holding down mouse)
Vector2 * GetInputDown(void) 
{
    static Vector2 Input_Points[MAX_INPUT_POINTS] = {0};
    int iPoints = GetTouchPointCount() + IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    if (iPoints >= MAX_INPUT_POINTS) iPoints = MAX_INPUT_POINTS;
    for (uint8_t i = 0; i < iPoints; i++) Input_Points[i] = GetTouchPosition(i);
    Input_Points[iPoints] = (Vector2) {NAN, NAN};
    return Input_Points;
}

static Vector2 GetInputTap_Ex(uint8_t id)
{
    static Vector2 last_check[MAX_INPUT_POINTS] = {0};

    Vector2 current_check = GetTouchPosition(id);

    if (current_check.x == 0 && current_check.y == 0) 
    {
        last_check[id] = (Vector2) {0, 0};
        return (Vector2) {0, 0};
    }

    if (last_check[id].x != 0 || last_check[id].y != 0) return (Vector2) {0, 0};

    last_check[id] = current_check;

    return current_check;
}   


// Gets the position of a tap or mouse click on screen
Vector2 GetInputTap(void)
{
    static uint8_t last_refresh = 0;
    static Vector2 tap_cache[MAX_INPUT_POINTS + 1] = {0};

    // Refreshes every frame (the gameplay main loop calls a refresh)

    if (last_refresh != refresh)
    {
        memset(tap_cache, 0, sizeof(tap_cache));
        tap_cache[0] = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ? GetMousePosition() : (Vector2) {0, 0};
        for (uint8_t i = 1; i < GetTouchPointCount() && i < MAX_INPUT_POINTS; i++)
        {
            tap_cache[i] = GetInputTap_Ex(i - 0);
        }

        last_refresh = refresh; // Refresh gets flipped every frame 
    }

    // Goes through every input to see if there has been a tap

    for (uint8_t i = 0; i < MAX_INPUT_POINTS; i++)
    {
        if (tap_cache[i].x != 0 && tap_cache[i].y != 0) return tap_cache[i];
    }

    return (Vector2) {0, 0};
}