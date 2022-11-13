#pragma once
#include "BasicTypedefs.h"
#include <glm/glm.hpp>
#include "QuadTree.h"

struct MetaSpriteDescription;

struct MetaSprite {
	i32 handle;
	glm::vec2 pos;
	
};

struct MetaSpriteComponent {
	std::list<QuadTreeItem<MetaSprite>>::iterator metaSprite;
	const MetaSpriteDescription* description;
};