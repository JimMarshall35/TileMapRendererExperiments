#pragma once
#include "EditorToolBase.h"
#include <vector>
#include "flecs.h"

class MetaAtlas;
class AtlasLoader;
struct MetaSprite;

template<typename T>
class DynamicQuadTreeContainer;

class ECS;
class NewRenderer;

class MetaspriteTool
	:public EditorToolBase
{
	// Inherited via EditorToolBase
public:
	MetaspriteTool(MetaAtlas* metaAtlas, AtlasLoader* atlasLoader, DynamicQuadTreeContainer<flecs::entity>* metaspritesQuadTree, ECS* ecs, NewRenderer* renderer);
	virtual void DoUi() override;
	virtual void RecieveTileClick(i32 x, i32 y, i32 z) override;
	virtual void RecieveWorldspaceClick(const glm::vec2& click) override;
	virtual void TileSelectionChanged(u32 newTile) override;
	
	virtual const std::string& GetName() override;
	virtual bool WantsToDrawOverlay() const override;
	virtual void DrawOverlay(const Camera2D& camera, const glm::vec2& mouseWorldSpacePos) override;

private:
	void SaveMetaSprite(std::string name);
	u32 m_selectedMetaspriteTileIndex = 0;
	u32 m_currentMetaspriteWidth = 4;
	u32 m_currentMetaspriteHeight = 4;
	MetaAtlas* m_metaAtlas;
	AtlasLoader* m_atlasLoader;
	std::vector<u16> m_currentMetasprite = std::vector<u16>(4*4);
	DynamicQuadTreeContainer<flecs::entity>* m_metaspritesQuadTree;
	i32 m_currentMetaspriteHandle = -1;
	bool m_autoAdvance = true;
	ECS* m_ecs;
	NewRenderer* m_renderer;
};

