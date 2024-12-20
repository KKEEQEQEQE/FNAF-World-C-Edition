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

#pragma once


#include <stdint.h>
#define SCREEN_POSITION_TO_PIXEL_X(x, width, scale) ((int)GetScreenWidth()/2 + x * GetScreenWidth() / 2 - (int) (width * scale) / 2)
#define SCREEN_POSITION_TO_PIXEL_Y(y, height, scale) ((int)GetScreenHeight()/2 + y * GetScreenHeight() / 2 - (int) (height * scale) / 2)

#define PIXEL_TO_SCREEN_POSITION_X(x, texture, scale)


#include "input.h"
#include "Background.h"


enum UIType 
{
    UInotype, UIanimation, UItexture, UIanimationV2
};

#include "Animation.h"


enum UITextAlignment 
{
    LEFTMOST, CENTRE, RIGHTMOST
};

typedef Texture2D UITexture;

typedef struct UIVisual
{
    enum UIType type;
    union 
    {
        UITexture texture;
        Animation animation;
        Animation_V2 animation_V2;
    };
    Color tint;
} UIVisual;

#include "Particle.h"

typedef struct UIElement 
{
    UIVisual visual;
    float x;
    float y;
    float scale;
} UIElement;

typedef struct UIFont 
{
    Font font;
    float averageRatio;
} UIFont;

typedef struct UIButton 
{
    UIElement graphic;
    void (* press)(struct UIButton * button);
    void (* hover)(struct UIButton * hover);
    GamepadButton controllerBinding;
    
} UIButton;


// Gets the screen ratio
extern float GetScreenRatio(void);

// Gets the screen scale relative to the screen height
extern float GetScreenScale(void);

// Gets the screen scale relative to the screen width
extern float GetScreenScaleW(void);

// Gets the position x needed for a texture to not be visible in UI space
extern float GetOutsideWindowX(Texture2D texture);

// Gets the position y needed for a texture to not be visible in UI space
extern float GetOutsideWindowY(Texture2D texture);

// Gets the position x needed for a texture to not be visible in UI space
extern float GetOutsideWindowX_u16(uint16_t width);

// Gets the position y needed for a texture to not be visible in UI space
extern float GetOutsideWindowY_u16(uint16_t height);

// Creates a UIVisual of type UItexture via path
extern UIVisual CreateUIVisual_UITexture(UITexture texture, Color tint);

// Creates a UIVisual of type UIanimation via path
extern UIVisual CreateUIVisual_UITexture_P(const char * path, Color tint);

// Creates a UIVisual of type UIanimation via path
extern UIVisual CreateUIVisual_UIAnimation(const char * path, const uint8_t targetFPS, const Color tint);

// Creates a UIVisual of type UIanimationV2 via path
extern UIVisual CreateUIVisual_UIAnimation_V2(const char * path, const uint8_t targetFPS, const uint8_t amount, Vector2 tileSize, Color tint);

extern UIElement CreateUIElement(UIVisual visual, float x, float y, float scale);

// Copies a UIVisual to the heap and returns the address
extern UIVisual * UIVisual_Heap(UIVisual visual);

extern void FreeUIVisual(UIVisual * visual);

extern void FreeUIElement(UIElement * element);

// Scales and Renders a UITexture in UI Space
extern void RenderUITexture(UITexture texture, float x, float y, float scale);

// Scales and Renders a UIElement
extern void RenderUIElement(const UIElement * element);

// Scales and Renders a UIVisual at X, Y scaled
extern void RenderUIVisual(float x, float y, UIVisual * visual, float scale);

// Scales and Renders text in UI space
extern void RenderUIText(const char * text, float x, float y, float fontSize, enum UITextAlignment allignment, Font font, Color color);

// Scales and Renders a UIButton
extern void RenderUIButton(const UIButton * button);

// Checks and updates a button if it has been pressed
extern void UpdateUIButton(const UIButton * button);

// Updates and Renders a UIButton
extern void PutUIButton(const UIButton * button);

// Draws a Sprite from a UITexture Spritesheet
extern void DrawUITextureSpritesheet(Texture2D atlas, int16_t x, int16_t y, uint16_t index, uint16_t tileSize);

// Draws a Sprite from a UITexture Spritesheet with addition parameters
extern void DrawUITextureSpritesheetEx(Texture2D atlas, int16_t x, int16_t y, uint16_t index, Vector2 tileSize, float scale, Color tint);

// Scales and Renders a Sprite from a UITexture Spritesheet in UI space
extern void RenderUITextureSpritesheet(Texture2D atlas, float x, float y, uint16_t index, uint16_t tileSize);

// Scales and Renders a Sprite from a UITexture Spritesheet in UI space with addition parameters
extern void RenderUITextureSpritesheetEx(Texture2D atlas, float x, float y, uint16_t index, Vector2 tileSize, float scale, Color tint);