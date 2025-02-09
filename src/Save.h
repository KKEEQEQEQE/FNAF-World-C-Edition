/*
    Zlib License

    Copyright (c) 2024 SpyterDev

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from theuse of this software.

    Permission is granted to anyone touse this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If youuse this software
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.
    2. Altered sourceversions must be plainly marked as such, and must not be
        misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <stdint.h>
#include "Battle_Rework.h"
#include "../Include/raylib.h"

void CreateSave(const char * path);
void LoadSave(const char * path);
void WriteSave(Vector2 LastLocation);

 Vector2 GetLast_Location(void);

uint8_t GetZone_Level(void);
void SetZone_Level(uint8_t level);
uint16_t GetFaz_Tokens(void);
void UpdateFaz_Tokens(int16_t income);

typedef struct Animatronic
{
    uint8_t id, level; 
    uint32_t xp;
} Animatronic;

void AddAnimatronic(uint8_t id);
void UpdateAnimatronic(uint8_t index, uint16_t level_surplus, uint32_t xp_surplus);
//Animatronic GetAnimatronic(uint8_t index);
//Animatronic GetAnimatronicById(enum ENTITY_IDs id);

void UpdateParty_1(uint8_t index, uint8_t id);
void UpdateParty_2(uint8_t index, uint8_t id);

void GetChipInv(uint8_t (*dest) [21]);
void AddChip(uint8_t id);
void UpdateSelected_Chips(uint8_t index, uint8_t id);

void AddByte(uint8_t id);
void UpdateSelected_Bytes(uint8_t index, uint8_t id);