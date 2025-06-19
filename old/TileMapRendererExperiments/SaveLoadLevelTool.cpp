#include "SaveLoadLevelTool.h"
#include "imgui/imgui.h"
#include "ECS.h"
#include <string>
#include <fstream>
SaveLoadLevelTool::SaveLoadLevelTool(ECS* ecs)
    :m_ecs(ecs)
{
}
void SaveLoadLevelTool::DoUi()
{
    if (ImGui::Button("save to json")) {
        std::string json = std::string(m_ecs->GetWorld()->to_json().c_str());
        std::ofstream fs("levelEditorState.json");
        fs << json;
    }
    if (ImGui::Button("load from json")) {
        std::string json = std::string(m_ecs->GetWorld()->to_json().c_str());
        std::ifstream fs("levelEditorState.json");
        fs >> json;
        const char* msg = m_ecs->GetWorld()->from_json(json.c_str());
    }
}

void SaveLoadLevelTool::RecieveTileClick(i32 x, i32 y, i32 z)
{
}

const std::string& SaveLoadLevelTool::GetName()
{
    // TODO: insert return statement here
    static const std::string saveLoadLevelToolName = "Save / load level";
    return saveLoadLevelToolName;
}
