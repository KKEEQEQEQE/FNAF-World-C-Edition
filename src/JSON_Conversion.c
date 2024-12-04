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

#include "../Include/cJSON.h"
#include "World.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define LAYER_COLLIDABLE 1
#define LAYER_INVISIBLE 2

typedef struct tilemap_layer 
{
    // Transformation Variables

    uint16_t offsetX;
    uint16_t offsetY;
    uint16_t sizeX;
    uint16_t sizeY;

    void * tiles; // WORLDTile 2D Array Pointer

    // Flags

    uint8_t FLAGS;
} tilemap_layer;

typedef struct WORLDTilemap
{
    tilemap_layer * layers;
} WORLDTilemap;

typedef struct json_tile 
{
    uint16_t x;
    uint16_t y;
    uint16_t textureID;
} json_tile;

uint16_t CurrentTilemapSizeX;
uint16_t CurrentTilemapSizeY;

enum ConvertCrashID
{
    BADPATH, BADOBJECT, FMALLOC
};
void ConversionCrash(enum ConvertCrashID id)
{
    switch (id) 
    {

    case BADPATH:
        printf("Invalid JSON path!\n");
        break;
    case BADOBJECT:
        printf("Invalid object!\n");
        break;
    case FMALLOC:
        printf("Failed to allocate memory on heap!\n");
      break;
    }
    exit(EXIT_FAILURE);
}

// Returns 1 if the base string starts with the substring. I.E base = "Hi There\0", substring = "Hi\0", result = 1
uint8_t strstcmp(register const char * base, register const char * substring) {
    register uint64_t i = 0;
    while (base[i] == substring[i]) {
        if (base[i] == '\0' || substring[i] == '\0') break;
        i += 1;
    }
    if (substring[i] == '\0') return i;
    return 0;
}
void InitLayerFlag(tilemap_layer * dest, const cJSON * layer, const char * jsonFlag, uint8_t bitflag)
{
    uint8_t flag = bitflag;

    cJSON * object = cJSON_GetObjectItemCaseSensitive(layer, jsonFlag);

    if (!object)
    {
        printf("Invalid Layer Flag: \"%s\"!\n", jsonFlag);
        return;
    }
    flag *= object -> valueint;
    cJSON_free(object);
}

void InitLayerFlagFromName(tilemap_layer * dest, const cJSON * layer, const char * jsonFlag, uint8_t bitflag)
{
    uint8_t flag = bitflag;

    cJSON * object = cJSON_GetObjectItemCaseSensitive(layer, "name");

    if (!object)
    {
        printf("Invalid Layer, no name: \"%s\"!\n", jsonFlag);
        return;
    }
    flag *= strstcmp(object -> valuestring, jsonFlag);
    cJSON_free(object);
}

cJSON * InitWorldJSON(const char * path)
{
    FILE * jsonFile = fopen(path, "r"); // Opens JSON file
    if (!jsonFile) ConversionCrash(BADPATH); 

    // Getting file size

    fseek(jsonFile, 0L, SEEK_END);
    size_t jsonTextLength = ftell(jsonFile);
    rewind(jsonFile);

    // Creating copy in heap
    char * jsonText = malloc(jsonTextLength);
    if (!jsonText) ConversionCrash(FMALLOC);
    fread(jsonText, 1, jsonTextLength, jsonFile);
    fclose(jsonFile);

    // Parses tilemap json

    cJSON * jsonParsed =  cJSON_Parse(jsonText);

    free(jsonText);
    cJSON_Print(jsonParsed);
    return jsonParsed;
}

uint32_t GetWorldLayers(const cJSON * json)
{
    cJSON * layers = cJSON_GetObjectItemCaseSensitive(json,"layers"); 
    uint32_t numberOfLayer = cJSON_GetArraySize(layers);
    cJSON_free(layers);
    return numberOfLayer;
}
uint32_t strtou32(char * str)
{
    uint32_t u32 = 0;
    while (*str >= '0' && *str <= '9') u32 += *str - '0', u32 *= 10, str++;
    return u32;
}

