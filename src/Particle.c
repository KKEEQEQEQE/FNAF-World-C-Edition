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

#include "Particle.h"
#include "Animation.h"
#include "UI.h"
#include <stdint.h>
#include <memory.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

UIParticleIndex ParticlesIndex[255] = {0};
UIParticle AllParticles[MAX_PARTICLES] = {0};

static float absf(float x)
{
    *(int *)&x &= 0x7fffffff;
    return x;
}

// Creates a Particle type with an UIanimation
uint8_t CreateParticleIndexA(const char * path, const uint8_t targetFPS, float scale)
{
    uint8_t id = 0;
    for (; ParticlesIndex[id].visual.type != UInotype && id < MAX_PARTICLES; id++); // Gets the an avaliable index id

    ParticlesIndex[id].visual.type = UIanimation;
    ParticlesIndex[id].visual.animation = CreateAnimation(path, targetFPS);
    ParticlesIndex[id].scale = scale;

    if (id == MAX_PARTICLES) return 0; // There are no avaliable index ids so 0 is returned

    return id;
}

// Creates a Particle type with an UIanimationV2
uint8_t CreateParticleIndexA_V2(const char * path, const uint8_t targetFPS, uint16_t amount, Vector2 spriteSize, float scale)
{
    uint8_t id = 0;
    for (; ParticlesIndex[id].visual.type != UInotype && id < MAX_PARTICLES; id++); // Gets the an avaliable index id

    ParticlesIndex[id].visual.type = UIanimationV2;
    ParticlesIndex[id].visual.animation_V2 = CreateAnimation_V2(path, targetFPS, amount, spriteSize.x, spriteSize.y);
    ParticlesIndex[id].scale = scale;

    if (id == MAX_PARTICLES) return 0; // There are no avaliable index ids so 0 is returned

    return id;
}

// Creates a Particle type with an UItexture
uint8_t CreateParticleIndexT(const char * path, float scale)
{
    uint8_t id = 0;
    for (; ParticlesIndex[id].visual.type != UInotype && id < MAX_PARTICLES; id++); // Gets the an avaliable index id

    ParticlesIndex[id].visual.type = UItexture;
    ParticlesIndex[id].visual.texture = LoadTexture(path);
    ParticlesIndex[id].scale = scale;

    if (id == MAX_PARTICLES) return 0; // There are no avaliable index ids so 0 is returned

    return id;
}

// Creates a Particle type with an UItexture
uint8_t CreateParticleIndexT_Snippet(UITexture atlas, Rectangle snippet, float scale)
{
    uint8_t id = 0;
    for (; ParticlesIndex[id].visual.type != UInotype && id < MAX_PARTICLES; id++); // Gets the an avaliable index id
    
    if (id == MAX_PARTICLES) return 0; // There are no avaliable index ids so 0 is returned
    
    ParticlesIndex[id].visual.type = UItextureSnippet;
    ParticlesIndex[id].visual.texture = atlas;
    ParticlesIndex[id].visual.snippet = snippet;
    ParticlesIndex[id].scale = scale;

    return id;
}

// Creates a Particle instance
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
    AllParticles[id].angularFrequency = 0;
    AllParticles[id].additionalUpdater = NULL;
}

// Creates a Particle instance with extra parameters
void CreateParticleEx(uint8_t textureID, float x, float y, float velocityX, float velocityY, float angularFrequency, void (*additionalUpdater)(UIParticle *))
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
    AllParticles[id].angularFrequency = angularFrequency;
    AllParticles[id].additionalUpdater = additionalUpdater;
}

// Removes a particle type (NOTE: UItextureSnippet textures don't get unloaded)
void RemoveParticleIndex(uint16_t id)
{
    if (ParticlesIndex[id].visual.type == UInotype) return;
    switch (ParticlesIndex[id].visual.type) {
        case UIanimation:
            FreeAnimation(&ParticlesIndex[id].visual.animation);
            break;
        case UItexture:
            UnloadTexture(ParticlesIndex[id].visual.texture);
            break;
        case UIanimationV2:
            FreeAnimation_V2(&ParticlesIndex[id].visual.animation_V2);
            break;
        case UItextureSnippet:
        default:
            break;
    }
    
    // Marks Particle type to be overwriten

    ParticlesIndex[id].visual.type = UInotype; 
}

// Deletes a Particle instance
void DeleteParticle(uint16_t id)
{
    AllParticles[id].startTime = 0;
}

