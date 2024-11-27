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

#include "Particle.h"
#include "Animation.h"
#include "UI.h"
#include <stdint.h>
#include <memory.h>
#include <time.h>

UIParticleIndex ParticlesIndex[255] = {0};
UIParticle AllParticles[MAX_PARTICLES] = {0};

float absf(float x)
{
    *(int *)&x &= 0x7fffffff;
    return x;
}

// Returns the index for an animated particle
uint8_t CreateParticleIndexA(const char * path, const uint8_t targetFPS, float scale)
{
    uint8_t id = 0;
    for (; ParticlesIndex[id].type != UInotype && id < MAX_PARTICLES; id++); // Gets the an avaliable index id

    ParticlesIndex[id].type = UIanimation;
    ParticlesIndex[id].animation = CreateAnimation(path, targetFPS);
    ParticlesIndex[id].scale = scale;

    if (id == MAX_PARTICLES) return 0; // There are no avaliable index ids so 0 is returned

    return id;
}

// Returns the index for a non-animated particle
uint8_t CreateParticleIndexT(const char * path, float scale)
{
    uint8_t id = 0;
    for (; ParticlesIndex[id].type != UInotype && id < MAX_PARTICLES; id++); // Gets the an avaliable index id

    ParticlesIndex[id].type = UItexture;
    ParticlesIndex[id].texture = LoadTexture(path);
    ParticlesIndex[id].scale = scale;

    if (id == MAX_PARTICLES) return 0; // There are no avaliable index ids so 0 is returned

    return id;
}

void CreateParticle(uint8_t textureID, float x, float y, float velocityX, float velocityY)
{
    uint8_t id = 0;
    for (; AllParticles[id].startTime != 0; id++); // Gets the an avaliable index id
    if (id >= MAX_PARTICLES) id = MAX_PARTICLES-1;
    AllParticles[id].textureID = textureID;
    AllParticles[id].x = x;
    AllParticles[id].y = y;
    AllParticles[id].velocityX = velocityX;
    AllParticles[id].velocityY = velocityY;
    AllParticles[id].startTime = clock();
}

void DeleteParticle(uint16_t id)
{
    AllParticles[id].startTime = 0;
}

void FlushParticles(void)
{
    for (uint16_t i = 0; i < MAX_PARTICLES; i++) DeleteParticle(i);
}

// Updates a UIParticle position with its velocity
void UpdateUIParticle(uint16_t id) 
{
    AllParticles[id].x += AllParticles[id].velocityX * GetFrameTime();
    AllParticles[id].y += AllParticles[id].velocityY * GetFrameTime();

    Texture2D texture = {0};
    switch (ParticlesIndex[id].type) 
    {
        case UIanimation:
            Animation * animation = &ParticlesIndex[AllParticles[id].textureID].animation;
            texture = animation -> Frames[GetCurrentAnimationFrame(animation)];
            break;
        case UItexture:
            texture = ParticlesIndex[AllParticles[id].textureID].texture;
            break;
    }
    if (absf(AllParticles[id].x) > GetOutsideWindowX(texture) ||
        absf(AllParticles[id].y) > GetOutsideWindowY(texture))
    {
        DeleteParticle(id); // Marks particle to be overwritten
    }
}

void RenderUIParticle(uint16_t id, register float screenScale)
{
    uint8_t indexID = AllParticles[id].textureID;
    switch (ParticlesIndex[indexID].type) 
    {
        case UIanimation:
            RenderAnimation(&ParticlesIndex[indexID].animation, 
                            AllParticles[id].x, 
                            AllParticles[id].y, 
                            ParticlesIndex[indexID].scale * screenScale, 
                            AllParticles[id].startTime);
            break;
        case UItexture:
            RenderUITexture(ParticlesIndex[indexID].texture, 
                            AllParticles[id].x, 
                            AllParticles[id].y, 
                            ParticlesIndex[indexID].scale * screenScale);
            break;
    }  
}

// Renders all particles on screen
void UpdateUIParticles(void)
{
    for (int id = 0; id < MAX_PARTICLES; id++) 
    {
        if (AllParticles[id].startTime == 0) continue;
        UpdateUIParticle(id);
    }
}

void RenderUIParticles(void)
{
    register float screenScale = GetScreenHeight() / 720.;
    for (uint16_t id = 0; id < MAX_PARTICLES; id++) 
    {
        if (AllParticles[id].startTime == 0) continue;
        RenderUIParticle(id, screenScale);
    }
}

void PutUIParticles(void) {
    UpdateUIParticles();
    RenderUIParticles();
}