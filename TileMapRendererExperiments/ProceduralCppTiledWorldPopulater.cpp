#include "ProceduralCppTiledWorldPopulater.h"
#include "GeneratedTileDefs.h"
#include <glm/glm.hpp>
#include <vector>

#include "ProceduralHelpers.h"

static const u16 MUD_CENTERS[] = { 893,894,974,1009, };
static const u16 GRASS_CENTERS[] = { 964 ,889,890 ,1019 };
static const u16 GRASS_CENTER_MUD_L = 963;
static const u16 GRASS_CENTER_MUD_R = 965;
static const u16 GRASS_CENTER_MUD_T = 927;
static const u16 GRASS_CENTER_MUD_B = 1001;

static const u16 GRASS_CENTER_MUD_BR = 1002;
static const u16 GRASS_CENTER_MUD_BL = 1000;
static const u16 GRASS_CENTER_MUD_TL = 926;
static const u16 GRASS_CENTER_MUD_TR = 928;

const f32 MUD_CENTER_PROBABILITY[] = { 0.31,0.31,0.31,0.07 };
const f32 GRASS_CENTER_PROBABILITY[] = { 0.33,0.33,0.33,0.01 };


u32 GetMudCenterTile() {
	return MUD_CENTERS[SelectWeighted(MUD_CENTER_PROBABILITY, 4)];
}
u32 GetGrassCenterTile() {
	return GRASS_CENTERS[SelectWeighted(GRASS_CENTER_PROBABILITY, 4)];
}

#define MAX_HOUSE_WALLS_WIDTH 15
#define MAX_TOTAL_HOUSE_WIDTH MAX_HOUSE_WALLS_WIDTH
struct HouseWalls {
	glm::ivec2 topTiles[MAX_TOTAL_HOUSE_WIDTH];
	u32 numTopTiles = 0;
	void PushTopTile(const glm::ivec2& wall) {
		topTiles[numTopTiles++] = wall;
	}
};
struct House {
	HouseWalls walls;
};
House GenerateRandomHouseWalls(u16* data, u32 w, u32 h, u32 mapX, u32 mapY) {
	House r;
	const u32 wallsWidthMin = 3;
	const u32 wallsWidthMax = MAX_HOUSE_WALLS_WIDTH;
	const u32 wallsHeightMin = 3;
	const u32 wallsHeightMax = 15;


	u32 HouseWidth = GetRandomIntBetween(wallsWidthMin, wallsWidthMax);
	u32 HouseHeight = GetRandomIntBetween(wallsHeightMin, wallsHeightMax);
	u16* WritePtr = &data[mapY * w + mapX];
	*(WritePtr++) = HOUSE_BOTTOM_LEFT_EDGE;
	for (u32 i = 1; i < HouseWidth - 1; i++) {
		*(WritePtr++) = HOUSE_BOTTOM_MIDDLE;
	}
	*(WritePtr++) = HOUSE_BOTTOM_RIGHT_EDGE;
	for (u32 i = 0; i < HouseHeight - 1; i++) {
		WritePtr -= w;
		WritePtr -= HouseWidth;
		if (i == HouseHeight - 2) {
			u32 index = WritePtr - &data[0];
			r.walls.PushTopTile({
					index % w,
					index / w
				});
		}
		*(WritePtr++) = HOUSE_MIDDLE_LEFT_EDGE;

		for (u32 i = 1; i < HouseWidth - 1; i++) {
			if (i == HouseHeight - 2) {
				u32 index = WritePtr - &data[0];
				r.walls.PushTopTile({
						index % w,
						index / w
					});
			}
			*(WritePtr++) = HOUSE_MIDDLE;
		}
		if (i == HouseHeight - 2) {
			u32 index = WritePtr - &data[0];
			r.walls.PushTopTile({
					index % w,
					index / w,
				});
		}
		*(WritePtr++) = HOUSE_MIDDLE_RIGHT_EDGE;
	}
	return r;

}

void AddRoof(u16* data, u32 w, u32 h, House& house) {
	const u32 houseDepthMax = 10;
	const u32 houseDepthMin = 3;
	u32 HouseDepth = GetRandomIntBetween(houseDepthMin, houseDepthMax);
	auto& topTile = house.walls.topTiles[0];

	data[topTile.y * w + topTile.x] = ROOF_FRONT_LEFT_CORNER;
	for (u32 i = 1; i < HouseDepth - 1; i++) {
		data[((topTile.y - i) * w) + topTile.x] = ROOF_BACK_LEFT_EDGE;
	}
	data[((topTile.y - (HouseDepth - 1)) * w) + topTile.x] = ROOF_BACK_LEFT_CORNER;

	for (u32 i = 1; i < house.walls.numTopTiles - 1; i++) {
		topTile = house.walls.topTiles[i];
		data[(topTile.y * w) + topTile.x] = ROOF_FRONT_MIDDLE;
		for (u32 i = 1; i < HouseDepth - 1; i++) {
			data[((topTile.y - i) * w) + topTile.x] = ROOF_MIDDLE;
		}
		data[((topTile.y - HouseDepth - 1) * w) + topTile.x] = ROOF_BACK_MIDDLE;
	}

	topTile = house.walls.topTiles[house.walls.numTopTiles - 1];
	data[topTile.y * w + topTile.x] = ROOF_FRONT_RIGHT_CORNER;
	for (u32 i = 1; i < HouseDepth - 1; i++) {
		data[((topTile.y - i) * w) + topTile.x] = ROOF_BACK_RIGHT_EDGE;
	}
	data[((topTile.y - (HouseDepth - 1)) * w) + topTile.x] = ROOF_BACK_RIGHT_CORNER;

}
void ProceduralCppTiledWorldPopulater::PopulateLayer(u32 layerNumber, u16* layer, u32 layerWidthTiles, u32 layerHeightTiles)
{
	static std::vector<House> houses;
	switch (layerNumber) {
	case 0: {
		houses.clear();
		u32 layerSize = layerWidthTiles * layerHeightTiles;

		for (u32 i = 0; i < layerSize; i++) {
			//m_layersData[layer][i] = GetMudCenterTile();
			layer[i] = GetGrassCenterTile();

		}
	}
		  break;
	case 1: {
		houses.push_back(GenerateRandomHouseWalls(layer, layerWidthTiles, layerHeightTiles, 100, 100));
	}
		  break;
	case 2: {
		for (House& house : houses) {
			AddRoof(layer, layerWidthTiles, layerHeightTiles, house);
		}
	}
		  break;
	}
}
