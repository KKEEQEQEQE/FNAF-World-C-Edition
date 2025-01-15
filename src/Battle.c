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
#include "Game_State.h"
#include "Particle.h"
#include "UI.h"
#include "Battle.h"
#include "World.h"
#include <malloc.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define BATTLE_PIXEL_SCALE (66.6666666667)

#define BATTLE_POSITION_TO_PIXEL_X(x) (BATTLE_PIXEL_SCALE * (x + 854. / BATTLE_PIXEL_SCALE / 2))

#define BATTLE_POSITION_TO_PIXEL_Y(y) (BATTLE_PIXEL_SCALE * (y + 480. / BATTLE_PIXEL_SCALE / 2))

Font Battle_Font = {0};
Music theme = {0};

UITexture BattleBackground = {0};

_BattleParty Party_Enemy = {0};
_BattleParty Party_Player = {0};

uint8_t particle_test = 0;

enum ENTITY_POSITIONS_FIELD 
{
    L_U,
    L_L,
    L_R,
    L_D,
    R_U,
    R_L,
    R_R,
    R_D

};

void SetBattleEntity_Position(_BattleEntity * dest, enum ENTITY_POSITIONS_FIELD position)
{
    switch (position) {
        case L_U:
            dest -> hitbox.x = -3.5;
            dest -> hitbox.y = -3;
            return;
        case L_L:
            dest -> hitbox.x = -5.5;
            dest -> hitbox.y = -2;
            return;
        case L_R:
            dest -> hitbox.x = -2.5;
            dest -> hitbox.y = -1;
            return;
        case L_D:
            dest -> hitbox.x = -4.5;
            dest -> hitbox.y = 0;
            return;
        case R_U:
            dest -> hitbox.x = 2;
            dest -> hitbox.y = -3;
            return;
        case R_L:
            dest -> hitbox.x = 4;
            dest -> hitbox.y = -2;
            return;
        case R_R:
            dest -> hitbox.x = 1;
            dest -> hitbox.y = -1;
            return;
        case R_D:
            dest -> hitbox.x = 3;
            dest -> hitbox.y = 0;
            return;
    }
}

void LoadEntity(_BattleEntity * dest, _BattleEntity entity, enum ENTITY_POSITIONS_FIELD position)
{
    *dest = entity;

    SetBattleEntity_Position(dest, position);
}

// Turns a Unsigned Long into a Stack Allocated String
static char * __fastcall uintstr(char * dest, uint8_t len, uint64_t number) 
{
    uint64_t temp = number;
    uint64_t length = 1;
    while (temp > 9) temp /= 10, length++; // Checks number of digits and length of string
    if (length > len) length = len;
    temp = 0;
    while (temp < length) dest[length-temp-1] = '0' + number - (number / 10) * 10, number/=10, temp++;
    dest[temp] = '\0';
    char * stringLocation = dest;
    return stringLocation;
}

#define ENTITY_MACRO_BOUNCEPOT (_BattleEntity) {.name = "Bouncepot", \
                                                .full_health = 30, .remaining_health = 30, \
                                                .hitbox = (Rectangle) {0, 0, 1.75, 2.75}, \
                                                .sprite_idle = CreateAnimation_V2("Assets/Battle/Entity_Sprites/Bouncepot/atlas.png", 30, 10, 250, 250), \
                                                .attacks = {{.type=HIT, .damage={8, 12}, .delay=0}}, \
                                                .num_of_attacks = 1}

#define ENTITY_MACRO_GEARRAT (_BattleEntity) {.name = "Gearrat", \
                                                .full_health = 30, .remaining_health = 30, \
                                                .hitbox = (Rectangle) {0, 0, 1.75, 2.75}, \
                                                .sprite_idle = CreateAnimation_V2("Assets/Battle/Entity_Sprites/Gearrat/atlas.png", 30, 10, 200, 200), \
                                                .attacks = {{HIT, {8, 12}}}, \
                                                .num_of_attacks = 1}

#define ENTITY_MACRO_MECHRAB (_BattleEntity) {.name = "Mechrab", \
                                                .full_health = 30, .remaining_health = 30, \
                                                .hitbox = (Rectangle) {0, 0, 1.75, 2.75}, \
                                                .sprite_idle = CreateAnimation_V2("Assets/Battle/Entity_Sprites/Mechrab/atlas.png", 30, 10, 200, 200), \
                                                .attacks = {(_Attack){.damage={8, 12}, .type=HIT, .delay=CLOCKS_PER_SEC}}, \
                                                .num_of_attacks = 1}

