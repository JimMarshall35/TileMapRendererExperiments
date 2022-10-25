#pragma once
#include "BasicTypedefs.h"
class ITiledWorldPopulater {
public:
	virtual void PopulateLayer(u32 layerNumber, u16* layer, u32 layerWidthTiles, u32 layerHeightTiles) = 0;
};