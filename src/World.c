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

#include "Animation.h"
#include "Background.h"
#include "Particle.h"
#include "Settings.h"
#include <stdlib.h>
#include <string.h>
#include "UI.h"
#include "Yellowwood.h"
#include "World_Chip_Note.h"
#include <math.h>
#include "../Include/raymath.h"
#include <stdint.h>
#include <stdio.h>
#include "Save.h"
#include "input.h"
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
WORLDEntity WorldBuildings_Pre[10] = {0}; // World Buildings rendered before Freddy
WORLDEntity WorldWheel = {0}; // Note: The Ferris Wheel has to be seperate due to being behind trees

WORLDEntity WORLDEntities[5] = {0};
WORLDEntity Lolbit[5] = {0};

WORLDEntity Freddy = {0};

UIVisual FreddyIdle = {0};
UIVisual FreddyWLeft = {0};
UIVisual FreddyWUp = {0};
UIVisual FreddyWRight = {0};
UIVisual FreddyWDown = {0};

WORLDEntity WorldZoneButtonUpdater[2];

#define NUMBER_OF_BUTTONS (sizeof(WorldZoneButtonUpdater) / sizeof(WORLDEntity))

UIVisual ButtonUp = {0};
UIVisual ButtonDown = {0};

enum WORLDItemType
{
    FAZTOKENS, 
    CHIP,
    BYTE
};

typedef struct WORLDItem 
{
    enum WORLDItemType type;
    union
    {
        uint16_t token_amount; // Number of Faz-tokens
        uint8_t id; // Byte or Chip IDs
    };
} WORLDItem;

typedef struct WORLDBox 
{
    WORLDItem content;
    WORLDEntity entity;
    _Bool open;
} WORLDBox;

#define NUMBER_OF_CHIPS 7

WORLDBox ChipBoxes[NUMBER_OF_CHIPS] = {0};
Texture2D ItemAtlas = {0};

#define NUMBER_OF_MINES 2

WORLDEntity Ent_MinesTeleporters[NUMBER_OF_MINES];
WORLDEntity Ex_MinesTeleporters[NUMBER_OF_MINES];

UITexture ZoneHeader[4] = {0};
char * ZoneNames[] = {"Fazbear Hills", "Choppy's Woods", "Dusting Fields"};

UIElement JumpVisual = {0};

#define NUMBER_OF_WARP_BUTTONS 7
_WarpButton WarpButtons[NUMBER_OF_WARP_BUTTONS] = {0};
Sound WarpSoundEffect = {0};

UIButton PartyButton = {0};
UIButton ChipsButton = {0};
UIButton BytesButton = {0};
UIButton SaveButton = {0};

TouchJoystick Mobile_Joystick = {0};

// Particles

uint8_t BirdParticle = 0;

