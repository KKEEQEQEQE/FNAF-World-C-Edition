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
#include "Settings.h"
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct parsed_tile 
{
    uint16_t x;
    uint16_t y;
    uint16_t textureID;
} parsed_tile;

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
    if (substring[i] == '\0') return 1;
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
    dest -> FLAGS |= flag;
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
    if (flag) printf("Layer %s Is %s\n", object->valuestring, jsonFlag);
    cJSON_free(object);
    dest -> FLAGS |= flag;
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
    return u32 / 10;
}

parsed_tile ParseJSONTile(const cJSON * tileJSON)
{
    cJSON * jsonID = cJSON_GetObjectItem(tileJSON, "id");
    if (jsonID == NULL)
    {   
        printf("Invalid JSON Tile!\n");
        exit(EXIT_FAILURE);
    }
    

    char * stringID = cJSON_GetStringValue(jsonID);
    uint32_t textureID = strtou32(stringID);
    
    cJSON * jsonX = cJSON_GetObjectItem(tileJSON, "x");
    if (!jsonX)
    {
        printf("Invalid JSON Tile!\n");
        exit(EXIT_FAILURE);
    }
    uint16_t x = jsonX -> valueint;

    cJSON * jsonY = cJSON_GetObjectItem(tileJSON, "y");
    if (!jsonY)
    {
        printf("Invalid JSON Tile!\n");
        exit(EXIT_FAILURE);
    }
    uint16_t y = jsonY -> valueint;

    cJSON_free(jsonID);
    cJSON_free(jsonX);
    cJSON_free(jsonY);
    return (parsed_tile) {x, y, textureID};
}

void PrintParsedTile(parsed_tile tile)
{
    printf("X = %u, Y = %u, ID = %u\n", tile.x, tile.y, tile.textureID);
}

void PrintLayer(tilemap_layer * layer)
{
    register WORLDTile (*tiles)[layer -> sizeY][layer -> sizeX] = layer -> tiles;
    for (uint16_t y = 0; y < layer->sizeY; y++) {
        for (uint16_t x = 0; x < layer->sizeX; x++)
        {
            if (x != 0) printf(", ");
            printf("%u", AccessPositionInLayer(x + layer -> offsetX, y + layer -> offsetY, layer));
        }
        printf("\n");
    }
    printf("\n\n");
}

void InitTitlemapLayer(tilemap_layer * dest, const cJSON * layerJSON)
{
    // Getting tile array

    cJSON * tiles = cJSON_GetObjectItemCaseSensitive(layerJSON, "tiles");
    if (!tiles) ConversionCrash(BADOBJECT);

    // Creating variables

    uint32_t AmountOfTiles = cJSON_GetArraySize(tiles);
    if (!MinimalPrinting) printf("There are %u tiles\n", AmountOfTiles);

    uint16_t OffsetX = UINT16_MAX;
    uint16_t OffsetY = UINT16_MAX;

    uint16_t SizeX = 0;
    uint16_t SizeY = 0;

    parsed_tile * parsed_tiles = malloc(sizeof(parsed_tile) * AmountOfTiles);
    if (!parsed_tiles) ConversionCrash(FMALLOC);

    // Getting layer dimensions
    for (uint16_t i = 0; i < AmountOfTiles; i++)
    {
        cJSON * TileJSON = cJSON_GetArrayItem(tiles, i);
        parsed_tiles[i] = ParseJSONTile(TileJSON);
        //cJSON_free(TileJSON);

        if (OffsetX > parsed_tiles[i].x) OffsetX = parsed_tiles[i].x;
        if (SizeX < parsed_tiles[i].x) SizeX = parsed_tiles[i].x;

        if (OffsetY > parsed_tiles[i].y) OffsetY = parsed_tiles[i].y;
        if (SizeY < parsed_tiles[i].y) SizeY = parsed_tiles[i].y;
    }

    SizeX -= OffsetX - 1;
    SizeY -= OffsetY - 1;
    if (!MinimalPrinting) printf("    Offset: %u, %u | Size: %u, %u\n", OffsetX, OffsetY, SizeX, SizeY);
    // Creating 2D Map array

    WORLDTile (*map)[SizeY][SizeX] = malloc(sizeof(WORLDTile) * SizeX * SizeY);
    memset(map, 0, sizeof(WORLDTile) * SizeX * SizeY);

    if (!map) 
    {
        if (!MinimalPrinting) printf("Failed to allocate %lluB in heap!", sizeof(WORLDTile) * SizeX * SizeY);
        exit(EXIT_FAILURE);
    }

    // Filling map array

    for (uint16_t i = 0; i < AmountOfTiles; i++)
    {
        if (AmountOfTiles == 5) PrintParsedTile(parsed_tiles[i]);

        (*map)[parsed_tiles[i].y - OffsetY][parsed_tiles[i].x - OffsetX] = parsed_tiles[i].textureID + 1;
    }

    cJSON_free(tiles);

    // Setting flags

    dest -> tiles = map;
    dest -> offsetX = OffsetX;
    dest -> offsetY = OffsetY;
    dest -> sizeX = SizeX;
    dest -> sizeY = SizeY;
    dest ->  FLAGS = 0;
    InitLayerFlag(dest, layerJSON, "collider", LAYER_COLLIDABLE);
    InitLayerFlagFromName(dest, layerJSON, "inv_", LAYER_INVISIBLE);

    if (!MinimalPrinting) PrintLayer(dest);
        
}

WORLDTilemap * CreateTilemap(const char * jsonPath)
{
    cJSON * MainJSON = InitWorldJSON(jsonPath);
    if (!MainJSON)
    {
        printf("Invalid Path \"%s\"!\n", jsonPath);
        exit(EXIT_FAILURE);
    }
    
    cJSON * LayersJSON = cJSON_GetObjectItemCaseSensitive(MainJSON, "layers");

    uint16_t AmountOfLayers = cJSON_GetArraySize(LayersJSON);

    WORLDTilemap * tilemap = malloc(sizeof(WORLDTilemap));
    tilemap -> layers = malloc(sizeof(tilemap_layer) * AmountOfLayers);
    tilemap -> amount = AmountOfLayers;
    

    if (LayersJSON == NULL)
    {
        printf("Invalid Map JSON \"%s\"!\n", jsonPath);
        exit(EXIT_FAILURE);
    }

    for (uint16_t i = 0; i < AmountOfLayers; i++)
    {
        if (!MinimalPrinting) printf("%p, %s\n", cJSON_GetArrayItem(LayersJSON,i), cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(LayersJSON, i), "name") -> valuestring);
        InitTitlemapLayer(tilemap -> layers + i, cJSON_GetArrayItem(LayersJSON,i));
    }
    return tilemap;
}