#define ENTITY_MACRO_FREDDY  (_BattleEntity) { .name = "Freddy", .full_health = 100, .remaining_health = 100, .hitbox = (Rectangle) {0, 0, 1.75, 2.75}, .sprite_idle = CreateAnimation_V2("Assets/Battle/Entity_Sprites/Freddy/idle.png", 30, 10, 250, 250)}

#define ENTITY_MACRO_BONNIE  (_BattleEntity) { .name = "Bonnie", .full_health = 100, .remaining_health = 100, .hitbox = (Rectangle) {0, 0, 1.75, 2.75}, .sprite_idle = CreateAnimation_V2("Assets/Battle/Entity_Sprites/Bonnie/idle.png", 30, 10, 250, 250)}

enum ENTITY_IDs
{
    UNKNOWN,
    FREDDY,
    BONNIE,
    CHICA,
    FOXY,
    BOUNCEPOT,
    GEARRAT,
    MECHRAB
};

_BattleEntity GetBattleEntity(enum ENTITY_IDs id)
{
    _BattleEntity entity;
    switch (id) {
        case UNKNOWN:
            return entity = ENTITY_MACRO_FREDDY;
        case FREDDY:
            return entity = ENTITY_MACRO_FREDDY;
        case BONNIE:
            return entity = ENTITY_MACRO_BONNIE;
        case CHICA:
        case FOXY:
        case BOUNCEPOT:
            return ENTITY_MACRO_BOUNCEPOT;
        case GEARRAT:
            return ENTITY_MACRO_GEARRAT;
        case MECHRAB:
            return ENTITY_MACRO_MECHRAB;
    }
}
uint8_t damage_particles[5] = {0};

#define ATTACK_COOLDOWN (5 * CLOCKS_PER_SEC)

Animation_V2 grave = {0};

void InitBattle(void)
{
    SetWindowTitle("FNaF World: C Edition - Battle");
    
    SetTraceLogLevel(LOG_NONE);
    particle_test = CreateParticleIndexA_V2("Assets/Particles/bird.png", 30, 10, (Vector2) {50, 50}, 1.5);;
    switch ((enum WORLDZONES) (GetZone() + 1))
    {
        case FAZBEARHILLS:
            BattleBackground = LoadTexture("Assets/Battle/Background/Fazbear_Hills.png");
            break;
        case CHOPPYSWOODS:
            BattleBackground = LoadTexture("Assets/Battle/Background/Choppys_Woods.png");;
            break;
        case DUSTINGFIELDS:
            BattleBackground = LoadTexture("Assets/Battle/Background/Dusting_Fields.png");
            break;
        case MYSTERIOUSMINES:
            BattleBackground = LoadTexture("Assets/Battle/Background/Mines.png");;
            break;
    }

    SetTextureFilter(BattleBackground, TEXTURE_FILTER_BILINEAR);

    Party_Enemy.size = 4;

    LoadEntity(Party_Enemy.member, GetBattleEntity(MECHRAB), L_U);
    LoadEntity(Party_Enemy.member + 1, GetBattleEntity(MECHRAB), L_L);
    LoadEntity(Party_Enemy.member + 2, GetBattleEntity(MECHRAB), L_R);
    LoadEntity(Party_Enemy.member + 3, GetBattleEntity(MECHRAB), L_D);

    Party_Player.size = 4;
    
    LoadEntity(Party_Player.member, GetBattleEntity(FREDDY), R_U);
    LoadEntity(Party_Player.member + 1, GetBattleEntity(FREDDY), R_L);
    LoadEntity(Party_Player.member + 2, GetBattleEntity(FREDDY), R_R);
    LoadEntity(Party_Player.member + 3, GetBattleEntity(FREDDY), R_D);
    
    for (uint8_t i = 0; i < Party_Enemy.size; i++)
    {
        Party_Enemy.member[i].last_attack = clock() + (i * 0.5 * CLOCKS_PER_SEC);
    }

    for (uint8_t i = 0; i < Party_Player.size; i++)
    {
        Party_Player.member[i].last_attack = clock() + (i * 0.5 * CLOCKS_PER_SEC);
    }

    theme = LoadMusicStream("Assets/Themes/battle_normal.wav");
    theme.looping = 1;
    
    Battle_Font = LoadFont("Assets/Battle/font.ttf");
    SetTextureFilter(Battle_Font.texture, TEXTURE_FILTER_BILINEAR);

    UITexture damage_particle_atlas = LoadTexture("Assets/Particles/damage.png");

    SetTextureFilter(damage_particle_atlas, TEXTURE_FILTER_BILINEAR);
    
    damage_particles[0] = CreateParticleIndexT_Snippet(damage_particle_atlas, (Rectangle) {0, 0, 40, 40}, 1.5);
    damage_particles[1] = CreateParticleIndexT_Snippet(damage_particle_atlas, (Rectangle) {40, 0, 40, 40}, 1.5);
    damage_particles[2] = CreateParticleIndexT_Snippet(damage_particle_atlas, (Rectangle) {80, 0, 40, 40}, 1.5);
    damage_particles[3] = CreateParticleIndexT_Snippet(damage_particle_atlas, (Rectangle) {120, 0, 40, 40}, 1.5);
    damage_particles[4] = CreateParticleIndexT_Snippet(damage_particle_atlas, (Rectangle) {160, 0, 40, 40}, 1.5);

    PlayMusicStream(theme);
    
    SetTraceLogLevel(LOG_ALL);
    grave = CreateAnimation_V2("Assets/Battle/Entity_Sprites/grave.png", 1, 1, 112, 140);
}

