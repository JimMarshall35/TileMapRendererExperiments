#pragma once
#include "EditorToolBase.h"
class TiledWorld;
class SingleTileDrawTool :
    public EditorToolBase
{
public:
    SingleTileDrawTool(TiledWorld* tiledWorld);
    // Inherited via EditorToolBase
    virtual void DoUi() override;
    virtual void RecieveTileClick(i32 x, i32 y, i32 z) override;
    virtual void TileSelectionChanged(u32 newTile) override;
    virtual const std::string& GetName() override;
private:
    u32 m_selectedTile = 0;
    TiledWorld* m_tiledWorld;
};

