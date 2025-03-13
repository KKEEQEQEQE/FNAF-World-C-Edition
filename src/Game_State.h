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

#include "Title_Screen.h"
#include "rayclock.h"
#include <stdint.h>
#include <time.h>

typedef void (*_AdditionUpdater)(uint8_t);

#define MAX_ADDITIONAL_UPDATERS_GS 10

typedef struct _GameStateScene
{
    // Time
    clock_t * SceneClock;
    float TimeScale; // Scene Time Past = Time Past * TimeScale

    // Init and Freeing
    void (*InitScene)(void);
    void (*FreeScene)(void);

    // Rendering and Updating
    void (*RenderScene)(void);
    void (*UpdateScene)(void);
    
    /*
        When GameStateStep is called, not only does it update the scene 
        but all additional updaters are called
    */
    _AdditionUpdater *(AdditionalUpdaters[MAX_ADDITIONAL_UPDATERS_GS]); 
} _GameStateScene;

enum GameStateTypes
{
    Disclamer, SpookyWarning, Title, Save, Party, World, Chips, Bytes, Battle, Dialogue
};

void SwapGameState(enum GameStateTypes state);
enum GameStateTypes GetGameState(void);

enum TransitionAnimationTypes
{
    NOCURRENT, FADE
};

void SwapGameState_Animated(enum TransitionAnimationTypes type, enum GameStateTypes state, float duration);
void PutTransitionAnimation(void);

_Bool IsGameStateSwitching(void);