enum ATTACK_MOVE_INDEX
{
    FIRST = 0, 
    SECOND = 1, 
    THIRD = 2, 
    RANDOM = 3
};

typedef struct _AttackQueue 
{
    _Bool imminent;
    clock_t start_time;
    clock_t delay;
    _Attack attack;
    _BattleEntity * target;
    _BattleEntity * source;
} _AttackQueue;

#define MAX_ATTACKS_IN_QUEUE 15
#define ATTACK_QUEUE_FREE target = NULL
_AttackQueue attack_queue[MAX_ATTACKS_IN_QUEUE] = {0};

static uint8_t GetAvaliable_attack_queue(void)
{
    uint8_t i = 0;
    for (; i < MAX_ATTACKS_IN_QUEUE; i++)
    {
        if (!attack_queue[i].target) break;
    }
    return i;
}

static uint8_t GetUnavaliable_attack_queue(void)
{
    uint8_t i = 0;
    for (; i < MAX_ATTACKS_IN_QUEUE; i++)
    {
        if (attack_queue[i].target) break;
    }
    return i;
}

Vector2 BattleSpaceToUiSpace(Vector2 position)
{
    float scale = GetScreenRatio() <= RATIO_16_9 ? GetScreenWidth() / 854. : GetScreenHeight() / 480.;

    Vector2 Vscreen_offset = (Vector2) {GetScreenWidth() / 2. - 427. * scale, 
                                        GetScreenHeight() / 2. - 240. * scale };

    Vector2 pixel_position = (Vector2) { scale * BATTLE_POSITION_TO_PIXEL_X(position.x) + Vscreen_offset.x, 
                                         scale * BATTLE_POSITION_TO_PIXEL_Y(position.y) + Vscreen_offset.y };
    
    return (Vector2) {  (pixel_position.x / (float) GetScreenWidth()) * 2 - 1,
                        (pixel_position.y / (float) GetScreenHeight()) * 2 - 1  };
}

#include "Particle_Updaters.h"

void CreateDamageEffect(Vector2 position)
{
    float scale = GetScreenRatio() <= RATIO_16_9 ? GetScreenWidth() / 854. : GetScreenHeight() / 480.;

    Vector2 Vscreen_offset = (Vector2) {(GetScreenWidth() - 854 * scale) / 2, (GetScreenHeight() - 480 * scale) / 2};
    Vector2 start_position = BattleSpaceToUiSpace(position);
    uint8_t num_of_particles = GetRandomValue(4, 8);

    for (uint8_t i = 0; i < num_of_particles; i++)
    {
        Vector2 random_offset = (Vector2) { GetRandomValue(-10000, 10000) / 1e5, 
                                            GetRandomValue(-10000, 10000) / 1e5};
        
        CreateParticleEx(   damage_particles[i % 5], 
                            start_position.x + random_offset.x, start_position.y + random_offset.y, 
                            GetRandomValue(-10000, 10000) / 2e4, 1,
                            480,
                            Updater_DeleteAfterQuarterSecond);
    }
}

