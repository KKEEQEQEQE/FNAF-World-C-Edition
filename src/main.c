/*
    MIT License

    Copyright (c) 2024 SpyterDev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "../Include/raylib.h"
#include "Title_Screen.h"
#include "UI.h"
#include "input.h"
#include "types.h"
#include "World.h"
#include "Game_State.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "Tilemap_JSON_Conversion.h"

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "FNAF World: C Edition");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(1280, 720); 
    InitAudioDevice();
    clock_t start = clock();
    LoadWorldTilemap();
    clock_t stop = clock();
    printf("%f\n", (stop - start) / (float) CLOCKS_PER_SEC);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        
        switch (GetGameState()) 
        {
            case Title:
                PutTitleScreen();
                break;
            default:
                RenderUIText("Unknown / Invalid Game State Entered.\nTap to go back to title screen!", 0, 0, 0.06, CENTRE, (Font){0}, WHITE);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) SwapGameState(Title);
                break;
        }
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}