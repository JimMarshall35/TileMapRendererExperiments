
#include <glm/glm.hpp>
#include <iostream>
#include "TileChunk.h"
#include "Camera2D.h"
#include "TiledWorld.h"
#include "NewRenderer.h"
#include "BasicTypedefs.h"


void TileChunk::DrawVisibleChunks(ArrayTexture2DHandle tilesTexture, const NewRenderer& renderer, const Camera2D& cam, const TiledWorld& world, u32 chunkSizeX, u32 chunkSizeY, u32 screenW, u32 screenH)
{
	using namespace glm;
	auto tlbr = cam.GetTLBR();
	auto chunkTL = ivec2(tlbr[1] / chunkSizeX, tlbr[0] / chunkSizeY);
	auto chunkBR = ivec2(tlbr[3] / chunkSizeX, tlbr[2] / chunkSizeY);


	int startX = chunkTL.x;
	int finishX = chunkBR.x;
	int startY = chunkTL.y;
	int finishY = chunkBR.y;
	u32 chunksDrawn = 0;
	for (int y = startY; y <= finishY; y++) {
		for (int x = startX; x <= finishX; x++) {
			if (x < 0 || y < 0 || x * chunkSizeX >= world.GetMapWidth() || y * chunkSizeY >= world.GetMapHeight()) {
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