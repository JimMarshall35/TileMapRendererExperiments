#include "WfPlayerStart.h"
#include "Entities.h"
#include "BinarySerializer.h"
#include "Game2DLayer.h"
#include "ObjectPool.h"
#include "WfPlayer.h"
#include "GameFramework.h"
#include "WfWorld.h"
#include "string.h"

struct WfPlayerStartData
{
    char from[64];
    char thisLocation[64];
};

static OBJECT_POOL(struct WfPlayerStartData) gPlayerStartDataPool;

void WfInitPlayerStart()
{
    gPlayerStartDataPool = NEW_OBJECT_POOL(struct WfPlayerStartData, 32);
}

void WfPlayerStartEntityOnInit(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer)
{
    struct GameLayer2DData* pLayerData = pLayer->userData;
    Entity2DOnInit(pEnt, pLayer);
    
    if(strcmp(WfWorld_GetCurrentLocationName(), gPlayerStartDataPool[pEnt->user.hData].from) == 0)
    {
        
        struct Entity2D ent;
        WfMakeIntoPlayerEntity(&ent, pLayer->userData, pEnt->transform.position);
        Et2D_AddEntity(&pLayerData->entities, &ent);
        WfWorld_SetCurrentLocationName(gPlayerStartDataPool[pEnt->user.hData].thisLocation);
    }
}

void WfPlayerStartEntityOnDestroy(struct Entity2D* pEnt, struct GameFrameworkLayer* pData)
{
    FreeObjectPoolIndex(gPlayerStartDataPool, pEnt->user.hData);
    Entity2DOnDestroy(pEnt, pData);
}

void WfDeSerializePlayerStartEntityV1(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData)
{
    gPlayerStartDataPool = GetObjectPoolIndex(gPlayerStartDataPool, &pOutEnt->user.hData);
    BS_DeSerializeStringInto(gPlayerStartDataPool[pOutEnt->user.hData].from, bs);

    BS_DeSerializeStringInto(gPlayerStartDataPool[pOutEnt->user.hData].thisLocation, bs);
    HGeneric hPlayer = pOutEnt->user.hData;
    pOutEnt->init = &WfPlayerStartEntityOnInit;
    pOutEnt->bKeepInDynamicList = false;
    pOutEnt->bKeepInQuadtree = false;
}

void WfDeSerializePlayerStartEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData)
{
    u32 version;
    BS_DeSerializeU32(&version, bs);
    switch (version)
    {
    case 1:
        /* code */
        WfDeSerializePlayerStartEntityV1(bs, pOutEnt, pData);
        break;   
    default:
        break;
    }
}

void WfSerializePlayerStartEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData)
{
    BS_SerializeU32(1, bs); // version
    BS_SerializeString(gPlayerStartDataPool[pInEnt->user.hData].from, bs);
}
