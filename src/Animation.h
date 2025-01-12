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
#include <time.h>
#include <stdint.h>

typedef clock_t anim_t;

// Deprecated UIanimation structure
typedef struct Animation {
    uint16_t Amount;
    uint8_t FPS;
    clock_t Clock;
    Texture2D * Frames;
} Animation;

// UIanimationV2 structure
typedef struct Animation_V2 {
    uint16_t Amount;
    uint8_t FPS;
    clock_t Clock;
    Texture2D Atlas;
    uint16_t TileSize_x, TileSize_y;
} Animation_V2;

// UIanimation functions (NOTE THESE ALL OF FUNCTIONS ARE DEPRECATED)

// Gets the amount of frames in a directory (has to be named n.png)
extern uint16_t GetFrameAmount(const char * directory);

// Gets current animation frame from an animation struct
extern uint16_t GetCurrentAnimationFrame(const Animation * animation);

// Gets current animation frame from clock_t, entered frames, and entered FPS
extern uint16_t GetCurrentAnimationFrameC(clock_t startTime, uint16_t frames, uint8_t FPS);

// Returns created UIanimation struct on stack
extern Animation CreateAnimation(const char * path, const uint8_t targetFPS);

// Scales and Renders a UIanimation
extern void RenderAnimation(const Animation * animation, float x, float y, float scale, clock_t timeOverride);

// Free a UIanimation's variables
extern void FreeAnimation(Animation * animation);

// UIanimationV2 function

// Returns created UIanimationV2 struct on stack
extern Animation_V2 CreateAnimation_V2(const char * path, const uint8_t targetFPS, const uint16_t amount, const uint16_t tileSize_x, const uint16_t tileSize_y);

// Draws a UIanimationV2 in pixel space
extern void DrawAnimation_V2(const Animation_V2 *animation, int16_t x, int16_t y, float scale, clock_t timeOverride);

// Scales and Renders a UIanimationV2 in UI space
extern void RenderAnimation_V2(const Animation_V2 *animation, float x, float y, float scale, clock_t timeOverride);

extern void RenderAnimation_V2Ex(const Animation_V2 *animation, float x, float y, float scale, float rotation, clock_t timeOverride);

// Free a UIanimationV2's variables
extern void FreeAnimation_V2(Animation_V2 * animation);