#define ATTACK_PARAMETERS _BattleParty * target_party, _BattleEntity * source, _Attack attack

void RunAttackQueue(_AttackQueue * attack)
{
    if (!attack || 
        !attack -> target) return;

    if (!attack -> imminent && 
        attack -> source -> remaining_health == 0)
        {
            attack -> ATTACK_QUEUE_FREE;
            return;
        }
    
    Rectangle hitbox = attack->target->hitbox;

    hitbox.x += hitbox.width / 2;
    hitbox.y += hitbox.height / 2;
    
    switch (attack -> attack.type)
    {
        case NONE:
            CreateDamageEffect((Vector2) {hitbox.x, hitbox.y});
            attack->target->remaining_health -= GetRandomValue(attack->attack.damage.min, attack->attack.damage.max);
            break;
        case HIT:
            CreateDamageEffect((Vector2) {hitbox.x, hitbox.y});
            attack->target->remaining_health -= GetRandomValue(attack->attack.damage.min, attack->attack.damage.max);
            printf("Hi");
            break;
    }

    if (attack -> target -> remaining_health > attack -> target -> full_health) 
    {
        attack -> target -> remaining_health = 0;
    }

    attack -> ATTACK_QUEUE_FREE;
}

void UpdateAttackQueue(void)
{
    for (uint8_t i = 0; i < MAX_ATTACKS_IN_QUEUE; i++)
    {
        if (!attack_queue[i].start_time || 
            clock() < attack_queue[i].delay + attack_queue[i].start_time) continue;

        RunAttackQueue(&attack_queue[i]);
    }
}
void Print_AttackQueue_struct(_AttackQueue * queue)
{
    printf("{\n\timminent = %u,\n\tattack %u,\n\ttarget = %p,\n\tsource = %p\n}\n", queue->imminent, queue -> attack.type, queue -> target, queue->source);
}

void Print_All_Attack_Queue(void)
{
    printf("\n\n");

    for (uint8_t i = 0; i < MAX_ATTACKS_IN_QUEUE; i++)
    {
        if (!attack_queue[i].target) continue;
        printf("ID %u:\n", i);
        Print_AttackQueue_struct(attack_queue + i);
    }
}
static void _BattleEntity_Attack_Push(_BattleParty *target_party, _BattleEntity *source, _Attack attack)
{
    if (!target_party) return;

    uint8_t array_size = target_party -> size;

    uint8_t potental_targets[array_size];
    memset(potental_targets, 0, array_size);

    uint8_t num_of_potental_targets = 0;

    for (uint8_t attempt_index = 0; attempt_index < target_party -> size; attempt_index++)
    {
        if (!target_party -> member[attempt_index].remaining_health) continue;

        potental_targets[num_of_potental_targets] = attempt_index;

        num_of_potental_targets++;
    }

    if (num_of_potental_targets == 0) return;
    
    uint8_t target_id = GetRandomValue(0, num_of_potental_targets - 1);
    target_id = potental_targets[target_id];
    
    uint8_t queue_id = GetAvaliable_attack_queue();

    if (queue_id == MAX_ATTACKS_IN_QUEUE) return;

    attack_queue[queue_id] = (_AttackQueue) {
        .attack = attack,
        .imminent = 1,
        .source = source,
        .target = target_party -> member + target_id,
        .start_time = clock(),
        .delay = attack.delay
    };
}

void _BattleEntity_Attack(uint8_t id, enum ATTACK_MOVE_INDEX attack_index)
{
    _BattleParty * target_party = id < Party_Player.size ? &Party_Enemy : &Party_Player;

    _BattleEntity * source = id < Party_Player.size ? Party_Player. member + id : 
                                                      Party_Enemy.member + id - Party_Player.size;

    if (source -> last_attack < ATTACK_COOLDOWN) return;

    if (attack_index == RANDOM) attack_index = GetRandomValue(FIRST, THIRD);
    
    _Attack attack = source -> attacks[attack_index - FIRST];

    _BattleEntity_Attack_Push(target_party, source, attack);
}

void UninitBattle(void)
{
    UnloadTexture(BattleBackground);
    UnloadFont(Battle_Font);
}

