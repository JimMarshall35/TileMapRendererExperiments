#pragma once
#include "BasicTypedefs.h"
#include <glm/glm.hpp>
#include "QuadTree.h"

struct MetaSpriteDescription;
namespace flecs {
	struct entity;
};

struct MetaSprite {
	i32 handle;
	glm::vec2 pos;
};

struct MetaSpriteComponent {
	i32 handle;
	glm::vec2 pos;
	std::list<QuadTreeItem<flecs::entity>>::iterator quadtreeIterator;
};