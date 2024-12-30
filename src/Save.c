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
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../Include/raylib.h"
#include "Save.h"
#define INVALID_ANIMATRONIC ((struct Animatronic) {0,0,0})

typedef struct Selection
{
    uint8_t amount;
    uint8_t id[256];
} Selection;

// Save Information

    // The cJSON containing the entire save
    static cJSON * SaveJSON = {0};

    // Target path to save
    static char Selected_Save[1000]; 

// Zone Info info
    static cJSON * Zone_LevelJSON = {0};

// Faz_Token info
    static cJSON * Faz_TokenJSON = {0};

// Chips Info

    // Owned Chips array in the SaveJSON
    static cJSON * ChipsJSON = {0};

    // Selected Chips
    static cJSON * Selected_Chips;

// Byte Info

    // Owned Bytes array in the SaveJSON
    static cJSON * BytesJSON = {0};

    // Selected Bytes
    static cJSON * Selected_Bytes;

// Animatronic Info

    // Owned Animatronic array in the SaveJSON
    static cJSON * AnimatronicJSON = {0};

    // Selected Animatronic
    static cJSON * Party_1;
    static cJSON * Party_2;

// Last location info

static cJSON * Last_LocationJSON = {0};

static struct 
{
    cJSON * x;
    cJSON * y;
} Last_Location = {0};

// Returns the cJSON of a path
cJSON * cJSON_LoadJSON(const char * path)
{
    FILE * jsonFile = fopen(path, "r"); // Opens JSON file
    if (!jsonFile)  return NULL;

    // Getting file size

    fseek(jsonFile, 0L, SEEK_END);
    size_t jsonTextLength = ftell(jsonFile);
    rewind(jsonFile);

    // Creating copy in heap
    char * jsonText = malloc(jsonTextLength);
    if (!jsonText) return NULL;
    fread(jsonText, 1, jsonTextLength, jsonFile);
    fclose(jsonFile);

    // Parses tilemap json

    cJSON * jsonParsed =  cJSON_Parse(jsonText);

    free(jsonText);
    return jsonParsed;
}

void CreateSave(const char * path)
{
    // Sets current save path

    if (path) strcpy_s(Selected_Save, sizeof(Selected_Save), path);

    SaveJSON = cJSON_CreateObject();

    ChipsJSON = cJSON_AddArrayToObject(SaveJSON, "Chips");
    BytesJSON = cJSON_AddArrayToObject(SaveJSON, "Bytes");
    AnimatronicJSON = cJSON_AddArrayToObject(SaveJSON, "Animatronics");

    Selected_Chips = cJSON_AddArrayToObject(SaveJSON, "Selected_Chips");
    Selected_Bytes = cJSON_AddArrayToObject(SaveJSON, "Selected_Bytes");
    Party_1 = cJSON_AddArrayToObject(SaveJSON, "Party_1");
    Party_2 = cJSON_AddArrayToObject(SaveJSON, "Party_2");

    Zone_LevelJSON = cJSON_AddNumberToObject(SaveJSON, "Zone_Level", 1);
    cJSON_SetIntValue(Zone_LevelJSON, 1);

    Faz_TokenJSON = cJSON_AddNumberToObject(SaveJSON, "Faz-Tokens", 0);
    cJSON_SetIntValue(Faz_TokenJSON, 0);

    Last_LocationJSON = cJSON_AddObjectToObject(SaveJSON, "Last_Location");
    Last_Location.x = cJSON_AddNumberToObject(Last_LocationJSON, "x", 38);
    Last_Location.y = cJSON_AddNumberToObject(Last_LocationJSON, "y", 21);

    const char * directory = GetDirectoryPath(path);
    if (!DirectoryExists(directory)) MakeDirectory(directory);
    if (path) SaveFileText(path, cJSON_Print(SaveJSON));
}

