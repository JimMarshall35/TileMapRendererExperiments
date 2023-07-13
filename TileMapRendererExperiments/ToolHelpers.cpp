#include "ToolHelpers.h"
#include "Camera2D.h"
#include "NewRenderer.h"

glm::vec2 EditorToolHelpers::GetSnappedPosition(const glm::vec2& worldpsacePosition, f32 tileDivider)
{
    if (tileDivider == 0.0f) {
        return worldpsacePosition;
    }
    return glm::vec2(
        round_to_multiple(worldpsacePosition.x, 1.0f / (f32)tileDivider) - 0.5f,
        round_to_multiple(worldpsacePosition.y, 1.0f / (f32)tileDivider) - 0.5f
    );
}

f32 EditorToolHelpers::round_to_multiple(f32 val, f32 step)
{
    return step * std::round(val / step);
}

void EditorToolHelpers::DrawCrossCursor(const NewRenderer* renderer, const Camera2D& camera, const glm::vec2& pos)
{
    // draw a green cross
    glm::vec2 pt1 = pos + glm::vec2{ 0.1, 0.1 };
    glm::vec2 pt2 = pos + glm::vec2{ -0.1, -0.1 };

    renderer->DrawLine(pt1, pt2, { 0,1,0,1.0 }, 3, camera);

    pt1 = pos + glm::vec2{ -0.1, 0.1 };
    pt2 = pos + glm::vec2{ 0.1, -0.1 };

    renderer->DrawLine(pt1, pt2, { 0,1,0,1.0 }, 3, camera);
}
