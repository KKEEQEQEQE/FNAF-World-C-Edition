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

#include "Game_State.h"
#include "Battle.h"
#include "Title_Screen.h"
#include "World.h"
#include <time.h>
#include "Particle.h"

enum GameStateTypes GameState = 1000;

void SwapGameState(enum GameStateTypes state)
{
    switch (GameState)
    {
        case Title:
            UninitTitleScreen();
            break;
        case SpookyWarning:
        case Save:
        case Party:
        case World:
            break;
        case Chips:
        case Bytes:
        case Battle:
          break;
    }

    FlushParticles();
    
    switch (state) 
    {
        case Title:
            InitTitleScreen();
            ResetTitleScreen();
            break;
        case World:
            InitWorld();
            ResetWorld();
            break;
        case Battle:
            InitBattle();
        default:
            break;
    }
    GameState = state;
}

enum TransitionAnimationTypes animation = NOCURRENT;
enum GameStateTypes target_gamestate = 0;
clock_t start_time = 0;
clock_t end_time = 0;

void SwapGameState_Animated(enum TransitionAnimationTypes type, enum GameStateTypes state, float duration)
{
    animation = type;
    start_time = clock();
    end_time = start_time + CLOCKS_PER_SEC * duration;
    target_gamestate = state;
}

void UpdateTransitionAnimation(void)
{
    if (clock() >= end_time) 
    {
        SwapGameState(target_gamestate);
        start_time = 0;
        end_time = 0;
        target_gamestate = 0;
    }
}

float GetAnimationPercentage(void)
{
    return (float) (clock() - start_time) / (end_time - start_time);
}

_Bool IsGameStateSwitching(void)
{
    return end_time != 0;
}

void animation_RenderFade(void)
{
    DrawRectangle(  0, 0, 
                    GetScreenWidth(), GetScreenHeight(), 
                    (Color) {0, 0, 0, 255 * GetAnimationPercentage()});
}

void RenderTransitionAnimation(void)
{
    switch (animation) 
    {
        case NOCURRENT:
            break;
        case FADE:
            animation_RenderFade();
            break;
    }
}

void PutTransitionAnimation(void)
{
    if (!start_time) return;
    UpdateTransitionAnimation();
    RenderTransitionAnimation();
}

enum GameStateTypes GetGameState(void)
{
    return GameState;
}