#include "EditorToolWithSnappingBase.h"
#include "imgui/imgui.h"
#include "ToolHelpers.h"

void EditorToolWithSnappingBase::DoUi()
{
	if (ImGui::InputInt("snapping", (int*)&m_snappingIncrementTileDivider)) {
	}
}

glm::vec2 EditorToolWithSnappingBase::GetSnappedPosition(const glm::vec2& worldSpace)
{
	return EditorToolHelpers::GetSnappedPosition(worldSpace, m_snappingIncrementTileDivider);
}
