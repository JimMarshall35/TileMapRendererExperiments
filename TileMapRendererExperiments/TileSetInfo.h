#pragma once
#include <string>
#include "BasicTypedefs.h"

struct TileSetInfo {
	std::string Path;
	u32 TileWidth;
	u32 TileHeight;
	// the inital start of the rect containing the tiles, the top left x coord in pixels
	u32 PixelColStart;
	// the inital start of the rect containing the tiles, the top left y coord in pixels
	u32 PixelRowStart;
	// space between each repeating tile
	u32 RightMargin;
	u32 BottomMargin;
	// the dimensions of the rect containg the tiles
	u32 RowsOfTiles;
	u32 ColsOfTiles;
	// does the last tile have a right and bottom margin
	// NOT IMPLEMENTED
	bool LastTileInRowHasRightMargin = true;
	bool LastTileInColHasBottomMargin = true;
};