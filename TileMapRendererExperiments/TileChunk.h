#pragma once

#include "BasicTypedefs.h"

class Camera2D;
class NewRenderer;
struct TiledWorld;
using ArrayTexture2DHandle = u32;

class TileChunk
{
public:
	static void DrawVisibleChunks(ArrayTexture2DHandle tilesTexture, const NewRenderer& renderer, const Camera2D& cam, const TiledWorld& world, u32 chunkSizeX, u32 chunkSizeY, u32 screenW, u32 screenH);
};