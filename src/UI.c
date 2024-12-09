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

#include "UI.h"
#include "Animation.h"
#include "input.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "Particle.h"
#include <stdint.h>

float GetScreenRatio(void)
{
    uint8_t widthGreater = (float) GetScreenWidth() / GetScreenHeight() >= 4/3.;
    return widthGreater ? (float) GetScreenWidth() / GetScreenHeight() : (float) GetScreenHeight() / GetScreenWidth();
}

static uint16_t gcd(uint16_t a, uint16_t b)
{
    uint16_t result = ((a < b) ? a : b);
    while (result > 0) {
        if (a % result == 0 && b % result == 0) {
            break;
        }
        result--;
    }
    return result;
}


Vector2 GetAspectRatioFraction(uint16_t x, uint16_t y)
{
    uint16_t scale = gcd(x,y);
    x /= scale;
    y /= scale;
    return (Vector2) {x, y};
}
float GetScreenScale(void)
{
    return GetScreenHeight()/720.;
}

float GetScreenScaleW(void)
{
    return GetScreenWidth()/1280.;
}

void FreeUIElement(UIElement * element)
{
    switch (element -> type) 
    {
        case UItexture:
            UnloadTexture(element -> texture);
            break;
        case UIanimation:
            FreeAnimation(&element -> animation);
            break;
        default:
            break;
        memset(element, 0, sizeof(UIElement));
    }
}

void DrawDebugTexture(Texture2D texture, Vector2 position, float rotation, float scale, Color tint)
{
    printf("%f, %f\n", position.x, position.y);
    DrawTextureEx(texture, position, rotation, scale, tint);
}

void RenderUITexture(UITexture texture, float x, float y, float scale)
{
    DrawTextureEx(  texture, 
                    (Vector2) { (float) SCREEN_POSITION_TO_PIXEL_X(x, texture.width, scale), 
                                 (float)SCREEN_POSITION_TO_PIXEL_Y(y, texture.height, scale) },
                    0, 
                    scale, 
                    WHITE);
}

void RenderUITextureDebug(UITexture texture, float x, float y, float scale)
{
    DrawTextureEx(  texture, 
                    (Vector2) { (float) SCREEN_POSITION_TO_PIXEL_X(x, texture.width, scale), 
                                 (float)SCREEN_POSITION_TO_PIXEL_Y(y, texture.height, scale) },
                    0, 
                    scale, 
                    WHITE);
}

void RenderUIElement(const UIElement * element) 
{
    register float scale = element -> scale * GetScreenHeight() / 720.;
    switch (element -> type) {

        case UIanimation:
            RenderAnimation(&element -> animation, 
                            element -> x, 
                            element -> y, 
                            scale, 0);
            return;
        case UItexture:
            RenderUITexture(element -> texture, element -> x, element -> y, scale);
            return;
        default:
        return;
    }
}

void RenderUIButton(const UIButton * button)
{
    RenderUIElement(&button->visual);
}



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

void UpdateUIButton(const UIButton * button) 
{
    register uint16_t inputX = 0;
    register uint16_t inputY = 0;

    register float scale = button -> visual.scale * GetScreenHeight() / 720.;

    UITexture buttonTexture = {0};

    switch (button -> visual.type)
    {
        case UIanimation: 
            uint16_t currentFrame = GetCurrentAnimationFrame(&button -> visual.animation);
            buttonTexture = button -> visual.animation.Frames[currentFrame];
            break;
        case UItexture:
            buttonTexture = button -> visual.texture;
            break;
        default:
        return;
    }

    register uint16_t buttonX = SCREEN_POSITION_TO_PIXEL_X(button -> visual.x, buttonTexture.width, scale);
    register uint16_t buttonY = SCREEN_POSITION_TO_PIXEL_Y(button -> visual.y, buttonTexture.height, scale);

    register uint16_t buttonW = buttonTexture.width * scale;
    register uint16_t buttonH = buttonTexture.height * scale;
    
    Vector2 * inputs = GetInputMouseTouch();
    for (uint8_t i = 0; inputs[i].x != NAN && inputs[i].y != NAN && i < MAX_INPUT_POINTS; i++) 
    {
        inputX = (uint16_t) inputs[i].x;
        inputY = (uint16_t) inputs[i].y;

        if ((inputX > buttonX &&  inputX <= buttonX + buttonW) &&
            (inputY > buttonY && inputY <= buttonY + buttonH) )
        {
            if (button -> press) button -> press(button);
        }
    }
}

void PutUIButton(const UIButton * button)
{
    UpdateUIButton(button);
    RenderUIButton(button);
}

float TileSpaceToScreenSpace(float n)
{
    return (n + 1)/2;
}