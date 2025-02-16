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

#include "../Include/cJSON.h"
#include <stdbool.h>
#include <stdint.h>
#include "Battle_Rework.h"
#include "Entity_Info.h"

#define MAX_LINES 255

_Bool contains_phase_2 = false;
uint8_t number_of_lines = 0;
char line_terminator = '\0';
char * lines[MAX_LINES] = {NULL};

// The Entity ID referenced for drawing/rendering the 1st entity (speaker) in Phase 1 Dialogue
enum ENTITY_IDs id_E1P1 = 0;

// The Entity ID referenced for drawing/rendering the 1st entity (speaker) in Phase 2 Dialogue
enum ENTITY_IDs id_E1P2 = 0;

// The Entity ID referenced for drawing/rendering the 2st entity (listener), usually Freddy
enum ENTITY_IDs id_E2 = 1;

enum ENTITY_IDs DialogueIdToEntityId(uint8_t id)
{
    return id ? id + NUMBER_OF_ENEMY_IDS - 1 : UNKNOWN;
}

void LoadDialogue(const char * path)
{

}