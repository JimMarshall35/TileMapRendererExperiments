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
