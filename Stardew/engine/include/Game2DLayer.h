#ifndef H_GAME2DLAYER
#define XMLUI_GAME_LAYER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "IntTypes.h"
#include "DynArray.h"
#include "HandleDefs.h"
#include "cglm/cglm.h"

// the real type of this should be hSprite ie u32 but i want to save memory so u16 it is - that 
// should be enough for anyone - just store the tiles in the first 16 bits worth of indexes
typedef u16 TileIndex; 

struct Transform2D
{
	vec2 position;
	vec2 scale;
	float rotation;
	bool bMirrored;
};

struct TilemapRenderData;

struct TileMapLayer
{
	struct TilemapRenderData* pRenderData;
	struct Transform2D transform;
	int widthTiles;
	int heightTiles;
	TileIndex* Tiles; 
};

struct TileMap
{
	VECTOR(struct TileMapLayer) layers;
	char* dataFilePath;
};

struct GameLayer2DData
{
	hAtlas hAtlas;
	struct TileMap tilemap;
};

struct Game2DLayerOptions
{
	const char* atlasFilePath;
	
	/*
		One file can contain several layers, but multiple files can be loaded.
		Each file is an array of layers, and when multiple files are loaded together,
		the layers stack up into the tilemap in order:
		
		totalLayers = numLayersInFile1 + numLayersInFile2 + numLayersInFile3 ...
	*/
	const char** tilemapFilePaths;
	int numTilemapFiles;
	
	/*
		Each entity file is an array of entities.
		Entities in all files contribute to a single population of entities in the game.
	*/
	const** entitiesFilePaths;
	int numEntityFiles;
	
};

struct GameFrameworkLayer;

void Game2DLayer_Get(struct GameFrameworkLayer* pLayer, struct Game2DLayerOptions* pOptions);

#ifdef __cplusplus
}
#endif

#endif // !H_GAME2DLAYER
