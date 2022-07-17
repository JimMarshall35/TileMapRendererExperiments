#pragma once
#include <glm/glm.hpp>
class Camera2D
{
public:
	glm::vec2 FocusPosition;
	float Zoom;
public:
	Camera2D() : FocusPosition(glm::vec2(0.0)), Zoom(1.0) {}
	glm::mat4x4 GetProjectionMatrix(int windowWidth, int windowHeight) const;
	glm::vec4 GetTLBR(int _ScreenW, int _ScreenH) const;
	glm::vec2 MouseScreenPosToWorld(double mousex, double mousey, unsigned int windowW, unsigned int windowH) const;
private:

};

