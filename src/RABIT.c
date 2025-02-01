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

#include "RABIT.h"
#include "Animation.h"
#include "Battle_Rework.h"
#include "rayclock.h"
#include <stdint.h>

// R.A.B.I.T System API Targets

extern _BattleParty Party_Enemy; 
extern _BattleParty Party_Player;
extern float enemy_speed;
extern float player_speed;

// From Battle System
void DealDamage(uint32_t amount, uint8_t target);

// Converts Entity Index to pointer (0-3 = &Party_Enemy.member[0-3], 4-7 = &Party_Player.member[0-3])
_BattleEntity * EntityIndexToPointer(uint8_t id)
{
    id %= 8;
    return &(id < 4 ? Party_Enemy.member : Party_Player.member)[id];
}
typedef struct _AttackVisualIndex
{
    _Bool in_use;
    Animation_V2 animation;
} _AttackVisualIndex; 

_AttackVisualIndex AttackVisual[NUMBER_OF_ATTACKS] = {0};

void LoadAttackVisuals(enum ALL_ATTACKS ID)
{
    if (AttackVisual[ID].in_use) return;

    switch (ID) 
    {
        case HIT:
        {
            break;
        }
        default:
        {
            break;
        }
            
    }

    AttackVisual[ID].in_use = true;
}

Animation_V2 AccessAttackAnimation(enum ALL_ATTACKS ID)
{
    _AttackVisualIndex * target = AttackVisual + ID;

    if (!target -> in_use) LoadAttackVisuals(ID);

    return target -> animation.Amount ? target -> animation : (Animation_V2){0};
}

void FreeAttackVisuals(enum ALL_ATTACKS ID)
{
    if (!AttackVisual[ID].in_use) return;

    if (IsTextureValid(AttackVisual[ID].animation.Atlas)) UnloadTexture(AttackVisual[ID].animation.Atlas);    
    AttackVisual[ID].animation.Amount = 0;
    AttackVisual[ID].in_use = false;
}

void FlushAttackVisuals(void)
{
    for (enum ALL_ATTACKS animation = HIT; animation < NUMBER_OF_ATTACKS; animation++)
    {
        FreeAttackVisuals(animation);
    }
}


const uint32_t AttackDamageDelay[NUMBER_OF_ATTACKS] =
{
    [HIT] = 0.5 * RAYCLOCKS_PER_SEC
};

// Damage formulas

#define DamageFormulaTemplate(ID, formula) static uint32_t DamageFormula_ ##ID(uint32_t level, uint8_t projectile_id) {return formula;}
        
// Basic attacks

DamageFormulaTemplate(HIT, 
                        level)
DamageFormulaTemplate(MICTOSS, 
                        level + 15 + GetRandomValue(0, 4))
DamageFormulaTemplate(BITE, 
                        level + 20 + GetRandomValue(0, 4))
DamageFormulaTemplate(BITE2, 
                        level + 200 + GetRandomValue(0, 49))

// Red attacks

DamageFormulaTemplate(PIZZAWHEEL, 
                        level + 1 + GetRandomValue(0, 1))

DamageFormulaTemplate(PIZZAWHEEL2, 
                        level + 21 + GetRandomValue(0, 1))

DamageFormulaTemplate(BASHJAM, 
                        level + 10 + GetRandomValue(0, 19) + GetRandomValue(0, 5))

DamageFormulaTemplate(HOTCHEESE, 
                        level + 5 + GetRandomValue(0, 3))

DamageFormulaTemplate(HOTCHEESE2, 
                        level + 53 + GetRandomValue(0, 3))

DamageFormulaTemplate(BALLOONS, 
                        (level + GetRandomValue(0, 2) + 5) * GetRandomValue(0, 3) + 1)

DamageFormulaTemplate(MUNCHIES, 
                        level + 5 + GetRandomValue(0, 4))


#define DamageFormulaInitAssign(ID) [ ID ] = DamageFormula_ ##ID

uint32_t (* const DamageFunctions[NUMBER_OF_ATTACKS])(uint32_t, uint8_t) =
{
    DamageFormulaInitAssign(HIT),
    DamageFormulaInitAssign(MICTOSS)
};