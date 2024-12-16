/*
    MIT License

    Copyright (c) 2024 SpyterDev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#include "../Include/raylib.h"
#include <time.h>
#include <stdint.h>

typedef clock_t anim_t;

typedef struct Animation {
    uint16_t Amount;
    uint8_t FPS;
    clock_t Clock;
    Texture2D * Frames;
} Animation;

typedef struct Animation_V2 {
    uint16_t Amount;
    uint8_t FPS;
    clock_t Clock;
    Texture2D Atlas;
    uint16_t TileSize_x;
    uint16_t TileSize_y;
} Animation_V2;


extern uint16_t GetFrameAmount(const char * directory);
extern uint16_t GetCurrentAnimationFrame(const Animation * animation);
extern uint16_t GetCurrentAnimationFrameC(clock_t startTime, uint16_t frames, uint8_t FPS);
extern void RenderAnimation(const Animation * animation, float x, float y, float scale, clock_t timeOverride);
extern Animation CreateAnimation(const char * path, const uint8_t targetFPS);
extern void FreeAnimation(Animation * animation);

extern Animation_V2 CreateAnimation_V2(const char * path, const uint8_t targetFPS, const uint16_t amount, const uint16_t tileSize_x, const uint16_t tileSize_y);
extern void DrawAnimation_V2(const Animation_V2 *animation, int16_t x, int16_t y, float scale, clock_t timeOverride);
extern void RenderAnimation_V2(const Animation_V2 *animation, float x, float y, float scale, clock_t timeOverride);
extern void FreeAnimation_V2(Animation_V2 * animation);