#pragma once
#include "ITiledWorldPopulater.h"
#include <string>
#include <memory>


class JSONTiledWorldPopulator : public ITiledWorldPopulater
{
public:
	JSONTiledWorldPopulator(const std::string& jsonFolder, const std::string& jsonFile);
	// Inherited via ITiledWorldPopulater
	virtual void PopulateLayer(u32 layerNumber, u16* layer, u32 layerWidthTiles, u32 layerHeightTiles, std::string& outName) override;
	virtual u32 GetRequiredNumLayers() override;
	
private:
	bool TryLoadData(const std::string& jsonFolder, const std::string& jsonFile, std::string& errorMessage);

private:
	struct TileLayerRawData 
	{
		std::unique_ptr<u16[]> Data;
		u32 WidthTiles;
		u32 HeightTiles;
		std::string Name;
	};
	u32 m_numLayers = 0;
	std::unique_ptr<TileLayerRawData[]> m_tileLayerData;

	// Inherited via ITiledWorldPopulater
	virtual u32 GetWidthTiles() override;
	virtual u32 GetHeightTiles() override;
};