void RenderBattleEntity_Hitbox(register _BattleEntity * entity)
{
    DrawRectangle(  BATTLE_POSITION_TO_PIXEL_X(entity -> hitbox.x), 
                    BATTLE_POSITION_TO_PIXEL_Y(entity -> hitbox.y), 
                    BATTLE_PIXEL_SCALE * entity -> hitbox.width, 
                    BATTLE_PIXEL_SCALE * entity -> hitbox.height, BLUE);
}

void RenderBattleEntity(register _BattleEntity * entity, _Bool is_player)
{
    static float entity_scale = 1;
    Vector2 position = (Vector2) {  BATTLE_POSITION_TO_PIXEL_X(entity -> hitbox.x),
                                    BATTLE_POSITION_TO_PIXEL_Y(entity -> hitbox.y)};


    
    #ifdef DEBUG
    
    RenderBattleEntity_Hitbox(entity);

    #endif
 
    if (!entity -> remaining_health && is_player)
    {
        DrawAnimation_V2(   &grave, 
                        position.x + (entity -> hitbox.width * BATTLE_PIXEL_SCALE / 2.) - grave.TileSize_x / 2.f * entity_scale, 
                        position.y + (entity -> hitbox.height * BATTLE_PIXEL_SCALE / 2.) - grave.TileSize_y / 2.f * entity_scale, entity_scale, 0);
        return;
    } else if (!entity -> remaining_health) return;

    DrawAnimation_V2(   &entity -> sprite_idle, 
                        position.x + (entity -> hitbox.width * BATTLE_PIXEL_SCALE / 2.) - entity -> sprite_idle.TileSize_x / 2.f * entity_scale, 
                        position.y + (entity -> hitbox.height * BATTLE_PIXEL_SCALE / 2.) - entity -> sprite_idle.TileSize_y / 2.f * entity_scale, entity_scale, 0);
}

void RenderBattle(void)
{
    SetTextureFilter(BattleBackground, TEXTURE_FILTER_BILINEAR);
    RenderBackground(BattleBackground);

    static RenderTexture2D virtual_screen = {0};

    if (virtual_screen.texture.height == 0) virtual_screen = LoadRenderTexture(854, 480);

    BeginTextureMode(virtual_screen);
    ClearBackground(BLANK);
    for (uint8_t i = 0; i < Party_Enemy.size; i++)
    {
        RenderBattleEntity(Party_Enemy.member + i, 0);
    }

    for (uint8_t i = 0; i < Party_Player.size; i++)
    {
        RenderBattleEntity(Party_Player.member + i, 1);
    }

    EndTextureMode();

    float scale = GetScreenRatio() <= RATIO_16_9 ? GetScreenWidth() / 854. : GetScreenHeight() / 480.;
    
    SetTextureFilter(virtual_screen.texture, TEXTURE_FILTER_BILINEAR);
    
    DrawTexturePro( virtual_screen.texture, 
                    (Rectangle) {0, -virtual_screen.texture.height, (float) virtual_screen.texture.width, (float) -virtual_screen.texture.height}, 
                    (Rectangle) {   GetScreenWidth() / 2. - virtual_screen.texture.width / 2. * scale, 
                                        GetScreenHeight() / 2. - virtual_screen.texture.height / 2. * scale, 
                                        virtual_screen.texture.width * scale, virtual_screen.texture.height * scale},
                    (Vector2) {0,0},
                    0,
                    WHITE);
    SetUIScreenScaleMode(HEIGHT);
}

void RenderHealthBar(_BattleEntity * entity, Color colour, uint8_t id, float scale)
{
    static float border_facter = 0.025;
    static UITexture background = {0};

    if (!background.width) 
    {
        background = LoadTexture("Assets/Battle/UI/Health_Bar.png");
        SetTextureFilter(background, TEXTURE_FILTER_BILINEAR);
    }
    Rectangle dest = (Rectangle) {  GetScreenWidth() - background.width * scale - 10 * scale, 
                                    background.height * id * scale + 10 * scale, 
                                    background.width * scale, background.height * scale     };

    Rectangle health_back_rec = (Rectangle) {   dest.x + dest.width * border_facter, 
                                                dest.y + dest.width * border_facter, 
                                                dest.width * (1-border_facter*2), 
                                                dest.height - dest.width * (border_facter * 2)};
    Rectangle health_rec = (Rectangle) {dest.x + dest.width * border_facter, dest.y + dest.width * border_facter, dest.width * (1-border_facter*2), dest.height - dest.width * (border_facter * 2)};
    
    DrawRectangleRec(health_rec, (Color){0,0,0,100});

    health_rec.width *= (float) entity->remaining_health / entity->full_health;
    
    DrawRectangleRec(health_rec, colour);
    
    DrawTextureEx(background, (Vector2){dest.x, dest.y}, 0, scale, WHITE);
    char number[5];
    uintstr(number, 5, entity->remaining_health);
    

    DrawTextPro( (Font){0}, 
                number, 
                (Vector2) { health_back_rec.x + health_back_rec.width, health_back_rec.y + health_back_rec.height / 2.}, 
                (Vector2){MeasureText("100", 30), 30/2.}, 
                0, 30, 1, WHITE);
}

