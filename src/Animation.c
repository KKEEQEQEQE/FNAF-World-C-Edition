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


#include "Animation.h"
#include "UI.h"
#include "types.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>

// Turns a Unsigned Long into a Stack Allocated String
static char * __fastcall uintstr(uint64_t number) 
{
    uint64_t temp = number;
    uint64_t length = 1;
    while (temp > 9) temp /= 10, length++; // Checks number of digits and length of string
    char * string = alloca(length);
    temp = 0;
    while (temp < length) string[length-temp-1] = '0' + number - (number / 10) * 10, number/=10, temp++;
    string[temp] = '\0';
    char * stringLocation = string;
    return stringLocation;
}

// Gets the amount of frames in a directory (has to be named n.png)
uint16_t GetFrameAmount(const char * directory) 
{
    uint16_t pathLength = strlen(directory);
    char testPath[pathLength+10];
    strcpy(testPath, directory);

    uint16_t frameNumber = 0;
    FILE * previousFileHandle = NULL;
    
    do 
    {
        if (previousFileHandle) fclose(previousFileHandle);
        testPath[pathLength] = '\0';
        strcpy(testPath+pathLength, uintstr(frameNumber));
        strcat(testPath, ".png");
        frameNumber++;

    } while ((previousFileHandle = fopen(testPath, "r")));
    return frameNumber - 1;

}

// Gets current animation frame from an animation struct
uint16_t GetCurrentAnimationFrame(const Animation * animation)
{
    float currentFrame =  (float)(clock() - animation -> Clock) / CLOCKS_PER_SEC;
    currentFrame /= 1. / animation -> FPS;
    return (uint16_t)currentFrame % animation -> Amount;
}

// Gets current animation frame from clock_t, entered frames, and entered FPS
uint16_t GetCurrentAnimationFrameC(clock_t startTime, uint16_t frames, uint8_t FPS)
{
    float currentFrame =  (float)(clock() - startTime) / CLOCKS_PER_SEC;
    currentFrame /= 1. / FPS;
    return (uint16_t) currentFrame % frames;
}

// Returns created UIanimation struct on stack
Animation CreateAnimation(const char * path, const uint8_t targetFPS) 
{
    Animation animationPlaceholder = (Animation) { GetFrameAmount(path), targetFPS, 0, NULL};
    animationPlaceholder.Frames = malloc(animationPlaceholder.Amount * sizeof(Texture2D));
    uint16_t pathLength = strlen(path);
    char testPath[pathLength+10];
    strcpy(testPath, path);

    uint16_t frameNumber = 0;
    FILE * previousFileHandle = NULL;
    
    do 
    {
        if (previousFileHandle) fclose(previousFileHandle);
        testPath[pathLength] = '\0';
        strcpy(testPath + pathLength, uintstr(frameNumber));
        strcat(testPath, ".png");
        animationPlaceholder.Frames[frameNumber] = LoadTexture(testPath);
        printf("%d, %d\n", animationPlaceholder.Frames[frameNumber].width, animationPlaceholder.Frames[frameNumber].height);
        SetTextureFilter(animationPlaceholder.Frames[frameNumber], TEXTURE_FILTER_BILINEAR);
        frameNumber++;

    } while (frameNumber < animationPlaceholder.Amount);
    return animationPlaceholder;
}

// Free a UIanimation's variables
void FreeAnimation(Animation * animation)
{
    for (uint16_t i = 0; i < animation -> Amount; i++) UnloadTexture(animation -> Frames[i]);
    free(animation -> Frames);
}

// Returns a clock_t in seconds
static clock_t ClockSeconds(clock_t time) 
{
    return (time / CLOCKS_PER_SEC);
}

// Scales and Renders a UIanimation
void RenderAnimation(const Animation * animation, float x, float y, float scale, clock_t timeOverride) 
{
    static uint16_t windowWidth = 0;
    static uint16_t windowHeight = 0;

    if (GetScreenWidth() != windowWidth) windowWidth = GetScreenWidth();
    if (GetScreenHeight() != windowHeight) windowHeight = GetScreenHeight();

    float currentFrame = timeOverride ? 
        GetCurrentAnimationFrameC(timeOverride, animation -> Amount, animation -> FPS) 
        : GetCurrentAnimationFrame(animation);

    RenderUITexture(animation -> Frames[(uint16_t)currentFrame], x, y, scale);
}

// Animation V2 functions

// Returns created UIanimationV2 struct on stack
Animation_V2 CreateAnimation_V2(const char * path, const uint8_t targetFPS, const uint16_t amount, const uint16_t tileSize_x, const uint16_t tileSize_y)
{
    Animation_V2 animation = {0};
    animation.Atlas = LoadTexture(path);
    SetTextureFilter(animation.Atlas, TEXTURE_FILTER_BILINEAR);
    animation.Amount = amount;
    animation.Clock = clock();
    animation.FPS = targetFPS;
    animation.TileSize_x = tileSize_x;
    animation.TileSize_y = tileSize_y;
    return animation;
}

// Draws a UIanimationV2 in pixel space
void DrawAnimation_V2Ex(const Animation_V2 *animation, int16_t x, int16_t y, float scale, float rotation, clock_t timeOverride)
{
    uint16_t frame = timeOverride ? timeOverride : 
                                    GetCurrentAnimationFrameC(  animation -> Clock,
                                                                animation -> Amount, 
                                                                animation -> FPS);
    printf("\n%f | ", rotation);                                               
    DrawUITextureSpritesheetPro(   animation -> Atlas, 
                                x, y, 
                                frame, 
                                (Vector2) {animation -> TileSize_x, animation -> TileSize_y}, 
                                scale, rotation,
                                WHITE   );
}

// Draws a UIanimationV2 in pixel space
void DrawAnimation_V2(const Animation_V2 *animation, int16_t x, int16_t y, float scale, clock_t timeOverride)
{
    DrawAnimation_V2Ex(animation, x, y, scale, 0, timeOverride);
}

// Scales and Renders a UIanimationV2 in UI space
void RenderAnimation_V2Ex(const Animation_V2 *animation, float x, float y, float scale, float rotation, clock_t timeOverride)
{
    scale *= GetScreenScale();
    printf("RenderAnimation_V2Ex = %f | ", rotation);
    DrawAnimation_V2Ex(animation, 
        SCREEN_POSITION_TO_PIXEL_X(x, animation -> TileSize_x, scale),
        SCREEN_POSITION_TO_PIXEL_Y(y, animation -> TileSize_y, scale),
        scale, rotation, 0);
}

// Scales and Renders a UIanimationV2 in UI space
void RenderAnimation_V2(const Animation_V2 *animation, float x, float y, float scale, clock_t timeOverride)
{
    scale *= GetScreenScale();
    DrawAnimation_V2(animation, 
        SCREEN_POSITION_TO_PIXEL_X(x, animation -> TileSize_x, scale),
        SCREEN_POSITION_TO_PIXEL_Y(y, animation -> TileSize_y, scale),
        scale, 0);
}

// Free a UIanimationV2's variables
void FreeAnimation_V2(Animation_V2 * animation)
{
    UnloadTexture(animation -> Atlas);
}