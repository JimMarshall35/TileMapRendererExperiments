#pragma once
#include "ITiledWorldPopulater.h"
class ProceduralCppTiledWorldPopulater :
    public ITiledWorldPopulater
{
public:
    // Inherited via ITiledWorldPopulater
    virtual void PopulateLayer(u32 layerNumber, u16* layer, u32 layerWidthTiles, u32 layerHeightTiles) override;
};

