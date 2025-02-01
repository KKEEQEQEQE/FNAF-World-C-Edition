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


#include "Animation.h"
#include "RABIT.h"
#include <stdint.h>
#include <time.h>

typedef struct _Damage_Range
{
    uint32_t min, max;
} _Damage_Range;

typedef struct _Battle_Projectile 
{
    clock_t max_lifetime;
    _Damage_Range damage;
    Vector2 velocity;
    Rectangle hitbox;
    Animation_V2 sprite;
} _Battle_Projectile;

enum ATTACK_SUPPORTED_TYPES
{
    SINGLE_TARGET_BITE, MULTI_TARGET_BITE, // Bites or hitting
    SINGLE_PROJECTILE, MULTI_PROJECTILES, // Projectiles
    HEAL, SIPHON, // Healing and taking health from target(s)
    AOE, // Area of Effect
    TOXIC_BITE, TOXIC_SINGLE_PROJECTILE, TOXIC_MULTI_PROJECTILES, TOXIC_AOE, // Toxic Supersets
    BUFF, // Buffs or Debuffs
    SUMMON, // Summons Entities to fight for X amount of time
    RELAY, // Pushes a new type of attack on execution (can be used for Prize Ball)
    INSTAKILL, // Puts UIVisual at X,Y and runs instakill type update after X secs
    ATTACK_CUSTOM /* DIRECT access to the Battle system to modify and update ANY values 
                     (DO NOT USE THERE IS NO WAY TO IMPLEMENT AN ATTACK, 
                     THIS IS CAN BE SKECTCHY AS FREDBEAR)
                     YOU HAVE BEEN WARNED >:((((                                         */
};  

enum ENTITY_IDs
{
    UNKNOWN,
    BOUNCEPOT,
    GEARRAT,
    MECHRAB,
    NUMBER_OF_ENEMIES,
    FREDDY,
    BONNIE,
    CHICA,
    FOXY,
    NUMBER_OF_ENEMY_IDS
};

typedef struct _BattleEntity 
{
    uint32_t remaining_health, level; // Remaining health and level
    enum ENTITY_IDs ID; // Animatronic or Enemy ID
    _Bool is_attacking; // Whether or not to render attack animation or send an attack queue
    Rectangle hitbox; // Position and size in Battle space
} _BattleEntity;

#define MAX_PARTY_MEMBERS 4

typedef struct _BattleParty
{
    uint8_t size;
    _BattleEntity member[MAX_PARTY_MEMBERS];
} _BattleParty;

extern void InitBattle(void);
extern void DealDamage(uint32_t amount, uint8_t target);
extern void UninitBattle(void);
extern void PutBattle(void);