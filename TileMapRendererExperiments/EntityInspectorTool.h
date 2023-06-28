#pragma once
#include "EditorToolBase.h"

class ECS;

class EntityInspectorTool :
    public EditorToolBase
{
public:
    EntityInspectorTool(ECS* ecs);
    // Inherited via EditorToolBase
    virtual void DoUi() override;
    virtual void RecieveTileClick(i32 x, i32 y, i32 z) override;
    virtual void TileSelectionChanged(u32 newTile) override;
    virtual const std::string& GetName() override;
private:
    void SaveEntitiesTest();
private:
    ECS* m_ecs;
};

