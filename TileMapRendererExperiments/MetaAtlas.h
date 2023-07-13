#pragma once
#include "BasicTypedefs.h"
#include <memory>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include "ECS.h"
#include "MetaSpriteComponent.h"

using MetaSpriteHandle = i32;
#define MAX_NUM_METASPRITES 512
class ECS;

class MetaAtlas
{
public:
	MetaAtlas(u32 width, u32 height, ECS* ecs);
	MetaSpriteHandle LoadMetaSprite(const MetaSpriteDescription& description);
	void GetSprites(const MetaSpriteDescription** sprites, u32* numsprites);
	u32 GetAtlasTextureHandle() const;
	const glm::ivec2* GetOffset(MetaSpriteHandle handle) const;
	const flecs::entity GetECSEntity(MetaSpriteHandle handle) const;

	const MetaSpriteDescription* getDescription(MetaSpriteHandle handle) const;
	void SaveToFile(std::string path) const;
	void LoadFromFile(std::string path);
	bool LoadFromJSON(const std::string& folder, const std::string& file, std::string& outErrorMsg);
	void SaveAtlasAsECSEntities();
private:
	void ExpandAtlas(u32 tooBigSpriteSizeW = 0, u32 tooBigSpriteSizeH=0);
	void DoubleAtlasDims();
	void CreateGlTextureFromAtlas();
	u32 GetBufferSizeRequiredToSave() const;
private:
	u32 m_atlasWidth;
	u32 m_atlasHeight;
	u32 m_atlasSize;
	std::unique_ptr<u16[]> m_atlasData;
	u32 m_atlasTextureHandle = 0;
	flecs::entity m_loadedMetaSpriteDescriptionEntities[MAX_NUM_METASPRITES];
	MetaSpriteDescription m_loadedMetaspriteDescriptions[MAX_NUM_METASPRITES];
	glm::ivec2 m_loadedMetaSpriteOffsets[MAX_NUM_METASPRITES];
	u32 m_numLoadedMetasprites = 0;
	u32 m_nextX = 0;
	u32 m_nextY = 0;
	u32 m_currentY = 0;
	ECS* m_ecs;
};

