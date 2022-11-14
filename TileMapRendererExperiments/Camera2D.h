#pragma once
#include <glm/glm.hpp>
#include <string>
#include "BasicTypedefs.h"

class Camera2D
{
public:
	Camera2D(u32 screenW, u32 screenH, std::string name);
	glm::vec2 FocusPosition;
	float Zoom;
public:
	Camera2D(std::string name) : FocusPosition(glm::vec2(0.0)), Zoom(1.0), _name(name) {}
	glm::mat4x4 GetProjectionMatrix(int windowWidth, int windowHeight) const;
	glm::vec4 GetTLBR() const;
	glm::vec2 MouseScreenPosToWorld(double mousex, double mousey) const;
	virtual void SetWindowWidthAndHeight(u32 newW, u32 newH);
	std::string Name() { return _name; }
protected:
	u32 _screenWidth;
	u32 _screenHeight;
	std::string _name;
};

