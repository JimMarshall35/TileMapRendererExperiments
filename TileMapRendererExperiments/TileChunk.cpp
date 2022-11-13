
#include <glm/glm.hpp>
#include <iostream>
#include "TileChunk.h"
#include "Camera2D.h"
#include "TiledWorld.h"
#include "NewRenderer.h"
#include "BasicTypedefs.h"


void TileChunk::DrawVisibleChunks(ArrayTexture2DHandle tilesTexture, const NewRenderer& renderer, const Camera2D& cam, const TiledWorld& world, u32 screenW, u32 screenH)
{
	using namespace glm;
	auto tlbr = cam.GetTLBR();
	
	auto rawTL = ivec2(tlbr[1], tlbr[0]);
	auto rawBR = ivec2(tlbr[3], tlbr[2]);
	rawTL -= 0.5f;
	rawBR -= 0.5f;

	rawTL.x = rawTL.x < 0 ? 0 : rawTL.x;
	rawTL.y = rawTL.y < 0 ? 0 : rawTL.y;

	rawTL.x >= world.GetMapWidth() ? world.GetMapWidth() - 1 : rawTL.x;
	rawTL.y >= world.GetMapHeight() ? world.GetMapHeight() - 1 : rawTL.y;

	rawBR.x = rawBR.x < 0 ? 0 : rawBR.x;
	rawBR.y = rawBR.y < 0 ? 0 : rawBR.y;

	rawBR.x >= world.GetMapWidth() ? world.GetMapWidth() - 1 : rawBR.x;
	rawBR.y >= world.GetMapHeight() ? world.GetMapHeight() - 1 : rawBR.y;

	const auto padding = 2;
	auto chunkSize = ivec2{ (rawBR.x - rawTL.x) + padding, (rawBR.y - rawTL.y) + padding };
	renderer.DrawChunk(
		rawTL,
		rawTL,
		{1,1},
		0,
		world,
		tilesTexture,
		cam,
		chunkSize
	);

}