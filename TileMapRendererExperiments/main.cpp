
#include "Engine.h"
#include "TileSetInfo.h"
#include "DIContainer.h"
#include "DIMacros.h"

//#include "Netcode.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 1200
#define kb 1000
#define mb (kb * kb)

#define BytesToCells(bytes)(bytes/sizeof(Cell))


int main()
{

    


    auto config = TileMapConfigOptions();
    config.AtlasWidthPx = 800;

    NewRendererInitialisationInfo rendererInit(SCR_WIDTH, SCR_HEIGHT);
    rendererInit.windowHeight = SCR_HEIGHT;
    rendererInit.windowWidth = SCR_WIDTH;

    ForthEngineSystemInitArgs forthInit;
    forthInit.intStackSizeCells = BytesToCells(1 * kb);
    forthInit.returnStackSizeCells = BytesToCells(1 * kb);
    forthInit.memorySizeCells = BytesToCells(2 * mb);
    Engine e(config, rendererInit, forthInit);

    e.LoadJsonTileMap("data\\json", "lvl1_test5.json");

    e.RegisterStartupFunction(StartupFunctionType::Normal, [&](MetaAtlas* metaAtlas) {
        e.StartLoadingTilesets();

        e.TryLoadTilesetFromJSONInfo("data\\json", "city.json");

        TileSetInfo info;
        info.BottomMargin = 0;
        info.RightMargin = 0;
        info.PixelColStart = 0;
        info.PixelRowStart = 0;
        info.TileHeight = 24;
        info.TileWidth = 24;
        info.RowsOfTiles = 6;
        info.ColsOfTiles = 4;
        info.Path = "sprites\\24by24ModernRPGGuy_edit.png";
        info.Name = "RPG guy";
        e.TryLoadTileset(info);

        e.StopLoadingTilesets(AtlasLoaderAtlasType::ArrayTexture | AtlasLoaderAtlasType::SingleTextureAtlas);

        std::string errorString;
        if (!metaAtlas->LoadFromJSON("data\\json", "city.json", errorString))
        {
            std::cerr << errorString << "\n";
        }
    });


    return e.Run();
}

