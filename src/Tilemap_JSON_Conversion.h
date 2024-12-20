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