#pragma once
#include <memory>
#include <vector>
#include "TileChunk.h"

class IRenderer;
class Camera2D;
class TileMapWorld
{
public:
	TileMapWorld(const std::shared_ptr<IRenderer>& renderer);
	void Draw(const Camera2D& camera);
	void AddChunk(std::unique_ptr<u32[]>& tiles, u32 tilesWidth, u32 tilesHeight, f32 originX, f32 originY);
private:
	std::vector<TileChunk> _chunks;
	std::shared_ptr<IRenderer> _renderer;
};