float enemy_speed = 1;

void UpdateEnemyParty(void)
{
    for (uint8_t i = 0; i < Party_Enemy.size; i++)
    {
        if (clock() - Party_Enemy.member[i].last_attack < (1 / enemy_speed) * ATTACK_COOLDOWN
            || !Party_Enemy.member[i].num_of_attacks) continue;
        
        int8_t attack_id = GetRandomValue(0, Party_Enemy.member[i].num_of_attacks - 1);

        _BattleEntity_Attack_Push(&Party_Player, Party_Enemy.member + i, Party_Enemy.member[i].attacks[attack_id]);
        Party_Enemy.member[i].last_attack = clock();
    }
}

_Bool GetGameOver(void)
{
    uint8_t i = 0;
    for (; i < Party_Player.size; i++)
    {
        if (Party_Player.member[i].remaining_health) break;
    }
    return (i == Party_Player.size);
}

#define NO_ATTACK_ANIMATION_ANIMATION_LENGTH 1

float GetBattleEntityAnimationPercentage(_BattleEntity * entity)
{
    _Bool has_attack_animation = entity -> sprite_attack.Amount > 0;

    clock_t animation_frame_len = has_attack_animation ? 
                                    CLOCKS_PER_SEC * (1 / entity -> sprite_attack.FPS) :
                                    NO_ATTACK_ANIMATION_ANIMATION_LENGTH * CLOCKS_PER_SEC;   

    clock_t animation_len = has_attack_animation ? animation_frame_len * entity -> sprite_attack.Amount :
                                animation_frame_len * 1;
    return (float) (clock() - entity -> last_attack) / animation_len;
}

_Bool IsBattleEntityInAttackAnimation(_BattleEntity * entity)
{
    return GetBattleEntityAnimationPercentage(entity) <= 1;
}

Vector2 GetNoAnimationPosition(float percentage)
{
    float unit = (cosf(PI * percentage + PI) + 1) / 2;

    return (Vector2) {2 * unit, 1 * unit};
}

void UpdateBattleParty_Animations(_BattleParty * target_party)
{
    
    for (uint8_t i = 0; i < target_party -> size; i++)
    {
        _Bool has_attack_animation = target_party -> member[i].sprite_attack.Amount > 0;

        if (has_attack_animation) continue;

        if (!IsBattleEntityInAttackAnimation(target_party -> member + i)) continue;

        target_party -> member[i].hitbox.x;
        target_party -> member[i].hitbox.y;
    }
}

void PutBattle(void)
{
    UpdateMusicStream(theme);

    UpdateAttackQueue();

    UpdateEnemyParty();

    RenderBattle();
    if (IsKeyPressed(KEY_S)) _BattleEntity_Attack_Push(&Party_Enemy, NULL, (_Attack){.type = HIT});
    if (IsKeyPressed(KEY_W))
    {
        attack_queue[GetUnavaliable_attack_queue()].target = NULL;
    }

    RenderUIText(Party_Enemy.member[0].name, -0.945, -0.795, 0.06, LEFTMOST, Battle_Font, BLACK);
    RenderUIText(Party_Enemy.member[0].name, -0.95, -0.8, 0.06, LEFTMOST, Battle_Font, WHITE);
    if (IsKeyPressed(KEY_SPACE)) CreateDamageEffect((Vector2) {0,0});
    PutUIParticles();

    for (uint8_t i = 0; i < Party_Player.size; i++)
    {
        RenderHealthBar(Party_Player.member + i, RED, i, GetScreenScale() * 1.5);
    }

    if (GetGameOver()) SwapGameState(Title);
}