#include "main.h"
#include "GameFramework.h"
#include <string.h>
#include "Game2DLayer.h"
#include "XMLUIGameLayer.h"
#include "DynArray.h"
#include "Entities.h"
#include "EntityQuadTree.h"
#include "WfEntities.h"
#include "Physics2D.h"
#include "WfInit.h"
#include "Random.h"
#include "WfGameLayerData.h"

void WfGameLayerOnPush(struct GameFrameworkLayer* pLayer, DrawContext* drawContext, InputContext* inputContext)
{
    struct GameLayer2DData* pEngineLayer = pLayer->userData;
    pEngineLayer->pUserData = malloc(sizeof(struct WfGameLayerData));
    memset(pEngineLayer->pUserData, 0, sizeof(struct WfGameLayerData));
    GameLayer2D_OnPush(pLayer, drawContext, inputContext);
}

void WfPreFirstInit(struct GameLayer2DData* pEngineLayer)
{
    WfInitGameLayerData(pEngineLayer, (struct WfGameLayerData*)pEngineLayer->pUserData);
}

void WfGameLayerOnPop(struct GameFrameworkLayer* pLayer, DrawContext* drawContext, InputContext* inputContext)
{
    Game2DLayer_OnPop(pLayer, drawContext, inputContext);
    struct GameLayer2DData* pEngineLayer = pLayer->userData;
    free(pEngineLayer->pUserData);
}

void WfPushGameLayer(DrawContext* pDC, const char* lvlFilePath)
{
    struct GameFrameworkLayer testLayer;
    memset(&testLayer, 0, sizeof(struct GameFrameworkLayer));
    struct Game2DLayerOptions options;
    memset(&options, 0, sizeof(struct Game2DLayerOptions));
    options.atlasFilePath = "./Assets/out/main.atlas";
    options.levelFilePath = lvlFilePath;
    Game2DLayer_Get(&testLayer, &options, pDC);
    testLayer.onPush = &WfGameLayerOnPush;
    testLayer.onPop = &WfGameLayerOnPop;
    struct GameLayer2DData* pEngineLayer = testLayer.userData;
    pEngineLayer->preFirstInitCallback = &WfPreFirstInit;
    testLayer.flags |= (EnableOnPop | EnableOnPush | EnableUpdateFn | EnableDrawFn | EnableInputFn);
    GF_PushGameFrameworkLayer(&testLayer);
}

void WfEngineInit()
{
    unsigned int seed = Ra_SeedFromTime();
    printf("seed: %u\n", seed);
    Ph_Init();
    InitEntity2DQuadtreeSystem();
    Et2D_Init(&WfRegisterEntityTypes);
}

void GameInit(InputContext* pIC, DrawContext* pDC)
{
    /*struct GameFrameworkLayer testLayer;
    memset(&testLayer, 0, sizeof(struct GameFrameworkLayer));
    struct XMLUIGameLayerOptions options;
    options.bLoadImmed	float tlx, tly, brx, bry;
    options.xmlPath = "./Assets/test.xml";
    options.pDc = pDC;
    testLayer.flags |= (EnableOnPush | EnableOnPop);
    printf("making xml ui layer\n");
    XMLUIGameLayer_Get(&testLayer, &options);
    printf("done\n");
    printf("pushing framework layer\n");
    GF_PushGameFrameworkLayer(&testLayer);*/
    WfEngineInit();
    WfInit();
    WfPushGameLayer(pDC, "./Assets/out/Farm.tilemap");
    printf("done\n");
}

int main(int argc, char** argv)
{

    EngineStart(argc, argv, &GameInit);
}