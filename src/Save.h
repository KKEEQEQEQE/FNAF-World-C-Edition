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

#include <stdint.h>
#include "../Include/raylib.h"

extern void CreateSave(const char * path);
extern void LoadSave(const char * path);
extern void WriteSave(Vector2 LastLocation);

extern Vector2 GetLast_Location(void);

extern uint8_t GetZone_Level(void);
extern void SetZone_Level(uint8_t level);
extern uint16_t GetFaz_Tokens(void);
extern void UpdateFaz_Tokens(int16_t income);

typedef struct Animatronic
{
    uint8_t id, level; 
    uint32_t xp;
} Animatronic;

extern void AddAnimatronic(uint8_t id);
extern void UpdateAnimatronic(uint8_t index, uint16_t level_surplus, uint32_t xp_surplus);
extern Animatronic GetAnimatronic(uint8_t index);

extern void UpdateParty_1(uint8_t index, uint8_t id);
extern void UpdateParty_2(uint8_t index, uint8_t id);

extern void GetChipInv(uint8_t (*dest) [21]);
extern void AddChip(uint8_t id);
extern void UpdateSelected_Chips(uint8_t index, uint8_t id);

extern void AddByte(uint8_t id);
extern void UpdateSelected_Bytes(uint8_t index, uint8_t id);