json_tile ParseJSONTile(const cJSON * layerJSON)
{
    cJSON * jsonID = cJSON_GetObjectItemCaseSensitive(layerJSON, "id");
    if (jsonID == NULL)
    {
        printf("Invalid JSON Tile!\n");
        exit(EXIT_FAILURE);
    }
    

    char * stringID = cJSON_GetStringValue(jsonID);
    uint32_t textureID = strtou32(stringID);
    
    cJSON * jsonX = cJSON_GetObjectItemCaseSensitive(layerJSON, "x");
    if (!jsonX)
    {
        printf("Invalid JSON Tile!\n");
        exit(EXIT_FAILURE);
    }
    uint16_t x = jsonX -> valueint;

    cJSON * jsonY = cJSON_GetObjectItemCaseSensitive(layerJSON, "y");
    if (!jsonY)
    {
        printf("Invalid JSON Tile!\n");
        exit(EXIT_FAILURE);
    }
    uint16_t y = jsonX -> valueint;

    cJSON_free(jsonID);
    cJSON_free(jsonX);
    cJSON_free(jsonY);
    return (json_tile) {x, y, textureID};
}

void InitTitlemapLayer(tilemap_layer * dest, const cJSON * layerJSON)
{
    // Getting tile array

    cJSON * tiles = cJSON_GetObjectItemCaseSensitive(layerJSON, "tiles");
    if (!tiles) ConversionCrash(BADOBJECT);

    // Creating variables

    uint32_t AmountOfTiles = cJSON_GetArraySize(layerJSON);

    uint16_t OffsetX = UINT16_MAX;
    uint16_t OffsetY = UINT16_MAX;

    uint16_t FurtherestX = 0;
    uint16_t FurtherestY = 0;

    // Getting layer dimensions
    for (uint16_t i = 0; i < AmountOfTiles; i++)
    {
        cJSON * TileJSON = cJSON_GetArrayItem(tiles, i);
        json_tile CurrentTile = ParseJSONTile(TileJSON);

        cJSON_free(TileJSON);

        if (OffsetX > CurrentTile.x) OffsetX = CurrentTile.x;
        else if (FurtherestX < CurrentTile.x) FurtherestX = CurrentTile.x;

        if (OffsetY > CurrentTile.y) OffsetY = CurrentTile.y;
        else if (FurtherestY < CurrentTile.y) FurtherestY = CurrentTile.y;
    }

    FurtherestX -= OffsetX;
    FurtherestY -= OffsetY;
    
    // Creating 2D Map array

    WORLDTile (*map)[FurtherestY][FurtherestX] = malloc(sizeof(WORLDTile) * FurtherestX * FurtherestY);

    if (!map) 
    {
        printf("Failed to allocate %lluB in heap!", sizeof(WORLDTile) * FurtherestX * FurtherestY);
        exit(EXIT_FAILURE);
    }

    // Filling map array

    for (uint16_t i = 0; i < AmountOfTiles; i++)
    {
        cJSON * TileJSON = cJSON_GetArrayItem(tiles, i);
        if (!TileJSON) 
        {
            printf("Failed to get layer element!\n");
            exit(EXIT_FAILURE);
        }

        json_tile CurrentTile = ParseJSONTile(TileJSON);

        cJSON_free(TileJSON);
        (*map)[CurrentTile.y - OffsetY][CurrentTile.x - OffsetX] = CurrentTile.textureID + 1;
    }
    cJSON_free(tiles);

    // Setting flags

    dest -> tiles = map;
    dest -> offsetX = OffsetX;
    dest -> offsetY = OffsetY;
    dest -> sizeX = FurtherestX;
    dest -> sizeY = FurtherestY;

    InitLayerFlag(dest, layerJSON, "collider", LAYER_COLLIDABLE);
    InitLayerFlagFromName(dest, layerJSON, "inv_", LAYER_INVISIBLE);

}

void CreateTilemap(const char * jsonPath)
{
    cJSON * MainJSON = InitWorldJSON(jsonPath);
    if (!MainJSON)
    {
        printf("Invalid Path \"%s\"!\n", jsonPath);
        exit(EXIT_FAILURE);
    }
    uint16_t TilemapLayers = GetWorldLayers(MainJSON);
    WORLDTilemap * tilemap = malloc(sizeof(WORLDTilemap));
    tilemap -> layers = malloc(sizeof(tilemap_layer));
    
    cJSON * LayersJSON = cJSON_GetObjectItemCaseSensitive(MainJSON, "layers");

    if (LayersJSON == NULL)
    {
        printf("Invalid Map JSON \"%s\"!\n", jsonPath);
        exit(EXIT_FAILURE);
    }

    for (uint16_t i = 0; i < TilemapLayers; i++)
    {
        cJSON_Print(cJSON_GetArrayItem(LayersJSON,i));
        InitTitlemapLayer(tilemap -> layers + i, cJSON_GetArrayItem(LayersJSON,i));
    }
}