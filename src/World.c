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

#include "Animation.h"
#include "Background.h"
#include "Particle.h"
#include "UI.h"
#include "Tilemap_JSON_Conversion.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "World.h"

#define WORLD_SIZE_X 1000
#define WORLD_SIZE_Y 1000
#define SKY_TINT (Color) {185, 255, 255, 255}

// Refers index in tile dictionary
WORLDTilemap * CurrentWorld = NULL;

// Current tilemap spritesheet
Music CurrentTheme = {0};
uint16_t CurrentTileSize = 50;
UITexture CurrentWorldSpriteSheet = {0};

RenderTexture2D WorldVirtualScreen = {0};

UITexture SunHeader = {0};
UIVisual LegacyZoneEffect = {0};
WORLDCamera WorldCamera = {0};

WORLDEntity Freddy = {0};

UIVisual FreddyIdle = {0};

UIVisual FreddyWLeft = {0};
UIVisual FreddyWUp = {0};
UIVisual FreddyWLRight = {0};
UIVisual FreddyWDown = {0};
char * ZoneNames[] = {"Fazbear Hills", "Choppy's Woods"};

// Particles

uint8_t BirdParticle = 0;

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

    BirdParticle = CreateParticleIndexA("Assets/Particles/bird/", 30, 1);
    CurrentTheme = LoadMusicStream("Assets/Themes/fazbearhills.mp3");
    CurrentTheme.looping = 1;

    PlayMusicStream(CurrentTheme);

    LegacyZoneEffect = (UIVisual) {UItexture, {LoadTexture("Assets/Overworld/Fazbear_Hills/sun_effect_mod.png")}, SKY_TINT};
    SetTextureFilter(LegacyZoneEffect.texture, TEXTURE_FILTER_BILINEAR);

    SunHeader = LoadTexture("Assets/Overworld/sun_effect_top.png");
    SetTextureFilter(SunHeader, TEXTURE_FILTER_BILINEAR);

    FreddyIdle.type = UItexture;
    FreddyIdle.texture = LoadTexture("Assets/Overworld/Freddy_Overworld/idle.png");
    FreddyIdle.tint = WHITE;
    Freddy.visual = &FreddyIdle;
}

void ResetWorld(void)
{
    Freddy.collisionTargets = LAYER_COLLIDABLE;
    Freddy.size = (Vector2) {0.8, 0.8};
    Freddy.position = (Vector2){CurrentWorld->layers[7].offsetX, CurrentWorld->layers[7].offsetY};

    WorldCamera.target = (Vector2) {0, 0};
    WorldCamera.position = (Vector2) {Freddy.position.x + Freddy.size.x / 2, Freddy.position.y + Freddy.size.y / 2};
    WorldCamera.zoom = 9;
    PlayMusicStream(CurrentTheme);
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
    x -= layer -> offsetX;
    y -= layer -> offsetY;
    if (x >= layer -> sizeX || y >= layer -> sizeY) return 0;

    register WORLDTile (*tiles)[layer -> sizeY][layer -> sizeX] = layer -> tiles;
    
    return (*tiles)[y][x];
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
    if (check.x != NAN)
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
    for (uint16_t i = 1; i < CurrentWorld -> amount; i++)
    {
        //MoveOutsideWall(entity, &CurrentWorld -> layers[i]);
    }
}


