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

typedef struct _RABITS_Attack_Format
{
    enum ATTACK_SUPPORTED_TYPES type;
    union
    {
        _RABITS_Bite bite;
    };
    uint32_t lifetime;
} _RABITS_Attack_Format;