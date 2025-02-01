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

//   R. udimentary
//   A. ttack
//   B. attle
//   I. nterface
//   T. yped
//   S. ystem

// I needed an acronym because the name was too long

#pragma once

#include <stdint.h>
#include "Battle_Rework.h"

enum ALL_ATTACKS
{
    NONE, 
    HIT, // Any Enemy
    MICTOSS, PIZZAWHEEL, BIRTHDAY, // Freddy
    NUMBER_OF_ATTACKS
};

typedef struct _RABITS_Bite
{
    enum ALL_ATTACKS ID;
    int target: 4;
    int level: 12;
} _RABITS_Bite;

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

typedef struct _RABITS_Attack_Format
{
    enum ATTACK_SUPPORTED_TYPES type;
    union
    {
        _RABITS_Bite bite;
    };
    uint32_t lifetime;
} _RABITS_Attack_Format;