#include "WfGameLayerData.h"
#include "Game2DLayer.h"

void WfInitGameLayerData(struct GameLayer2DData* pEngineLayerData, struct WfGameLayerData* pOutData)
{
    WfInitSprites(&pOutData->sprites, pEngineLayerData);
}
