#pragma once
#include "EditorToolBase.h"
#include "BasicTypedefs.h"
#include <memory>
#include <string>

struct TileUserInfo {
    std::string tileName = "";
};

class AtlasLoader;
class TileInfoTool :
    public EditorToolBase
{
public:
    TileInfoTool(const AtlasLoader* atlasLoader);
    // Inherited via EditorToolBase
    virtual void DoUi() override;
    virtual void RecieveTileClick(i32 x, i32 y, i32 z) override;
    virtual void TileSelectionChanged(u32 newTile) override;
    virtual const std::string& GetName() override;
private:
    void SaveTileInfosAsCppHeader();
private:
    const AtlasLoader* m_atlasLoader;
    u32 m_selectedIndex = 0;
    std::unique_ptr<TileUserInfo[]> m_tileInfos;
    u32 m_numTileInfos = 0;
};

