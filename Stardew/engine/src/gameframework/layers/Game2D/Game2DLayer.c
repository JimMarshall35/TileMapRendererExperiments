#include "Game2DLayer.h"
#include "GameFramework.h"
#include "Atlas.h"
#include <stdlib.h>
#include <string.h>
#include "BinarySerializer.h"
#include "DrawContext.h"
#include "AssertLib.h"
#include "DynArray.h"
#include <cglm/cglm.h>

static void LoadTilesUncompressedV1(struct TileMapLayer* pLayer, struct BinarySerializer* pBS)
{
	int allocSize = pLayer->heightTiles * pLayer->widthTiles * sizeof(TileIndex);
	pLayer->Tiles = malloc(allocSize);
	BS_BytesRead(pBS, allocSize, (char*)pLayer->Tiles);
}

static void LoadTilesRLEV1(struct TileMapLayer* pTileMap, struct BinarySerializer* pBS)
{
	EASSERT(false);
}


static void LoadTilemapV1(struct TileMap* pTileMap, struct BinarySerializer* pBS)
{
	u32 numLayers = 0;
	BS_DeSerializeU32(&numLayers, pBS);
	pTileMap->layers = VectorResize(pTileMap->layers, numLayers);
	for (int i = 0; i < numLayers; i++)
	{
		struct TileMapLayer layer;
		u32 width, height, x, y, compression;
		BS_DeSerializeU32(&width, pBS);
		BS_DeSerializeU32(&height, pBS);
		BS_DeSerializeU32(&x, pBS);
		BS_DeSerializeU32(&y, pBS);
		BS_DeSerializeU32(&compression, pBS);
		layer.widthTiles = width;
		layer.heightTiles = height;
		layer.transform.position[0] = x;
		layer.transform.position[1] = y;
		switch (compression)
		{
		case 1:         // RLE
			LoadTilesRLEV1(&layer, pBS);
			break;
		case 2:         // uncompressed
			LoadTilesUncompressedV1(&layer, pBS);
			break;
		default:
			printf("unexpected value for compression enum %i\n", compression);
			break;
		}
		pTileMap->layers = VectorPush(pTileMap->layers, &layer);
	}
}

static void LoadTilemap(struct TileMap* pTileMap, const char* tilemapFilePath, DrawContext* pDC, hAtlas atlas)
{
	pTileMap->layers = NEW_VECTOR(struct TileMapLayer);
	struct BinarySerializer bs;
	memset(&bs, 0, sizeof(struct BinarySerializer));
	BS_CreateForLoad(tilemapFilePath, &bs);
	u32 version = 0;
	BS_DeSerializeU32(&version, &bs);
	switch (version)
	{
	case 1:
		LoadTilemapV1(pTileMap, &bs);
		break;
	default:
		printf("Unexpected tilemap file version %u\n", version);
		break;
	}
	BS_Finish(&bs);
}

static void Update(struct GameFrameworkLayer* pLayer, float deltaT)
{

}

static void OutputTilemapVertices(
	struct TileMap* pData, 
	struct Transform2D* pCam, 
	VECTOR(struct Worldspace2DVert)* outVerts,
	VECTOR(struct VertIndexT)* outIndices)
{
	VECTOR(struct Worldspace2DVert) verts = *outVerts;
	VECTOR(struct VertIndexT) inds = *outIndices;
	// TODO: implement here
	// 1.) Get first and last row and column based on the camera and ortho matrix
	// 2.) Output vertices and indices for these in model space
	*outVerts = verts;
	*outIndices = inds;
}

static void Draw(struct GameFrameworkLayer* pLayer, DrawContext* context)
{
	struct GameLayer2DData* pData = pLayer->userData;
	pData->pWorldspaceVertices = VectorClear(pData->pWorldspaceVertices);
	pData->pWorldspaceIndices = VectorClear(pData->pWorldspaceIndices);
	OutputTilemapVertices(&pData->tilemap, &pData->camera, &pData->pWorldspaceVertices, &pData->pWorldspaceIndices);
	context->WorldspaceVertexBufferData(pData->vertexBuffer, pData->pWorldspaceVertices, VectorSize(pData->pWorldspaceVertices), pData->pWorldspaceIndices, VectorSize(pData->pWorldspaceIndices));
	mat4 view;
	glm_mat4_identity(view);
	// TODO: set here based on camera
	context->DrawWorldspaceVertexBuffer(pData->vertexBuffer, VectorSize(pData->pWorldspaceIndices), view);
}

static void Input(struct GameFrameworkLayer* pLayer, InputContext* context)
{

}

static void OnPush(struct GameFrameworkLayer* pLayer, DrawContext* drawContext, InputContext* inputContext)
{

}

static void OnPop(struct GameFrameworkLayer* pLayer, DrawContext* drawContext, InputContext* inputContext)
{
	
}

static void OnWindowDimsChange(struct GameFrameworkLayer* pLayer, int newW, int newH)
{

}

void Game2DLayer_Get(struct GameFrameworkLayer* pLayer, struct Game2DLayerOptions* pOptions, DrawContext* pDC)
{
	pLayer->userData = malloc(sizeof(struct GameLayer2DData));
	memset(pLayer->userData, 0, sizeof(struct GameLayer2DData));
	struct GameLayer2DData* pData = pLayer->userData;
	pData->tilemap.layers = NEW_VECTOR(struct TileMapLayer);

	struct BinarySerializer bs;
	memset(&bs, 0, sizeof(struct BinarySerializer));
	BS_CreateForLoad(pOptions->atlasFilePath, &bs);
	At_SerializeAtlas(&bs, &pData->hAtlas, pDC);
	BS_Finish(&bs);

	pLayer->update = &Update;
	pLayer->draw = &Draw;
	pLayer->input = &Input;
	pLayer->onPush = &OnPush;
	pLayer->onWindowDimsChanged = &OnWindowDimsChange;

	pData->vertexBuffer = pDC->NewWorldspaceVertBuffer(256);
	pData->pWorldspaceVertices = NEW_VECTOR(Worldspace2DVert);
	pData->pWorldspaceIndices = NEW_VECTOR(VertIndexT);

	LoadTilemap(&pData->tilemap, pOptions->tilemapFilePath, pDC, pData->hAtlas);
}