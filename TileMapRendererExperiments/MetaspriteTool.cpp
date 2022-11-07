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
    if(ImGui::InputInt("metasprite w", (int*)&m_currentMetaspriteWidth) 
        || ImGui::InputInt("metasprite h", (int*)&m_currentMetaspriteHeight)) {
        m_currentMetasprite.resize(m_currentMetaspriteWidth * m_currentMetaspriteHeight);
    }
    ;
    for (u32 i = 0; i < m_currentMetaspriteHeight; i++) {
        for (u32 j = 0; j < m_currentMetaspriteWidth; j++) {
            auto currentMetaspriteIndex = (i * m_currentMetaspriteWidth) + j;
            const Tile& t = m_atlasLoader->GetIndividualTiles()[m_currentMetasprite[currentMetaspriteIndex]];
            ImGui::PushID(currentMetaspriteIndex);
            if (ImGui::ImageButton((void*)(intptr_t)m_atlasLoader->GetAtlasTextureHandle(),
                { 32,32 },
                { t.UTopLeft, t.VTopLeft },
                { t.UBottomRight, t.VBottomRight },
                0)) {
                m_selectedMetaspriteTileIndex = currentMetaspriteIndex;
            }
            ImGui::PopID();
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }
    static char metaspriteNameBuffer[200];
    ImGui::InputText("metasprite name", metaspriteNameBuffer, 200);
    if(ImGui::Button("Save Ms")) {
        MetaSpriteDescription d;
        d.name = std::string(metaspriteNameBuffer);
        d.numTiles = m_currentMetaspriteWidth * m_currentMetaspriteHeight;
        d.spriteTilesHeight = m_currentMetaspriteHeight;
        d.spriteTilesWidth = m_currentMetaspriteWidth;
        memcpy(d.tiles, &m_currentMetasprite[0], sizeof(u16) * d.numTiles);
        m_currentMetaspriteHandle = m_metaAtlas->LoadMetaSprite(d);
    }
}

void MetaspriteTool::RecieveTileClick(i32 x, i32 y, i32 z)
{
}

void MetaspriteTool::TileSelectionChanged(u32 newTile)
{
    m_currentMetasprite[m_selectedMetaspriteTileIndex] = newTile;
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
