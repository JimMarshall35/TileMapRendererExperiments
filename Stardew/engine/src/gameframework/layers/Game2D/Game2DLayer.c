#include "Game2DLayer.h"
#include "GameFramework.h"
#include <stdlib.h>
#include <string.h>

void Game2DLayer_Get(struct GameFrameworkLayer* pLayer, struct Game2DLayerOptions* pOptions)
{
	pLayer->userData = malloc(sizeof(struct GameLayer2DData));
	memset(pLayer->userData, 0, sizeof(struct GameLayer2DData));
	struct GameLayer2DData* pData = pLayer->userData;
	pData->tilemap.layers = NEW_VECTOR(struct TileMapLayer);
}