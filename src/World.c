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

#include "UI.h"
#include <stdint.h>
#include "World.h"
#define WORLD_SIZE_X 1000
#define WORLD_SIZE_Y 1000

// Contains all types of floor tiles
WORLDTileIndex WORLDFloorDictionary[]; // Note floor image is 12:6 (25px * 25px)

// Refers index in floor tile dictionary
uint8_t WORLDFloormap[WORLD_SIZE_Y][WORLD_SIZE_X] = {0};

// Contains all types of tiles
WORLDTileIndex WORLDTileDictionary[];

// Refers index in tile dictionary
uint8_t WORLDTitlemap[WORLD_SIZE_Y][WORLD_SIZE_X] = {0};


Rectangle WorldCamera = {0};
WORLDEntity Freddy = {0};

float GetFloorTileScale(void)
{
    return GetScreenWidth() > GetScreenHeight() ? 1280 / 25. : 720 / 25.;
}

void RenderTileRow(uint16_t startX, uint16_t y,  uint8_t (* tilemap)[WORLD_SIZE_Y][WORLD_SIZE_X], WORLDTileIndex * dictionary)
{
    float screenY = y / (WorldCamera.y + WorldCamera.height) * 2 - 1;
    for (uint16_t x = startX; x < startX + WorldCamera.width; x++)
        {
            uint8_t tileIndex = (*tilemap)[y][x];
            if (!tileIndex) continue;
            float tileScale = 1280/25.;
            RenderUITexture(dictionary[tileIndex].texture, x / (WorldCamera.x + WorldCamera.width) * 2 - 1, screenY, GetScreenScale() * GetFloorTileScale());
    }
}

void RenderFloor(void)
{
    uint16_t startX = WorldCamera.x > 0 ? WorldCamera.x : 0;
    if (startX + WorldCamera.width >= WORLD_SIZE_X) startX = WORLD_SIZE_X - (uint16_t) WorldCamera.width;

    uint16_t startY = WorldCamera.y > 0 ? WorldCamera.y : 0;
    if (startY + WorldCamera.height >= WORLD_SIZE_Y) startY = WORLD_SIZE_Y - (uint16_t) WorldCamera.height;

    for (uint16_t y = startY; y < startY + WorldCamera.height; y++)
    {
        RenderTileRow(startX, y, &WORLDFloormap, WORLDFloorDictionary);
    }
    
}

void RenderWorld(void)
{

    RenderTexture2D RenderedMap = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    BeginTextureMode(RenderedMap);
    RenderFloor();
    EndTextureMode();
    DrawTexture(RenderedMap.texture, 0, 0, WHITE);
}