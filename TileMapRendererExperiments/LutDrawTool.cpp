#include "LutDrawTool.h"
#include "imgui/imgui.h"
#include "AtlasLoader.h"
#include <fstream>
#include "Undo.h"
#include "TiledWorld.h"

LutDrawTool::LutDrawTool(IFileSystem* fs, TiledWorld* tw, AtlasLoader* al)
    :m_fileSystem(fs),
    m_tiledWorld(tw),
    m_atlasLoader(al)
{
}

void LutDrawTool::DoUi()
{
    ImGui::Text((std::string("Selected tile ") + std::to_string(m_tileIndexToSet)).c_str());

    ImGui::Separator();

    ImGui::Checkbox("##tiletool0", &m_tileToolBoxes[0]);
    ImGui::SameLine();
    ImGui::Checkbox("##tiletool1", &m_tileToolBoxes[1]);
    ImGui::SameLine();
    ImGui::Checkbox("##tiletool2", &m_tileToolBoxes[2]);

    ImGui::Checkbox("##tiletool3", &m_tileToolBoxes[3]);
    ImGui::SameLine();
    bool middleVal = false;
    ImGui::Checkbox("##tiletool4", &middleVal);
    ImGui::SameLine();
    ImGui::Checkbox("##tiletool5", &m_tileToolBoxes[4]);

    ImGui::Checkbox("##tiletool6", &m_tileToolBoxes[5]);
    ImGui::SameLine();
    ImGui::Checkbox("##tiletool7", &m_tileToolBoxes[6]);
    ImGui::SameLine();
    ImGui::Checkbox("##tiletool8", &m_tileToolBoxes[7]);

    ImGui::SameLine();
    if (ImGui::Button("Push tile to LUT")) {
        PushLookupTableCase();
    }
    ImGui::SameLine();
    if (ImGui::Button("PopTileFrom LUT")) {
        PopLookupTableCase();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save LUT")) {
        SaveToolLUT("Tool.lut");
    }
    ImGui::SameLine();
    if (ImGui::Button("Load LUT")) {
        LoadToolLUT("Tool.lut");
    }

    const auto& tiles = m_atlasLoader->GetIndividualTiles();
    u32 lutCase = GetCurrentLookupTableCaseIndex();
    for (u32 i = 0; i < m_tileToolLookupTableEntriesPerCase[lutCase]; i++) {
        auto tileIndex = m_tileToolLookupTable[lutCase][i];
        //ImGui::Text(std::to_string(tileIndex).c_str());
        const auto& t = tiles[tileIndex];
        ImGui::Image((void*)(intptr_t)m_atlasLoader->GetAtlasTextureHandle(),
            { 32,32 },
            { t.UTopLeft, t.VTopLeft },
            { t.UBottomRight, t.VBottomRight });
    }
}

void LutDrawTool::RecieveTileClick(i32 x, i32 y, i32 z)
{
    u8 caseIndex = GetLutCaseIndex(x, y, z);
    u32 numThisCase = m_tileToolLookupTableEntriesPerCase[caseIndex];
    u16 tile = numThisCase > 0 ? m_tileToolLookupTable[caseIndex][0] : m_tileToolLookupTable[0][0];

    auto edit = UndoableEdit{ EditType::SingleTile };
    SingleTileEditData& d = edit.data.singleTile;
    d.oldVal = m_tiledWorld->GetTile(x, y, z);
    d.newVal = tile;
    d.x = x;
    d.y = y;
    d.z = z;
    PushEdit(edit);

    m_tiledWorld->SetTile(x, y, z, tile);
}

void LutDrawTool::TileSelectionChanged(u32 newTile)
{
    m_tileIndexToSet = newTile;
}

const std::string& LutDrawTool::GetName()
{
    static const std::string s = "lookup table Draw tool";
    return s;
}

void LutDrawTool::PushLookupTableCase(u8 caseIndex, u32 valueToPush)
{
    if (m_tileIndexToSet == 0) {
        return;
    }
    u16* entriesThisCase = m_tileToolLookupTable[caseIndex];
    u32& numEntriesThisCase = m_tileToolLookupTableEntriesPerCase[caseIndex];
    for (u32 i = 0; i < numEntriesThisCase; i++) {
        if (valueToPush == entriesThisCase[i]) {
            return;
        }
    }
    if (numEntriesThisCase + 1 == MAX_TILES_PER_LUT_CASE) {
        return;
    }
    entriesThisCase[numEntriesThisCase++] = valueToPush;
}

void LutDrawTool::PopLookupTableCase(u8 caseIndex)
{
    u16* entriesThisCase = m_tileToolLookupTable[caseIndex];
    u32& numEntriesThisCase = m_tileToolLookupTableEntriesPerCase[caseIndex];
    if (numEntriesThisCase == 0) {
        return;
    }
    numEntriesThisCase--;
}

