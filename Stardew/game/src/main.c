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
#include "WfGameLayer.h"

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
    WfEngineInit();
    WfInit();
    WfPushGameLayer(pDC, "./Assets/out/Farm.tilemap");
    printf("done\n");
}

int main(int argc, char** argv)
{

    EngineStart(argc, argv, &GameInit);
}