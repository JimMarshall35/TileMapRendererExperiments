#include "TileInfoTool.h"
#include "AtlasLoader.h"
#include "imgui/imgui.h"
#include <fstream>

TileInfoTool::TileInfoTool(const AtlasLoader* atlasLoader)
    :m_atlasLoader(atlasLoader),
    m_numTileInfos(atlasLoader->GetNumTiles())
{
    m_tileInfos = std::make_unique<TileUserInfo[]>(m_numTileInfos);
}

#define NAME_BUFFER_MAX_CHARS 200
static char s_tileNameInputBuffer[NAME_BUFFER_MAX_CHARS];

void TileInfoTool::DoUi()
{

    if (ImGui::InputText("name", s_tileNameInputBuffer, NAME_BUFFER_MAX_CHARS)) {
        m_tileInfos[m_selectedIndex].tileName = std::string(s_tileNameInputBuffer);
    }
    if (ImGui::Button("save as header")) {
        SaveTileInfosAsCppHeader();
    }
}

void TileInfoTool::RecieveTileClick(i32 x, i32 y, i32 z)
{
}

void TileInfoTool::TileSelectionChanged(u32 newTile)
{
    m_selectedIndex = newTile;
    
    strcpy_s(s_tileNameInputBuffer, m_tileInfos[newTile].tileName.c_str());
}

const std::string& TileInfoTool::GetName()
{
    static const std::string name = "TileInfo";
    return name;
}

void TileInfoTool::SaveTileInfosAsCppHeader()
{
    std::ofstream wf("GeneratedTileDefs.h", std::ios::out);

    wf << "#pragma once\n\n";
    for (int i = 0; i < m_numTileInfos; i++) {
        if (!m_tileInfos[i].tileName.empty()) {
            wf << "#define " + m_tileInfos[i].tileName + " " + std::to_string(i) + "\n";
        }
    }
}
