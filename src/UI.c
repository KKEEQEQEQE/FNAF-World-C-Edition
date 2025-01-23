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


#include "UI.h"
#include "Animation.h"
#include <stdlib.h>
#include "input.h"
#include <math.h>
#include "../Include/rlgl.h"
#include <string.h>
#include <stdio.h>
#include "Particle.h"
#include <stdint.h>
#include <time.h>

// Gets the screen ratio
float GetScreenRatio(void)
{
    uint8_t widthGreater = (float) GetScreenWidth() / GetScreenHeight() >= 4/3.;
    return widthGreater ? (float) GetScreenWidth() / GetScreenHeight() : (float) GetScreenHeight() / GetScreenWidth();
}

// Gets the screen scale relative to the screen height
float GetScreenScaleH(void)
{
    return GetScreenHeight()/720.;
}

// Gets the screen scale relative to the screen width
float GetScreenScaleW(void)
{
    return GetScreenWidth()/1280.;
}

// The current scaling is relative to 720p / Scale_Mode, DEFAULT VALUE: HEIGHT
enum UI_SCREEN_SCALE_MODE Scale_Mode = HEIGHT;

void SetUIScreenScaleMode(enum UI_SCREEN_SCALE_MODE mode)
{
    Scale_Mode = mode;
}

// Sets UI Scaling Mode (for improved scaling on smaller aspect ratios)
float GetScreenScale(void)
{
    switch (Scale_Mode) {
        case WIDTH:
            return GetScreenScaleW();
        case HEIGHT:
            return GetScreenScaleH();
    }
}
// Gets the position x needed for a texture to not be visible in UI space
float GetOutsideWindowX_u16(uint16_t width) 
{
    return 1 + (float)(width) / (GetScreenWidth());
}

// Gets the position y needed for a texture to not be visible in UI space
float GetOutsideWindowY_u16(uint16_t height) 
{
    return 1 + (float)(height) / (GetScreenWidth());
}

// Gets the position x needed for a texture to not be visible in UI space
float GetOutsideWindowX(Texture2D texture) 
{
    return 1 + (float)(texture.width) / (GetScreenWidth());
}

// Gets the position y needed for a texture to not be visible in UI space
float GetOutsideWindowY(Texture2D texture) 
{
    return 1 + (float)(texture.height) / (GetScreenHeight());
}

void FreeUIElement(UIElement * element)
{
    switch (element -> visual.type) 
    {
        case UItextureSnippet:
        case UItexture:
            UnloadTexture(element -> visual.texture);
            break;
        case UIanimation:
            FreeAnimation(&element -> visual.animation);
            break;
        case UIanimationV2:
            FreeAnimation_V2(&element -> visual.animation_V2);
            break;
        default:
            break;  
    }

    memset(element, 0, sizeof(UIElement));
}

// Debugging DrawTextureEx wrapper which also prints position
void DrawDebugTexture(Texture2D texture, Vector2 position, float rotation, float scale, Color tint)
{
    printf("%f, %f\n", position.x, position.y);
    DrawTextureEx(texture, position, rotation, scale, tint);
}

// Scales and Renders a UITexture in UI Space
void RenderUITexture(UITexture texture, float x, float y, float scale)
{
    DrawTextureEx(  texture, 
                    (Vector2) { (float) SCREEN_POSITION_TO_PIXEL_X(x, texture.width, scale), 
                                 (float) SCREEN_POSITION_TO_PIXEL_Y(y, texture.height, scale) },
                    0, 
                    scale, 
                    WHITE);
}

// Scales and Renders a UITexture in UI Space
void RenderUITextureEx(UITexture texture, float x, float y, float scale)
{
    DrawTextureEx(  texture, 
                    (Vector2) { (float) SCREEN_POSITION_TO_PIXEL_X(x, texture.width, scale), 
                                 (float) SCREEN_POSITION_TO_PIXEL_Y(y, texture.height, scale) },
                    0, 
                    scale, 
                    WHITE);
}

// Scales and Renders a UITexture in UI Space
void RenderUITexturePro(UITexture texture, float x, float y, float scale, float rotation)
{
    RenderUITextureSnippetPro(texture, x, y, (Rectangle){0, 0, texture.width, texture.height}, scale, rotation, WHITE);
        
}

// Copies a UIVisual to the heap and returns the address
UIVisual * UIVisual_Heap(UIVisual visual)
{
    UIVisual * visual_heap = malloc(sizeof(UIVisual));
    if (!visual_heap) exit(EXIT_FAILURE);
    *visual_heap = visual;
    return visual_heap;
}

