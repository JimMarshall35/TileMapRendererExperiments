#pragma once
#include "Camera2D.h"
class GameCamera : public Camera2D
{
public:
	GameCamera();
private:
	void ClampPosition();
};

