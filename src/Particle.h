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

#include "UI.h"
#include <stdint.h>
#include <time.h>

#define MAX_PARTICLES 100

typedef struct UIParticleIndex
{
    UIVisual visual;
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

// Creates a Particle type with an UIanimation
extern uint8_t CreateParticleIndexA(const char * path, const uint8_t targetFPS, float scale);

// Creates a Particle type with an UIanimationV2
extern uint8_t CreateParticleIndexA_V2(const char * path, const uint8_t targetFPS, uint16_t amount, Vector2 spriteSize, float scale);

// Creates a Particle type with an UItexture
extern uint8_t CreateParticleIndexT(const char * path, float scale);

// Creates a Particle instance
extern void CreateParticle(uint8_t textureID, float x, float y, float velocityX, float velocityY);

// Removes a particle type
extern void RemoveParticleIndex(uint16_t id);

// Deletes a Particle instance
extern void DeleteParticle(uint16_t id);

// Deletes all Particle instance
extern void FlushParticles(void);

// Updates a UIParticle position with its velocity
void UpdateUIParticle(uint16_t id);

// Scales and Renders all particles on screen
void RenderUIParticles(void);

// Updates all particles on screen
void UpdateUIParticles(void);

// Updates and Renders all particles on screen
void PutUIParticles(void);