#pragma once
#include "BasicTypedefs.h"
#include <string>
class ITiledWorldPopulater {
public:
	virtual void PopulateLayer(u32 layerNumber, u16* layer, u32 layerWidthTiles, u32 layerHeightTiles, std::string& outName) = 0;
	virtual u32 GetRequiredNumLayers() = 0;
	virtual u32 GetWidthTiles() = 0;
	virtual u32 GetHeightTiles() = 0;

};