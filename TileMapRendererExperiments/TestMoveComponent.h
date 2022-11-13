#pragma once
#include <glm/glm.hpp>
struct TestMoveComponent {
	glm::vec2 startPos;
	glm::vec2 endPos;
	float cycleTime;
	float t;
};