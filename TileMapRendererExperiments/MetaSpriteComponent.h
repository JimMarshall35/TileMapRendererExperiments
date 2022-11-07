#pragma once
#include "BasicTypedefs.h"
#include <glm/glm.hpp>

struct MetaSpriteDescription;

struct MetaSpriteComponent {
	i32 handle;
	glm::vec2 pos;
	const MetaSpriteDescription* description;
};

