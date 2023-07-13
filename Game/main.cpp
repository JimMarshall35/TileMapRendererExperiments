
#include "Engine.h"
#include "TileSetInfo.h"

#include "Netcode.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 1200

int main()
{
    
    auto config = TileMapConfigOptions();
    config.AtlasWidthPx = 800;

    NewRendererInitialisationInfo rendererInit;
    rendererInit.windowHeight = SCR_HEIGHT;
    rendererInit.windowWidth = SCR_WIDTH;
    Engine e(config,rendererInit);
    NetcodeServer::Init();

    e.RegisterStartupFunction(StartupFunctionType::Normal, [&]() {
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

            e.LoadJsonTileMap("data\\json", "city.json");
        });
    

    return e.Run();
}

