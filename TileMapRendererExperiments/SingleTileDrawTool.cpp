#include "SingleTileDrawTool.h"
#include "TiledWorld.h"
#include "imgui/imgui.h"
#include "Undo.h"

SingleTileDrawTool::SingleTileDrawTool(TiledWorld* tiledWorld)
    :m_tiledWorld(tiledWorld)
{
}

void SingleTileDrawTool::DoUi()
{
    ImGui::Text((std::string("Selected tile ") + std::to_string(m_selectedTile)).c_str());
}

void SingleTileDrawTool::RecieveTileClick(i32 x, i32 y, i32 z)
{
    auto edit = UndoableEdit{ EditType::SingleTile };
    SingleTileEditData& d = edit.data.singleTile;
    d.oldVal = m_tiledWorld->GetTile(x, y, z);
    d.newVal = m_selectedTile;
    d.x = x;
    d.y = y;
    d.z = z;
    PushEdit(edit);
    m_tiledWorld->SetTile(x, y, z, m_selectedTile);
}

void SingleTileDrawTool::TileSelectionChanged(u32 newTile)
{
    m_selectedTile = newTile;
}

const std::string& SingleTileDrawTool::GetName()
{
    static const std::string s = "Draw Tiles";
    return s;
}
