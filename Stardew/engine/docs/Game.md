# Game

To create a game, create a new executable that links to the engine:

```cmake
cmake_minimum_required(VERSION 3.25)
project(MyGame)
add_subdirectory(game) # defines the target Game

target_link_libraries(Game PUBLIC StardewEngine)
```

In the main function of the game, call EngineStart, passing in a GameInit function that pushes a game framework layer to start your game.
Your game can define its own game framework layer or use a built in one.

Here's an example of starting a game using the builtin Game2DLayer:

```c
/* */

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

void GameInit(InputContext* pIC, DrawContext* pDC)
{
    // init any engine systems - just an example
    unsigned int seed = Ra_SeedFromTime();
    printf("seed: %u\n", seed);
    WfInit();
    Ph_Init();
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
}

int main(int argc, char** argv)
{

    EngineStart(argc, argv, &GameInit);
}
```

If you use the built in Game2DLayer your game can register entity types with asset tooling and the entity system.

For more information on this see Entities.md and Assets.md