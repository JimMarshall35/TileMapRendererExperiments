#include "WfPlayer.h"
#include "BinarySerializer.h"
#include "Game2DLayer.h"
#include "Entities.h"
#include "ObjectPool.h"

struct WfPlayerEntData
{
    int f;
};

static OBJECT_POOL(struct WfPlayerEntData) gPlayerEntDataPool = NULL;

void WfInitPlayer()
{
    gPlayerEntDataPool = NEW_OBJECT_POOL(struct WfPlayerEntData, 4);
}

static void OnInitPlayer(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer)
{
    Entity2DOnInit(pEnt,pLayer);
}

static void OnDestroyPlayer(struct Entity2D* pEnt, struct GameFrameworkLayer* pData)
{
    Entity2DOnDestroy(pEnt, pData);
}

static void OnUpdatePlayer(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT)
{
    Entity2DUpdate(pEnt, pLayer, deltaT);
}

static void OnInputPlayer(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, InputContext* context)
{
    Entity2DInput(pEnt, pLayer, context);
}

void WfDeSerializePlayerEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData)
{
    HGeneric data = NULL_HANDLE;
    GetObjectPoolIndex(gPlayerEntDataPool, &data);
    pOutEnt->user.hData = data;
}

void WfSerializePlayerEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData)
{

}