// Deletes all Particle instance
void FlushParticles(void)
{
    for (uint16_t i = 0; i < MAX_PARTICLES; i++) DeleteParticle(i);
}

// Updates a UIParticle position with its velocity
void UpdateUIParticle(uint16_t id) 
{
    AllParticles[id].x += AllParticles[id].velocityX * GetFrameTime();
    AllParticles[id].y += AllParticles[id].velocityY * GetFrameTime();

    Vector2 size = (Vector2) {0, 0};

    switch (ParticlesIndex[id].visual.type) 
    {
        case UIanimation:
        {
            Animation * animation = &ParticlesIndex[AllParticles[id].textureID].visual.animation;
            Texture2D texture = animation -> Frames[GetCurrentAnimationFrame(animation)];
            size = (Vector2) {texture.width, texture.height};
            break;
        }
            
        case UItexture:
        {
            Texture2D texture = ParticlesIndex[AllParticles[id].textureID].visual.texture;
            size = (Vector2) {texture.width, texture.height};
            break;
        }
        
        case UItextureSnippet:
        {
            size = (Vector2) {  ParticlesIndex[AllParticles[id].textureID].visual.snippet.width,
                                ParticlesIndex[AllParticles[id].textureID].visual.snippet.height  };
            break;
        }

        case UIanimationV2:
        {
            size = (Vector2) {  ParticlesIndex[AllParticles[id].textureID].visual.animation_V2.TileSize_x, 
                                ParticlesIndex[AllParticles[id].textureID].visual.animation_V2.TileSize_y};
            break;
        }   
    }
    if (absf(AllParticles[id].x) > GetOutsideWindowX_u16(size.x) ||
        absf(AllParticles[id].y) > GetOutsideWindowY_u16(size.y))
    {
        DeleteParticle(id); // Marks particle to be overwritten
    }
}

// Renders a Particle instance
void RenderUIParticle(uint16_t id, register float screenScale)
{
    uint8_t indexID = AllParticles[id].textureID;

    float rotation = fmodf((float) (clock() - AllParticles[id].startTime) / CLOCKS_PER_SEC * AllParticles[id].angularFrequency, 360.);

      
    if (rotation < 0) rotation = 360 - absf(rotation);
    switch (ParticlesIndex[indexID].visual.type) 
    {
        case UIanimation:
            RenderAnimation(&ParticlesIndex[indexID].visual.animation, 
                            AllParticles[id].x, 
                            AllParticles[id].y, 
                            ParticlesIndex[indexID].scale * screenScale, 
                            AllParticles[id].startTime);
            break;
        case UItexture:
            RenderUITexturePro(ParticlesIndex[indexID].visual.texture, 
                            AllParticles[id].x, 
                            AllParticles[id].y, 
                            ParticlesIndex[indexID].scale * screenScale,
                            rotation);
            break;
        case UItextureSnippet:
            RenderUITextureSnippetPro(  ParticlesIndex[indexID].visual.texture,
                                        AllParticles[id].x, 
                                        AllParticles[id].y, 
                                        ParticlesIndex[indexID].visual.snippet, 
                                        ParticlesIndex[indexID].scale, 
                                        rotation, WHITE);
            break;
        case UIanimationV2:
            RenderAnimation_V2Ex(&ParticlesIndex[indexID].visual.animation_V2,
                                AllParticles[id].x, 
                                AllParticles[id].y, 
                                ParticlesIndex[indexID].scale,
                                rotation, 
                                AllParticles[id].startTime);
            break;
        default:
            break;
    }  
}

// Scales and Renders all particles on screen
void RenderUIParticles(void)
{
    register float screenScale = GetScreenHeight() / 720.;
    for (uint16_t id = 0; id < MAX_PARTICLES; id++) 
    {
        if (!AllParticles[id].startTime) continue;
        RenderUIParticle(id, screenScale);
    }
}

// Updates all particles on screen
void UpdateUIParticles(void)
{
    for (uint16_t id = 0; id < MAX_PARTICLES; id++) 
    {
        if (!AllParticles[id].startTime) continue;
        
        UpdateUIParticle(id);
        
        if (AllParticles[id].additionalUpdater)
        {
            AllParticles[id].additionalUpdater(AllParticles + id);
        }
    }
}

// Updates and Renders all particles on screen
void PutUIParticles(void) {
    UpdateUIParticles();
    RenderUIParticles();
}