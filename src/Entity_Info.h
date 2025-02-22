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

#include <stdbool.h>
#include "Battle_Rework.h"
#include "RABIT.h"
#include "UI.h"

extern Vector2 EntitySize[NUMBER_OF_ENTITY_IDS];

#define MAX_ATTACKS 3

extern enum ALL_ATTACKS EntityAttacks[NUMBER_OF_ENTITY_IDS][MAX_ATTACKS];

extern uint16_t EntityBaseHealth[NUMBER_OF_ENTITY_IDS];

uint16_t GetEntityLevel(enum ENTITY_IDs id);

uint16_t GetEntityFullHealth(enum ENTITY_IDs id);

_Bool EntityHasAttackAnimation(enum ENTITY_IDs ID);

void LoadEntityVisuals(enum ENTITY_IDs ID);

Animation_V2 AccessEntityAnimation(enum ENTITY_IDs ID, _Bool is_attacking);

void FreeEntityVisuals(enum ENTITY_IDs ID);

void FlushEntityVisuals(void);

char * GetEntityName(enum ENTITY_IDs ID);