void LoadSave(const char * path)
{
    if (!FileExists(path)) return CreateSave(path);
    
    // Loads all save values

    if (path) strcpy_s(Selected_Save, sizeof(Selected_Save), path);

    SaveJSON = cJSON_LoadJSON(path);
    printf("\nLoaded at %p\n", SaveJSON);
    if (!SaveJSON) return CreateSave(path);
    
    Zone_LevelJSON = cJSON_GetObjectItem(SaveJSON, "Zone_Level");
    Faz_TokenJSON = cJSON_GetObjectItem(SaveJSON, "Faz-Tokens");
    

    ChipsJSON = cJSON_GetObjectItem(SaveJSON, "Chips");
    Selected_Chips = cJSON_GetObjectItem(SaveJSON, "Selected_Chips");

    BytesJSON = cJSON_GetObjectItem(SaveJSON, "Bytes");
    Selected_Bytes = cJSON_GetObjectItem(SaveJSON, "Selected_Bytes");

    AnimatronicJSON = cJSON_GetObjectItem(SaveJSON, "Animatronics");
    Party_1 = cJSON_GetObjectItem(SaveJSON, "Party_1");
    Party_2 = cJSON_GetObjectItem(SaveJSON, "Party_2");

    Last_LocationJSON = cJSON_GetObjectItem(SaveJSON, "Last_Location");
    Last_Location.x = cJSON_GetObjectItem(Last_LocationJSON, "x");
    Last_Location.y = cJSON_GetObjectItem(Last_LocationJSON, "y");

    // Resets any missing save values
    
    if (!Zone_LevelJSON) Zone_LevelJSON = cJSON_AddNumberToObject(SaveJSON, "Zone_Level", 1), cJSON_SetIntValue(Zone_LevelJSON, 1);
    if (!Faz_TokenJSON) Faz_TokenJSON = cJSON_AddNumberToObject(SaveJSON, "Faz-Tokens", 0), cJSON_SetIntValue(Faz_TokenJSON, 0);
    if (!ChipsJSON) ChipsJSON = cJSON_AddArrayToObject(SaveJSON, "Chips");
    if (!Selected_Chips) Selected_Chips = cJSON_AddArrayToObject(SaveJSON, "Selected_Chips");
    if (!BytesJSON) BytesJSON = cJSON_AddArrayToObject(SaveJSON, "Bytes");
    if (!Selected_Bytes) Selected_Bytes = cJSON_AddArrayToObject(SaveJSON, "Selected_Bytes");
    if (!AnimatronicJSON) AnimatronicJSON = cJSON_AddArrayToObject(SaveJSON, "Animatronics");
    if (!Party_1) Party_1 = cJSON_AddArrayToObject(SaveJSON, "Party_1");
    if (!Party_2) Party_2 = cJSON_AddArrayToObject(SaveJSON, "Party_2");
    if (!Last_LocationJSON) Last_LocationJSON = cJSON_AddObjectToObject(SaveJSON, "Last_Location");
    if (!Last_Location.x) Last_Location.x = cJSON_AddNumberToObject(Last_LocationJSON, "x", 38);
    if (!Last_Location.y) Last_Location.y = cJSON_AddNumberToObject(Last_LocationJSON, "y", 21);
}

void WriteSave(Vector2 LastLocation)
{
    if (*Selected_Save == '\0') return;

    cJSON_SetIntValue(Last_Location.x, (uint16_t)LastLocation.x);
    printf("%u, %u\n", (uint16_t)LastLocation.x, (uint16_t)LastLocation.y);
    cJSON_SetIntValue(Last_Location.y, (uint16_t)LastLocation.y);

    SaveFileText(Selected_Save, cJSON_Print(SaveJSON));
}

uint8_t GetZone_Level(void)
{
    if (Zone_LevelJSON -> valueint > 3) 
    {
        cJSON_SetIntValue(Zone_LevelJSON, 3);
    }
    return Zone_LevelJSON -> valueint;
}

void SetZone_Level(uint8_t level)
{
    if (level > 3) level = 3;
    cJSON_SetIntValue(Zone_LevelJSON, level);
}

uint16_t GetFaz_Tokens(void)
{
    return Faz_TokenJSON -> valueint;
}

void UpdateFaz_Tokens(int16_t income)
{
    cJSON_SetIntValue(Faz_TokenJSON, GetFaz_Tokens() + income);
}

