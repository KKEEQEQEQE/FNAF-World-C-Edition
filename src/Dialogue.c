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

#include "../Include/cJSON.h"
#include "../Include/raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "Animation.h"
#include "Background.h"
#include "Battle_Rework.h"
#include "UI.h"
#include "World.h"
#include <stdlib.h>
#include <time.h>
#include "Entity_Info.h"
#include "Game_State.h"
#include "rayclock.h"

clock_t DialogueClock = 0;
#define MAX_LINES 255

uint8_t AmountOfLines_1 = 0;
uint8_t AmountOfLines_2 = 0;

typedef struct _DialogueLine
{
    char * line;
    _Bool speaker; // false = 1, true = 2
    float rate; // Characters Per Second 
} _DialogueLine;

_DialogueLine phase_1_lines[MAX_LINES] = {0};
_DialogueLine phase_2_lines[MAX_LINES] = {0};

// The Entity ID referenced for drawing/rendering the 1st entity (speaker) in Phase 1 Dialogue
enum ENTITY_IDs IdEntity1Phase1 = 0;

// The Entity ID referenced for drawing/rendering the 1st entity (speaker) in Phase 2 Dialogue
enum ENTITY_IDs IdEntity1Phase2 = 0;

// The Entity ID referenced for drawing/rendering the 2st entity (listener), usually Freddy
enum ENTITY_IDs IdEntity2 = FREDDY;

enum ENTITY_IDs DialogueIdToEntityId(uint8_t id)
{
    return id ? id + NUMBER_OF_ENEMIES : UNKNOWN;
}

_AdditionUpdater dialogue_addition_updater[MAX_ADDITIONAL_UPDATERS_GS];

// For freeing purposes
static cJSON * dialogue_json = NULL;

void FreeDialougeLines(void)
{
    cJSON_free(dialogue_json);
}


_DialogueLine ParseDialogueLine(cJSON * line)
{
    cJSON * string_line = cJSON_GetObjectItem(line, "Line");
    cJSON * speaker = cJSON_GetObjectItem(line, "Speaker");
    cJSON * chars_per_sec = cJSON_GetObjectItem(line, "Chars_Per_Sec");
    return (_DialogueLine) 
    {
        .line = string_line ? string_line->valuestring : NULL,
        .speaker = speaker ? (uint8_t) cJSON_GetNumberValue(speaker) : 0,
        .rate = chars_per_sec ? (float) cJSON_GetNumberValue(chars_per_sec) : 0,
    };
}

void LoadDialogue(const char * path)
{
    if (!FileExists(path)) 
    {
        printf("Dialogue JSON: \"%s\" | Does Not Exist!\n", path);
        return;
    }

    char * dialogue_json_raw = LoadFileText(path);
    cJSON * dialogue_json = cJSON_Parse(dialogue_json_raw);

    cJSON * Character_1_Phase_1 = cJSON_GetObjectItem(dialogue_json, "Character_1_Phase_1");
    cJSON * Character_1_Phase_2 = cJSON_GetObjectItem(dialogue_json, "Character_1_Phase_2");
    cJSON * Character_2 = cJSON_GetObjectItem(dialogue_json, "Character_2");
    cJSON * Phase_2_Dialogue_Delay = cJSON_GetObjectItem(dialogue_json, "Phase_2_Dialogue_Delay");
    cJSON * Phase_1_Dialogue = cJSON_GetObjectItem(dialogue_json, "Phase_1_Dialogue");
    cJSON * Phase_2_Dialogue = cJSON_GetObjectItem(dialogue_json, "Phase_2_Dialogue");

#define CONVERTID(x) DialogueIdToEntityId(x)

    IdEntity1Phase1 = Character_1_Phase_1 ? CONVERTID(cJSON_GetNumberValue(Character_1_Phase_1)) : 0;
    IdEntity1Phase2 = Character_1_Phase_2 ? CONVERTID(cJSON_GetNumberValue(Character_1_Phase_2)) : 0;

    IdEntity2 = Character_2 ? CONVERTID(cJSON_GetNumberValue(Character_2)) : FREDDY;
    printf("%d\n", IdEntity2); 

#undef CONVERTID

    AmountOfLines_1 = Phase_1_Dialogue ? cJSON_GetArraySize(Phase_1_Dialogue) : 0;

    for (uint8_t i = 0; i < AmountOfLines_1; i++)
    {
        phase_1_lines[i] = ParseDialogueLine(cJSON_GetArrayItem(Phase_1_Dialogue, i));
    }

    AmountOfLines_2 = Phase_2_Dialogue ? cJSON_GetArraySize(Phase_2_Dialogue) : 0;

    for (uint8_t i = 0; i < AmountOfLines_2; i++)
    {
        phase_2_lines[i] = ParseDialogueLine(cJSON_GetArrayItem(Phase_2_Dialogue, i));
    }

    free(dialogue_json_raw);
}

void PrintDialogue(void)
{
    for (uint8_t i = 0; i < MAX_LINES; i++)
    {
        if (!phase_1_lines[i].line) break;
        enum ENTITY_IDs speaker = !(phase_1_lines[i].speaker - 1) ? IdEntity1Phase1 : IdEntity2;
        printf("%s: \"%s\"\n", GetEntityName(speaker), phase_1_lines[i].line);
    }
}

Texture2D BackgroundImage = {0};
UIElement VisualComponents[5] = {0};
Music DialogueTheme = {0};

void InitDialogueScene(void)
{
    if (!DialogueTheme.looping) 
    {
        DialogueTheme = LoadMusicStream("Assets/Themes/yappen.mp3");
        DialogueTheme.looping = 1;
    }

    SeekMusicStream(DialogueTheme, 0);

    switch ((enum WORLDZONES) (GetZone() + 1))
    {
        case FAZBEARHILLS:
            BackgroundImage = LoadTexture("Assets/Battle/Background/Fazbear_Hills.png");
            break;
        case CHOPPYSWOODS:
            BackgroundImage = LoadTexture("Assets/Battle/Background/Choppys_Woods.png");;
            break;
        case DUSTINGFIELDS:
            BackgroundImage = LoadTexture("Assets/Battle/Background/Dusting_Fields.png");
            break;
        case MYSTERIOUSMINES:
            BackgroundImage = LoadTexture("Assets/Battle/Background/Mines.png");;
            break;
    }

    SetTextureFilter(BackgroundImage, TEXTURE_FILTER_BILINEAR);
    PlayMusicStream(DialogueTheme);
}

void FreeDialogueScene(void)
{
    FreeDialougeLines();
    UnloadTexture(BackgroundImage);
    StopMusicStream(DialogueTheme);
}
void RenderDialogue(void)
{
    RenderBackground(BackgroundImage);
    Animation_V2 E1 = AccessEntityAnimation(IdEntity1Phase1, 0);
    RenderAnimation_V2(&E1, -0.70, 0.25, 1.5, 0);

    Animation_V2 E2 = AccessEntityAnimation(IdEntity2, 0);
    RenderAnimation_V2(&E2, 0.70, 0.25, 1.5, 0);
}

void UpdateDialouge(void)
{
    UpdateMusicStream(DialogueTheme);
}

void PutDialogue(void)
{
    RenderDialogue();
    UpdateDialouge();
}

_GameStateScene Dialogue_Scene =
{
    .SceneClock = &DialogueClock,

    .RenderScene = RenderDialogue,
    .UpdateScene = UpdateDialouge,

    .FreeScene = FreeDialougeLines
};