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

#include "Animation.h"
#include "UI.h"
#include <stdint.h>

enum WORLDZONES 
{
    FAZBEARHILLS=1
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
    void (*customCollision)(struct WORLDEntity *);
} WORLDEntity;

typedef struct WORLDCamera
{
    Vector2 position; // The position of the camera
    Vector2 target; // Ranges from (-1.0, -1.0) to (1.0, 1.0)
    float zoom; // The amount of tiles you can see on the Y axis
} WORLDCamera;

#define LAYER_COLLIDABLE 1
#define LAYER_INVISIBLE 2
#define LAYER_SPAWN 4

typedef struct tilemap_layer 
{
    // Transformation Variables

    uint16_t offsetX;
    uint16_t offsetY;
    uint16_t sizeX;
    uint16_t sizeY;

    void * tiles; // WORLDTile 2D Array Pointer

    // Flags

    uint8_t FLAGS;
} tilemap_layer;

#define TILES_POINTER_TYPE(layer_pointer) WORLDTile (*)[layer_pointer->sizeY][layer_pointer->sizeX]

typedef struct WORLDTilemap
{
    tilemap_layer * layers;
    uint16_t amount;
} WORLDTilemap;

extern void LoadWorldTilemap(void);

#define ACCESS_TILEMAP(x, y, tilemap) 

extern void InitWorld(void);
extern void ResetWorld(void);
enum Corner 
{
    TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTONRIGHT
};

extern Vector2 GetEntityCorner(WORLDEntity * entity, enum Corner corner);
extern WORLDTile AccessPositionInLayer(uint16_t x, uint16_t y, tilemap_layer * layer);
extern void UpdateWorldEntity(WORLDEntity * entity);
extern void RenderWorld(void);
extern void PutWorld(void);