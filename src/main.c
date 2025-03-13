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


#include "../Include/raylib.h"
#include "Animation.h"
#include "Battle_Rework.h"
#include "Particle.h"
#include "Particle_Updaters.h"
#include "Title_Screen.h"
#include "UI.h"
#include "input.h"
#include "types.h"
#include "World.h"
#include "Game_State.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "rayclock.h"
#include "Save.h"
#include "Dialogue.h"
// Well this is the main function and yup that's about what it is
int main(void)
{
    // Init Window

    InitWindow(1280, 720, "FNAF World: C Edition (Overworld Preview 2)");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(1280, 720); 

    InitAudioDevice();

    // Loading important stuff

    clock_t start = clock();

    LoadSave(NULL);

    SwapGameState(10000);    
    SetTargetFPS(240);
    //ToggleBorderlessWindowed();

    // Main Game Loop
    uint8_t temp = CreateParticleIndexA_V2("Assets/Particles/titlestar.png", 3,8, (Vector2) {12, 12}, 2);
    
    LoadWorldTilemap();

    LoadDialogue("example_dialogue.json");
    PrintDialogue();
    FreeDialougeLines();
    Texture2D Cursor = LoadTexture("Assets/Cursor.png");
    SetTextureFilter(Cursor, TEXTURE_FILTER_BILINEAR);
    HideCursor();
    while (!WindowShouldClose())
    {
        UpdateRayclock();
        BeginDrawing();
        ClearBackground((Color) {45,45,45,255});
        switch (GetGameState()) 
        {
            case Title:
                PutTitleScreen();
                break;
            case World:
                PutWorld();
                break;
            case Battle:
                PutBattle();
                break;
            case Dialogue:
                PutDialogue();
                break;
            case Disclamer:
                RenderUIText("Note: This is a Fanmade recreation of FNaF World\n I do not own the assets, and music\nThis is a passion project\n The code will be 100% Free and Open Source\n(When the first demo comes out)", 0, 0, 0.06, CENTRE, (Font) {0}, WHITE);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) SwapGameState(Title);
                break;
            default:
                
                CreateParticleEx(temp, 0, 0, cosf(clock()/1000.) / 3, sinf(clock()/1000.) / 3, 0, NULL);
                CreateParticleEx(temp, 0, 0, -cosf(clock()/1000.) / 3, -sinf(clock()/1000.) / 3, 0, NULL);

                CreateParticleEx(temp, 0.5, 0.5, cosf(clock()/1000.) / 3, sinf(clock()/1000.) / 3, 0, NULL);
                CreateParticleEx(temp, 0, 0, -cosf(clock()/1000.) / 3, -sinf(clock()/1000.) / 3, 0, NULL);

                CreateParticleEx(temp, -0.5, 0.5, cosf(clock()/1000.) / 3, sinf(clock()/1000.) / 3, 0, NULL);
                CreateParticleEx(temp, 0, 0, -cosf(clock()/1000.) / 3, -sinf(clock()/1000.) / 3, 0, NULL);

                CreateParticleEx(temp, 0.5, -0.5, cosf(clock()/1000.) / 3, sinf(clock()/1000.) / 3, 0, NULL);
                CreateParticleEx(temp, 0, 0, -cosf(clock()/1000.) / 3, -sinf(clock()/1000.) / 3, 0, NULL);

                CreateParticleEx(temp, 0, 0, cosf(clock()/1000.) / 3, sinf(clock()/1000.) / 3, 0, NULL);
                CreateParticleEx(temp, -0.5, -0.5, -cosf(clock()/1000.) / 3, -sinf(clock()/1000.) / 3, 0, NULL);
                PutUIParticles();
                
                SetWindowTitle("FNaF World: C Edition - Unknown State");
                RenderUIText("Unknown / Invalid Game State Entered.\nTap to go back to title screen!", 0, 0, 0.06, CENTRE, (Font) {0}, WHITE);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) SwapGameState(Title);
                break;
        }
        DrawFPS(10,10);
        
        // Refreshes Touch Input
        RefreshInput();
        PutTransitionAnimation();

        if (GetInputType() == KEYBOARD) DrawTextureEx(Cursor, GetMousePosition(), 0, GetWindowScaleDPI().y  * 0.75f, WHITE);
        EndDrawing();
    }
    
    // Uniniting stuff

    CloseAudioDevice();
    CloseWindow();
    return 0;
}