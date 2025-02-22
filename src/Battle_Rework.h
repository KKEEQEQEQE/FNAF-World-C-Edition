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
    FREDBEAR=NUMBER_OF_ENEMIES+39,
    NUMBER_OF_ENTITY_IDS
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