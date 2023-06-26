#include "JSONPopulator.h"
#include "jsonhelper.h"

JSONTiledWorldPopulator::JSONTiledWorldPopulator(const std::string& jsonFolder, const std::string& jsonFile)
{
	std::string error;
	if (!TryLoadData(jsonFolder, jsonFile, error))
	{
		std::cerr << error << "\n";
	}
}

void JSONTiledWorldPopulator::PopulateLayer(u32 layerNumber, u16* layer, u32 layerWidthTiles, u32 layerHeightTiles, std::string& outName)
{
	memcpy(layer, m_tileLayerData[layerNumber].Data.get(), sizeof(u16) * (layerWidthTiles * layerHeightTiles));
	outName = m_tileLayerData[layerNumber].Name;
	m_tileLayerData[layerNumber].Data.reset();
	if (layerNumber >= m_numLayers - 1) {
		m_tileLayerData.reset();
	}
}

u32 JSONTiledWorldPopulator::GetRequiredNumLayers()
{
	return m_numLayers;
}

u32 GetTotalTileLayers(const rapidjson::Value& layersArray) {
	u32 numTotalLayers = layersArray.Size();
	u32 numTileLayers = 0;
	for (int i = 0; i < numTotalLayers; i++)
	{
		if(layersArray[i].HasMember("type")){
			const char* typeString = layersArray[i]["type"].GetString();
			if (strcmp(typeString, "tilelayer") == 0) {
				numTileLayers++;
			}
		}
	}
	return numTileLayers;
}

namespace LvlJson {
	const char* layersName = "layers";
}

bool JSONTiledWorldPopulator::TryLoadData(const std::string& jsonFolder, const std::string& jsonFile, std::string& outErrorMessage)
{
	using namespace rapidjson;
	Document doc;
	if (!loadJSONFile(doc, (jsonFolder + "/" + jsonFile).c_str())) {
		outErrorMessage = "problem loading " + (jsonFolder + "\\" + jsonFile) + "\n";
		return false;
	}

	if (!checkJSONValue(LvlJson::layersName, JSONTYPE::ARRAY, doc)){
		outErrorMessage = "field" + std::string(LvlJson::layersName) + "not found or not an array.\n";
		return false;
	}

	const Value& layersArray = doc[LvlJson::layersName];
	m_numLayers = GetTotalTileLayers(layersArray);
	m_tileLayerData = std::make_unique<TileLayerRawData[]>(m_numLayers);
	u32 nextTilemapLayerDataIndex = 0;
	for (int i = 0; i < layersArray.Size(); i++)
	{
		const Value& layer = layersArray[i];
		if (!layersArray[i].HasMember("type") || strcmp(layersArray[i]["type"].GetString(), "tilelayer") != 0) {
			continue;
		}
		const Value& layerTileData = layer["data"];

		u32 widthTiles = layer["width"].GetUint();
		u32 heightTiles = layer["height"].GetUint();
		u32 tileCount = widthTiles * heightTiles;
		m_tileLayerData[nextTilemapLayerDataIndex].HeightTiles = heightTiles;
		m_tileLayerData[nextTilemapLayerDataIndex].WidthTiles = widthTiles;
		m_tileLayerData[nextTilemapLayerDataIndex].Data = std::make_unique<u16[]>(tileCount);
		m_tileLayerData[nextTilemapLayerDataIndex].Name = layer["name"].GetString();
		for (int j = 0; j < tileCount; j++) 
		{
			m_tileLayerData[nextTilemapLayerDataIndex].Data[j] = (u16)layerTileData[j].GetUint();
		}
		nextTilemapLayerDataIndex++;
	}
	return true;
}

u32 JSONTiledWorldPopulator::GetWidthTiles()
{
	u32 maxWidth = 0;
	for (int i = 0; i < m_numLayers; i++) {
		if (maxWidth == 0) {
			maxWidth = m_tileLayerData[i].WidthTiles;
		}
		if (m_tileLayerData[i].WidthTiles != maxWidth) {
			maxWidth = m_tileLayerData[i].WidthTiles;
			std::cerr << "Error - all tile layers must be same size for now\n";
		}
	}
	return maxWidth;
}

u32 JSONTiledWorldPopulator::GetHeightTiles()
{
	u32 maxHeight = 0;
	for (int i = 0; i < m_numLayers; i++) {
		if (maxHeight == 0) {
			maxHeight = m_tileLayerData[i].HeightTiles;
		}
		if (m_tileLayerData[i].WidthTiles != maxHeight) {
			maxHeight = m_tileLayerData[i].HeightTiles;
			std::cerr << "Error - all tile layers must be same size for now\n";
		}
	}
	return maxHeight;
}
