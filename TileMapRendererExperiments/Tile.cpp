#include "Tile.h"

Tile::Tile(std::unique_ptr<u8[]>& bytes, u32 PixelsRows, u32 PixelsCols, const std::string& filePath, const std::string& tilesetName)
	:_bytes(std::move(bytes)),
	_tile(TypeOfTile(PixelsCols, PixelsRows, filePath, tilesetName)),
	UTopLeft(0.0f),
	VTopLeft(0.0f),
	UBottomRight(0.0f),
	VBottomRight(0.0f),
	_tilesetName(tilesetName) {

}
