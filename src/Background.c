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

#include "Background.h"
#include <stdint.h>

void RenderBackground(Texture2D background)
{
    uint8_t widthGreater = (float) GetScreenWidth() / GetScreenHeight() >= (float) background.width / background.height;
    float scaleFactor = widthGreater ? (float) GetScreenWidth() / background.width : (float) GetScreenHeight() / background.height;
    DrawTextureEx(  background, 
                    (Vector2) {widthGreater ? 0 : GetScreenWidth() / 2 - background.width * scaleFactor / 2, 
                                        widthGreater ? GetScreenHeight() / 2 - background.height * scaleFactor / 2 : 0}, 
                    0, scaleFactor, WHITE);
}