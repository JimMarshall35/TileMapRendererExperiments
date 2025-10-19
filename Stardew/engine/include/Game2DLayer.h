#ifndef H_GAME2DLAYER
#define H_GAME2DLAYER

#ifdef __cplusplus
extern "C" {
#endif

#include "IntTypes.h"
#include "DynArray.h"
#include "HandleDefs.h"
#include "cglm/cglm.h"
#include "InputContext.h"
#include "FreeLookCameraMode.h"

#define MAX_GAME_LAYER_ASSET_FILE_PATH_LEN 128

typedef struct InputMapping InputMapping;

struct GameFrameworkEventListener;

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

enum ObjectLayer2DDrawOrder
{
	DrawOrder_TopDown,
	DrawOrder_Index	
};

struct TileMapLayer
{
	struct TilemapLayerRenderData* pRenderData;
	struct Transform2D transform;
	int tileWidthPx;
	int tileHeightPx;
	int widthTiles;
	int heightTiles;
	bool bIsObjectLayer;
	enum ObjectLayer2DDrawOrder drawOrder;
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
	HEntity2DQuadtreeNode hEntitiesQuadTree;
	hAtlas hAtlas;
	struct TileMap tilemap;
	bool bLoaded;

	/*
		When in this mode the game is paused and you have
		a different set of controls to freely move the camera around,
		in future I might add editing
	*/
	bool bFreeLookMode;

	struct Transform2D camera;

	struct FreeLookCameraModeControls freeLookCtrls;

	VECTOR(struct Worldspace2DVert) pWorldspaceVertices;
	VECTOR(VertIndexT) pWorldspaceIndices;
	H2DWorldspaceVertexBuffer vertexBuffer;

	char atlasFilePath[MAX_GAME_LAYER_ASSET_FILE_PATH_LEN];
	char tilemapFilePath[MAX_GAME_LAYER_ASSET_FILE_PATH_LEN];

	bool bDebugLayerAttatched;
	char debugMsg[256];
	struct GameFrameworkEventListener* pDebugListener;

	int windowW;
	int windowH;
};

struct Game2DLayerOptions
{
	/* 
		if true, the data for the layer will load immediately,
		as soon as Game2DLayer_Get is called, if false it will
		load when the layer is pushed.
	*/
	bool loadImmediatly;

	const char* atlasFilePath;
	
	const char* tilemapFilePath;
	
	/*
		Each entity file is an array of entities.
		Entities in all files contribute to a single population of entities in the game.
	*/
	const char** entitiesFilePaths;
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
