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
#include <string.h>
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

WORLDEntity WorldBuildings_After[5] = {0}; // World Buildings rendered after Freddy
WORLDEntity WorldBuildings_Pre[5] = {0}; // World Buildings rendered before Freddy

WORLDEntity WORLDEntities[5] = {0};
WORLDEntity Freddy = {0};

UIVisual FreddyIdle = {0};

UIVisual FreddyWLeft = {0};
UIVisual FreddyWUp = {0};
UIVisual FreddyWRight = {0};
UIVisual FreddyWDown = {0};

UITexture ZoneHeader[2];
char * ZoneNames[] = {"Fazbear Hills", "Choppy's Woods"};

// Particles

uint8_t BirdParticle = 0;

WORLDEntity CreateWorldEntity(Vector2 position, Vector2 size, Vector2 velocity, UIVisual * visual, float scale, uint16_t collisionTargets, void (*customCollision)(WORLDEntity *))
{
    WORLDEntity entity = {0};
    entity.position = position;
    entity.size = size;
    entity.velocity = velocity;
    entity.visual = visual;
    entity.scale = scale;
    entity.customCollision = customCollision;
    return entity;
}

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

    // Temporary limits log level to minimize printing to console (for faster load times)

    SetTraceLogLevel(LOG_WARNING);
    SetWorldSpriteSheet("Assets/Overworld/Maps/Overworld/spritesheet.png", 50); 

    // Particles

    BirdParticle = CreateParticleIndexA_V2("Assets/Particles/bird.png", 30, 10, (Vector2) {50, 50}, 1);
    
    // Musics

    CurrentTheme = LoadMusicStream("Assets/Themes/fazbearhills.mp3");
    CurrentTheme.looping = 1;

    PlayMusicStream(CurrentTheme);

    // Zone Effects
    LegacyZoneEffect = (UIVisual) {UItexture, {LoadTexture("Assets/Overworld/Fazbear_Hills/sun_effect_mod.png")}, SKY_TINT};
    SetTextureFilter(LegacyZoneEffect.texture, TEXTURE_FILTER_BILINEAR);

    SunHeader = LoadTexture("Assets/Overworld/sun_effect_top.png");
    SetTextureFilter(SunHeader, TEXTURE_FILTER_BILINEAR);

    // Freddy Sprites
    
    FreddyIdle.type = UItexture;
    FreddyIdle.texture = LoadTexture("Assets/Overworld/Freddy_Overworld/idle.png"); 
    FreddyIdle.tint = WHITE;

    SetTextureFilter(FreddyIdle.texture, TEXTURE_FILTER_BILINEAR);

    
    FreddyWUp.type = UIanimationV2;
    FreddyWUp.animation_V2 = CreateAnimation_V2("Assets/Overworld/Freddy_Overworld/walking_up.png", 30, 15, 60, 60); 
    FreddyWUp.tint = WHITE;

    FreddyWLeft.type = UIanimationV2;
    FreddyWLeft.animation_V2 = CreateAnimation_V2("Assets/Overworld/Freddy_Overworld/walking_left.png", 30, 15, 60, 60); 
    FreddyWLeft.tint = WHITE;

    FreddyWRight.type = UIanimationV2;
    FreddyWRight.animation_V2 = CreateAnimation_V2("Assets/Overworld/Freddy_Overworld/walking_right.png", 30, 15, 60, 60); 
    FreddyWRight.tint = WHITE;

    FreddyWDown.type = UIanimationV2;
    FreddyWDown.animation_V2 = CreateAnimation_V2("Assets/Overworld/Freddy_Overworld/walking_down.png", 30, 15, 60, 60); 
    FreddyWDown.tint = WHITE;

    Freddy.visual = &FreddyIdle;
    Freddy.scale = 0.95;

    WorldBuildings_Pre[0] = CreateWorldEntity(  (Vector2) {10.8, 11}, 
                                                (Vector2) {0,0}, 
                                                (Vector2) {0,0}, 
                                                UIVisual_Heap(CreateUIVisual_UITexture_P( "Assets/Overworld/Buildings/Blue_Castle.png", 
                                                                                                        WHITE)), 
                                                1, 
                                                0, 
                                                NULL);

    WorldBuildings_Pre[1] = CreateWorldEntity(  (Vector2) {14.5, 10.5},
                                                (Vector2) {0,0}, 
                                                (Vector2) {0,0}, 
                                                UIVisual_Heap(CreateUIVisual_UITexture_P("Assets/Overworld/Buildings/Red_Castle.png", 
                                                                                                        WHITE)),
                                                1, 
                                                0, 
                                                NULL);

    WorldBuildings_Pre[2] = CreateWorldEntity(  (Vector2) {13.5, 15.5}, 
                                                (Vector2) {0,0},
                                                (Vector2) {0,0}, 
                                                UIVisual_Heap(CreateUIVisual_UITexture_P("Assets/Overworld/Buildings/Gear_House.png", 
                                                                                                        WHITE)), 
                                                1, 
                                                0, 
                                                NULL);

    WorldBuildings_Pre[3] = CreateWorldEntity(  (Vector2) {19.5, 12}, 
                                                (Vector2) {0,0}, 
                                                (Vector2) {0,0}, 
                                                UIVisual_Heap(CreateUIVisual_UITexture_P("Assets/Overworld/Buildings/Lumber_House.png", 
                                                                                                        WHITE)), 
                                                1, 
                                                0, 
                                                NULL);
                                                
    WorldBuildings_After[0] = CreateWorldEntity(    (Vector2) {8.5, 17}, 
                                                    (Vector2) {0,0}, 
                                                    (Vector2) {0,0}, 
                                                    UIVisual_Heap(CreateUIVisual_UIAnimation_V2("Assets/Overworld/Buildings/windmill_atlas.png", 30, 20, (Vector2) {200, 200}, WHITE)),
                                                    1, 
                                                    0,
                                                    NULL);

    ZoneHeader[0] = LoadTexture("Assets/Overworld/UI/Zone_Names/1.png"); 
    ZoneHeader[1] = LoadTexture("Assets/Overworld/UI/Zone_Names/2.png"); 
    SetTraceLogLevel(LOG_ALL);
}

