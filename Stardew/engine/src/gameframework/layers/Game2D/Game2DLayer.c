#include "Game2DLayer.h"
#include "GameFramework.h"
#include "Atlas.h"
#include <stdlib.h>
#include <string.h>
#include "BinarySerializer.h"
#include "DrawContext.h"
#include "AssertLib.h"

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

static void LoadTilemap(struct TileMap* pTileMap, const char* tilemapFilePath)
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
	memset(&bs, 0, sizeof(struct BinarySerializer));


}