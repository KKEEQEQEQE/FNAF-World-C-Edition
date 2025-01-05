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

#include "Animation.h"
#include "Yellowwood.h"
#include "UI.h"
#include <stdint.h>

enum WORLDZONES 
{
    FAZBEARHILLS=1,
    CHOPPYSWOODS,
    DUSTINGFIELDS,
    MYSTERIOUSMINES
};

typedef UIVisual WORLDTileDefinition;

typedef uint16_t WORLDTile;

typedef struct WORLDEntity 
{
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    Vector2 visualOffset;
    UIVisual * visual;
    float scale;
    uint16_t collisionTargets;
    uint16_t depth;
    void (*customCollision)(struct WORLDEntity *);
} WORLDEntity;

typedef struct WORLDCamera
{
    Vector2 position; // The position of the camera
    Vector2 target; // Ranges from (-1.0, -1.0) to (1.0, 1.0)
    float zoom; // The amount of tiles you can see on the Y axis
} WORLDCamera;

extern void LoadWorldTilemap(void);

#define ACCESS_TILEMAP(x, y, tilemap) 

extern void InitWorld(void);
extern void ResetWorld(void);

enum Corner 
{
    TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTONRIGHT
};

extern void UpdateWorldEntity(WORLDEntity * entity);
extern void RenderWorld(void);
extern void PutWorld(void);

typedef struct _WarpButton 
{
    UIButton button;
    uint16_t zoneRequirement;
} _WarpButton;

typedef struct TouchJoystick
{
    float x, y; // In UI Space
    Vector2 velocity; // Ranges from (-1.0, -1.0) to (1.0, 1.0)
    UITexture background;
    UITexture knob;
} TouchJoystick;