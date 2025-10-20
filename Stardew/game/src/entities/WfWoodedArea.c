#include "WfWoodedArea.h"
#include "BinarySerializer.h"
#include "Entities.h"
#include "Game2DLayer.h"
#include "AssertLib.h"
#include "ObjectPool.h"



struct WfWoodedAreaData
{
    float conif_percent, decid_percent, density;
};

static OBJECT_POOL(struct WfWoodedAreaData) gDataObjectPool;

void WfWoodedAreaInit()
{
    gDataObjectPool = NEW_OBJECT_POOL(struct WfWoodedAreaData, 8);
}

void WfWoodedAreaEntityOnInit(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer)
{
    /* spawn trees here */
    struct WfWoodedAreaData* pData = &gDataObjectPool[pEnt->user.hData];
    /* destroy the entity */
    Et2D_DestroyEntity(pEnt->thisEntity);
}

void WfWoodedAreaEntityOnDestroy(struct Entity2D* pEnt)
{
    FreeObjectPoolIndex(gDataObjectPool, pEnt->user.hData);
}

void WfDeSerializeWoodedAreaEntityV1(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData)
{
    int index = -1;
    gDataObjectPool = GetObjectPoolIndex(gDataObjectPool, &index);
    BS_DeSerializeFloat(&gDataObjectPool[index].conif_percent, bs);
    BS_DeSerializeFloat(&gDataObjectPool[index].decid_percent, bs);
    BS_DeSerializeFloat(&gDataObjectPool[index].density, bs);
    pOutEnt->user.hData = index;
    pOutEnt->init = &WfWoodedAreaEntityOnInit;
    pOutEnt->onDestroy = &WfWoodedAreaEntityOnDestroy;
}

void WfDeSerializeWoodedAreaEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData)
{
    u32 version = 0;
    BS_DeSerializeU32(&version, bs);
    switch (version)
    {
    case 1:
        WfDeSerializeWoodedAreaEntityV1(bs, pOutEnt, pData);
        break;
    default:
        EASSERT(false);
        break;
    }
}

void WfSerializeWoodedAreaEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData)
{

}