void RenderLayer(uint16_t n, Vector2 CameraMinorOffset)
{
    if (CurrentWorld -> layers[n].FLAGS & LAYER_INVISIBLE)
    {
        return;
    } 
    Rectangle CameraView = {WorldCamera.position.x - WorldCamera.zoom * ((float) GetScreenWidth() / GetScreenHeight()) / 2, 
                            WorldCamera.position.y - WorldCamera.zoom / 2, 
                            WorldCamera.zoom * ((float)GetScreenWidth() / GetScreenHeight()), 
                            WorldCamera.zoom};
    
    if (CameraView.x < 0) CameraView.width -= CameraView.x, CameraView.x = 0;
    if (CameraView.y < 0) CameraView.height -= CameraView.y, CameraView.y = 0;
    for (uint16_t y = (uint16_t) CameraView.y; y <= (uint16_t) CameraView.y + (uint16_t) CameraView.height + 1; y += 1)
    {
        for (uint16_t x = (uint16_t) CameraView.x; x <= (uint16_t) CameraView.x + (uint16_t) CameraView.width + 1; x += 1)
        {
            uint16_t id = AccessPositionInLayer(x, y, CurrentWorld -> layers + n);
            
            if (!id) continue;
            id--;
            Rectangle sprite = {    (uint16_t) (id * CurrentTileSize) % CurrentWorldSpriteSheet.width, 
                                    (uint16_t) (id * CurrentTileSize) / CurrentWorldSpriteSheet.width * CurrentTileSize,
                                    CurrentTileSize,
                                    CurrentTileSize};
            Vector2 screen_pos = (Vector2) {(x - (uint16_t) CameraView.x) * CurrentTileSize * 2 - (uint16_t) CameraMinorOffset.x, 
                                            (y - (uint16_t) CameraView.y) * CurrentTileSize * 2 - (uint16_t) CameraMinorOffset.y};
            DrawTexturePro( CurrentWorldSpriteSheet, 
                            sprite, 
                            (Rectangle) {screen_pos.x, screen_pos.y, CurrentTileSize * 2, CurrentTileSize * 2}, 
                            (Vector2) {0,0}, 
                            0, 
                            WHITE);
        }
    }
}

void RenderWorldTexture(Texture2D * texture, Vector2 position, Vector2 size)
{
    Rectangle CameraView = {WorldCamera.position.x - WorldCamera.zoom * ((float) GetScreenWidth() / GetScreenHeight()) / 2, 
                            WorldCamera.position.y - WorldCamera.zoom / 2, 
                            WorldCamera.zoom * ((float)GetScreenWidth() / GetScreenHeight()), 
                            WorldCamera.zoom};


    Vector2 screen_pos = (Vector2) {position.x * 50 - CameraView.x * 50., 
                                    position.y * 50 - CameraView.y * 50.};

    DrawTexturePro( *texture, 
                    (Rectangle) {0, 0, texture -> width, texture -> height},
                    (Rectangle) {screen_pos.x, screen_pos.y,
                                 size.x * CurrentTileSize, size.y * CurrentTileSize},
                    (Vector2) {0, 0},
                    0,
                    WHITE);
}

void RenderWorldAnimation(Animation * animation, Vector2 position, Vector2 size)
{
    uint16_t frame = GetCurrentAnimationFrame(animation);
    RenderWorldTexture(animation->Frames +frame, position, size);
}

void RenderWorldEntity(WORLDEntity * entity)
{
    switch (entity-> visual -> type) {
        case UIanimation:
            RenderWorldAnimation(&entity -> visual -> animation, entity -> position, entity -> size);
        case UItexture:
            RenderWorldTexture(&entity -> visual -> texture, entity -> position, entity -> size);
            break;
        default:
            break;
    }
}

void SpawnBirds(void)
{
    static clock_t timeSinceLastParticle = 0;
    static float wait = 2;
    uint8_t count = GetRandomValue(1,7);
    
    if (clock() - timeSinceLastParticle > CLOCKS_PER_SEC * wait) 
    {
        float degrees = 225/180.*PI;
        Vector2 start = (Vector2) {1, GetRandomValue(0, 200) / 100. - 1};
        if (GetRandomValue(0, 1)) 
        {
            start.x = start.y;
            start.y = 1;
        }
        CreateParticle(BirdParticle, start.x, start.y, cosf(degrees)/2.5, sinf(degrees)/2.5);

        timeSinceLastParticle = clock();
        wait = GetRandomValue(50, 400) / 100.;
    }
}

void RenderZoneEffect_Zone1(void)
{
    float screenRatio = (float) GetScreenWidth() / GetScreenHeight();
    int vWidth = (WorldCamera.zoom * CurrentTileSize) * screenRatio * 2;
    int vHeight = WorldCamera.zoom * CurrentTileSize * 2;

    BeginBlendMode(BLEND_ADDITIVE);

    DrawTexturePro( SunHeader, 
                    (Rectangle) {0, 0, SunHeader.width, SunHeader.height}, 
                    (Rectangle) {0, 0, vWidth, vHeight / 3}, 
                    (Vector2) {0, 0}, 0, 
                    SKYBLUE);

    DrawTexturePro( LegacyZoneEffect.texture, 
                    (Rectangle) {0, 0, LegacyZoneEffect.texture.width, LegacyZoneEffect.texture.height}, 
                    (Rectangle) {0, 0, vWidth, vHeight}, 
                    (Vector2) {0, 0}, 0, 
                    LegacyZoneEffect.tint);

    EndBlendMode();
}

