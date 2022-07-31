#pragma once
#include <memory>
#include "BasicTypedefs.h"

class IRenderer;
class TileChunk
{
public:
	TileChunk(std::unique_ptr<u32[]>& tiles, u32 tilesWidth, u32 tilesHeight, f32 originX, f32 originY, const IRenderer* renderer);
	inline u32 GetVaoHandle() {
		return _vao;
	}
	inline f32 GetOriginX() {
		return _originX;
	}
	inline f32 GetOriginY() {
		return _originY;
	}
	void ChangeTileAtXY(u32 x, u32 y, u32 newValue);
private:
	u32 _vao;
	u32 _vbo;
	std::unique_ptr<u32[]> _tiles;
	u32 _width;
	u32 _height;
	f32 _originX;
	f32 _originY;
};

