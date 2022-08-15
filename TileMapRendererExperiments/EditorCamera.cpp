#include "EditorCamera.h"

EditorCamera::EditorCamera(const EditorCameraInitializationSettings& init)
	:Camera2D(),
	_moveSpeed(init.moveSpeed)
{
}

void EditorCamera::UpdatePosition(Directions direction,float deltaT)
{
	switch (direction) {
	case Directions::UP:
		FocusPosition.y -= _moveSpeed * deltaT;
		break;
	case Directions::DOWN:
		FocusPosition.y += _moveSpeed * deltaT;
		break;
	case Directions::LEFT:
		FocusPosition.x -= _moveSpeed * deltaT;
		break;
	case Directions::RIGHT:
		FocusPosition.x += _moveSpeed * deltaT;
		break;
	default:
		break;
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

void EditorCamera::OnMouseMove(double lastX, double lastY, int WindowW, int WindowH, float deltaTime)
{
	const auto aLargeScrollMagnitude = 1500.0f;
	if (_isBeingDragged) {
		const auto MAX_MOVE_SPEED = 1200.0f;
		auto worldposDragStart = MouseScreenPosToWorld(_dragStartScreenPixelCoords.x, _dragStartScreenPixelCoords.y, WindowW, WindowH);
		auto worldposDragLast = MouseScreenPosToWorld(lastX, lastY, WindowW, WindowH);

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
