#pragma once
#include <memory>
#include "BasicTypedefs.h"
#include <functional>

#define MAX_LAYERS 64
using TextureHandle = u32;

using IterateTileLayersFunc = std::function<void(const TextureHandle&, u16*, bool)>;

class ITiledWorldPopulater;

class TiledWorld
{
public:
	TiledWorld(u32 sizeW, u32 sizeH, u32 numLayers, ITiledWorldPopulater* populater);
	void IterateTileLayers(IterateTileLayersFunc iterationFunc) const;
	void SetTile(u32 x, u32 y, u32 z, u32 newTileIndex);
	u32 GetTile(u32 x, u32 y, u32 z);
public:
	// getters
	inline u32 GetNumLayers() const { return m_numLayers; }
	inline u32 GetMapWidth() const { return m_mapWidthTiles; }
	inline u32 GetMapHeight() const { return m_mapHeightTiles; }
	inline bool* GetLayersVisibilities() { return m_layersVisible; }

private:
	std::unique_ptr<u16[]> m_layersData[MAX_LAYERS];
	TextureHandle m_layerTextureHandles[MAX_LAYERS];
	bool m_layersVisible[MAX_LAYERS];

	u32 m_mapWidthTiles;
	u32 m_mapHeightTiles;
	u32 m_numLayers;
	ITiledWorldPopulater* m_populater;
};