WORLDEntity CreateWorldEntity(Vector2 position, Vector2 size, Vector2 velocity, UIVisual * visual, float scale, uint16_t collisionTargets, void (*customCollision)(WORLDEntity *), uint16_t depth)
{
    WORLDEntity entity = {0};
    entity.position = position;
    entity.size = size;
    entity.velocity = velocity;
    entity.visual = visual;
    entity.scale = scale;
    entity.customCollision = customCollision;
    entity.depth = depth;
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

static void WarpButton_1(UIButton * button)
{
    PlaySound(WarpSoundEffect);
    Freddy.position = (Vector2) {   38 + 0.5 - Freddy.size.x / 2, 
                                    21 + 0.5 - Freddy.size.y / 2};
}

static void WarpButton_2(UIButton * button)
{
    PlaySound(WarpSoundEffect);
    Freddy.position = (Vector2) {   34 + 0.5 - Freddy.size.x / 2, 
                                    32 + 0.5 - Freddy.size.y / 2};
}

static void WarpButton_3(UIButton * button)
{
    PlaySound(WarpSoundEffect);
    Freddy.position = (Vector2) {   19 + 0.5 - Freddy.size.x / 2, 
                                    34 + 0.5 - Freddy.size.y / 2};
}

static void SaveButtonPress(UIButton * button)
{
    PlaySound(WarpSoundEffect);
    WriteSave((Vector2) {   Freddy.position.x + Freddy.size.x / 2,
                            Freddy.position.y + Freddy.size.y / 2});
}

static void InitOpenedChipBoxes(void)
{
    uint8_t chip_ids[21];
    GetChipInv(&chip_ids);

    for (uint8_t i = 0; i < 21; i++)
    {
        ChipBoxes[i].open = chip_ids[i];
    }
}

static void InitJoystick(void)
{
    Mobile_Joystick.background = LoadTexture("Assets/Overworld/UI_Touch/joystick/backgrounds/joystick_background_black.png");
    Mobile_Joystick.knob = LoadTexture("Assets/Overworld/UI_Touch/joystick/joystick_knob.png");
    Mobile_Joystick.velocity = (Vector2) {0, 0};
    Mobile_Joystick.x = -0.65;
    Mobile_Joystick.y = 0.5;

    SetTextureFilter( Mobile_Joystick.background, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter( Mobile_Joystick.knob, TEXTURE_FILTER_BILINEAR);
}
static void InitMines(void)
{
    static Texture2D mine_atlas = {0};
    static UIVisual mine_front = {0};
    static UIVisual mine_back = {0};

    mine_atlas = LoadTexture("Assets/Overworld/Buildings/Mine_Atlas.png");

    //SetTextureFilter(mine_atlas, TEXTURE_FILTER_BILINEAR);

    mine_front = CreateUIVisual_UITextureSnippet(mine_atlas, (Rectangle) {0, 0, 113, 75}, WHITE);
    mine_back = CreateUIVisual_UITextureSnippet(mine_atlas, (Rectangle) {113, 0, 80, 53}, WHITE);

    Ent_MinesTeleporters[0] = CreateWorldEntity((Vector2){44, 31},
                                                (Vector2){2, 2}, 
                                                (Vector2) {0, 0}, 
                                                &mine_front, 
                                                1, 
                                                0, 
                                                NULL, 
                                                3);
    Ent_MinesTeleporters[1] = CreateWorldEntity((Vector2){8, 21}, 
                                                (Vector2){2, 2}, 
                                                (Vector2) {0, 0}, 
                                                &mine_front, 
                                                1, 
                                                0, 
                                                NULL, 
                                                3);

    Ex_MinesTeleporters[0] = CreateWorldEntity( (Vector2){29, 72}, 
                                                (Vector2){1, 1}, 
                                                (Vector2) {0, 0}, 
                                                &mine_back, 
                                                1, 
                                                0, 
                                                NULL, 
                                                3);
    Ex_MinesTeleporters[1] = CreateWorldEntity( (Vector2){36, 62}, 
                                                (Vector2){1, 1}, 
                                                (Vector2) {0, 0}, 
                                                &mine_back, 
                                                1, 
                                                0, 
                                                NULL, 
                                                3);
}
static void InitBoxes(void)
{
    static UIVisual grey_chip_chest = {0};

    grey_chip_chest = CreateUIVisual_UITextureSnippet(ItemAtlas, (Rectangle) {0,0, 50, 50}, WHITE);

    // Initizing boxes

    ChipBoxes[0] = (WORLDBox) {{CHIP, .id=0 }, 
                                CreateWorldEntity(  (Vector2) {46, 12},
                                                    (Vector2) {1, 1},
                                                    (Vector2) {0, 0},
                                                    &grey_chip_chest, 
                                                    1, 
                                                    0, 
                                                    NULL, 
                                                    3), 
                                0};
    
    ChipBoxes[1] = (WORLDBox) {{CHIP, .id=1 }, 
                                CreateWorldEntity(  (Vector2) {38, 7},
                                                    (Vector2) {1, 1},
                                                    (Vector2) {0, 0},
                                                    &grey_chip_chest, 
                                                    1, 
                                                    0, 
                                                    NULL, 
                                                    3), 
                                0};

    ChipBoxes[2] = (WORLDBox) {{CHIP, .id=2 }, 
                                CreateWorldEntity(  (Vector2) {50, 22},
                                                    (Vector2) {1, 1},
                                                    (Vector2) {0, 0},
                                                    &grey_chip_chest, 
                                                    1, 
                                                    0, 
                                                    NULL, 
                                                    3), 
                                0};
    
    ChipBoxes[3] = (WORLDBox) {{CHIP, .id=3 }, 
                                CreateWorldEntity(  (Vector2) {26, 29},
                                                    (Vector2) {1, 1},
                                                    (Vector2) {0, 0},
                                                    &grey_chip_chest, 
                                                    1, 
                                                    0, 
                                                    NULL, 
                                                    3), 
                                0};
    
    ChipBoxes[4] = (WORLDBox) {{CHIP, .id=4 }, 
                                CreateWorldEntity(  (Vector2) {15, 27},
                                                    (Vector2) {1, 1},
                                                    (Vector2) {0, 0},
                                                    &grey_chip_chest, 
                                                    1, 
                                                    0, 
                                                    NULL, 
                                                    3), 
                                0};
    
    ChipBoxes[5] = (WORLDBox) {{CHIP, .id=5 }, 
                                CreateWorldEntity(  (Vector2) {33, 32},
                                                    (Vector2) {1, 1},
                                                    (Vector2) {0, 0},
                                                    &grey_chip_chest, 
                                                    1, 
                                                    0, 
                                                    NULL, 
                                                    3), 
                                0};

    ChipBoxes[6] = (WORLDBox) {{CHIP, .id=6 }, 
                                CreateWorldEntity(  (Vector2) {49, 18},
                                                    (Vector2) {1, 1},
                                                    (Vector2) {0, 0},
                                                    &grey_chip_chest, 
                                                    1, 
                                                    0, 
                                                    NULL, 
                                                    3), 
                                0};
    InitOpenedChipBoxes();
}

static void InitFreddy(void)
{
    // Freddy Sprites
    
    FreddyIdle.type = UItexture;
    FreddyIdle.texture = LoadTexture("Assets/Overworld/Freddy_Overworld/idle.png"); 
    FreddyIdle.tint = WHITE;
    
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

    SetTextureFilter(FreddyIdle.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(FreddyWUp.animation_V2.Atlas, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(FreddyWLeft.animation_V2.Atlas, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(FreddyWRight.animation_V2.Atlas, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(FreddyWDown.animation_V2.Atlas, TEXTURE_FILTER_BILINEAR);

    Freddy.visual = &FreddyIdle;
    Freddy.scale = 0.95;
    Freddy.depth = 2;
}

static void InitLolbit(void)
{
    Lolbit[0] = CreateWorldEntity( (Vector2) {36, 17}, 
                                            (Vector2) {1, 1},
                                            (Vector2) {0,0}, 
                                            UIVisual_Heap(CreateUIVisual_UIAnimation_V2("Assets/Overworld/NPCs/lolbit.png", 30, 15, (Vector2) {65, 65}, WHITE)), 
                                            0.93, 
                                            0, 
                                            NULL, 2);
}

static void InitBuildings_Pre(void)
{
    static UIVisual blue_castle = {0};
    static UIVisual red_castle = {0};
    static UIVisual gear_house = {0};
    static UIVisual lumber_house = {0};
    static UIVisual chimney_freddy = {0};

    blue_castle = CreateUIVisual_UITexture_P( "Assets/Overworld/Buildings/Blue_Castle.png", WHITE);
    red_castle = CreateUIVisual_UITexture_P("Assets/Overworld/Buildings/Red_Castle.png", WHITE);
    gear_house = CreateUIVisual_UITexture_P("Assets/Overworld/Buildings/Gear_House.png", WHITE);
    lumber_house = CreateUIVisual_UITexture_P("Assets/Overworld/Buildings/Lumber_House.png", WHITE);
    chimney_freddy = CreateUIVisual_UIAnimation_V2( "Assets/Overworld/NPCs/chimney_freddy.png", 
                                                    15, 
                                                    31, 
                                                    (Vector2) {50, 50}, 
                                                    WHITE);

    WorldBuildings_Pre[0] = CreateWorldEntity(  (Vector2) {33.8, 11}, 
                                                (Vector2) {0,0}, 
                                                (Vector2) {0,0}, 
                                                &blue_castle, 
                                                1, 
                                                0, 
                                                NULL, 3);

    WorldBuildings_Pre[1] = CreateWorldEntity(  (Vector2) {37.5, 10.5},
                                                (Vector2) {0,0}, 
                                                (Vector2) {0,0}, 
                                                &red_castle,
                                                1, 
                                                0, 
                                                NULL, 3);

    WorldBuildings_Pre[2] = CreateWorldEntity(  (Vector2) {36.5, 15.5}, 
                                                (Vector2) {0,0},
                                                (Vector2) {0,0}, 
                                                &gear_house, 
                                                1, 
                                                0, 
                                                NULL, 3);

    WorldBuildings_Pre[3] = CreateWorldEntity(  (Vector2) {42, 12}, 
                                                (Vector2) {0,0}, 
                                                (Vector2) {0,0}, 
                                                &lumber_house, 
                                                1, 
                                                0, 
                                                NULL, 3);
    WorldBuildings_Pre[4] = CreateWorldEntity(  (Vector2) {8, 19.25}, 
                                                (Vector2) {1,1}, 
                                                (Vector2) {0,0}, 
                                                &chimney_freddy, 
                                                1, 
                                                0, 
                                                NULL, 3);
}

static void InitBuildings_After(void)
{
    static UIVisual turbine = {0};

    if (!turbine.type) turbine = CreateUIVisual_UIAnimation_V2("Assets/Overworld/Buildings/turbine_atlas.png", 30, 10, (Vector2) {50, 100}, WHITE);

    WorldBuildings_After[0] = CreateWorldEntity((Vector2) {31.5, 17}, 
                                                (Vector2) {0,0}, 
                                                (Vector2) {0,0}, 
                                                UIVisual_Heap(CreateUIVisual_UIAnimation_V2("Assets/Overworld/Buildings/windmill_atlas.png", 30, 20, (Vector2) {200, 200}, WHITE)),
                                                1, 
                                                0,
                                                NULL,1);
    WorldBuildings_After[1] = CreateWorldEntity((Vector2) {33, 29.375}, 
                                                (Vector2) {1,2}, 
                                                (Vector2) {0,0}, 
                                                &turbine,
                                                1, 
                                                0, 
                                                NULL, 1);
    WorldBuildings_After[2] = CreateWorldEntity((Vector2) {35, 29.375}, 
                                                (Vector2) {1,2}, 
                                                (Vector2) {0,0}, 
                                                &turbine,  
                                                1, 
                                                0, 
                                                NULL, 1);
}

static void InitZoneButtons(void)
{
    ButtonUp = CreateUIVisual_UITextureSnippet( ItemAtlas,   
                                                (Rectangle) {50, 50, 50, 50}, 
                                                WHITE);
    ButtonDown = CreateUIVisual_UITextureSnippet(   ItemAtlas,   
                                                    (Rectangle) {100, 50, 50, 50}, 
                                                    WHITE);

    

    WorldZoneButtonUpdater[0] = CreateWorldEntity(  (Vector2) {32, 32}, 
                                                    (Vector2) {1,1}, 
                                                    (Vector2) {0,0}, 
                                                    &ButtonUp,  
                                                    1, 
                                                    0, 
                                                    NULL, 1);

    WorldZoneButtonUpdater[1] = CreateWorldEntity(  (Vector2) {19, 34}, 
                                                    (Vector2) {1,1}, 
                                                    (Vector2) {0,0}, 
                                                    NULL,  
                                                    1, 
                                                    0, 
                                                    NULL, 1);

    for (uint8_t i = 2; i <= GetZone_Level() && i - 2 < NUMBER_OF_BUTTONS; i++)
    {
        if (i - 2 == 1) continue;
        WorldZoneButtonUpdater[i - 2].visual = &ButtonDown;
    }
}

void InitWorld(void)
{
    if (!CurrentWorld) LoadWorldTilemap();

    // Temporary limits log level to minimize printing to console (for faster load times)

    //SetTraceLogLevel(LOG_WARNING);

    SetWorldSpriteSheet("Assets/Overworld/Maps/Overworld/spritesheet.png", 50); 

    // Particles

    BirdParticle = CreateParticleIndexA_V2("Assets/Particles/bird.png", 30, 10, (Vector2) {50, 50}, 1.5);
    
    // Musics and Sounds

    CurrentTheme = LoadMusicStream("Assets/Themes/fazbearhills.mp3");
    CurrentTheme.looping = 1;

    PlayMusicStream(CurrentTheme);

    WarpSoundEffect = LoadSound("Assets/Sound_Effects/Zone_Warping.wav");

    // Zone Effects

    LegacyZoneEffect = CreateUIVisual_UITexture_P("Assets/Overworld/Zone_Effects/sun_effect_mod.png", SKY_TINT);
    SetTextureFilter(LegacyZoneEffect.texture, TEXTURE_FILTER_BILINEAR);

    SunHeader = LoadTexture("Assets/Overworld/sun_effect_top.png");
    SetTextureWrap(SunHeader, TEXTURE_WRAP_CLAMP);
    SetTextureFilter(SunHeader, TEXTURE_FILTER_BILINEAR);

    InitFreddy();

    // Initizing repeated UIVisuals

    InitBuildings_Pre();
    InitBuildings_After();
    
    WorldWheel = CreateWorldEntity(  (Vector2) {14.25, 15.25}, 
                                                (Vector2) {5,5}, 
                                                (Vector2) {0,0}, 
                                                UIVisual_Heap(CreateUIVisual_UITexture_P("Assets/Overworld/Buildings/Wheel.png", 
                                                                                                        WHITE)), 
                                                1, 
                                                0, 
                                                NULL, 6);
    
    ItemAtlas = LoadTexture("Assets/Overworld/NPCs/items.png");

    InitZoneButtons();
    InitJoystick();
    InitMines();
    InitBoxes();

    ZoneHeader[0] = LoadTexture("Assets/Overworld/UI/Zone_Names/1.png"); 
    ZoneHeader[1] = LoadTexture("Assets/Overworld/UI/Zone_Names/2.png"); 
    ZoneHeader[2] = LoadTexture("Assets/Overworld/UI/Zone_Names/3.png"); 
    ZoneHeader[3] = LoadTexture("Assets/Overworld/UI/Zone_Names/4.png"); 

    WarpButtons[0] = (_WarpButton) {    {   CreateUIElement(CreateUIVisual_UITexture_P("Assets/Overworld/UI/Zone_Buttons/1.png", 
                                                                WHITE), 
                                            0.90, -0.8, 1.5),
                                        WarpButton_1,
                                        NULL, 
                                        0},
                                    1};

    WarpButtons[1] = (_WarpButton) {    {   CreateUIElement(CreateUIVisual_UITexture_P("Assets/Overworld/UI/Zone_Buttons/2.png", 
                                                                WHITE), 
                                            0.90, -0.6, 1.5),
                                        WarpButton_2,
                                        NULL, 
                                        0},
                                    1};
    
    WarpButtons[2] = (_WarpButton) {    {   CreateUIElement(CreateUIVisual_UITexture_P("Assets/Overworld/UI/Zone_Buttons/3.png", 
                                                                WHITE), 
                                            0.90, -0.4, 1.5),
                                        WarpButton_3,
                                        NULL, 
                                        0},
                                    1};

    JumpVisual = CreateUIElement(CreateUIVisual_UITexture_P("Assets/Overworld/UI/Zone_Buttons/Jump.png", 
                                                                WHITE), 
                                            0.90, -0.95, 1.5);

    PartyButton =  (UIButton)   {   CreateUIElement(CreateUIVisual_UITexture_P("Assets/Overworld/UI/Party.png", 
                                                    WHITE), 
                                                    -0.8, 0.9, 1.5),
                                    NULL,
                                    NULL, 
                                    0};

    ChipsButton =  (UIButton)   {   CreateUIElement(CreateUIVisual_UITexture_P("Assets/Overworld/UI/Chips.png", 
                                                    WHITE), 
                                                    -0.525, 0.9, 1.5),
                                    NULL,
                                    NULL, 
                                    0};

    BytesButton =  (UIButton)   {   CreateUIElement(CreateUIVisual_UITexture_P("Assets/Overworld/UI/Bytes.png", 
                                                    WHITE), 
                                                    -0.25, 0.9, 1.5),
                                    NULL,
                                    NULL, 
                                    0};

    SaveButton =  (UIButton)    {   CreateUIElement(CreateUIVisual_UITexture_P("Assets/Overworld/UI/Save.png", 
                                                    WHITE), 
                                                    0.025, 0.9, 1.5),
                                    SaveButtonPress,
                                    NULL, 
                                    0};

    SetTraceLogLevel(LOG_ALL);
}

void ResetWorld(void)
{
    SetWindowTitle("FNaF World: C Edition - Overworld");
    Freddy.collisionTargets = LAYER_COLLIDABLE;
    Freddy.size = (Vector2) {0.7, 0.45};
    Freddy.scale = 0.95;
    Freddy.visualOffset = (Vector2) {0, 0.5};
    Freddy.position = (Vector2) {   GetLast_Location().x + 0.5 - Freddy.size.x / 2, 
                                    GetLast_Location().y + 0.5 - Freddy.size.y / 2};
    Freddy.customCollision = NULL;

    WorldCamera.target = (Vector2) {0, 0};
    WorldCamera.position = (Vector2) {Freddy.position.x + Freddy.size.x / 2, Freddy.position.y + Freddy.size.y / 2};
    WorldCamera.zoom = 9;
    PlayMusicStream(CurrentTheme);
}

static Vector2 GetEntityCorner(WORLDEntity * entity, enum Corner corner)
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

static uint8_t CheckCollisionTilemap(WORLDEntity * entity, WORLDTilemapLayer * layer)
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

static Rectangle GetCameraView(void)
{
    Rectangle CameraView = (Rectangle) {    WorldCamera.position.x - WorldCamera.zoom * ((float) GetScreenWidth() / GetScreenHeight()) / 2, 
                                            WorldCamera.position.y - WorldCamera.zoom / 2, 
                                            WorldCamera.zoom * ((float)GetScreenWidth() / GetScreenHeight()) + 2, 
                                            WorldCamera.zoom + 2    };

    //if (CameraView.x + (CameraView.width - 2) / 2 >= CurrentWorld -> mapWidth) CameraView.x = CurrentWorld -> mapWidth - (CameraView.width - 2) / 2;
    if (CameraView.x <= 0) CameraView.x = 0;

    //if (CameraView.y + (CameraView.height - 2) / 2 >= CurrentWorld -> mapHeight) CameraView.y = CurrentWorld -> mapHeight - (CameraView.height - 2) / 2;
    if (CameraView.y <= 0) CameraView.y = 0;

    return CameraView;
}

// Gets the width of the virtual screen used when drawing the overworld
static uint32_t Get_V_Width(void)
{
    return WorldCamera.zoom * CurrentTileSize * GetScreenRatio();
}

// Gets the height of the virtual screen used when drawing the overworld
static uint32_t Get_V_Height(void)
{
    return WorldCamera.zoom * CurrentTileSize;
}

// Uses AABB Collision to check if collision has occured between to WORLDEntities
static _Bool CheckEntityCollision(WORLDEntity * collider, WORLDEntity * collidee)
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
static void HandleEntityCollision(WORLDEntity * entity)
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
    if (entity -> velocity.x == 0 && entity -> velocity.y == 0) return;
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


// Scales and Renders a Texture2D relative to the WORLDCamera
void RenderWorldTexture(Texture2D * texture, Vector2 position, Vector2 offset, float scale)
{
    Rectangle CameraView = GetCameraView();

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

// Scales and Renders a snippet of a Texture2D relative to the WORLDCamera
void RenderWorldTextureSnippet(Texture2D * atlas, Vector2 position, Rectangle snippet, Vector2 offset, float scale)
{
    Rectangle CameraView = GetCameraView();

    Vector2 screen_pos = (Vector2) {(position.x - (uint16_t)CameraView.x) * 50., 
                                    (position.y - (uint16_t)CameraView.y) * 50.};

    DrawTexturePro( *atlas, 
                    snippet,
                    (Rectangle) {screen_pos.x, screen_pos.y,
                                 snippet.width * scale, snippet.height * scale},
                    (Vector2) {snippet.width * scale * (offset.x + 1) / 2, snippet.height * scale * (offset.y + 1) / 2},
                    0,
                    WHITE);
}

// Scales and Renders a UIanimationV2 relative to the WORLDCamera
void RenderWorldAnimation_V2(Animation_V2 * animation, Vector2 position, Vector2 offset, float scale)
{
    Rectangle CameraView = GetCameraView();

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
    static uint8_t cam_extend = 3; // For quick entity skipping without having to first check the type of visual and getting that visual's sizw
    Vector2 position = position = (Vector2) {entity -> position.x + entity -> size.x / 2, entity -> position.y + entity -> size.y / 2};

    // Checking if the entity is off camera and if so skipping rendering

    Rectangle camera = GetCameraView();

    camera.x -= cam_extend;
    camera.y -= cam_extend;
    camera.width += cam_extend;
    camera.height += cam_extend;

    if (    entity -> position.x + entity -> size.x < camera.x ||
            entity -> position.x > camera.x + camera.width ||
            entity -> position.y + entity -> size.y < camera.y ||
            entity -> position.y > camera.y + camera.height) return;
    
    // Rendering entity

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

        case UItextureSnippet:
            texture = &entity -> visual -> texture;
            RenderWorldTextureSnippet(  texture,
                                        position,
                                        entity -> visual -> snippet,
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

enum FLAGS_ENTITY_ARRAY_RENDERING
{
    PROPER,
    FAST,
    IGNORE_DEPTH
};

// Scales and Renders all in a WORLDEntity array if the first element's depth is equal to depth
static void RenderWorldEntities_FAST(WORLDEntity * entities, uint16_t depth)
{
    if (entities[0].depth != depth) return;

    for (uint16_t i = 0; entities[i].visual != NULL; i++)
    {
        RenderWorldEntity(entities + i);
    }
}

// Scales and Renders all in a WORLDEntity array that is at depth
void RenderWorldEntities_PROPER(WORLDEntity * entities, uint16_t depth)
{
    for (uint16_t i = 0; entities[i].visual != NULL; i++)
    {
        if (entities[i].depth != depth) continue;
        RenderWorldEntity(entities + i);
    }
}

// Scales and Renders all in a WORLDEntity array
void RenderWorldEntities_IGNORE_DEPTH(WORLDEntity * entities)
{
    for (uint16_t i = 0; entities[i].visual != NULL; i++) RenderWorldEntity(entities + i);
}

// Scales and Renders all in a WORLDEntity array
void RenderWorldEntities(WORLDEntity * entities, uint16_t depth, enum FLAGS_ENTITY_ARRAY_RENDERING mode)
{
    switch (mode) 
    {
        case PROPER:
            RenderWorldEntities_PROPER(entities, depth);
            return;
        case FAST:
            RenderWorldEntities_FAST(entities, depth);
            return;
        case IGNORE_DEPTH:
            RenderWorldEntities_IGNORE_DEPTH(entities);
            return;
    }
}

// Spawns Bird Particles on screen every 2 seconds
void SpawnBirds(void)
{
    static clock_t timeSinceLastParticle = 0;
    static float wait = 2;
    uint8_t count = GetRandomValue(1,7);
    
    if (clock() - timeSinceLastParticle > CLOCKS_PER_SEC * wait) 
    {
        float degrees = 225/180.*PI;
        Vector2 start = (Vector2) {1, GetRandomValue(50, 150) / 100. - 1};
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

void RenderZoneEffect_Back_Texture(Vector2 offset, uint8_t stretched)
{
    float screenRatio = (float) GetScreenWidth() / GetScreenHeight();
    int vWidth = (WorldCamera.zoom * CurrentTileSize) * screenRatio;
    int vHeight = WorldCamera.zoom * CurrentTileSize;

    float scale =   GetScreenRatio() > 1.66666666667 ? 
                        (float) vWidth / LegacyZoneEffect.texture.width + 0.1 :
                        (float) vHeight / LegacyZoneEffect.texture.height + 0.1;

    uint16_t width = (LegacyZoneEffect.texture.width * scale);
    uint16_t height = (LegacyZoneEffect.texture.height * scale);

    if (!stretched)
    {
        offset.x += vWidth / 2. - width / 2.;
        offset.y += vHeight / 2. - height / 2.;
    }
    DrawTexturePro( LegacyZoneEffect.texture, 
                    (Rectangle) {0, 0, LegacyZoneEffect.texture.width, LegacyZoneEffect.texture.height}, 
                    (Rectangle) {offset.x, offset.y, stretched ? vWidth : width, stretched ? vHeight : height}, 
                    (Vector2) {0, 0}, 0, 
                    LegacyZoneEffect.tint);
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

    RenderZoneEffect_Back_Texture(offset, 1);
    EndBlendMode();
}

void RenderZoneEffect_Zone3(Vector2 offset)
{
    float screenRatio = (float) GetScreenWidth() / GetScreenHeight();

    int vWidth = (WorldCamera.zoom * CurrentTileSize) * screenRatio;
    int vHeight = WorldCamera.zoom * CurrentTileSize;

    float scale =   GetScreenRatio() > 1.66666666667 ? 
                        (float) vWidth / LegacyZoneEffect.animation_V2.TileSize_x + 0.1 :
                        (float) vHeight / LegacyZoneEffect.animation_V2.TileSize_y + 0.1;

    uint16_t width = (LegacyZoneEffect.animation_V2.TileSize_x * scale);
    uint16_t height = (LegacyZoneEffect.animation_V2.TileSize_y * scale);

    BeginBlendMode(BLEND_ADDITIVE);

    DrawTexturePro( SunHeader, 
                    (Rectangle) {0, 0, SunHeader.width, SunHeader.height}, 
                    (Rectangle) {offset.x, offset.y, vWidth, vHeight / 3}, 
                    (Vector2) {0, 0}, 0, 
                    WHITE);

    EndBlendMode();
    
    DrawAnimation_V2(   &LegacyZoneEffect.animation_V2, 
                        offset.x + vWidth / 2. - width / 2., 
                        offset.y + vHeight / 2. - height / 2., 
                        scale, 
                        0);
    
}


// Gets the zone Freddy is currently in
uint8_t GetZone(void)
{
    static uint16_t ZoneIds[] = {32, 33, 46, 89};
    Vector2 ZoneCheck = (Vector2) {Freddy.position.x + Freddy.size.x / 2, Freddy.position.y + Freddy.size.y / 2};

    if (!CurrentWorld) return 0;

    uint16_t Zone = AccessPositionInLayer((uint16_t) ZoneCheck.x, (uint16_t) ZoneCheck.y, CurrentWorld->layers + 0);

    uint16_t i = 0;

    for (; i <= sizeof(ZoneIds) / 2; i++) 
    {
        if (Zone == ZoneIds[i]) break;
    }
    if (i == sizeof(ZoneIds) / 2) return 0xff;
    return i;
}
void RenderZoneEffect(void)
{
    uint16_t zone = GetZone();
    Rectangle CameraView = GetCameraView();

    Vector2 CameraMinorOffset = (Vector2) { (float) (CameraView.x - (uint16_t) CameraView.x) * CurrentTileSize,
                                            (float) (CameraView.y - (uint16_t) CameraView.y) * CurrentTileSize};
    switch (zone + 1) {
        case DUSTINGFIELDS:
            RenderZoneEffect_Zone3(CameraMinorOffset);
            break;
        case MYSTERIOUSMINES:
            RenderZoneEffect_Back_Texture(CameraMinorOffset, 0);
            break;
        case FAZBEARHILLS:
        case CHOPPYSWOODS: 
        default: 
            SpawnBirds();
            RenderZoneEffect_Zone1(CameraMinorOffset);  
            break;
    }
}

void RenderZoneName(void)
{
    uint8_t zone = GetZone();
    if (zone == 0xff) RenderUIText("Unknown Zone", -0.95, -0.9, 0.03, LEFTMOST, (Font) {0}, WHITE);
    float scale = (float) ZoneHeader[zone].height / GetScreenHeight();
    Color tint = WHITE;
    if (zone == 2 && DustingFieldsLogoIsBlack) tint = BLACK; 
    DrawTextureEx(ZoneHeader[zone], (Vector2) {25. * GetScreenHeight() / 720, 25. * GetScreenHeight() / 720}, 0, 0.025/scale, tint);
}

void RenderWorldButtons(void)
{
    for (uint8_t i = 2; i <= GetZone_Level() && i - 2 < NUMBER_OF_BUTTONS; i++)
    {
        if (i - 2 == 1) continue;
        WorldZoneButtonUpdater[i - 2].visual = &ButtonDown;
    }
    RenderWorldEntities(WorldZoneButtonUpdater, 0, IGNORE_DEPTH);
}

void RenderChipBoxes(WORLDBox * boxes, uint16_t amount)
{
    for (uint16_t i = 0; i < amount; i++)
    {
        if (boxes[i].open) continue;
        RenderWorldEntity(&boxes[i].entity);
    }
}

// Renders WORLDTilemapLayer onto Virtual Screen
void RenderLayer(uint16_t n, Vector2 CameraMinorOffset)
{
    if (CurrentWorld -> layers[n].FLAGS & LAYER_INVISIBLE)
    {
        return;
    } 

    Rectangle CameraView = GetCameraView();
    
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

// Renders the entire overworld on-screen
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

    Vector2 CameraMinorOffset = (Vector2) { (float) (CameraView.x - (uint16_t) CameraView.x) * (GetScreenHeight() / WorldCamera.zoom),
                                            (float) (CameraView.y - (uint16_t) CameraView.y) * (GetScreenHeight() / WorldCamera.zoom)};
    BeginTextureMode(WorldVirtualScreen);

    ClearBackground(BLACK);

    // Renders all tiles in each layer

    for (uint16_t i = CurrentWorld -> amount; i > 0; i--) 
    {
        RenderLayer(i, CameraMinorOffset);

        // Renders all WORLDEntities

        RenderWorldEntities(Ent_MinesTeleporters, i, FAST);
        RenderWorldEntities(Ex_MinesTeleporters, i, FAST);
        if (i == 3) RenderWorldButtons();
        if (i == Freddy.depth) RenderChipBoxes(ChipBoxes, NUMBER_OF_CHIPS);
        if (i == Freddy.depth) RenderWorldEntity(&Freddy);
        if (i == WorldWheel.depth) RenderWorldEntity(&WorldWheel);

        RenderWorldEntities(WorldBuildings_Pre, i, FAST);
        
        RenderWorldEntities(WorldBuildings_After, i, FAST);
    }

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

static float absf(float x)
{
    *(int *)&x &= 0x7fffffff;
    return x;
}

void UpdateFreddy(void)
{
    static uint8_t lastDirection = 0;

    uint8_t CurrentDirection = 0;

    if (GetInputType() == KEYBOARD)
    {
        if (IsKeyDown(KEY_W)) Freddy.velocity.y = -2, CurrentDirection = 1;
        else if (IsKeyDown(KEY_S)) Freddy.velocity.y = 2, CurrentDirection = 4;
        else Freddy.velocity.y = 0;

        if (IsKeyDown(KEY_A)) Freddy.velocity.x = -2, CurrentDirection = 2;
        else if (IsKeyDown(KEY_D)) Freddy.velocity.x = 2, CurrentDirection = 3;
        else Freddy.velocity.x = 0;
    } else if (GetInputType() == TOUCH) {
        Freddy.velocity.x = Mobile_Joystick.velocity.x * 2;
        Freddy.velocity.y = Mobile_Joystick.velocity.y * 2;
    }
    

    if (Freddy.velocity.y > 0) CurrentDirection = 4;
    else if (Freddy.velocity.y < 0) CurrentDirection = 1;

    if (absf(Freddy.velocity.x) >= absf(Freddy.velocity.y))
    {
        if (Freddy.velocity.x > 0) CurrentDirection = 3;
        else if (Freddy.velocity.x < 0) CurrentDirection = 2;
    }
    

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

void UpdateZoneAssets(void)
{
    static enum WORLDZONES LastZoneCheck = 1;
    
    if (LastZoneCheck == GetZone() + 1) return;

    if ((LastZoneCheck == FAZBEARHILLS || LastZoneCheck == CHOPPYSWOODS)
         && (GetZone() + 1 == FAZBEARHILLS || GetZone() + 1 == CHOPPYSWOODS)) return;
    
    LastZoneCheck = GetZone() + 1;

    UnloadMusicStream(CurrentTheme);
    FreeUIVisual(&LegacyZoneEffect);

    UnloadTexture(Mobile_Joystick.background);
    
    memset(&LegacyZoneEffect, 0, sizeof(UIVisual));
    switch (LastZoneCheck) 
    {
        case DUSTINGFIELDS:
            CurrentTheme = LoadMusicStream("Assets/Themes/dustingfields.mp3");
            LegacyZoneEffect = CreateUIVisual_UIAnimation_V2(   "Assets/Overworld/Zone_Effects/dusting_fields_effect.png", 
                                                                60, 11,
                                                                (Vector2) {800, 480}, WHITE);
            SetTextureFilter(LegacyZoneEffect.animation_V2.Atlas, TEXTURE_FILTER_BILINEAR);
            Mobile_Joystick.background = LoadTexture("Assets/Overworld/UI_Touch/joystick/backgrounds/joystick_background_black.png");
            FlushParticles();
            break;
        case MYSTERIOUSMINES:
            CurrentTheme = LoadMusicStream("Assets/Themes/mysteriousmines.mp3");
            LegacyZoneEffect = CreateUIVisual_UITexture_P("Assets/Overworld/Zone_Effects/mysterious_mines_effect.png", WHITE);
            Mobile_Joystick.background = LoadTexture("Assets/Overworld/UI_Touch/joystick/backgrounds/joystick_background_blue.png");
            FlushParticles();
            break;
        case CHOPPYSWOODS:
        case FAZBEARHILLS:
        default:
            CurrentTheme = LoadMusicStream("Assets/Themes/fazbearhills.mp3");
            LegacyZoneEffect = CreateUIVisual_UITexture_P("Assets/Overworld/Zone_Effects/sun_effect_mod.png", SKY_TINT);
            Mobile_Joystick.background = LoadTexture("Assets/Overworld/UI_Touch/joystick/backgrounds/joystick_background_black.png");
            SetTextureFilter(LegacyZoneEffect.texture, TEXTURE_FILTER_BILINEAR);
    }
    
    SetTextureFilter( Mobile_Joystick.background, TEXTURE_FILTER_BILINEAR);

    CurrentTheme.looping = 1;
    PlayMusicStream(CurrentTheme);
}

void PutZoneWarp(void)
{
    JumpVisual.visual.tint = WHITE;
    if (GetZone() + 1 == 2 && DustingFieldsLogoIsBlack) JumpVisual.visual.tint = BLACK; 
    RenderUIElement(&JumpVisual);
    for (uint16_t i = 0; i < GetZone_Level(); i++)
    {
        PutUIButton(&WarpButtons[i].button);
    }
}


enum UIStyles 
{
    PCORIGINAL, MOBILE, CONTROLLER, PCMINIMAL
};

static void SwitchUI_PC_ORIGINAL(void)
{   
    for (uint8_t i = 0; i < NUMBER_OF_WARP_BUTTONS; i++) 
    {
        WarpButtons[i].button.graphic.x = 0.90f;
        WarpButtons[i].button.graphic.y = -0.8f + 0.2f * i;
    }

    JumpVisual.x = 0.90f;
    JumpVisual.y = -0.95f;

    PartyButton.graphic.x = -0.8f;
    PartyButton.graphic.y = 0.9f;

    ChipsButton.graphic.x = -0.525f;
    ChipsButton.graphic.y = 0.9f;

    BytesButton.graphic.x = -0.25f;
    BytesButton.graphic.y = 0.9f;

    SaveButton.graphic.x = 0.025f;
    SaveButton.graphic.y = 0.9f;
}

static void SwitchUI_MOBILE(void)
{   
    for (uint8_t i = 0; i < NUMBER_OF_WARP_BUTTONS; i++) 
    {
        WarpButtons[i].button.graphic.x = 0.90f;
        WarpButtons[i].button.graphic.y = -0.8f + 0.2f * i;
    }

    JumpVisual.x = 0.90f;
    JumpVisual.y = -0.95f;

    PartyButton.graphic.x = 0.55f;
    PartyButton.graphic.y = -0.9f;

    ChipsButton.graphic.x = 0.275f;
    ChipsButton.graphic.y = -0.9f;

    BytesButton.graphic.x = 0;
    BytesButton.graphic.y = -0.9f;

    SaveButton.graphic.x = -0.275f;
    SaveButton.graphic.y = -0.9f;
}

static void SwitchUI(enum Input_Types style)
{
    switch (style) {
        case MOBILE:
            SwitchUI_MOBILE();
            return;
        case PCORIGINAL:
        default:
            SwitchUI_PC_ORIGINAL();
            return;
    }
}

// Touch UI

float Vector2Strength(Vector2 v)
{
    return sqrtf((v.x * v.x) + (v.y * v.y));
}

Vector2 Vector2Norm(Vector2 v, float target)
{
    float angle = tanhf(v.y/v.x);
    return (Vector2) {cosf(angle) * target, sinf(angle) * target};
}

void UpdateJoystick(void)
{
    float scale = GetScreenScale();

    Vector2 joystick_position = (Vector2) { SCREEN_POSITION_TO_PIXEL_X(Mobile_Joystick.x, Mobile_Joystick.background.width, scale),
                                            SCREEN_POSITION_TO_PIXEL_Y(Mobile_Joystick.y, Mobile_Joystick.background.height, scale)};
    
    Vector2 joystick_centre = (Vector2) { joystick_position.x + Mobile_Joystick.background.width * scale / 2,
                                            joystick_position.y + Mobile_Joystick.background.height * scale / 2};

    Rectangle joystick_hitbox = (Rectangle) {   joystick_position.x - Mobile_Joystick.background.width * scale / (3/2.), 
                                                joystick_position.y - Mobile_Joystick.background.height * scale / (3/2.), 
                                                Mobile_Joystick.background.width * scale * 3, 
                                                Mobile_Joystick.background.height * scale * 3};

    Vector2 * touch_points = GetInputDown();

    Vector2 velocity = {0};

    if (IsKeyDown(KEY_A)) velocity.x = -1;
    else if (IsKeyDown(KEY_D)) velocity.x = 1;

    if (IsKeyDown(KEY_W)) velocity.y = -1;
    else if (IsKeyDown(KEY_S)) velocity.y = 1;

    if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_S)) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)))
    {
        Mobile_Joystick.velocity = velocity;
        return;
    }

    for (uint8_t i = 0; i < MAX_INPUT_POINTS + 1; i++) 
    {
        if (touch_points[i].x == NAN) break;
        if (!CheckCollisionPointRec(touch_points[i], joystick_hitbox)) continue;
        
        velocity = (Vector2) {  (-joystick_centre.x + touch_points[i].x) /  (Mobile_Joystick.background.width * scale / 2),
                                (-joystick_centre.y + touch_points[i].y) /  (Mobile_Joystick.background.height * scale / 2)};
    }

    if (Vector2Length(velocity) > 1) velocity = Vector2Normalize(velocity);
    Mobile_Joystick.velocity = velocity;
}

void RenderJoystick(void)
{
    float scale = GetScreenScale();
    Vector2 knob_position = (Vector2) { SCREEN_POSITION_TO_PIXEL_X(Mobile_Joystick.x, Mobile_Joystick.background.width, scale),
                                        SCREEN_POSITION_TO_PIXEL_Y(Mobile_Joystick.y, Mobile_Joystick.background.height, scale)};
    knob_position.x += Mobile_Joystick.velocity.x * Mobile_Joystick.background.width / 2 * scale;
    knob_position.y += Mobile_Joystick.velocity.y * Mobile_Joystick.background.height / 2 * scale;
    
    RenderUITexture(Mobile_Joystick.background, Mobile_Joystick.x, Mobile_Joystick.y, scale);

    DrawTexturePro( Mobile_Joystick.knob, 
                    (Rectangle) {0, 0, Mobile_Joystick.knob.width, Mobile_Joystick.knob.height}, 
                    (Rectangle) {knob_position.x, knob_position.y, Mobile_Joystick.knob.width * scale, Mobile_Joystick.knob.height * scale},
                    (Vector2) {-Mobile_Joystick.knob.width * scale / 2, -Mobile_Joystick.knob.height * scale / 2},
                    0,
                    WHITE);
}

void PutTouchJoystick(void)
{
    UpdateJoystick();
    RenderJoystick();
}

void PutTouchUI(void)
{
    PutZoneWarp();

    if (GetScreenRatio() <= 81/50.) SetUIScreenScaleMode(WIDTH);

    PutUIButton(&PartyButton);
    PutUIButton(&ChipsButton);
    PutUIButton(&BytesButton);
    PutUIButton(&SaveButton);

    PutTouchJoystick();

    SetUIScreenScaleMode(HEIGHT);

    
}

void PutPC_Original_UI(void)
{
    PutZoneWarp();

    if (GetScreenRatio() <= 81/50.) SetUIScreenScaleMode(WIDTH);

    PutUIButton(&PartyButton);
    PutUIButton(&ChipsButton);
    PutUIButton(&BytesButton);
    PutUIButton(&SaveButton);

    SetUIScreenScaleMode(HEIGHT);
}

void PutDefaultUI(void)
{
    static enum Input_Types UI_type = KEYBOARD;

    if (UI_type != GetInputType())
    {
        UI_type = GetInputType();
        SwitchUI(UI_type);
    }

    switch (UI_type) 
    {
        case KEYBOARD:
            return PutPC_Original_UI();
        case TOUCH:
            return PutTouchUI();
    }
}

void HandleWorldButtonCollision(void)
{
    for (uint8_t i = GetZone_Level() - 1; i < NUMBER_OF_BUTTONS; i++)
    {
        if (!CheckEntityCollision(WorldZoneButtonUpdater + i, &Freddy)) continue;
        SetZone_Level(i + 2);
        WriteSave(Freddy.position);
        return;
    }
}

void HandleSingleBoxCollision(WORLDBox * box)
{
    if (!CheckEntityCollision(&box->entity, &Freddy) ||
        box -> open) return;

    switch (box -> content.type)
    {
        case FAZTOKENS: 
            UpdateFaz_Tokens(box -> content.token_amount);
            break;
        case CHIP:
            AddChipNoteQueue(box -> content.id);
            AddChip(box -> content.id);
            break;
        case BYTE:
            AddByte(box -> content.id);
            break;
    }

    box -> open = 1;

    WriteSave(Freddy.position);
}

void HandleBoxCollisions(WORLDBox * boxes, uint16_t amount)
{
    for (uint16_t i = 0; i < amount; i++) 
    {
        if (boxes[i].open) continue;
        HandleSingleBoxCollision(boxes + i);
    }
}

static void HandleMineCollision_Ent(void)
{
    static Vector2 look_up_table[NUMBER_OF_MINES] = {   (Vector2) {29.15, 71.275},
                                                        (Vector2) {36.15, 61.275}};
    for (uint8_t i = 0; i < NUMBER_OF_MINES; i++)
    {
        if (CheckEntityCollision(&Freddy, Ent_MinesTeleporters + i))
        {
            Freddy.position = look_up_table[i];
            WorldCamera.position = look_up_table[i];
        }
    }

}

static void HandleMineCollision_Ex(void)
{
    static Vector2 look_up_table[NUMBER_OF_MINES] = {   (Vector2) {44.65, 33},
                                                        (Vector2) {8.65, 23}};
    for (uint8_t i = 0; i < NUMBER_OF_MINES; i++)
    {
        if (CheckEntityCollision(&Freddy, Ex_MinesTeleporters + i))
        {
            Freddy.position = look_up_table[i];
            WorldCamera.position = look_up_table[i];
        }
    }

}

static void HandleMineCollision(void)
{
    HandleMineCollision_Ent();
    HandleMineCollision_Ex();
}

void PutWorld(void)
{   
    UpdateMusicStream(CurrentTheme);
    UpdateFreddy();
    UpdateZoneAssets();
    RenderWorld();
    PutUIParticles();
    RenderZoneName();
    HandleWorldButtonCollision();
    HandleBoxCollisions(ChipBoxes, NUMBER_OF_CHIPS);
    HandleMineCollision();
    PutDefaultUI();
    RenderChipNoteBanner();
}