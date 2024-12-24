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
#include "World.h"
#include "Settings.h"
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>


_Bool EncounterError = 0;

// The intermediate tile format used during the layer parsing process
typedef struct intermediate_tile 
{
    uint16_t x;
    uint16_t y;
    uint16_t textureID;
} intermediate_tile;

// Potential error codes to be used in void ErrorEncountered(enum ErrorIDs id)
enum ErrorIDs
{
    NOERRMESSAGE, BADPATH, BADOBJECT, FMALLOC, BTILE
};

// Prints error message and toggles EncounterError boolean
void ErrorEncountered(enum ErrorIDs id)
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
        case BTILE:
            printf("Invalid Spritefusion Tile JSON!\n");
            break;
        case NOERRMESSAGE:
            break;
    }
    EncounterError = 1;
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

// Checks if a layer has object (jsonFlag) and gets value, if 1, turns on (bitflag) at a WORLDTilemapLayer destination
void InitLayerFlag(WORLDTilemapLayer * dest, const cJSON * layer, const char * jsonFlag, uint8_t bitflag)
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

// Checks if the name of a layer starts with (jsonFlag), if so, turns on (bitflag) at a WORLDTilemapLayer destination
void InitLayerFlagFromName(WORLDTilemapLayer * dest, const cJSON * layer, const char * jsonFlag, uint8_t bitflag)
{
    uint8_t flag = bitflag;

    cJSON * object = cJSON_GetObjectItemCaseSensitive(layer, "name");

    if (!object)
    {
        printf("Invalid Layer, no name: \"%s\"!\n", jsonFlag);
        return;
    }
    flag *= strstcmp(object -> valuestring, jsonFlag);
    if (flag && !MinimalPrinting) printf("Layer %s Is %s\n", object->valuestring, jsonFlag);
    cJSON_free(object);
    dest -> FLAGS |= flag;
}

// Returns the cJSON of a path
cJSON * InitWorldJSON(const char * path)
{
    FILE * jsonFile = fopen(path, "r"); // Opens JSON file
    if (!jsonFile) 
    {
        ErrorEncountered(BADPATH); 
        return NULL;
    }

    // Getting file size

    fseek(jsonFile, 0L, SEEK_END);
    size_t jsonTextLength = ftell(jsonFile);
    rewind(jsonFile);

    // Creating copy in heap
    char * jsonText = malloc(jsonTextLength);
    if (!jsonText) ErrorEncountered(FMALLOC);
    fread(jsonText, 1, jsonTextLength, jsonFile);
    fclose(jsonFile);

    // Parses tilemap json

    cJSON * jsonParsed =  cJSON_Parse(jsonText);

    free(jsonText);
    cJSON_Print(jsonParsed);
    return jsonParsed;
}

// Gets number of layers in a Spritefusion map JSON
uint32_t GetWorldLayers(const cJSON * json)
{
    cJSON * layers = cJSON_GetObjectItemCaseSensitive(json,"layers"); 
    uint32_t numberOfLayer = cJSON_GetArraySize(layers);
    cJSON_free(layers);
    return numberOfLayer;
}

// Converts a string to a unsigned 32-bit integer
uint32_t strtou32(char * str)
{
    uint32_t u32 = 0;
    while (*str >= '0' && *str <= '9') u32 *= 10, u32 += *str - '0', str++;
    return u32;
}

// Parses a Spritefusion tile JSON and converts it to an intermediate_tile
intermediate_tile ParseJSONTile(const cJSON * tileJSON)
{
    // Gets all tile JSON parameters

    cJSON * jsonID = cJSON_GetObjectItemCaseSensitive(tileJSON, "id");
    cJSON * jsonX = cJSON_GetObjectItemCaseSensitive(tileJSON, "x");
    cJSON * jsonY = cJSON_GetObjectItemCaseSensitive(tileJSON, "y");

    // Checks if parameters are valid

    if (jsonID == NULL)
    {   
        ErrorEncountered(BTILE);
        return (intermediate_tile) {0};
    }
    
    if (!jsonX)
    {
        ErrorEncountered(BTILE);
        return (intermediate_tile) {0};
    }

    if (!jsonY)
    {
        ErrorEncountered(BTILE);
        return (intermediate_tile) {0};
    }

    // Gets parameters values

    char * stringID = cJSON_GetStringValue(jsonID);
    uint32_t textureID = strtou32(stringID);
    uint16_t x = jsonX -> valueint;
    uint16_t y = jsonY -> valueint;

    // Frees JSON objects

    cJSON_free(jsonID);
    cJSON_free(jsonX);
    cJSON_free(jsonY);

    // Returns intermediate tile

    return (intermediate_tile) {x, y, textureID};
}

// Prints an intermediate_tile
void PrintIntermediateTile(intermediate_tile tile)
{
    printf("X = %u, Y = %u, ID = %u\n", tile.x, tile.y, tile.textureID);
}

// Prints a WORLDTilemapLayer (for debugging)
void PrintLayer(WORLDTilemapLayer * layer)
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

