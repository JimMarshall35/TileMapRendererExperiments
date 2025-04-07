#pragma once
#include "EditorToolBase.h"
#include "Camera2D.h"
#include "QuadTree.h"
#include <vector>
#include "ECS.h"

class NewRenderer;
class EntityInspectorTool :
    public EditorToolBase
{
public:
    EntityInspectorTool(ECS* ecs, DynamicQuadTreeContainer<flecs::entity>* entityQuadTree, NewRenderer* newRenderer);
    // Inherited via EditorToolBase
    virtual void DoUi() override;
    virtual void RecieveTileClick(i32 x, i32 y, i32 z) override;
    virtual void RecieveWorldspaceClick(const glm::vec2& worldspace) override;

    virtual void TileSelectionChanged(u32 newTile) override;
    virtual const std::string& GetName() override;
    virtual bool WantsToDrawOverlay() const override;
    virtual void DrawOverlay(const Camera2D& camera, const glm::vec2& mouseWorldSpacePos) override;

private:
    void SaveEntitiesTest();

private:
    ECS* m_ecs;
    DynamicQuadTreeContainer<flecs::entity>* m_entityQuadTree;
    NewRenderer* m_renderer;
    std::vector<std::pair<flecs::entity, AtlasRect>> m_visibileItems;
    flecs::entity m_nullEntity;
    flecs::entity m_selectedEntity = m_nullEntity;
};

