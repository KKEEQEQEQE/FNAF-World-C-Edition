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

typedef struct _ParticlePoolObject 
{
    UIParticle object;
    struct _ParticlePoolObject * next;
} _ParticlePoolObject;

UIParticleIndex ParticlesIndex[255] = {0};
_ParticlePoolObject AllParticles[MAX_PARTICLES] = {0};
_ParticlePoolObject * Particle_Free_List = NULL;

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

void __attribute__((constructor)) InitParticleFreeList(void)
{
    for (uint16_t i = 0; i < MAX_PARTICLES - 1; i++)
    {
        AllParticles[i].next = AllParticles + i + 1;
    }
    Particle_Free_List = AllParticles;
}

// Creates a Particle instance
void CreateParticle(uint8_t textureID, float x, float y, float velocityX, float velocityY)
{
    if (!Particle_Free_List) return;

    Particle_Free_List->object.textureID = textureID;
    Particle_Free_List->object.x = x;
    Particle_Free_List->object.y = y;
    Particle_Free_List->object.velocityX = velocityX;
    Particle_Free_List->object.velocityY = velocityY;
    Particle_Free_List->object.startTime = clock();
    Particle_Free_List->object.angularFrequency = 0;
    Particle_Free_List->object.additionalUpdater = NULL;

    Particle_Free_List = Particle_Free_List -> next;
}

// Creates a Particle instance with extra parameters
void CreateParticleEx(uint8_t textureID, float x, float y, float velocityX, float velocityY, float angularFrequency, void (*additionalUpdater)(UIParticle *))
{
    if (!Particle_Free_List) return;

    Particle_Free_List->object.textureID = textureID;
    Particle_Free_List->object.x = x;
    Particle_Free_List->object.y = y;
    Particle_Free_List->object.velocityX = velocityX;
    Particle_Free_List->object.velocityY = velocityY;
    Particle_Free_List->object.startTime = clock();
    Particle_Free_List->object.angularFrequency = 0;
    Particle_Free_List->object.additionalUpdater = NULL;

    Particle_Free_List = Particle_Free_List -> next;
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
    AllParticles[id].object.startTime = 0;

    _ParticlePoolObject * temp = Particle_Free_List;
    Particle_Free_List = AllParticles + id;
    Particle_Free_List -> next = temp;
}

// Deletes all Particle instance
void FlushParticles(void)
{
    for (uint16_t i = 0; i < MAX_PARTICLES; i++) DeleteParticle(i);
}

// Updates a UIParticle position with its velocity
void UpdateUIParticle(uint16_t id) 
{
    AllParticles[id].object.x += AllParticles[id].object.velocityX * GetFrameTime();
    AllParticles[id].object.y += AllParticles[id].object.velocityY * GetFrameTime();

    Vector2 size = (Vector2) {0, 0};

    switch (ParticlesIndex[id].visual.type) 
    {
        case UIanimation:
        {
            Animation * animation = &ParticlesIndex[AllParticles[id].object.textureID].visual.animation;
            Texture2D texture = animation -> Frames[GetCurrentAnimationFrame(animation)];
            size = (Vector2) {texture.width, texture.height};
            break;
        }
            
        case UItexture:
        {
            Texture2D texture = ParticlesIndex[AllParticles[id].object.textureID].visual.texture;
            size = (Vector2) {texture.width, texture.height};
            break;
        }
        
        case UItextureSnippet:
        {
            size = (Vector2) {  ParticlesIndex[AllParticles[id].object.textureID].visual.snippet.width,
                                ParticlesIndex[AllParticles[id].object.textureID].visual.snippet.height  };
            break;
        }

        case UIanimationV2:
        {
            size = (Vector2) {  ParticlesIndex[AllParticles[id].object.textureID].visual.animation_V2.TileSize_x, 
                                ParticlesIndex[AllParticles[id].object.textureID].visual.animation_V2.TileSize_y};
            break;
        }   
    }
    if (absf(AllParticles[id].object.x) > GetOutsideWindowX_u16(size.x) ||
        absf(AllParticles[id].object.y) > GetOutsideWindowY_u16(size.y))
    {
        DeleteParticle(id); // Marks particle to be overwritten
    }
}

// Renders a Particle instance
void RenderUIParticle(uint16_t id, register float screenScale)
{
    uint8_t indexID = AllParticles[id].object.textureID;

    float rotation = fmodf((float) (clock() - AllParticles[id].object.startTime) / CLOCKS_PER_SEC * AllParticles[id].object.angularFrequency, 360.);

      
    if (rotation < 0) rotation = 360 - absf(rotation);
    switch (ParticlesIndex[indexID].visual.type) 
    {
        case UIanimation:
            RenderAnimation(&ParticlesIndex[indexID].visual.animation, 
                            AllParticles[id].object.x, 
                            AllParticles[id].object.y, 
                            ParticlesIndex[indexID].scale * screenScale, 
                            AllParticles[id].object.startTime);
            break;
        case UItexture:
            RenderUITexturePro(ParticlesIndex[indexID].visual.texture, 
                            AllParticles[id].object.x, 
                            AllParticles[id].object.y, 
                            ParticlesIndex[indexID].scale * screenScale,
                            rotation);
            break;
        case UItextureSnippet:
            RenderUITextureSnippetPro(  ParticlesIndex[indexID].visual.texture,
                                        AllParticles[id].object.x, 
                                        AllParticles[id].object.y, 
                                        ParticlesIndex[indexID].visual.snippet, 
                                        ParticlesIndex[indexID].scale, 
                                        rotation, WHITE);
            break;
        case UIanimationV2:
            RenderAnimation_V2Ex(&ParticlesIndex[indexID].visual.animation_V2,
                                AllParticles[id].object.x, 
                                AllParticles[id].object.y, 
                                ParticlesIndex[indexID].scale,
                                rotation, 
                                AllParticles[id].object.startTime);
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
        if (!AllParticles[id].object.startTime) continue;
        RenderUIParticle(id, screenScale);
    }
}

// Updates all particles on screen
void UpdateUIParticles(void)
{
    for (uint16_t id = 0; id < MAX_PARTICLES; id++) 
    {
        if (!AllParticles[id].object.startTime) continue;
        
        UpdateUIParticle(id);
        
        if (AllParticles[id].object.additionalUpdater)
        {
            AllParticles[id].object.additionalUpdater(&AllParticles[id].object);
        }
    }
}

// Updates and Renders all particles on screen
void PutUIParticles(void) {
    UpdateUIParticles();
    RenderUIParticles();
}