#include "TileChunk.h"
#include "IRenderer.h"

TileChunk::TileChunk(std::unique_ptr<u32[]>& tiles, u32 tilesWidth, u32 tilesHeight, f32 originX, f32 originY, const IRenderer* renderer)
	:_tiles(std::move(tiles)), _width(tilesWidth), _height(tilesHeight), _originX(originX), _originY(originY)
{
	renderer->UploadTilemapVerticesBuffer(_tiles.get(), _width, _height,_vbo,_vao);
}

void TileChunk::ChangeTileAtXY(u32 x, u32 y, u32 newValue)
{
	_tiles[(_width * y) + x] = newValue;
}
