#include "Tile.h"

Tile::Tile(std::unique_ptr<u8[]>& bytes, u32 PixelsRows, u32 PixelsCols, const std::string& filePath)
	:_bytes(std::move(bytes)),
	_tile(TypeOfTile(PixelsCols, PixelsRows, filePath)),
	UTopLeft(0.0f),
	VTopLeft(0.0f),
	UBottomRight(0.0f),
	VBottomRight(0.0f){

}
