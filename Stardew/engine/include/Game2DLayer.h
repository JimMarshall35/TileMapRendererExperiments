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


struct Transform2D
{
	vec2 position;
	vec2 scale;
	float rotation;
	bool bMirrored;
};

struct TilemapRenderData;
struct TilemapLayerRenderData;

struct TileMapLayer
{
	struct TilemapLayerRenderData* pRenderData;
	struct Transform2D transform;
	int widthTiles;
	int heightTiles;
	TileIndex* Tiles; 
};

struct TileMap
{
	VECTOR(struct TileMapLayer) layers;
	char* dataFilePath;
	struct TilemapRenderData* pRenderData;
};

struct GameLayer2DData
{
	hAtlas hAtlas;
	struct TileMap tilemap;
	bool bLoaded;


	struct Transform2D camera;

	VECTOR(struct Worldspace2DVert) pWorldspaceVertices;
	VECTOR(VertIndexT) pWorldspaceIndices;
	H2DWorldspaceVertexBuffer vertexBuffer;
};

struct Game2DLayerOptions
{
	const char* atlasFilePath;
	
	const char* tilemapFilePath;
	
	/*
		Each entity file is an array of entities.
		Entities in all files contribute to a single population of entities in the game.
	*/
	const** entitiesFilePaths;
	int numEntityFiles;
	
};

struct GameFrameworkLayer;
struct DrawContext;
typedef struct DrawContext DrawContext;

void Game2DLayer_Get(struct GameFrameworkLayer* pLayer, struct Game2DLayerOptions* pOptions, DrawContext* pDC);

#ifdef __cplusplus
}
#endif

#endif // !H_GAME2DLAYER