// Creates a UIVisual of type UItexture via UItexture
UIVisual CreateUIVisual_UITexture(UITexture texture, Color tint)
{
    UIVisual temp = {0};
    temp.type = UItexture;
    temp.texture = texture;
    temp.tint = tint;
    return temp;
}

// Creates a UIVisual of type UItexture via path
UIVisual CreateUIVisual_UITexture_P(const char * path, Color tint)
{
    UIVisual temp = {0};
    temp.type = UItexture;
    temp.texture = LoadTexture(path);
    temp.tint = tint;
    return temp;
}

// Creates a UIVisual of type UITextureSnippet via UItexture
UIVisual CreateUIVisual_UITextureSnippet(UITexture texture, Rectangle snippet, Color tint)
{
    UIVisual temp = {0};
    temp.type = UItextureSnippet;
    temp.texture = texture;
    temp.tint = tint;
    temp.snippet = snippet;
    return temp;
}

// Creates a UIVisual of type UITextureSnippet via path
UIVisual CreateUIVisual_UITextureSnippet_P(const char * path, Rectangle snippet, Color tint)
{
    UIVisual temp = {0};
    temp.type = UItextureSnippet;
    temp.texture = LoadTexture(path);
    temp.tint = tint;
    temp.snippet = snippet;
    return temp;
}

// Creates a UIVisual of type UIanimation via path
UIVisual CreateUIVisual_UIAnimation(const char * path, const uint8_t targetFPS, const Color tint)
{
    UIVisual temp = {0};
    temp.type = UIanimation;
    temp.animation = CreateAnimation(path, targetFPS);
    temp.tint = tint;
    return temp;
}

// Creates a UIVisual of type UIanimationV2 via path
UIVisual CreateUIVisual_UIAnimation_V2(const char * path, const uint8_t targetFPS, const uint8_t amount, Vector2 tileSize, Color tint)
{
    UIVisual temp = {0};
    temp.type = UIanimationV2;
    temp.animation_V2 = CreateAnimation_V2(path, targetFPS, amount, tileSize.x, tileSize.y);
    temp.tint = tint;
    return temp;
}

void FreeUIVisual(UIVisual * visual)
{
    switch (visual -> type) 
    {
        case UItextureSnippet:
        case UItexture:
            UnloadTexture(visual -> texture);
            break;
        case UIanimation:
            FreeAnimation(&visual->animation);
            break;
        case UIanimationV2:
            FreeAnimation_V2(&visual -> animation_V2);
            break;
        default:
            break;
        
    }

    memset(visual, 0, sizeof(UIVisual));
}

UIElement CreateUIElement(UIVisual visual, float x, float y, float scale)
{
    UIElement temp = {0};
    temp.visual = visual;
    temp.scale = scale;
    temp.x = x;
    temp.y = y;
    return temp;
}

// Scales and Renders a UIElement
void RenderUIElement(const UIElement * element) 
{
    register float scale = element -> scale * GetScreenScale();
    switch (element -> visual.type) {

        case UIanimation:
            RenderAnimation(&element -> visual.animation, 
                            element -> x, 
                            element -> y, 
                            scale, 0);
            return;
        case UItexture:
            RenderUITexture(element -> visual.texture, 
                            element -> x, 
                            element -> y, 
                            scale);
            return;
        case UItextureSnippet:
            RenderUITextureSnippet( element -> visual.texture,
                                    element -> x,
                                    element -> y,
                                    element -> visual.snippet);
            break;
        case UIanimationV2:
            RenderAnimation_V2(&element -> visual.animation_V2, 
                                element -> x, element -> y, 
                                element -> scale, 0);
            return;
        default:
            return;
    }
}

// Scales and Renders a UIButton
void RenderUIButton(UIButton * button)
{
    if (button -> last_press && clock() - button -> last_press < button -> pressed_visual_duration)
    {
        RenderUIVisual( button -> graphic.x, button -> graphic.y, 
                        &button -> pressed_visual, 
                        button -> graphic.scale);
    } else RenderUIElement(&button -> graphic);
}

// Scales and Renders a UIVisual at X, Y scaled
void RenderUIVisual(float x, float y, UIVisual * visual, float scale)
{
    UIElement temp = (UIElement) {.visual=*visual, .x=x, .y=y, .scale=scale};
    RenderUIElement(&temp);
}

