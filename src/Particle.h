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

#include "UI.h"
#include <stdint.h>
#include <time.h>

#define MAX_PARTICLES 100

typedef struct UIParticleIndex
{
    enum UIType type;
    union {
        Animation animation;
        Texture2D texture;
    };
    float scale;
} UIParticleIndex;

typedef struct UIParticle 
{
    uint8_t textureID;
    float x;
    float y;
    float velocityX;
    float velocityY;
    clock_t startTime;
} UIParticle;
// Returns the index for an animated particle
extern uint8_t CreateParticleIndexA(const char * path, const uint8_t targetFPS, float scale);

// Returns the index for a non-animated particle
extern uint8_t CreateParticleIndexT(const char * path, float scale);

extern void CreateParticle(uint8_t textureID, float x, float y, float velocityX, float velocityY);

extern void DeleteParticle(uint16_t id);

extern void FlushParticles(void);

void UpdateUIParticle(uint16_t id);

// Renders all particles on screen
void RenderUIParticles(void);

// Updates all particles on screen
void UpdateUIParticles(void);

// Updates and Renders all particles on screen
void PutUIParticles(void);