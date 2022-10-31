#include "EditorUi.h"
#include "imgui/imgui.h"
#include "TiledWorld.h"
#include "AtlasLoader.h"
#include "flecs.h"
#include "Undo.h"
#include "Camera2D.h"
#include "IFilesystem.h"
#include <iostream>
#include <fstream>
#include "EditorToolBase.h"

EditorUi::EditorUi(TiledWorld* tiledWorld, AtlasLoader* atlasLoader, flecs::world* ecsWorld, const IFileSystem* fileSystem, EditorToolBase** tools, u32 numTools)
	:m_atlasLoader(atlasLoader),
	m_tiledWorld(tiledWorld),
    m_ecsWorld(ecsWorld),
    m_numTools(numTools),
    m_tools(tools)
{
}

void EditorUi::DoUiWindow()
{
    ImGui::Begin("Tiles");
    auto windowWidth = ImGui::GetWindowWidth();
    auto accumulatedWidth = 0;
    auto onIndex = 0;;
    auto zoom = 5.0f;
    ImGui::SliderInt("layer", &m_layerToSet, 0, m_tiledWorld->GetNumLayers() - 1);
    bool* visibilities = m_tiledWorld->GetLayersVisibilities();
    for (int i = 0; i < m_tiledWorld->GetNumLayers(); i++) {
        ImGui::Checkbox((std::string("layer ") + std::to_string(i)).c_str(), &visibilities[i]);
    }
    static const char* current_item = m_tools[m_selectedTool]->GetName().c_str();
    if (ImGui::BeginCombo("##toolcombo", current_item)) {
        for (int n = 0; n < m_numTools; n++)
        {
            bool is_selected = (current_item == m_tools[n]->GetName().c_str()); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(m_tools[n]->GetName().c_str(), is_selected)) {
                current_item = m_tools[n]->GetName().c_str();
                m_selectedTool = n;
            }
                
            if (is_selected)
                ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    m_tools[m_selectedTool]->DoUi();

    ImGui::Separator();

    ImGui::BeginChild("tiles");

    for (const auto& t : m_atlasLoader->GetIndividualTiles()) {
        ImGui::PushID(onIndex);
        if (ImGui::ImageButton((void*)(intptr_t)m_atlasLoader->GetAtlasTextureHandle(),
            { (float)t.GetPixelsCols() * zoom, (float)t.GetPixelsRows() * zoom },
            { t.UTopLeft, t.VTopLeft },
            { t.UBottomRight, t.VBottomRight })) {
            m_tools[m_selectedTool]->TileSelectionChanged(onIndex);
        }
        ImGui::PopID();
        accumulatedWidth += t.GetPixelsCols() * zoom;
        if (accumulatedWidth + t.GetPixelsCols() * zoom < windowWidth) {
            ImGui::SameLine();

        }
        else {
            accumulatedWidth = 0;
        }
        onIndex++;
    }
    ImGui::EndChild();

    ImGui::End();
}

void EditorUi::MouseButtonCallback(float lastX, float lastY, const Camera2D& cam)
{
    auto world = cam.MouseScreenPosToWorld(lastX, lastY);
    int tileX = world.x - 0.5f;
    int tileY = world.y - 0.5f;
    
    //m_tiledWorld->SetTile(tileX + 1, tileY + 1, m_layerToSet, m_tileIndexToSet);
    m_tools[m_selectedTool]->RecieveTileClick(tileX + 1, tileY + 1, m_layerToSet);
}