void ResetWorld(void)
{
    Freddy.collisionTargets = LAYER_COLLIDABLE;
    Freddy.size = (Vector2) {0.8, 0.3};
    Freddy.scale = 0.95;
    Freddy.visualOffset = (Vector2) {0, 0.5};
    Freddy.position = (Vector2) {15, 21};
    Freddy.customCollision = NULL;

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

WORLDTile AccessPositionInLayer(uint16_t x, uint16_t y, WORLDTilemapLayer * layer)
{
    x -= layer -> offsetX;
    y -= layer -> offsetY;
    if (x >= layer -> sizeX || y >= layer -> sizeY) return 0;

    register WORLDTile (*tiles)[layer -> sizeY][layer -> sizeX] = layer -> tiles;
    
    return (*tiles)[y][x];
}

uint8_t CheckCollisionTilemap(WORLDEntity * entity, WORLDTilemapLayer * layer)
{
    if (!(entity -> collisionTargets & layer -> FLAGS)) return 0;
    
    Vector2 cornerPos = entity -> position;

    if (AccessPositionInLayer((uint16_t) cornerPos.x, (uint16_t)cornerPos.y, layer))
    {
        return 1;
    }

    cornerPos = (Vector2) {entity -> position.x + entity -> size.x, entity -> position.y};

    if (AccessPositionInLayer((uint16_t) cornerPos.x, (uint16_t)cornerPos.y, layer))
    {
        return 1;
    }

    cornerPos = (Vector2) {entity -> position.x, entity -> position.y + entity -> size.y};

    if (AccessPositionInLayer((uint16_t) cornerPos.x, (uint16_t)cornerPos.y, layer))
    {
        return 1;
    }

    cornerPos = (Vector2) {entity -> position.x + entity -> size.x, entity -> position.y + entity -> size.y};

    if (AccessPositionInLayer((uint16_t) cornerPos.x, (uint16_t)cornerPos.y, layer))
    {
        return 1;
    }

    return 0;
}

Rectangle GetCameraView(void)
{
    return (Rectangle) {    WorldCamera.position.x - WorldCamera.zoom * ((float) GetScreenWidth() / GetScreenHeight()) / 2, 
                            WorldCamera.position.y - WorldCamera.zoom / 2, 
                            WorldCamera.zoom * ((float)GetScreenWidth() / GetScreenHeight()) + 2, 
                            WorldCamera.zoom + 2    };
}

// Uses AABB Collision to check if collision has occured between to WORLDEntities
_Bool CheckEntityCollision(WORLDEntity * collider, WORLDEntity * collidee)
{
    Rectangle colliderHitbox = (Rectangle) {collider -> position.x, 
                                            collider -> position.y, 
                                            collider -> size.x, 
                                            collider -> size.y};

    Rectangle collideeHitbox = (Rectangle) {collidee -> position.x, 
                                            collidee -> position.y, 
                                            collidee -> size.x, 
                                            collidee -> size.y};
    return CheckCollisionRecs(colliderHitbox, collideeHitbox);
}

// Checks entity collision and uses custom collision function if there is one
void HandleEntityCollision(WORLDEntity * entity)
{
    
    for (uint16_t i = 0; i < sizeof(WORLDEntities) / sizeof(WORLDEntity); i++)
    {
        if (WORLDEntities[i].visual -> type == UInotype) continue;
        if (CheckEntityCollision(entity, WORLDEntities + i) && WORLDEntities[i].customCollision) WORLDEntities[i].customCollision(entity);
    }
}

// Updates velocity and collision of a WORLDEntity
void UpdateWorldEntity(WORLDEntity * entity)
{
    if ((int16_t) entity -> velocity.x == 0 && (int16_t) entity -> velocity.y == 0) return;

    entity -> position.x += entity -> velocity.x * GetFrameTime();

    for (uint16_t i = 1; i < CurrentWorld -> amount; i++)
    {
        uint8_t check = CheckCollisionTilemap(entity, &CurrentWorld->layers[i]);
        if (check)
        {
            entity -> position.x -= entity -> velocity.x * GetFrameTime();
            break;
        }
    }

    entity -> position.y += entity -> velocity.y * GetFrameTime();

    for (uint16_t i = 1; i < CurrentWorld -> amount; i++)
    {
        uint8_t check = CheckCollisionTilemap(entity, &CurrentWorld->layers[i]);
        if (check)
        {
            entity -> position.y -= entity -> velocity.y * GetFrameTime();
            break;
        }
    }
}


// Scales and Renders a Texture2D relitive to the WORLDCamera
void RenderWorldTexture(Texture2D * texture, Vector2 position, Vector2 offset, float scale)
{
    Rectangle CameraView = GetCameraView();

    if (CameraView.x < 0) CameraView.x = 0;
    if (CameraView.y < 0) CameraView.x = 0;

    Vector2 screen_pos = (Vector2) {(position.x - (uint16_t)CameraView.x) * 50., 
                                    (position.y - (uint16_t)CameraView.y) * 50.};

    DrawTexturePro( *texture, 
                    (Rectangle) {0, 0, texture -> width, texture -> height},
                    (Rectangle) {screen_pos.x, screen_pos.y,
                                 texture -> width * scale, texture -> height * scale},
                    (Vector2) {texture -> width * scale * (offset.x + 1) / 2, texture -> height * scale * (offset.y + 1) / 2},
                    0,
                    WHITE);
}

// Scales and Renders a UIanimationV2 relitive to the WORLDCamera
void RenderWorldAnimation_V2(Animation_V2 * animation, Vector2 position, Vector2 offset, float scale)
{
    Rectangle CameraView = GetCameraView();

    if (CameraView.x < 0) CameraView.x = 0;
    if (CameraView.y < 0) CameraView.x = 0;

    Vector2 screen_pos = (Vector2) {(position.x - (uint16_t)CameraView.x) * 50., 
                                    (position.y - (uint16_t)CameraView.y) * 50.};
    
    offset = (Vector2) {animation->TileSize_x * scale * (offset.x + 1) / 2, animation -> TileSize_y * scale * (offset.y + 1) / 2};
    screen_pos.x -= offset.x;
    screen_pos.y -= offset.y;
    
    DrawAnimation_V2(animation, screen_pos.x, screen_pos.y, scale, 0);
}

// Scales and Renders a WORLDEntity
void RenderWorldEntity(WORLDEntity * entity)
{
    Vector2 position = position = (Vector2) {entity -> position.x + entity -> size.x / 2, entity -> position.y + entity -> size.y / 2};
    switch (entity-> visual -> type) {
        case UIanimation:
            uint16_t i = GetCurrentAnimationFrame(&entity -> visual -> animation);
            Texture2D * frame = entity -> visual -> animation.Frames + i;
            RenderWorldTexture( frame, 
                                position, 
                                entity -> visualOffset,
                                entity -> scale);
            break;
        case UItexture:
            Texture2D * texture = &entity -> visual -> texture;
            RenderWorldTexture( texture, 
                                position, 
                                entity -> visualOffset,
                                entity -> scale);
            break;
        case UIanimationV2:
            RenderWorldAnimation_V2(&entity -> visual -> animation_V2, 
                                    position, 
                                    entity -> visualOffset,
                                    entity -> scale);
            break;
        default:
            break;
    }
}

// Scales and Renders all in a WORLDEntity array
void RenderEntites(WORLDEntity * entities)
{
    for (uint16_t i = 0; entities[i].visual != NULL; i++)
    {
        RenderWorldEntity(entities + i);
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

void RenderZoneEffect_Zone1(Vector2 offset)
{
    float screenRatio = (float) GetScreenWidth() / GetScreenHeight();
    int vWidth = (WorldCamera.zoom * CurrentTileSize) * screenRatio;
    int vHeight = WorldCamera.zoom * CurrentTileSize;

    BeginBlendMode(BLEND_ADDITIVE);

    DrawTexturePro( SunHeader, 
                    (Rectangle) {0, 0, SunHeader.width, SunHeader.height}, 
                    (Rectangle) {offset.x, offset.y, vWidth, vHeight / 3}, 
                    (Vector2) {0, 0}, 0, 
                    SKYBLUE);

    DrawTexturePro( LegacyZoneEffect.texture, 
                    (Rectangle) {0, 0, LegacyZoneEffect.texture.width, LegacyZoneEffect.texture.height}, 
                    (Rectangle) {offset.x, offset.y, vWidth, vHeight}, 
                    (Vector2) {0, 0}, 0, 
                    LegacyZoneEffect.tint);

    EndBlendMode();
}

uint8_t GetZone(void)
{
    Vector2 ZoneCheck = (Vector2) {Freddy.position.x + Freddy.size.x / 2, Freddy.position.y + Freddy.size.y / 2};
    uint16_t zone = AccessPositionInLayer((uint16_t) ZoneCheck.x, (uint16_t) ZoneCheck.y, CurrentWorld->layers + 0) - 37;
    if (zone > 3) return 0xff;
    return zone - 1;
}
void RenderZoneEffect(void)
{
    uint16_t zone = GetZone();
    Rectangle CameraView = {WorldCamera.position.x - WorldCamera.zoom * ((float) GetScreenWidth() / GetScreenHeight()) / 2, 
                            WorldCamera.position.y - WorldCamera.zoom / 2, 
                            WorldCamera.zoom * ((float)GetScreenWidth() / GetScreenHeight()), 
                            WorldCamera.zoom};

    if (CameraView.x < 0) CameraView.x = 0;
    if (CameraView.y < 0) CameraView.x = 0;

    Vector2 CameraMinorOffset = (Vector2) { (float) (CameraView.x - (uint16_t) CameraView.x) * CurrentTileSize,
                                            (float) (CameraView.y - (uint16_t) CameraView.y) * CurrentTileSize};
    switch (zone) {
        case 1:
        case 2:
        default:
            RenderZoneEffect_Zone1(CameraMinorOffset);
            break;
    }
}

void RenderZoneName(void)
{
    uint8_t zone = GetZone();
    if (zone == 0xff) RenderUIText("Unknown Zone", -0.95, -0.9, 0.03, LEFTMOST, (Font) {0}, WHITE);
    float scale = (float) ZoneHeader[zone].height / GetScreenHeight();
    DrawTextureEx(ZoneHeader[zone], (Vector2) {25. * GetScreenHeight() / 720, 25. * GetScreenHeight() / 720}, 0, 0.025/scale, WHITE);
}

// Renders WORLDTilemapLayer onto Virtual Screen
void RenderLayer(uint16_t n, Vector2 CameraMinorOffset)
{
    if (CurrentWorld -> layers[n].FLAGS & LAYER_INVISIBLE)
    {
        return;
    } 
    Rectangle CameraView = GetCameraView();
    
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
            Vector2 screen_pos = (Vector2) {(x - (uint16_t) CameraView.x) * CurrentTileSize, 
                                            (y - (uint16_t) CameraView.y) * CurrentTileSize};
            DrawTexturePro( CurrentWorldSpriteSheet, 
                            sprite, 
                            (Rectangle) {screen_pos.x, screen_pos.y, CurrentTileSize, CurrentTileSize}, 
                            (Vector2) {0,0}, 
                            0, 
                            WHITE);
        }
    }
}

void RenderWorld(void)
{
    float screenRatio = (float) GetScreenWidth() / GetScreenHeight();

    int vWidth = (WorldCamera.zoom * CurrentTileSize) * screenRatio + CurrentTileSize;
    int vHeight = WorldCamera.zoom * CurrentTileSize + CurrentTileSize;
    if (!WorldVirtualScreen.id) WorldVirtualScreen = LoadRenderTexture(vWidth,vHeight);


    if ((uint16_t) WorldVirtualScreen.texture.width != vWidth
        || (uint16_t) WorldVirtualScreen.texture.height != vHeight)
    {
        UnloadRenderTexture(WorldVirtualScreen);
        WorldVirtualScreen = LoadRenderTexture(vWidth, vHeight);
    }

    Rectangle CameraView = GetCameraView();

    if (CameraView.x < 0) CameraView.x = 0;
    if (CameraView.y < 0) CameraView.x = 0;

    Vector2 CameraMinorOffset = (Vector2) { (float) (CameraView.x - (uint16_t) CameraView.x) * (GetScreenHeight() / WorldCamera.zoom),
                                            (float) (CameraView.y - (uint16_t) CameraView.y) * (GetScreenHeight() / WorldCamera.zoom)};
    BeginTextureMode(WorldVirtualScreen);

    ClearBackground(BLACK);

    // Renders all tiles in each layer

    for (uint16_t i = CurrentWorld -> amount; i > 0; i--) RenderLayer(i, CameraMinorOffset);
    
    // Renders all buildings behind Freddy

    RenderEntites(WorldBuildings_Pre);

    RenderWorldEntity(&Freddy);

    // Renders all buildings in front of Freddy

    RenderEntites(WorldBuildings_After);

    // Renders current zone effect

    RenderZoneEffect();

    EndTextureMode();

    // Sets Virtual Screen texture to BILINEAR for better upscaling
    SetTextureFilter(WorldVirtualScreen.texture, TEXTURE_FILTER_BILINEAR);
    
    float scaleFactor = (float) GetScreenHeight() / WorldVirtualScreen.texture.height;

    DrawTexturePro( WorldVirtualScreen.texture, 
                    (Rectangle) {0, -vHeight, (float) vWidth, (float) -vHeight}, 
                    (Rectangle) { -CameraMinorOffset.x, -CameraMinorOffset.y, GetScreenWidth() + (GetScreenHeight() / WorldCamera.zoom), GetScreenHeight() + (GetScreenHeight() / WorldCamera.zoom)},
                    (Vector2) {0,0},
                    0,
                    WHITE);
}

void UpdateFreddy(void)
{
    static uint8_t lastDirection = 0;

    uint8_t CurrentDirection = 0;

    if (IsKeyDown(KEY_W)) Freddy.velocity.y = -2, CurrentDirection = 1;
    else if (IsKeyDown(KEY_S)) Freddy.velocity.y = 2, CurrentDirection = 4;
    else Freddy.velocity.y = 0;

    if (IsKeyDown(KEY_A)) Freddy.velocity.x = -2, CurrentDirection = 2;
    else if (IsKeyDown(KEY_D)) Freddy.velocity.x = 2, CurrentDirection = 3;
    else Freddy.velocity.x = 0;

    if (lastDirection != CurrentDirection)
    {
        switch (CurrentDirection) {
            case 0:
                Freddy.visual = &FreddyIdle;
                break;
            case 1:
                Freddy.visual = &FreddyWUp;
                break;
            case 2:
                Freddy.visual = &FreddyWLeft;
                break;
            case 3:
                Freddy.visual = &FreddyWRight;
                break;
            case 4:
                Freddy.visual = &FreddyWDown;
                break;

        }
        lastDirection = CurrentDirection;
    }

    UpdateWorldEntity(&Freddy);

    WorldCamera.position = (Vector2) {Freddy.position.x + Freddy.size.x / 2, Freddy.position.y + Freddy.size.y / 2};
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