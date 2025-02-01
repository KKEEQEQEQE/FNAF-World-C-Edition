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

// Whoever made this page in the FNaF World Wiki is a W person
// https://freddy-fazbears-pizza.fandom.com/wiki/Battle_Mechanics_(FW)

#include "Entity_Info.h"
#include "Animation.h"
#include "Battle_Rework.h"
#include "Save.h"
#include "UI.h"
#include <stdint.h>
#include <string.h>

// TODO, change small and large size :|

#define SMALL_SIZE (Vector2) {1.75, 2.75}
#define MEDIUM_SIZE (Vector2) {1.75, 2.75}
#define LARGE_SIZE (Vector2) {1.75, 2.75}

// A lookup table for the hitbox sizes of each ENTITY_ID in Battle Space
Vector2 EntitySize[NUMBER_OF_ENEMY_IDS] = 
{   
    [BOUNCEPOT]=MEDIUM_SIZE, 
    [GEARRAT]=SMALL_SIZE,
    [MECHRAB]=SMALL_SIZE,
    [FREDDY]=MEDIUM_SIZE,
    [BONNIE]=MEDIUM_SIZE,
    [CHICA]=MEDIUM_SIZE,
    [FOXY]=MEDIUM_SIZE  
};

// A lookup table for the attacks of each ENTITY_ID in Battle Space
enum ATTACKS_IDs EntityAttacks[NUMBER_OF_ENEMY_IDS][MAX_ATTACKS] = 
{ 
    [BOUNCEPOT]={HIT}, 
    [GEARRAT]={HIT},
    [MECHRAB]={HIT},
    [FREDDY]={MICTOSS, PIZZAWHEEL, BIRTHDAY},
    [BONNIE]={},
    [CHICA]={},
    [FOXY]={}  
};

uint16_t EntityBaseHealth[NUMBER_OF_ENEMY_IDS] =
{
    [BOUNCEPOT]=15,
    [GEARRAT]=55,
    [MECHRAB]=30,
    [FREDDY]=95,
    [BONNIE]=105,
    [CHICA]=110,
    [FOXY]=115
};

uint16_t GetEntityLevel(enum ENTITY_IDs id)
{
    return id > NUMBER_OF_ENEMIES ? GetAnimatronicById(id).level : 0;
}

uint16_t GetEntityFullHealth(enum ENTITY_IDs id)
{
    return EntityBaseHealth[id] + 5 * GetEntityLevel(id);
}

typedef struct _EntityVisualIndex
{
    _Bool in_use;
    Animation_V2 idle;
    Animation_V2 attack;
} _EntityVisualIndex; 

_EntityVisualIndex EntityVisual[NUMBER_OF_ENEMY_IDS] = {0};

void LoadEntityVisuals(enum ENTITY_IDs ID)
{
    if (EntityVisual[ID].in_use) return;

    // I know I could just make it use the entity ID as the folder
    // or just make it access a string array with a json info file 
    // but idc
    // SUE MEEEEEE >:)

    switch (ID) 
    {
        // This is why spring is a scary time
        case BOUNCEPOT:
        {
            EntityVisual[BOUNCEPOT].idle = CreateAnimation_V2( "Assets/Battle/Entity_Sprites/Bouncepot/atlas.png", 
                                                               30, 
                                                               10, 
                                                               250, 250);
            break;
        }

        // 7FT RAAT
        case GEARRAT:
        {
            EntityVisual[GEARRAT].idle = CreateAnimation_V2( "Assets/Battle/Entity_Sprites/Gearrat/atlas.png", 
                                                             30, 
                                                             10, 
                                                             200, 200);
            break;
        }

        // I like the blue one better :(
        case MECHRAB:
        {
            EntityVisual[MECHRAB].idle = CreateAnimation_V2( "Assets/Battle/Entity_Sprites/Mechrab/atlas.png", 
                                                              30, 
                                                              10, 
                                                              200, 200);
            break;
        }

        // Is ThAt FrEdDy FaZbEaR HaR HaR HaR HaR
        case FREDDY:
        {
            EntityVisual[FREDDY].idle = CreateAnimation_V2( "Assets/Battle/Entity_Sprites/Freddy/idle.png", 
                                                            30, 
                                                            10, 
                                                            250, 250);
            break;
        }
        case UNKNOWN:
        default:
        {
            // OH NOES WHAT IS THIS THING!!!!!!
            // WHAT IS IT AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
            
            EntityVisual[ID].idle = CreateAnimation_V2( "OhNoes...png", 
                                                        30, 
                                                        1, 
                                                        250, 250);
            break;
        }
            
    }

    EntityVisual[ID].in_use = true;
}

_Bool EntityHasAttackAnimation(enum ENTITY_IDs ID)
{
    return EntityVisual[ID].attack.Amount;
}

Animation_V2 AccessEntityAnimation(enum ENTITY_IDs ID, _Bool is_attacking)
{
    _EntityVisualIndex * target = EntityVisual + ID;

    if (!target -> in_use) LoadEntityVisuals(ID);

    return (is_attacking || target -> attack.Amount) ? 
                target -> attack : target -> idle;
}

void FreeEntityVisuals(enum ENTITY_IDs ID)
{
    if (!EntityVisual[ID].in_use) return;

    if (IsTextureValid(EntityVisual[ID].idle.Atlas)) UnloadTexture(EntityVisual[ID].idle.Atlas);
    if (IsTextureValid(EntityVisual[ID].attack.Atlas)) UnloadTexture(EntityVisual[ID].attack.Atlas);
    
    EntityVisual[ID].idle.Amount = 0;
    EntityVisual[ID].attack.Amount = 0;
    EntityVisual[ID].in_use = false;
}

void FlushEntityVisuals(void)
{
    for (enum ENTITY_IDs entity = UNKNOWN; entity < NUMBER_OF_ENEMY_IDS; entity++)
    {
        FreeEntityVisuals(entity);
    }
}

char * GetEntityName(enum ENTITY_IDs ID)
{
    static char index[NUMBER_OF_ENEMY_IDS][100] = 
    {
        [UNKNOWN]="Oh noes something went wrong :(",
        [BOUNCEPOT]="Bouncepot",
        [GEARRAT]="Gearrat",
        [MECHRAB]="Mechrab",
        [FREDDY]="Freddy",
        [BONNIE]="Bonnie",
        [CHICA]="Chica",
        [FOXY]="Foxy"
    };
    return index[ID][0] ? &index[ID][0] : &index[UNKNOWN][0];
}