u8 LutDrawTool::GetLutCaseIndex(i32 x, i32 y, i32 z)
{
    u8 r = 0x00;
    if (y - 1 < 0 || x - 1 < 0 || IsTileIndexInLut(m_tiledWorld->GetTile(x - 1, y - 1, z))) {
        r |= (1 << 0);
    }
    if (y - 1 < 0 || IsTileIndexInLut(m_tiledWorld->GetTile(x, y - 1, z))) {
        r |= (1 << 1);

    }
    if (y - 1 < 0 || IsTileIndexInLut(m_tiledWorld->GetTile(x + 1, y - 1, z))) {
        r |= (1 << 2);

    }
    if (x - 1 < 0 || IsTileIndexInLut(m_tiledWorld->GetTile(x - 1, y, z))) {
        r |= (1 << 3);

    }
    if (IsTileIndexInLut(m_tiledWorld->GetTile(x + 1, y, z))) {
        r |= (1 << 4);

    }
    if (x - 1 < 0 || IsTileIndexInLut(m_tiledWorld->GetTile(x - 1, y + 1, z))) {
        r |= (1 << 5);

    }
    if (IsTileIndexInLut(m_tiledWorld->GetTile(x, y + 1, z))) {
        r |= (1 << 6);

    }
    if (IsTileIndexInLut(m_tiledWorld->GetTile(x + 1, y + 1, z))) {
        r |= (1 << 7);

    }
    return r;
}

bool LutDrawTool::IsTileIndexInLut(u16 tileIndex)
{
    for (int i = 0; i < 256; i++) {
        u32 thisCaseSize = m_tileToolLookupTableEntriesPerCase[i];
        for (int j = 0; j < thisCaseSize; j++) {
            if (m_tileToolLookupTable[i][j] == tileIndex) {
                return true;
            }
        }
    }
    return false;
}

void LutDrawTool::PushLookupTableCase()
{
    u32 caseIndex = GetCurrentLookupTableCaseIndex();
    UndoableEditDataUnion u;
    TileToolLUTCasePushData& d = u.tileToolLUTPush;
    d.lutCase = caseIndex;
    d.valuePushed = m_tileIndexToSet;
    UndoableEdit e = { EditType::TileToolLUTCasePush, u };
    PushEdit(e);
    PushLookupTableCase(caseIndex, m_tileIndexToSet);
}

void LutDrawTool::PopLookupTableCase()
{
    u32 caseIndex = GetCurrentLookupTableCaseIndex();
    UndoableEditDataUnion u;
    TileToolLUTCasePopData& d = u.tileToolLUTPop;
    d.lutCase = caseIndex;
    d.valuePopped = PeekLUTTableTop(caseIndex);
    UndoableEdit e = { EditType::TileToolLUTCasePop, u };
    PushEdit(e);
    PopLookupTableCase(caseIndex);
}

u8 LutDrawTool::GetCurrentLookupTableCaseIndex()
{
    u8 rVal = 0;
    for (u32 i = 0; i < 8; i++) {
        if (m_tileToolBoxes[i]) {
            rVal |= (1 << i);
        }
    }
    return rVal;
}

static const u32 LUT_TOOL_SAVE_LOAD_BUFFER_SIZE =
(256 * sizeof(u32) +                              // size of entries per lut case table
    (sizeof(u16) * MAX_TILES_PER_LUT_CASE * 256));    // size of lut itself 

static u8 s_saveLoadBuffer[LUT_TOOL_SAVE_LOAD_BUFFER_SIZE];

void LutDrawTool::SaveToolLUT(std::string pathToSave)
{
    memcpy(s_saveLoadBuffer, m_tileToolLookupTableEntriesPerCase, 256 * sizeof(u32));
    auto writePtr = s_saveLoadBuffer + 256 * sizeof(u32);
    for (int i = 0; i < 256; i++) {
        memcpy(writePtr, &m_tileToolLookupTable[i], sizeof(u16) * MAX_TILES_PER_LUT_CASE);
        writePtr += sizeof(u16) * MAX_TILES_PER_LUT_CASE;
    }
    using namespace std;
    ofstream wf(pathToSave, ios::out | ios::binary);
    wf.write((const char*)s_saveLoadBuffer, LUT_TOOL_SAVE_LOAD_BUFFER_SIZE);
}

void LutDrawTool::LoadToolLUT(std::string pathToLoad)
{
    using namespace std;
    ifstream rf(pathToLoad.c_str(), ios::in | ios::binary);
    rf.read((char*)s_saveLoadBuffer, LUT_TOOL_SAVE_LOAD_BUFFER_SIZE);
    memcpy(m_tileToolLookupTableEntriesPerCase, s_saveLoadBuffer, 256 * sizeof(u32));
    auto readPtr = s_saveLoadBuffer + (256 * sizeof(u32));
    for (int i = 0; i < 256; i++) {
        memcpy(&m_tileToolLookupTable[i], readPtr, sizeof(u16) * MAX_TILES_PER_LUT_CASE);
        readPtr += sizeof(u16) * MAX_TILES_PER_LUT_CASE;
    }
}
