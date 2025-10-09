#include "main.h"
#include "GameFramework.h"
#include <string.h>
#include "Game2DLayer.h"
#include "XMLUIGameLayer.h"
#include "DynArray.h"
#include "Entities.h"
#include "EntityQuadTree.h"
#include "WfEntities.h"

void GameInit(InputContext* pIC, DrawContext* pDC)
{
    /*struct GameFrameworkLayer testLayer;
    memset(&testLayer, 0, sizeof(struct GameFrameworkLayer));
    struct XMLUIGameLayerOptions options;
    options.bLoadImmediately = false;
    options.xmlPath = "./Assets/test.xml";
    options.pDc = pDC;
    testLayer.flags |= (EnableOnPush | EnableOnPop);
    printf("making xml ui layer\n");
    XMLUIGameLayer_Get(&testLayer, &options);
    printf("done\n");
    printf("pushing framework layer\n");
    GF_PushGameFrameworkLayer(&testLayer);*/
    InitEntity2DQuadtreeSystem();
    Et2D_Init(&WfRegisterEntityTypes);
    struct GameFrameworkLayer testLayer;
    memset(&testLayer, 0, sizeof(struct GameFrameworkLayer));
    struct Game2DLayerOptions options;
    memset(&options, 0, sizeof(struct Game2DLayerOptions));
    options.atlasFilePath = "./Assets/out/main.atlas";
    options.tilemapFilePath = "./Assets/out/Farm.tilemap";
    Game2DLayer_Get(&testLayer, &options, pDC);
    testLayer.flags |= (EnableOnPop | EnableOnPush | EnableUpdateFn | EnableDrawFn | EnableInputFn);
    GF_PushGameFrameworkLayer(&testLayer);
    printf("done\n");
}

int main(int argc, char** argv)
{

    EngineStart(argc, argv, &GameInit);
}