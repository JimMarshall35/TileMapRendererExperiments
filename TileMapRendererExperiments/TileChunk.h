#pragma once
#include <memory>
#include "BasicTypedefs.h"

class IRenderer;
class TileChunk
{
public:
	TileChunk(std::unique_ptr<u32[]>& tiles, u32 tilesWidth, u32 tilesHeight, const IRenderer* renderer);
	inline u32 GetVerticesHandle() {
		return _verticesHandle;
	}
private:
	u32 _verticesHandle;
	std::unique_ptr<u32[]> _tiles;
	u32 _width;
	u32 _height;
};

