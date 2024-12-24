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

#include "../Include/cJSON.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct WORLDTilemapLayer 
{
    // Transformation Variables

    uint16_t offsetX;
    uint16_t offsetY;
    uint16_t sizeX;
    uint16_t sizeY;

    void * tiles; // WORLDTile 2D Array Pointer

    // Flags

    uint8_t FLAGS;
} WORLDTilemapLayer;

#define TILES_POINTER_TYPE(layer_pointer) WORLDTile (*)[layer_pointer->sizeY][layer_pointer->sizeX]

typedef struct WORLDTilemap
{
    WORLDTilemapLayer * layers;
    uint16_t amount;
    uint16_t mapWidth; // Used for camera collision
    uint16_t mapHeight; // Used for camera collision
} WORLDTilemap;

// tilemap_layer FLAGS

#define LAYER_COLLIDABLE 1
#define LAYER_INVISIBLE 2
#define LAYER_SPAWN 4

// Returns the address of a parsed tilemap based on a Spritefusion map JSON
extern WORLDTilemap * CreateTilemap(const char * jsonPath);

// Prints a tilemap_layer (for debugging)
extern void PrintLayer(WORLDTilemapLayer * layer);