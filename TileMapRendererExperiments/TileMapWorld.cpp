#include "TileMapWorld.h"

TileMapWorld::TileMapWorld(const std::shared_ptr<IRenderer>& renderer)
	:_renderer(renderer)
{
}

void TileMapWorld::Draw(const Camera2D& camera)
{
}

void TileMapWorld::AddChunk(std::unique_ptr<u32[]>& tiles, u32 tilesWidth, u32 tilesHeight, f32 originX, f32 originY)
{
	_chunks.push_back(TileChunk(tiles, tilesWidth, tilesHeight, originX,originY, _renderer.get()));
}
