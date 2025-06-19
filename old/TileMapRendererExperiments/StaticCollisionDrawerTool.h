#pragma once
#include "EditorToolBase.h"
#include <vector>
class NewRenderer;

namespace flecs {
	struct entity;
};
template<typename T>
class DynamicQuadTreeContainer;

class PhysicsWorld;
class ECS;

class StaticCollisionDrawerTool : public EditorToolBase
{
public:
	StaticCollisionDrawerTool(NewRenderer* newRenderer, DynamicQuadTreeContainer<flecs::entity>* entityQuadTree, PhysicsWorld* physicsWorld, ECS* ecs);
	// Inherited via EditorToolBase
	virtual void DoUi() override;
	virtual void RecieveTileClick(i32 x, i32 y, i32 z) override;
	virtual void TileSelectionChanged(u32 newTile) override;
	virtual const std::string& GetName() override;
	virtual void RecieveWorldspaceClick(const glm::vec2& worldspace) override;
	virtual void RecieveWorldspaceRightClick(const glm::vec2& worldspace) override;
	virtual bool WantsToDrawOverlay() const override;
	virtual void DrawOverlay(const Camera2D& camera, const glm::vec2& mouseWorldSpacePos) override;
private:
	glm::vec2 GetSnappedPosition(const glm::vec2& worldpsacePosition) const;
	void DrawVertexCursor(const Camera2D& camera, const glm::vec2& pos) const;
private:
	u32 m_snappingIncrementTileDivider = 0; // 0 = off
	std::vector<glm::vec2> m_points;
	DynamicQuadTreeContainer<flecs::entity>* m_entityQuadTree;
	PhysicsWorld* m_physicsWorld;
	NewRenderer* m_newRenderer;
	ECS* m_ecs;
};