// Parses a Spritefusion layer JSON to a WORLDTilemapLayer destination
void InitTitlemapLayer(WORLDTilemapLayer * dest, const cJSON * layerJSON)
{
    // Getting tile array

    cJSON * tiles = cJSON_GetObjectItemCaseSensitive(layerJSON, "tiles");
    if (!tiles)
    {
        ErrorEncountered(BADOBJECT);
        return;
    } 

    // Creating variables

    uint32_t AmountOfTiles = cJSON_GetArraySize(tiles);
    if (!MinimalPrinting) printf("There are %u tiles\n", AmountOfTiles);

    uint16_t OffsetX = UINT16_MAX;
    uint16_t OffsetY = UINT16_MAX;

    uint16_t SizeX = 0;
    uint16_t SizeY = 0;

    intermediate_tile * intermediate_tiles = malloc(sizeof(intermediate_tile) * AmountOfTiles);
    if (!intermediate_tiles)
    {
        ErrorEncountered(FMALLOC);
    } 

    // Getting layer dimensions

    for (uint16_t i = 0; i < AmountOfTiles; i++)
    {
        cJSON * TileJSON = cJSON_GetArrayItem(tiles, i);
        intermediate_tiles[i] = ParseJSONTile(TileJSON);
        //cJSON_free(TileJSON); Off due to freeing crash possibility (not sure why)

        if (OffsetX > intermediate_tiles[i].x) OffsetX = intermediate_tiles[i].x;
        if (SizeX < intermediate_tiles[i].x) SizeX = intermediate_tiles[i].x;

        if (OffsetY > intermediate_tiles[i].y) OffsetY = intermediate_tiles[i].y;
        if (SizeY < intermediate_tiles[i].y) SizeY = intermediate_tiles[i].y;
    }

    if (EncounterError) return;

    // Offseting size

    SizeX -= OffsetX - 1;
    SizeY -= OffsetY - 1;

    if (!MinimalPrinting) printf("    Offset: %u, %u | Size: %u, %u\n", OffsetX, OffsetY, SizeX, SizeY);
    // Creating 2D Map array

    WORLDTile (*map)[SizeY][SizeX] = malloc(sizeof(WORLDTile) * SizeX * SizeY);
    memset(map, 0, sizeof(WORLDTile) * SizeX * SizeY);

    if (!map) 
    {
        printf("Failed to allocate %lluB in heap!", sizeof(WORLDTile) * SizeX * SizeY);
        ErrorEncountered(NOERRMESSAGE);
        return;
    }

    // Filling map array

    for (uint16_t i = 0; i < AmountOfTiles; i++)
    {
        (*map)[intermediate_tiles[i].y - OffsetY][intermediate_tiles[i].x - OffsetX] = intermediate_tiles[i].textureID + 1;
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

// Returns the address of a parsed tilemap based on a Spritefusion map JSON
WORLDTilemap * CreateTilemap(const char * jsonPath)
{
    // Resets Error Detection

    EncounterError = 0;

    // Gets main cJSON object

    cJSON * MainJSON = InitWorldJSON(jsonPath);

    if (!MainJSON)
    {
        printf("Invalid Path \"%s\"!\n", jsonPath);
        ErrorEncountered(NOERRMESSAGE);
        return NULL;
    }
    
    // Gets layer JSON

    cJSON * LayersJSON = cJSON_GetObjectItemCaseSensitive(MainJSON, "layers");

    if (!LayersJSON)
    {
        printf("Invalid Spritefusion map (couldn't find layer JSON)!\n");
        ErrorEncountered(NOERRMESSAGE);
        return NULL;
    }

    // Gets map size to be potentially used for camera collisions (with the void)

    cJSON * SizeXJSON = cJSON_GetObjectItemCaseSensitive(MainJSON, "mapWidth");
    cJSON * SizeYJSON = cJSON_GetObjectItemCaseSensitive(MainJSON, "mapWidth");

    uint16_t mapWidth = SizeXJSON ? SizeXJSON -> valueint : UINT16_MAX;
    uint16_t mapHeight = SizeYJSON ? SizeYJSON -> valueint : UINT16_MAX;

    uint16_t AmountOfLayers = cJSON_GetArraySize(LayersJSON);

    // Allocates tilemap struct and layer memory

    WORLDTilemap * tilemap = malloc(sizeof(WORLDTilemap));
    tilemap -> layers = malloc(sizeof(WORLDTilemapLayer) * AmountOfLayers);
    tilemap -> amount = AmountOfLayers;
    tilemap -> mapWidth = mapWidth;
    tilemap -> mapHeight = mapHeight;
    
    // Gets if the layer object exists to test map JSON valitidy

    if (LayersJSON == NULL)
    {
        printf("Invalid Map JSON \"%s\"!\n", jsonPath);
        ErrorEncountered(NOERRMESSAGE);
        return NULL;
    }

    // Parses all layers and converts them to 2D uint16_t arrays
    for (uint16_t i = 0; i < AmountOfLayers; i++)
    {
        if (!MinimalPrinting) printf("%p, %s\n", cJSON_GetArrayItem(LayersJSON,i), cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(LayersJSON, i), "name") -> valuestring);
        InitTitlemapLayer(tilemap -> layers + i, cJSON_GetArrayItem(LayersJSON,i));
        if (EncounterError) return NULL;
    }
    return tilemap;
}