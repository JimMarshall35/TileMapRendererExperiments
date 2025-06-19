#pragma once
#include "ITiledWorldPopulater.h"
class ProceduralCppTiledWorldPopulater :
    public ITiledWorldPopulater
{
public:
    // Inherited via ITiledWorldPopulater
    virtual void PopulateLayer(u32 layerNumber, u16* layer, u32 layerWidthTiles, u32 layerHeightTiles, std::string& outName) override;
    virtual u32 GetRequiredNumLayers() override;

    // Inherited via ITiledWorldPopulater
    virtual u32 GetWidthTiles() override;
    virtual u32 GetHeightTiles() override;
};

