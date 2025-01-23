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


#include "Title_Screen.h"
#include "Background.h"
#include "Game_State.h"
#include "Particle.h"
#include "Particle_Updaters.h"
#include "UI.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define TITLE_SECONDS_TO_CENTRE 3
#define PARTY_SECONDS_TO_CENTRE 3
#define START_SECONDS_TO_CENTRE 2


Font TitleScreenFont = {0};
UIElement UIBackground = {0};
UIElement UIParty = {0};
UIElement UITitle = {0};

uint8_t ButtonClicked = 0;

Music Theme = {0};

void ButtonPlayPressed(UIButton * button)
{
    SwapGameState_Animated(FADE, World, 0.5);
    button -> graphic.scale = 0;
}

UIButton UIPlay = {0};
anim_t StartTime = 0;
uint8_t ParticleStars = 0;

void InitTitleScreen(void) 
{

    Theme = LoadMusicStream("Assets/Themes/theme.wav");
    Theme.looping = 1;

    PlayMusicStream(Theme);

    UIBackground.visual.texture = LoadTexture("Assets/Menu/Title_Screen/Background.png");
    SetTextureFilter(UIBackground.visual.texture, TEXTURE_FILTER_BILINEAR);
    
    UIParty.visual = CreateUIVisual_UITexture_P("Assets/Menu/Title_Screen/Party.png", WHITE);
    SetTextureFilter(UIParty.visual.texture, TEXTURE_FILTER_BILINEAR);
    UIParty.scale = 1.5;
    
    SetTextureWrap(UIParty.visual.texture, TEXTURE_WRAP_CLAMP);

    UITitle.visual = CreateUIVisual_UIAnimation_V2("Assets/Menu/Title_Screen/Title.png", 3, 3, (Vector2){650, 127}, WHITE);
    SetTextureWrap(UITitle.visual.animation_V2.Atlas, TEXTURE_WRAP_CLAMP);
    UITitle.scale = 1.3;

    UIPlay.graphic.visual = CreateUIVisual_UIAnimation_V2("Assets/Menu/Title_Screen/play_button.png", 30, 11, (Vector2) {269, 66}, WHITE);
    UIPlay.graphic.scale = 1.5;

    UIPlay.pressed_visual = CreateUIVisual_UIAnimation_V2("Assets/button_pressed.png", 60, 11, (Vector2) {489, 120}, WHITE);
    UIPlay.pressed_visual_duration = 11 * CLOCKS_PER_SEC / 60;
    UIPlay.press_update_delay = 11 * CLOCKS_PER_SEC / 60;
    UIPlay.press = ButtonPlayPressed;

    ParticleStars = CreateParticleIndexA_V2("Assets/Particles/titlestar2.png", 20,8, (Vector2) {90, 90}, 0.5);
    TitleScreenFont = LoadFont("Assets/Menu/Title_Screen/font.ttf");
    SetTextureFilter(TitleScreenFont.texture, TEXTURE_FILTER_BILINEAR);
    ResetTitleScreen();
}

void UninitTitleScreen(void)
{
    FreeUIElement(&UITitle);
    FreeUIElement(&UIBackground);
    FreeUIElement(&UIParty);
    FreeUIElement(&UIPlay.graphic);
    FlushParticles();
    RemoveParticleIndex(ParticleStars);
    UnloadMusicStream(Theme);
    //UnloadFont(TitleScreenFont);
}

void ResetTitleScreen(void) 
{
    SetWindowTitle("FNaF World: C Edition - Title Screen");

    StartTime = clock();
    
    UITitle.visual.animation.Clock = StartTime;

    UITitle.x = GetOutsideWindowX_u16(UITitle.visual.animation_V2.TileSize_x);
    UITitle.y = -0.775;

    UIParty.x = 0;
    UIParty.y = GetOutsideWindowY(UIParty.visual.texture);

    UIPlay.graphic.x = 0;
    UIPlay.graphic.y = 0.8125;
    PlayMusicStream(Theme);
    
}

void UpdateTitle(void) 
{
    register float timeSinceStart = (float)(clock() - StartTime) / CLOCKS_PER_SEC;
    UITitle.x = -GetOutsideWindowX_u16(UITitle.visual.animation_V2.TileSize_x) + timeSinceStart * (GetOutsideWindowY_u16(UITitle.visual.animation_V2.TileSize_y) / TITLE_SECONDS_TO_CENTRE);

    if (UITitle.x > 0)
    {
        UITitle.x = 0;
        UITitle.visual.animation_V2.FPS = 0;
        UITitle.visual.animation_V2.Clock = 0;
    }
}

void UpdateParty(void) 
{
    register float timeSinceStart = (float)(clock() - StartTime) / CLOCKS_PER_SEC;
    UIParty.y = GetOutsideWindowY(UIParty.visual.texture) + timeSinceStart * -(GetOutsideWindowY(UIParty.visual.texture) / PARTY_SECONDS_TO_CENTRE);
    UIParty.y = UIParty.y <= 0.26 ? 0.26 : UIParty.y;
}

void UpdatePlay(void) 
{
    register float timeSinceStart = (float)(clock() - StartTime) / CLOCKS_PER_SEC;
    UIPlay.graphic.x = GetOutsideWindowX(UIPlay.graphic.visual.texture) + timeSinceStart * -(GetOutsideWindowX(UIPlay.graphic.visual.texture) / START_SECONDS_TO_CENTRE);
    UIPlay.graphic.x = UIPlay.graphic.x < 0 ? 0: UIPlay.graphic.x;
}

void CreateTitlestars(void)
{
    static clock_t timeSinceLastParticle = 0;
    if (clock() - timeSinceLastParticle > 50 && UIParty.y <= 0.26) 
    {
        float degrees = GetRandomValue(0, 200 *PI)/100.;
        CreateParticleEx(ParticleStars, 0, 0, cosf(degrees), sinf(degrees), 80, NULL);
        timeSinceLastParticle = clock();
    }
}

void UpdateTitleScreen(void)
{
    UpdateTitle();
    UpdateParty();
    UpdatePlay();
    UpdateUIParticles();
    CreateTitlestars();

}

void RenderTitleScreen(void)
{
    UpdateMusicStream(Theme);

    RenderBackground(UIBackground.visual.texture);
    RenderUIParticles();
    RenderUIElement(&UITitle);
    RenderUIElement(&UIParty);
    PutUIButton(&UIPlay);
    RenderUIText("Demo: 1", -0.95, 0.875,0.06, LEFTMOST, TitleScreenFont, WHITE);
    RenderUIText("Original Game By: Scott Cawthon", 0.9, 0.8,0.04, RIGHTMOST,TitleScreenFont, WHITE);
    RenderUIText("Remake By: SpyterDev", 0.9, 0.9,0.04, RIGHTMOST, TitleScreenFont, WHITE);
}
void PutTitleScreen(void) 
{
    if (!IsGameStateSwitching()) UpdateTitleScreen();
    RenderTitleScreen();
}