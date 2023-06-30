#include "EditorToolBase.h"

void EditorToolBase::RecieveWorldspaceClick(const glm::vec2& worldspace)
{
}

void EditorToolBase::RecieveWorldspaceRightClick(const glm::vec2& worldspace)
{
}

bool EditorToolBase::WantsToDrawOverlay() const
{
	return false;
}

void EditorToolBase::DrawOverlay(const Camera2D& camera, const glm::vec2& mouseWorldSpacePos)
{
}
