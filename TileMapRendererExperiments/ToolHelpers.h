#pragma once
#include <glm/glm.hpp>
#include "BasicTypedefs.h"
class Camera2D;
class NewRenderer;

namespace EditorToolHelpers {
	glm::vec2 GetSnappedPosition(const glm::vec2& worldpsacePosition, f32 tileDivider);
	f32 RoundToMultiple(f32 val, f32 step);
	void DrawCrossCursor(const NewRenderer* renderer, const Camera2D& camera, const glm::vec2& pos);
}