#pragma once
#include "BasicTypedefs.h"

struct MetaSpriteDescription;

struct MetaSpriteComponent {
	i32 handle;
	MetaSpriteDescription* description;
};

