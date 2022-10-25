
#include <glm/glm.hpp>
#include <iostream>
#include "TileChunk.h"
#include "Camera2D.h"
#include "TiledWorld.h"
#include "NewRenderer.h"


void TileChunk::DrawVisibleChunks(ArrayTexture2DHandle tilesTexture, const NewRenderer& renderer, const Camera2D& cam, const TiledWorld& world, u32 chunkSizeX, u32 chunkSizeY, u32 screenW, u32 screenH)
{
	using namespace glm;
	auto tlbr = cam.GetTLBR(screenW,screenH);
	auto chunkTL = ivec2(tlbr[1] / chunkSizeX, tlbr[0] / chunkSizeY);
	auto chunkBR = ivec2(tlbr[3] / chunkSizeX, tlbr[2] / chunkSizeY);


	u32 startX = chunkTL.x > 0 ? chunkTL.x : 0;
	startX = chunkTL.x < world.GetMapWidth() ? chunkTL.x : world.GetMapWidth() - 1;
	u32 finishX = chunkBR.x > 0 ? chunkBR.x : 0;
	u32 startY = chunkTL.y > 0 ? chunkTL.y : 0;
	u32 finishY = chunkBR.y > 0 ? chunkBR.y : 0;
	u32 chunksDrawn = 0;
	for (u32 y = startY; y <= finishY; y++) {
		for (u32 x = startX; x <= finishX; x++) {
			if (x * chunkSizeX >= world.GetMapWidth() || y * chunkSizeY >= world.GetMapHeight()) {
				continue;
			}
			chunksDrawn++;
			renderer.DrawChunk(
				{ chunkSizeX * x, chunkSizeY * y },
				{ chunkSizeX * x, chunkSizeY * y },
				{ 1,1 },
				0,
				world,
				tilesTexture,
				cam
			);
		}
	}

	//std::cout << "tl x: " << chunkTL.x <<
	//	" tl y: " << chunkTL.y <<
	//	" br x: " << chunkBR.x <<
	//	" br y: " << chunkBR.y << 
	//	" chunks drawn: "<< chunksDrawn << "\n\n\n";
}