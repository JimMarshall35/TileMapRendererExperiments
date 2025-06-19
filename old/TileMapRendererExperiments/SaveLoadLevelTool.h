#pragma once
#include "EditorToolBase.h"
class ECS;

class SaveLoadLevelTool :
    public EditorToolBase
{
public:
    SaveLoadLevelTool(ECS* ecs);
    // Inherited via EditorToolBase
    virtual void DoUi() override;
    virtual void RecieveTileClick(i32 x, i32 y, i32 z) override;
    virtual const std::string& GetName() override;
private:
    ECS* m_ecs;
};

