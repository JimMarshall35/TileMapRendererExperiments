#pragma once
#include "EditorToolBase.h"
#include <vector>

class MetaAtlas;
class AtlasLoader;
struct MetaSpriteComponent;

template<typename T>
class StaticQuadTree;

class MetaspriteTool
	:public EditorToolBase
{
	// Inherited via EditorToolBase
public:
	MetaspriteTool(MetaAtlas* metaAtlas, AtlasLoader* atlasLoader, StaticQuadTree<MetaSpriteComponent>* metaspritesQuadTree);
	virtual void DoUi() override;
	virtual void RecieveTileClick(i32 x, i32 y, i32 z) override;
	virtual void RecieveWorldspaceClick(const glm::vec2& click) override;
	virtual void TileSelectionChanged(u32 newTile) override;
	
	virtual const std::string& GetName() override;
private:
	u32 m_selectedMetaspriteTileIndex = 0;
	u32 m_currentMetaspriteWidth = 4;
	u32 m_currentMetaspriteHeight = 4;
	MetaAtlas* m_metaAtlas;
	AtlasLoader* m_atlasLoader;
	std::vector<u16> m_currentMetasprite = std::vector<u16>(4*4);
	StaticQuadTree<MetaSpriteComponent>* m_metaspritesQuadTree;
	i32 m_currentMetaspriteHandle = -1;
};