// Scales and Renders text in UI space
void RenderUIText(const char * text, float x, float y, float fontSize, enum UITextAlignment allignment, Font font, Color color)
{
    fontSize *= GetScreenHeight();
    Vector2 position = (Vector2) {SCREEN_POSITION_TO_PIXEL_X(x, 0, 1), SCREEN_POSITION_TO_PIXEL_Y(y, fontSize, 1)};
    Vector2 origin = {0}; 
    if (!font.baseSize) font = GetFontDefault();
    Vector2 size = MeasureTextEx(font, text, fontSize, 1);
    switch (allignment) 
    {
        case LEFTMOST:
            origin = (Vector2){0, size.y/2};
            break;
        case CENTRE:
            origin = (Vector2){size.x/2, size.y/2};
            break;
        case RIGHTMOST:
            origin = (Vector2){size.x, size.y/2};
            break;
    }
    DrawTextPro(font, text, position, origin, 0, fontSize, 1, color);
}

// Checks and updates a button if it has been pressed
void UpdateUIButton(UIButton * button) 
{
    register float scale = button -> graphic.scale * GetScreenScale();

    Rectangle button_rect = {0};

    switch (button -> graphic.visual.type)
    {
        case UIanimation: 
        {
            uint16_t currentFrame = GetCurrentAnimationFrame(&button -> graphic.visual.animation);
            button_rect.width = button -> graphic.visual.animation.Frames[currentFrame].width;
            button_rect.height = button -> graphic.visual.animation.Frames[currentFrame].height;
            break;
        } 

        case UIanimationV2:
        {
            button_rect.width = button -> graphic.visual.animation_V2.TileSize_x;
            button_rect.height = button -> graphic.visual.animation_V2.TileSize_y;
            break;
        }

        case UItextureSnippet:
        {
            button_rect.width = button -> graphic.visual.snippet.width;
            button_rect.height = button -> graphic.visual.snippet.height;
            break;
        }

        case UItexture:
        {
            button_rect.width = button -> graphic.visual.texture.width;
            button_rect.height = button -> graphic.visual.texture.height;
            break;
        }

        default:
            return;
    }

    button_rect.x = SCREEN_POSITION_TO_PIXEL_X(button -> graphic.x, button_rect.width, scale);
    button_rect.y = SCREEN_POSITION_TO_PIXEL_Y(button -> graphic.y, button_rect.height, scale);
    button_rect.width *= scale;
    button_rect.height *= scale;

    if (CheckCollisionPointRec(GetInputTap(), button_rect) && !button -> last_press)
    {
        button -> last_press = clock();
        if (button -> pressed_visual.type == UIanimationV2)
        {
            button -> pressed_visual.animation_V2.Clock = button -> last_press;
        }
    }

    if (button -> last_press && clock() - button -> last_press >= button -> press_update_delay)
    {
        button -> press(button);
        button -> last_press = 0;
    }
}

// Updates and Renders a UIButton
void PutUIButton(UIButton * button)
{
    UpdateUIButton(button);
    RenderUIButton(button);
}

// Draws a Sprite from a UITexture Spritesheet
void DrawUITextureSpritesheet(Texture2D atlas, int16_t x, int16_t y, uint16_t index, uint16_t tileSize)
{
    DrawUITextureSpritesheetEx(atlas, x, y, index, (Vector2) {tileSize, tileSize}, 1, WHITE);
}

// Draws a Sprite from a UITexture Spritesheet with addition parameters
void DrawUITextureSpritesheetEx(Texture2D atlas, int16_t x, int16_t y, uint16_t index, Vector2 tileSize, float scale, Color tint)
{
    uint16_t adjustedWidth = atlas.width / (uint16_t) tileSize.x;
    adjustedWidth *= (uint16_t) tileSize.x; 
    Rectangle source = {    (index * (uint16_t) tileSize.x) % adjustedWidth, 
                            (uint16_t)((float) (index * (uint16_t) tileSize.x) / adjustedWidth) * tileSize.y,
                            tileSize.x,
                            tileSize.y   };
    
    Rectangle dest = { x, y, tileSize.x * scale, tileSize.y * scale};

    DrawTexturePro(atlas, source, dest, (Vector2) {0, 0}, 0, tint);
}

