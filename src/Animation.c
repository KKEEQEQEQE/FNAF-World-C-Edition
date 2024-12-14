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

// Returns created animation struct on stack
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

void FreeAnimation(Animation * animation)
{
    for (uint16_t i = 0; i < animation -> Amount; i++) UnloadTexture(animation -> Frames[i]);
    free(animation -> Frames);
}

clock_t ClockSeconds(clock_t time) {
    return (time / CLOCKS_PER_SEC);
}

float GetOutsideWindowX(Texture2D texture) {
    return 1 + (float)(texture.width)/(GetScreenWidth());
}

float GetOutsideWindowY(Texture2D texture) {
    return 1 + (float)(texture.height)/(GetScreenHeight());
}

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