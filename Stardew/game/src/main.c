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
#include "WfWorld.h"

void WfInitWorldLevels()
{
    struct WfLocation farm = {
        .bIsInterior = false,
        .levelFilePath = "./Assets/out/Farm.tilemap"
    };
    struct WfLocation roadToTown = {
        .bIsInterior = false,
        .levelFilePath = "./Assets/out/RoadToTown.tilemap"
    };
    struct WfLocation house = {
        .bIsInterior = true,
        .levelFilePath = "./Assets/out/House.tilemap"
    };
    WfWorld_AddLocation(&house, "House");
    WfWorld_AddLocation(&roadToTown, "RoadToTown");
    WfWorld_AddLocation(&farm, "Farm");

    WfWorld_SetCurrentLocationName("House");
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
    WfEngineInit();
    WfInit();
    WfInitWorldLevels(); /* temporary - a world will be loaded as part of a game file, to be implemented in WfGame.c */
    WfWorld_LoadLocation("Farm", pDC);
    printf("done\n");
}

int main(int argc, char** argv)
{

    EngineStart(argc, argv, &GameInit);
}