uint8_t GetZone(void)
{
    Vector2 ZoneCheck = (Vector2) {Freddy.position.x + Freddy.size.x / 2, Freddy.position.y + Freddy.size.y / 2};
    uint16_t zone = AccessPositionInLayer((uint16_t) ZoneCheck.x, (uint16_t) ZoneCheck.y, CurrentWorld->layers + 0) - 31;
    if (zone < 3) zone = 1;
    return zone;
}
void RenderZoneEffect(void)
{
    uint16_t zone = GetZone();
    switch (zone) {
        case 1:
        case 2:
        default:
            RenderZoneEffect_Zone1();
            break;
    }
}

void RenderZoneName(void)
{
    RenderUIText(ZoneNames[GetZone() - 1], -0.95, -0.9, 0.03, LEFTMOST, (Font) {0}, WHITE);
}

void RenderWorld(void)
{
    float screenRatio = (float) GetScreenWidth() / GetScreenHeight();

    int vWidth = (WorldCamera.zoom * CurrentTileSize) * screenRatio * 2;
    int vHeight = WorldCamera.zoom * CurrentTileSize * 2;
    if (!WorldVirtualScreen.id) WorldVirtualScreen = LoadRenderTexture(vWidth,vHeight);


    if ((uint16_t) WorldVirtualScreen.texture.width != vWidth
        || (uint16_t) WorldVirtualScreen.texture.height != vHeight)
    {
        UnloadRenderTexture(WorldVirtualScreen);
        WorldVirtualScreen = LoadRenderTexture(vWidth, vHeight);
    }

    Rectangle CameraView = {WorldCamera.position.x - WorldCamera.zoom * ((float) GetScreenWidth() / GetScreenHeight()) / 2, 
                            WorldCamera.position.y - WorldCamera.zoom / 2, 
                            WorldCamera.zoom * ((float)GetScreenWidth() / GetScreenHeight()), 
                            WorldCamera.zoom};

    Vector2 CameraMinorOffset = (Vector2) { (float) (CameraView.x - (uint16_t) CameraView.x) * CurrentTileSize * 2,
                                            (float) (CameraView.y - (uint16_t) CameraView.y) * CurrentTileSize * 2};
    BeginTextureMode(WorldVirtualScreen);
    ClearBackground(BLACK);
    for (uint16_t i = CurrentWorld -> amount; i > 0; i--) RenderLayer(i, CameraMinorOffset);
    RenderZoneEffect();

    EndTextureMode();
    SetTextureFilter(WorldVirtualScreen.texture, TEXTURE_FILTER_BILINEAR);
    
    float scaleFactor = (float) GetScreenHeight() / WorldVirtualScreen.texture.height;

    DrawTexturePro( WorldVirtualScreen.texture, 
                    (Rectangle) {0, -vHeight, (float) vWidth, (float) -vHeight}, 
                    (Rectangle) { 0, 0, GetScreenWidth(), GetScreenHeight()},
                    (Vector2) {0,0},
                    0,
                    WHITE);
}

void UpdateFreddy(void)
{
    if (IsKeyDown(KEY_W)) WorldCamera.position.y -= 2*GetFrameTime();
    else if (IsKeyDown(KEY_S)) WorldCamera.position.y += 2*GetFrameTime();
    else Freddy.velocity.y = 0;

    if (IsKeyDown(KEY_A)) WorldCamera.position.x -= 2 * GetFrameTime();
    else if (IsKeyDown(KEY_D)) WorldCamera.position.x += 2 * GetFrameTime();
    else Freddy.velocity.x = 0;
}

void PutWorld(void)
{
    UpdateMusicStream(CurrentTheme);
    UpdateFreddy();
    RenderWorld();
    SpawnBirds();
    PutUIParticles();
    RenderZoneName();
}