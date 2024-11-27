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

enum WORLDZONES 
{
    FAZBEARHILLS=1
};

typedef struct WORLDTileIndex
{
    Texture2D texture;
    uint8_t collidable: 1;

} WORLDTileIndex;

typedef struct WORLDEntity 
{
    float x;
    float y;
    UITexture texture;
    float scale;
    void (*customCollision)(void);
} WORLDEntity;

typedef struct WORLDZone 
{
    enum WORLDZONES zoneID;
    uint16_t ** ZONETitlemap;
    uint16_t globalX;
    uint16_t globalY;
    WORLDEntity * ZONEEntities;
    uint8_t * ZONECharacters;
} WORLDZone;

extern void RegisterZone(WORLDZone * zone);
extern void UseShader(Shader shader);