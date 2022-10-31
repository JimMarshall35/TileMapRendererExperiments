#include "EditorCamera.h"

EditorCamera::EditorCamera(const EditorCameraInitializationSettings& init)
	:Camera2D(init.screenWidth, init.screenHeight),
	_moveSpeed(init.moveSpeed)
{
}

void EditorCamera::UpdatePosition(Directions direction, float deltaT)
{
	glm::vec2 moveVec = { 0,0 };
	bool move = false;
	if (direction & Directions::UP) {
		moveVec.y = -1.0f;
		move = true;
	}
	if (direction & Directions::DOWN) {
		moveVec.y = 1.0f;
		move = true;

	}
	if (direction & Directions::LEFT) {
		moveVec.x = -1.0f;
		move = true;

	}
	if (direction & Directions::RIGHT) {
		moveVec.x = 1.0f;
		move = true;

	}

	if (move) {
		FocusPosition += glm::normalize(moveVec) * _moveSpeed * deltaT;
	}

}

void EditorCamera::StartDrag(double x, double y)
{
	_maxScalar = 0;
	_isBeingDragged = true;
	_dragStartScreenPixelCoords.x = x;
	_dragStartScreenPixelCoords.y = y;
}

void EditorCamera::StopDrag()
{
	_isBeingDragged = false;
	_cameraMoveVector = glm::dvec2(0.0f);
}

void EditorCamera::OnMouseMove(double lastX, double lastY, float deltaTime)
{
	const auto aLargeScrollMagnitude = 1500.0f;
	if (_isBeingDragged) {
		const auto MAX_MOVE_SPEED = 1200.0f;
		auto worldposDragStart = MouseScreenPosToWorld(_dragStartScreenPixelCoords.x, _dragStartScreenPixelCoords.y);
		auto worldposDragLast = MouseScreenPosToWorld(lastX, lastY);

		auto scaler = glm::distance(worldposDragStart, worldposDragLast) / aLargeScrollMagnitude;
		if (scaler > _maxScalar) {
			_maxScalar = scaler;
		}

		_cameraMoveVector = -(glm::normalize(glm::vec2(lastX, lastY) - glm::vec2(_dragStartScreenPixelCoords)) * deltaTime * MAX_MOVE_SPEED * _maxScalar);

	}
}

void EditorCamera::OnUpdate(float deltaTime)
{
	if (_isBeingDragged) {
		FocusPosition.x += _cameraMoveVector.x;
		FocusPosition.y += _cameraMoveVector.y;
	}
}


