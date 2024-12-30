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


#include <stdint.h>
#define SCREEN_POSITION_TO_PIXEL_X(x, width, scale) ((int)GetScreenWidth()/2 + x * GetScreenWidth() / 2 - (int) (width * scale) / 2)
#define SCREEN_POSITION_TO_PIXEL_Y(y, height, scale) ((int)GetScreenHeight()/2 + y * GetScreenHeight() / 2 - (int) (height * scale) / 2)

#define PIXEL_TO_SCREEN_POSITION_X(x, texture, scale)


#include "input.h"
#include "Background.h"


enum UIType 
{
    UInotype, UIanimation, UItexture, UIanimationV2, UItextureSnippet
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
    Rectangle snippet; // for UItextureSnippet
    Color tint;
} UIVisual;

#include "Particle.h"

typedef struct UIElement 
{
    UIVisual visual;
    float x, y;
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

// UItexture

    // Creates a UIVisual of type UItexture via path
    extern UIVisual CreateUIVisual_UITexture(UITexture texture, Color tint);

    // Creates a UIVisual of type UItexture via path
    extern UIVisual CreateUIVisual_UITexture_P(const char * path, Color tint);

// UItextureSnippet

    // Creates a UIVisual of type UITextureSnippet via UItexture
    extern UIVisual CreateUIVisual_UITextureSnippet(UITexture texture, Rectangle snippet, Color tint);

    // Creates a UIVisual of type UITextureSnippet via path
    extern UIVisual CreateUIVisual_UITextureSnippet_P(const char * path, Rectangle snippet, Color tint);


// Deprecated UIanimation
    // Creates a UIVisual of type UIanimation via path
    extern UIVisual CreateUIVisual_UIAnimation(const char * path, const uint8_t targetFPS, const Color tint);

// UIanimationV2
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


// UItextureSnippet

    // Draws a snippet from a UItexture
    extern void DrawUITextureSnippet(Texture2D atlas, int16_t x, int16_t y, Rectangle snippet);

    // Draws a snippet from a UItexture with addition parameters
    extern void DrawUITextureSnippetEx(Texture2D atlas, int16_t x, int16_t y, Rectangle snippet, float scale, Color tint);

    // Scales and Renders a snippet from a UItextureSnippet in UI space
    extern void RenderUITextureSnippet(Texture2D atlas, float x, float y, Rectangle snippet);

    // Scales and Renders a snippet from a UItextureSnippet in UI space with addition parameters
    extern void RenderUITextureSnippetEx(Texture2D atlas, float x, float y, Rectangle snippet, float scale, Color tint);
