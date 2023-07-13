#pragma once
#include "BasicTypedefs.h"
#include <glm/glm.hpp>
#include "QuadTree.h"
#include <string>
#include <vector>

namespace flecs {
	struct entity;
};

struct MetaSpriteComponent {
	i32 handle;
	glm::vec2 pos;
	bool ready = false;
};
#define MAX_METASPRITE_SIZE 128


struct MetaSpriteDescription {
	std::string name;
	u32 spriteTilesWidth;
	u32 spriteTilesHeight;
	//u32 numTiles = 0;
	std::vector<u16> tiles;
};