static void UpdateNumberArray(uint8_t index, uint8_t value, cJSON * array, uint8_t cap)
{
    if (index >= cap || !array) return;

    uint16_t size = cJSON_GetArraySize(array);
    if (index < size) 
    {
        cJSON * item = cJSON_GetArrayItem(array, index);
        if (!item) return;
        cJSON_SetIntValue(item, value);
        return;
    }
    
    for (uint16_t i = 0; i < size - index; i++) cJSON_AddItemToArray(array, cJSON_CreateNumber(0));
    cJSON_AddItemToArray(array, cJSON_CreateNumber(value));
}

Vector2 GetLast_Location(void)
{
    return (Vector2) {  (float) Last_Location.x -> valueint, 
                        (float) Last_Location.y -> valueint  };
}

void UpdateParty_1(uint8_t index, uint8_t id)
{
    if (id >= cJSON_GetArraySize(Party_1)) return;
    UpdateNumberArray(index, id, Party_1, 4);
}

void UpdateParty_2(uint8_t index, uint8_t id)
{
    if (id >= cJSON_GetArraySize(Party_2)) return;
    UpdateNumberArray(index, id, Party_2, 4);
}

void UpdateSelected_Chips(uint8_t index, uint8_t id)
{
    if (id >= cJSON_GetArraySize(ChipsJSON)) return;
    UpdateNumberArray(index, id, Selected_Chips, 4);
}

void UpdateSelected_Bytes(uint8_t index, uint8_t id)
{
    if (id >= cJSON_GetArraySize(BytesJSON)) return;
    UpdateNumberArray(index, id, Selected_Bytes, 4);
}

static void AddGameItem(cJSON * array, uint8_t id)
{
    cJSON * item = cJSON_CreateObject();
    if (!item) return;

    if (!cJSON_AddNumberToObject(item, "ID", id)) return;

    cJSON_AddItemToArray(array, item);
}

void AddChip(uint8_t id)
{
    AddGameItem(ChipsJSON, id);
}

void AddByte(uint8_t id)
{
    AddGameItem(BytesJSON, id);
}

void AddAnimatronic(uint8_t id)
{
    cJSON * item = cJSON_CreateObject();
    if (!item) return;

    if (!cJSON_AddNumberToObject(item, "ID", id)) return;
    if (!cJSON_AddNumberToObject(item, "Level", 1)) return;
    if (!cJSON_AddNumberToObject(item, "XP", 0)) return;

    cJSON_AddItemToArray(AnimatronicJSON, item);
}

void UpdateAnimatronic(uint8_t index, uint16_t level_surplus, uint32_t xp_surplus)
{
    if (index >= cJSON_GetArraySize(AnimatronicJSON)) return;
    
    cJSON * animatronic = cJSON_GetArrayItem(AnimatronicJSON, index);
    if (!animatronic) return;

    cJSON * levelJSON = cJSON_GetObjectItem(animatronic, "Level");
    cJSON * xpJSON = cJSON_GetObjectItem(animatronic, "XP");
    if (!levelJSON) levelJSON = cJSON_AddNumberToObject(animatronic, "Level", 1);
    if (!xpJSON) xpJSON = cJSON_AddNumberToObject(animatronic, "XP", 0);

    cJSON_SetNumberHelper(levelJSON, levelJSON -> valueint + level_surplus);
    cJSON_SetNumberHelper(xpJSON, xpJSON -> valueint + xp_surplus);
}

Animatronic GetAnimatronic(uint8_t index)
{
    if (index >= cJSON_GetArraySize(AnimatronicJSON)) return INVALID_ANIMATRONIC;
    
    cJSON * animatronic = cJSON_GetArrayItem(AnimatronicJSON, index);
    if (!animatronic) return INVALID_ANIMATRONIC;

    cJSON * idJSON = cJSON_GetObjectItem(animatronic, "ID");
    cJSON * levelJSON = cJSON_GetObjectItem(animatronic, "Level");
    cJSON * xpJSON = cJSON_GetObjectItem(animatronic, "XP");
    if (!idJSON) return INVALID_ANIMATRONIC;
    if (!levelJSON) levelJSON = cJSON_AddNumberToObject(animatronic, "Level", 1);
    if (!xpJSON) xpJSON = cJSON_AddNumberToObject(animatronic, "XP", 0);

    return (Animatronic) {   idJSON->valueint, 
                                    levelJSON -> valueint, 
                                    xpJSON -> valueint  };
}