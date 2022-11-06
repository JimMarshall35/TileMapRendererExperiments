#pragma once
#include "BasicTypedefs.h"
#include <memory>
#include <map>
#include <string>
#include <glm/glm.hpp>
using MetaSpriteHandle = i32;
#define MAX_METASPRITE_SIZE 128
#define MAX_NUM_METASPRITES 512

namespace flecs {
	struct entity;
}

struct MetaSpriteDescription {
	std::string name;
	u32 spriteTilesWidth;
	u32 spriteTilesHeight;
	u32 numTiles = 0;
	u16 tiles[MAX_METASPRITE_SIZE];
};

class MetaAtlas
{
public:
	MetaAtlas(u32 width, u32 height);
	MetaSpriteHandle LoadMetaSprite(const MetaSpriteDescription& description);
	void AddMetaSpriteComponentToEntity(flecs::entity& entity, MetaSpriteHandle handle);
	void GetSprites(const MetaSpriteDescription** sprites, u32* numsprites);
	u32 GetAtlasTextureHandle() const;
	const glm::ivec2* GetOffset(MetaSpriteHandle handle) const;
	const MetaSpriteDescription* getDescription(MetaSpriteHandle handle) const;
private:
	void ExpandAtlas(u32 tooBigSpriteSizeW = 0, u32 tooBigSpriteSizeH=0);
	void DoubleAtlasDims();
	void CreateGlTextureFromAtlas();
private:
	u32 m_atlasWidth;
	u32 m_atlasHeight;
	u32 m_atlasSize;
	std::unique_ptr<u16[]> m_atlasData;
	u32 m_atlasTextureHandle = 0;
	MetaSpriteDescription m_loadedMetaspriteDescriptions[MAX_NUM_METASPRITES];
	glm::ivec2 m_loadedMetaSpriteOffsets[MAX_NUM_METASPRITES];
	u32 m_numLoadedMetasprites = 0;
	u32 m_nextX = 0;
	u32 m_nextY = 0;
	u32 m_currentY = 0;
};

