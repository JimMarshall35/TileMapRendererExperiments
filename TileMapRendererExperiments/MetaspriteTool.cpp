#include "MetaspriteTool.h"
#include "imgui/imgui.h"
#include "AtlasLoader.h"
#include "MetaAtlas.h"
#include "MetaspriteComponent.h"
#include "QuadTree.h"

MetaspriteTool::MetaspriteTool(MetaAtlas* metaAtlas, AtlasLoader* atlasLoader, DynamicQuadTreeContainer<MetaSpriteComponent>* metaspritesQuadTree)
    :m_metaAtlas(metaAtlas),
    m_atlasLoader(atlasLoader),
    m_metaspritesQuadTree(metaspritesQuadTree)
{
}

void MetaspriteTool::DoUi()
{
    if (ImGui::Checkbox("AutoAdvance", &m_autoAdvance));
    if(ImGui::InputInt("metasprite w", (int*)&m_currentMetaspriteWidth) 
        || ImGui::InputInt("metasprite h", (int*)&m_currentMetaspriteHeight)) {
        m_currentMetasprite.resize(m_currentMetaspriteWidth * m_currentMetaspriteHeight);
    }

    ImGui::BeginGroup();
    for (u32 i = 0; i < m_currentMetaspriteHeight; i++) {
        for (u32 j = 0; j < m_currentMetaspriteWidth; j++) {
            auto currentMetaspriteIndex = (i * m_currentMetaspriteWidth) + j;
            const Tile& t = m_atlasLoader->GetIndividualTiles()[m_currentMetasprite[currentMetaspriteIndex]];
            ImGui::PushID(currentMetaspriteIndex);
            bool pushedStyle = false;
            if (currentMetaspriteIndex == m_selectedMetaspriteTileIndex) {
                ImGui::PushStyleColor(ImGuiCol_Button, { 1.0,1.0,1.0,1.0 });
                pushedStyle = true;
            }
            if (ImGui::ImageButton((void*)(intptr_t)m_atlasLoader->GetAtlasTextureHandle(),
                { 32,32 },
                { t.UTopLeft, t.VTopLeft },
                { t.UBottomRight, t.VBottomRight },
                0)) {
                m_selectedMetaspriteTileIndex = currentMetaspriteIndex;
            }
            if (pushedStyle) {
                ImGui::PopStyleColor(1);

            }
            ImGui::PopID();
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::BeginListBox("loaded meta sprites");
    
    const MetaSpriteDescription* d;
    u32 numSprites;
    m_metaAtlas->GetSprites(&d, &numSprites);
    for (int i = 0; i < numSprites; i++) {
        bool selected = false;
        if (m_currentMetaspriteHandle == i) {
            selected = true;
        }
        if (ImGui::Selectable(d[i].name.c_str(), selected)) {
            m_currentMetaspriteHandle = i;
            const auto d = m_metaAtlas->getDescription(i);
            m_currentMetaspriteHeight = d->spriteTilesHeight;
            m_currentMetaspriteWidth = d->spriteTilesWidth;
            m_currentMetasprite.clear();
            for (int i = 0; i < d->numTiles; i++) {
                m_currentMetasprite.push_back(d->tiles[i]);
            }
        }
    }

    ImGui::EndListBox();
    ImGui::EndGroup();
    static char metaspriteNameBuffer[200];
    ImGui::InputText("metasprite name", metaspriteNameBuffer, 200);
    if(ImGui::Button("Save Ms")) {
        SaveMetaSprite(metaspriteNameBuffer);
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Ms file")) {
        m_metaAtlas->SaveToFile("data\\metasprites.atlas");
    }
}

void MetaspriteTool::RecieveTileClick(i32 x, i32 y, i32 z)
{
}

void MetaspriteTool::TileSelectionChanged(u32 newTile)
{
    if (m_selectedMetaspriteTileIndex < m_currentMetaspriteHeight * m_currentMetaspriteWidth) {
        m_currentMetasprite[m_selectedMetaspriteTileIndex] = newTile;
    }
    if (m_autoAdvance) {
        m_selectedMetaspriteTileIndex++;
        if (m_selectedMetaspriteTileIndex >= m_currentMetaspriteWidth * m_currentMetaspriteHeight) {
            m_selectedMetaspriteTileIndex = 0;
        }
    }
    
}

const std::string& MetaspriteTool::GetName()
{
    static std::string name = "Meta sprite";
    return name;
}

void MetaspriteTool::RecieveWorldspaceClick(const glm::vec2& click)
{
    if (m_currentMetaspriteHandle < 0) {
        return;
    }
    const auto d = m_metaAtlas->getDescription(m_currentMetaspriteHandle);
    MetaSpriteComponent c = {m_currentMetaspriteHandle, click, d};
    Rect r;
    r.pos = click;
    r.dims = { d->spriteTilesWidth, d->spriteTilesHeight };

    m_metaspritesQuadTree->insert(c, r);
}

void MetaspriteTool::SaveMetaSprite(std::string name) {
    MetaSpriteDescription d;
    d.name = name;
    d.numTiles = m_currentMetaspriteWidth * m_currentMetaspriteHeight;
    d.spriteTilesHeight = m_currentMetaspriteHeight;
    d.spriteTilesWidth = m_currentMetaspriteWidth;
    memcpy(d.tiles, &m_currentMetasprite[0], sizeof(u16) * d.numTiles);
    m_currentMetaspriteHandle = m_metaAtlas->LoadMetaSprite(d);
}