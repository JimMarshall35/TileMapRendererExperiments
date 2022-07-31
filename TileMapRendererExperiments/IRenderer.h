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
	virtual bool GPULoadAtlasTexture(const unsigned char* data, unsigned int width, unsigned int height, unsigned int* id) = 0;
	virtual bool GPULoadTileData(const std::vector<Tile>& tiles) = 0;
	virtual void DrawTileMap(
		VertexBufferObjectHandle vertexBufferHandle,
		u32 numVertices,
		const glm::ivec2& mapSize,
		const glm::vec2& pos,
		const glm::vec2& scale,
		float rotation,
		const Camera2D& cam) const = 0;
	virtual void SetWindowWidthAndHeight(u32 width, u32 height) = 0;
	virtual void UploadTilemapVerticesBuffer(const u32* vertices, u32 tilesWidth, u32 tilesHeight,
		VertexBufferObjectHandle& vbo, VertexAttributeObjectHandle& vao) const = 0;
};