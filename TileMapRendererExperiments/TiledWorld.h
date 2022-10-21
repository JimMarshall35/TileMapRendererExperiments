#pragma once
#include <memory>
#include "BasicTypedefs.h"
#include <functional>
#define MAX_LAYERS 64
using TextureHandle = u32;

using IterateTileLayersFunc = std::function<void(const TextureHandle&, u16*)>;

struct TiledWorld
{
public:
	TiledWorld(u32 sizeW, u32 sizeH, u32 numLayers);
	void IterateTileLayers(IterateTileLayersFunc iterationFunc) const;
public:
	// getters
	inline u32 GetNumLayers() const { return m_numLayers; }
	inline u32 GetMapWidth() const { return m_mapWidthTiles; }
	inline u32 GetMapHeight() const { return m_mapHeightTiles; }

private:
	std::unique_ptr<u16[]> m_layersData[MAX_LAYERS];
	TextureHandle m_layerTextureHandles[MAX_LAYERS];
	bool m_layersDrawEnabled[MAX_LAYERS];

	u32 m_mapWidthTiles;
	u32 m_mapHeightTiles;
	u32 m_numLayers;
};

