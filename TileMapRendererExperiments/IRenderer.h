#pragma once
#include <vector>
#include <glm/fwd.hpp>

#include "BasicTypedefs.h"

typedef u32 VertexBufferObjectHandle;
typedef u32 VertexAttributeObjectHandle;
class Camera2D;
class Tile;
class IRenderer {
public:
	virtual void SetWindowWidthAndHeight(u32 width, u32 height) = 0;
};