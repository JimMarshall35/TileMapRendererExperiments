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
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "GameInput.h"
#include "CameraManager.h"
#include "EditorCamera.h"
#include "Undo.h"

#define EDITOR_CAM_NAME "Editor"


EditorUi::~EditorUi()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

EditorUi::EditorUi(TiledWorld* tiledWorld, AtlasLoader* atlasLoader, ECS* ecsWorld, const IFileSystem* fileSystem, EditorToolBase** tools, u32 numTools, GLFWwindow* window, CameraManager* camManager)
	:m_atlasLoader(atlasLoader),
	m_tiledWorld(tiledWorld),
    m_ecsWorld(ecsWorld),
    m_numTools(numTools),
    m_tools(tools),
    m_camManager(camManager),
    m_cam((EditorCamera*)camManager->GetCameraByName(EDITOR_CAM_NAME))
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = &ImGui::GetIO(); (void)m_io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void EditorUi::DoUiWindow()
{
    ImGui::Begin("Tiles");
    auto windowWidth = ImGui::GetWindowWidth();
    auto accumulatedWidth = 0;
    auto onIndex = 0;;
    auto zoom = 5.0f;
    ImGui::SliderInt("layer", &m_layerToSet, 0, m_tiledWorld->GetNumLayers() - 1);
    //bool* visibilities = m_tiledWorld->GetLayersVisibilities();
    TiledWorld::TileLayer* tileLayers = m_tiledWorld->GetTileLayers();
    for (int i = 0; i < m_tiledWorld->GetNumLayers(); i++) {
        ImGui::BeginGroup();

        ImGui::Columns(2);
        char buf[256];
        strcpy(buf, tileLayers[i].Name.c_str());
        ImGui::InputText("", buf, 256);
        if (strcmp(buf, tileLayers[i].Name.c_str())!=0)
        {
            tileLayers[i].Name = std::string(buf);
        }
        
        ImGui::NextColumn();
        ImGui::Checkbox((std::string("layer ") + std::to_string(i)).c_str(), &tileLayers[i].Visible);
        ImGui::NextColumn();

        ImGui::EndGroup();

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

void EditorUi::MouseButtonCallback()
{
    auto world = m_cam->MouseScreenPosToWorld(m_lastMouseX, m_lastMouseY);
    int tileX = world.x - 0.5f;
    int tileY = world.y - 0.5f;
    
    m_tools[m_selectedTool]->RecieveWorldspaceClick(world);
    //m_tiledWorld->SetTile(tileX + 1, tileY + 1, m_layerToSet, m_tileIndexToSet);
    m_tools[m_selectedTool]->RecieveTileClick(tileX + 1, tileY + 1, m_layerToSet);
}

void EditorUi::RightMouseButtonCallback()
{
    auto world = m_cam->MouseScreenPosToWorld(m_lastMouseX, m_lastMouseY);
    int tileX = world.x - 0.5f;
    int tileY = world.y - 0.5f;

    m_tools[m_selectedTool]->RecieveWorldspaceRightClick(world);
}

void EditorUi::Draw(const Camera2D& camera) const
{
    if (m_tools[m_selectedTool]->WantsToDrawOverlay()) {
        glm::vec2 mouseWorld = camera.MouseScreenPosToWorld(m_lastMouseX, m_lastMouseY);
        m_tools[m_selectedTool]->DrawOverlay(camera, mouseWorld);
    }
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

bool EditorUi::MasksPreviousDrawableLayer() const
{
    return false;
}

std::string EditorUi::GetDrawableLayerName() const
{
    return "Editor";
}

void EditorUi::OnDrawablePush()
{
    m_oldCameraName = m_camManager->GetActiveCameraName();
    m_camManager->SetActiveCamera(EDITOR_CAM_NAME);
}

void EditorUi::OnDrawablePop()
{
    m_camManager->SetActiveCamera(m_oldCameraName);
}

void EditorUi::ReceiveInput(const GameInput& input)
{
    switch (input.type) {
    case GameInputType::CtrlHeldKeyPress:
        if (input.data.ctrlKeyCombo.keyPressed == 'z') {

        }
        else if (input.data.ctrlKeyCombo.keyPressed == 'y') {
            //Redo(*m_tiledWorld,)
        }
        break;
    case GameInputType::DirectionInput:
        m_camMoveDirections = input.data.direction.directions;
        break;
    case GameInputType::MouseButton:
        if (m_wantMouseInput) {
            break;
        }
        if (input.data.mouseButton.button == MouseButton::Left && input.data.mouseButton.action == MouseButtonAction::Press) {
            
            //cam->StartDrag(lastX, lastY);
            MouseButtonCallback();
            m_dragging = true;
            
        }
        else if (input.data.mouseButton.button == MouseButton::Left && input.data.mouseButton.action == MouseButtonAction::Release) {
            //cam->StopDrag();
            m_dragging = false;
        }
        else if (input.data.mouseButton.button == MouseButton::Right && input.data.mouseButton.action == MouseButtonAction::Press) {

            RightMouseButtonCallback();

        }

        break;
    case GameInputType::MouseMove:
        m_lastMouseX = input.data.mouseMove.xposIn;
        m_lastMouseY = input.data.mouseMove.yposIn;
        m_cam->OnMouseMove(m_lastMouseX, m_lastMouseY,m_lastDeltaT);
        if (m_dragging) {
            MouseButtonCallback();
        }
        break;
    case GameInputType::MouseScrollWheel:
        if (!m_wantMouseInput) {
            m_cam->Zoom *= input.data.mouseScrollWheel.offset > 0 ? (1.1 * input.data.mouseScrollWheel.offset) : 0.9 / abs(input.data.mouseScrollWheel.offset);
        }
        break;
    default:
        break;
    }
}

bool EditorUi::MasksPreviousInputLayer() const
{
    return true;
}

std::string EditorUi::GetInputLayerName() const
{
    return "Editor";
}

void EditorUi::OnInputPush()
{
}

void EditorUi::OnInputPop()
{
}

void EditorUi::Update(float deltaT)
{
    m_lastDeltaT = deltaT;
    m_wantMouseInput = m_io->WantCaptureMouse;
    m_wantKeyboardInput = m_io->WantCaptureKeyboard;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    DoUiWindow();
    ImGui::ShowDemoWindow();

    m_cam->OnUpdate(deltaT);
    m_cam->UpdatePosition(m_camMoveDirections, deltaT);
    // render
    // ------
    ImGui::Render();
}

bool EditorUi::MasksPreviousUpdateableLayer() const
{
    return true;
}

std::string EditorUi::GetUpdateableLayerName() const
{
    return "Editor";
}

void EditorUi::OnUpdatePush()
{
}

void EditorUi::OnUpdatePop()
{
}
