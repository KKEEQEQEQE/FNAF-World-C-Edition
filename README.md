### Update 2025-07-16
I've decided to make the repo for the original code for the game to be archived because this codebase is incredibly messy and unorganized. This original repo is only here for history, even though the original code is not great and some parts should be rewritten and redesigned.

### Update 2025-03-15
I'm no longer making any commits, HOWEVER, I will check and accept pull requests, and instead of this project being "solo"-ed by me, hopefully it will become a FNaF community project. I might also transfer ownership of this repo to someone else because I don't really know how to "maintain" a repo. If this project is successful I'll definately update the Gamejolt Page (https://gamejolt.com/games/fnaf-world-c-edition/963862). The src right now as of me making this public is messy, unorganized and could use more comments to be honest (mostly because I'm a C noob). I didn't want the src and couple months I worked on this project to go down the drain and I still think that something like FNaF World C Edition should be a thing so I'm making what I have public. I've mostly just lost interest in the project because after working on the same thing for a couple of months, you want to try something new. I still might come back in the future with more commits from myself but probably not in the near future. Scott if you want me to remove the assets folders that's a ok (it has the textures from the original game so running the game is easier).

DISCLAIMER: This is a free and open-source, fan-made passion project made by a FNaF Fan for the FNaF Fans. It is a recreation of FNaF World. I don't own the sprites, music, or original game, what I own is the code (which I put under the Zlib License), feel free-to-use it, you don't need any permission from me!

Also, the source code will be made public when Demo 1 is released because it is a little messy right now and needs some polishing, as well as some comments.

# About the Game:
FNaF World: C Edition is a completely open-source recreation of FNaF World in C and Raylib for the purpose of being multi-platform (so that the game can run on Mac, Windows, Android, iOS (this Safari similarly to Sonic 3 A.I.R), Linux, your web browser, etc instead of just Windows) and having a couple of quality-of-life features such as proper widescreen support (in fact you can run the native perfectly at any aspect ratio), windowed mode, etc. It will be easy to modify the game (if you want to make mods but that isn't the main game of this remake). This is a recreation from entirely scratch (except for the original sprites and music). This version of the game doesn't contain ANY source code from the original game and is made in pure C with Raylib instead of Clickteam Fusion, so there will be major performance increases and use significantly less RAM (never more than 100 MB worst case scenario). Although the recreation aims for it to be 1 to 1 (in terms of gameplay and graphics) there is bound to be at least 1 or 2 minor differences, nobody is perfect ¯\\_(ツ)_/¯

Right now the src only supports Windows but it shouldn't be hard to port it

## Gamejolt Page
- https://gamejolt.com/games/fnaf-world-c-edition/963862
  
# Licenses

FNaF World: C Edition:
Copyright (c) 2024-2025 SpyterDev

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

Raylib:
Copyright (c) 2013-2025 Ramon Santamaria (@raysan5)

This software is provided "as-is", without any express or implied warranty. In no event 
will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim that you 
  wrote the original software. If you use this software in a product, an acknowledgment 
  in the product documentation would be appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be misrepresented
  as being the original software.

  3. This notice may not be removed or altered from any source distribution.

cJSON:
Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

# Credits
- SpyterDev: Main developer and started the project
- Raysan5, and the Raylib community and contributors: Created the absolutely goated Raylib (https://github.com/raysan5/raylib) library. Without this the project wouldn't be a thing! Definately check it out if you want to make a game in C or any other language.
- DaveGamble and the cJSON contributors: Created cJSON, the library used to load and save json files. This is used to load the map, dialogue, and saves. A critical part of the game.
