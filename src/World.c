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
#include "Tilemap_JSON_Conversion.h"
#include <math.h>
#include <stdint.h>
#include <time.h>
#include "World.h"
#define WORLD_SIZE_X 1000
#define WORLD_SIZE_Y 1000

// Refers index in tile dictionary
WORLDTilemap * CurrentWorld = NULL;

// Current tilemap spritesheet
uint16_t CurrentTileSize = 50;
UITexture CurrentWorldSpriteSheet = {0};

Camera2D WorldCamera = {0};
WORLDEntity Freddy = {0};

void LoadWorldTilemap(void)
{
    CurrentWorld = CreateTilemap("Assets/Overworld/Maps/Overworld/map.json");
}

void FreeTilemap(WORLDTilemap ** tilemap)
{
    free((*tilemap) -> layers);
    free(tilemap);
}

void FreeWorldTilemap(void)
{
    FreeTilemap(&CurrentWorld);
}

float GetFloorTileScale(void)
{
    return GetScreenWidth() > GetScreenHeight() ? 1280 / 25. : 720 / 25.;
}

void SetWorldSpriteSheet(const char * path, uint16_t tileSize)
{
    if (CurrentWorldSpriteSheet.height)
    {
        UnloadTexture(CurrentWorldSpriteSheet);
    }
    CurrentWorldSpriteSheet = LoadTexture(path);
}

void InitWorld(void)
{
    if (!CurrentWorld) LoadWorldTilemap();

    SetWorldSpriteSheet("Assets/Overworld/Maps/Overworld/spritesheet.png", 50); 
    
}

void ResetWorld(void)
{
    Freddy.collisionTargets = LAYER_COLLIDABLE;
    Freddy.size = (Vector2) {40, 40};
    Freddy.position = (Vector2) {CurrentWorld -> layers[CurrentWorld -> amount - 2].offsetX, CurrentWorld -> layers[CurrentWorld -> amount - 2].offsetY};

    WorldCamera.offset = (Vector2) {GetScreenWidth()/2., GetScreenHeight()/2.};
    WorldCamera.target = Freddy.position;
    WorldCamera.rotation = 0;
    WorldCamera.zoom = 16 * GetScreenScaleW();
}
Vector2 GetEntityCorner(WORLDEntity * entity, enum Corner corner)
{
    switch (corner) {
        case TOPLEFT:
            return entity -> position;
        case TOPRIGHT:
            return (Vector2) {entity -> position.x + entity -> size.x, entity -> position.y};
        case BOTTOMLEFT:
            return (Vector2) {entity -> position.x, entity -> position.y + entity -> size.y};
        case BOTTONRIGHT:
            return (Vector2) {entity -> position.x + entity -> size.x, entity -> position.y + entity -> size.y};
    }
}

WORLDTile AccessPositionInLayer(uint16_t x, uint16_t y, tilemap_layer * layer)
{
    if (x < layer -> offsetX || x >= layer -> sizeX || y < layer -> offsetY || y >= layer -> sizeY) return 0;
    register WORLDTile (*tiles)[layer -> sizeY][layer -> sizeX] = layer -> tiles;
    return *tiles[y - layer -> offsetY][x - layer -> offsetX];
}

Vector2 CheckCollisionTilemap(WORLDEntity * entity, tilemap_layer * layer)
{
    if (!(entity -> collisionTargets & layer -> FLAGS)) return (Vector2){NAN, NAN};
    register WORLDTile (*tiles)[layer -> sizeY][layer -> sizeX] = layer -> tiles;
    for (uint8_t corner = 0; corner < 4; corner++)
    {
        Vector2 cornerPos = GetEntityCorner(entity, corner);
        if (AccessPositionInLayer((uint16_t)cornerPos.x, (uint16_t)cornerPos.y, layer))
        {
            if (entity -> customCollision) entity -> customCollision();
            return (Vector2) {floorf(cornerPos.x), floorf(cornerPos.y)};
        }
    }
    return (Vector2){0};
}

void MoveOutsideWall(WORLDEntity * entity, tilemap_layer * layer)
{
    Vector2 check = CheckCollisionTilemap(entity, layer);
    while (check.x != NAN)
    {
        if ((uint16_t) entity -> position.x == (uint16_t)check.x)
        {
            entity -> position.x -= (float) ((int16_t) entity -> velocity.x / abs((int16_t) entity -> velocity.x));
        }

        if ((uint16_t) entity -> position.y == (uint16_t)check.y)
        {
            entity -> position.y -= (float) ((int16_t) entity -> velocity.y / abs((int16_t) entity -> velocity.y));
        }

        check = CheckCollisionTilemap(entity, layer);
    }
}
void UpdateWorldEntity(WORLDEntity * entity)
{
    if (entity -> velocity.x == 0 && entity -> velocity.y == 0) return;
    entity -> position.x += entity -> velocity.x * GetFrameTime();
    entity -> position.y += entity -> velocity.y * GetFrameTime();
    for (uint16_t i = 0; i < CurrentWorld -> amount; i++)
    {
        MoveOutsideWall(entity, &CurrentWorld -> layers[i]);
    }
}

void RenderLayer(uint16_t n)
{
    if (CurrentWorld -> layers[n].FLAGS & LAYER_INVISIBLE) return;
    Vector2 Aspect = (Vector2) {GetScreenWidth() / 50., GetScreenHeight() / 50.};
    Rectangle CameraView = {Freddy.position.x - Aspect.x / 2, Freddy.position.y - Aspect.y / 2, Freddy.position.x + Aspect.x / 2, Freddy.position.y + Aspect.y / 2};

    for (uint16_t y = (uint16_t) CameraView.y; y <= (uint16_t) CameraView.height; y += 1)
    {
        for (uint16_t x = (uint16_t) CameraView.x; x <= (uint16_t) CameraView.width; x += 1)
        {
            uint16_t id = AccessPositionInLayer(x, 0, &CurrentWorld->layers[n]);
            if (!id) continue;
            id--;

            Rectangle sprite = {    (id * CurrentTileSize) % CurrentWorldSpriteSheet.width, 
                                    (float) (id * CurrentTileSize) / CurrentWorldSpriteSheet.width,
                                    CurrentTileSize,
                                    CurrentTileSize};
            DrawTextureRec(CurrentWorldSpriteSheet, sprite, GetScreenToWorld2D((Vector2) {x, y}, WorldCamera), WHITE);
        }
    }
}

void RenderWorld(void)
{

    RenderTexture2D RenderedMap = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    BeginTextureMode(RenderedMap);
    BeginMode2D(WorldCamera);
    for (uint16_t i = CurrentWorld->amount-1; i >= 0; i--) RenderLayer(i);
    EndMode2D();
    EndTextureMode();
    DrawTexture(RenderedMap.texture, 0, 0, WHITE);
}

void PutWorld(void)
{
    RenderWorld();
}