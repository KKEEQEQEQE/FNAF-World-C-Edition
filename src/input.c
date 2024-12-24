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
#include <stdint.h>

#define MAX_INPUT_POINTS 11

Vector2 * GetInputMouseTouch(void) 
{
    static Vector2 Input_Points[MAX_INPUT_POINTS + 1] = {0};
    int iPoints = GetTouchPointCount() + IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (iPoints >= MAX_INPUT_POINTS) iPoints = MAX_INPUT_POINTS;
    for (uint8_t i = 0; i < iPoints; i++) Input_Points[i] = GetTouchPosition(i);
    Input_Points[iPoints] = (Vector2) {NAN, NAN};
    return Input_Points;
}

Vector2 * GetInputTap(void) 
{
    static Vector2 Input_Points[MAX_INPUT_POINTS + 1] = {0};
    int iPoints = GetTouchPointCount() + IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (iPoints >= MAX_INPUT_POINTS) iPoints = MAX_INPUT_POINTS;
    for (uint8_t i = 0; i < iPoints; i++) Input_Points[i] = GetTouchPosition(i);
    Input_Points[iPoints] = (Vector2) {NAN, NAN};
    return Input_Points;
}