#include "TileChunk.h"
#include "IRenderer.h"

TileChunk::TileChunk(std::unique_ptr<u32[]>& tiles, u32 tilesWidth, u32 tilesHeight, const IRenderer* renderer)
	:_tiles(std::move(tiles)), _width(tilesWidth), _height(tilesHeight)
{
	_verticesHandle = renderer->UploadTilemapVerticesBuffer(_tiles.get(), _width, _height);
}