// Draws a Sprite from a UITexture Spritesheet with extra addition parameters
void DrawUITextureSpritesheetPro(Texture2D atlas, int16_t x, int16_t y, uint16_t index, Vector2 tileSize, float scale, float rotation, Color tint)
{
    uint16_t adjustedWidth = atlas.width / (uint16_t) tileSize.x;
    adjustedWidth *= (uint16_t) tileSize.x; 
    Rectangle source = {    (index * (uint16_t) tileSize.x) % adjustedWidth, 
                            (uint16_t)((float) (index * (uint16_t) tileSize.x) / adjustedWidth) * tileSize.y,
                            tileSize.x,
                            tileSize.y   };
    
    Rectangle dest = { x + tileSize.x * scale / 2., y + tileSize.y * scale / 2., tileSize.x * scale, tileSize.y * scale};

    DrawTexturePro(atlas, source, dest, (Vector2) {tileSize.x / 2 * scale, tileSize.y / 2 * scale}, rotation, tint);
}

// Scales and Renders a Sprite from a UITexture Spritesheet in UI space
void RenderUITextureSpritesheet(Texture2D atlas, float x, float y, uint16_t index, uint16_t tileSize)
{
    DrawUITextureSpritesheetEx(   atlas, 
                                SCREEN_POSITION_TO_PIXEL_X(x, tileSize, GetScreenScale()),
                                SCREEN_POSITION_TO_PIXEL_Y(y, tileSize, GetScreenScale()),
                                index, 
                                (Vector2) {tileSize, tileSize}, 
                                GetScreenScale(), 
                                WHITE);
}

// Scales and Renders a Sprite from a UITexture Spritesheet in UI space with addition parameters
void RenderUITextureSpritesheetEx(Texture2D atlas, float x, float y, uint16_t index, Vector2 tileSize, float scale, Color tint)
{
    scale *= GetScreenScale();
    DrawUITextureSpritesheetEx(   atlas, 
                                SCREEN_POSITION_TO_PIXEL_X(x, tileSize.x, scale),
                                SCREEN_POSITION_TO_PIXEL_Y(y, tileSize.y, scale), 
                                index, 
                                tileSize, 
                                scale, 
                                WHITE);
}

// Draws a snippet from a UItexture
void DrawUITextureSnippet(Texture2D atlas, int16_t x, int16_t y, Rectangle snippet)
{
    DrawUITextureSnippetEx(atlas, x, y, snippet, 1, WHITE);
}

// Draws a snippet from a UItexture with addition parameters
void DrawUITextureSnippetEx(Texture2D atlas, int16_t x, int16_t y, Rectangle snippet, float scale, Color tint)
{
    Rectangle dest = { x, y, snippet.width * scale, snippet.height * scale};

    DrawTexturePro(atlas, snippet, dest, (Vector2) {0, 0}, 0, tint);
}

// Scales and Renders a snippet from a UItextureSnippet in UI space
void RenderUITextureSnippet(Texture2D atlas, float x, float y, Rectangle snippet)
{
    DrawUITextureSnippetEx(     atlas, 
                                SCREEN_POSITION_TO_PIXEL_X(x, snippet.width, GetScreenScale()),
                                SCREEN_POSITION_TO_PIXEL_Y(y, snippet.height, GetScreenScale()),
                                snippet,
                                GetScreenScale(),
                                WHITE);
}

// Scales and Renders a snippet from a UItextureSnippet in UI space with addition parameters
void RenderUITextureSnippetEx(Texture2D atlas, float x, float y, Rectangle snippet, float scale, Color tint)
{
    scale *= GetScreenScale();
    DrawUITextureSnippetEx(   atlas, 
                                SCREEN_POSITION_TO_PIXEL_X(x, snippet.width, scale),
                                SCREEN_POSITION_TO_PIXEL_Y(y, snippet.height, scale), 
                                snippet, 
                                scale,  
                                WHITE);
}

// Scales and Renders a snippet from a UItextureSnippet in UI space with addition parameters
void RenderUITextureSnippetPro(Texture2D atlas, float x, float y, Rectangle snippet, float scale, float rotation, Color tint)
{
    scale *= GetScreenScale();

    Rectangle dest = { SCREEN_POSITION_TO_PIXEL_X(x, 0, scale), SCREEN_POSITION_TO_PIXEL_Y(y, 0, scale), snippet.width * scale, snippet.height * scale};

    DrawTexturePro(atlas, snippet, dest, (Vector2) {snippet.width / 2 * scale, snippet.height / 2 * scale}, rotation, tint);
}