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


#pragma once

#include "../Include/raylib.h"
#include <math.h>

// The max number of input points recorded at once
#define MAX_INPUT_POINTS 11

// Types of supported input methods
enum Input_Types
{
    KEYBOARD,
    TOUCH
};

// Gets the most recent input method
extern enum Input_Types GetInputType(void);

// GetInputType is refreshed and on next GetInputTap, a refresh occurs
extern void RefreshInput(void);

// Returns a Vector2[MAX_INPUT_POINTS] with all input points on screen (finger touching screen or holding down mouse)
extern Vector2 * GetInputDown(void);

// Gets the position of a tap or mouse click on screen
extern Vector2 GetInputTap(void);