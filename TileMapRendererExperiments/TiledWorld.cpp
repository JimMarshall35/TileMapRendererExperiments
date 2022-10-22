#include "TiledWorld.h"
#include <glad/glad.h>

inline static u16 GetRandomIntBetween(u16 min, u16 max) {
	return (u16)rand() % (max - min + 1) + min;
}

inline static f32 GetRandomFloatZeroToOne() {
	return rand() / (RAND_MAX + 1.);
}


static void GetRandomTileMap(u16* map, u32 size) {
	for (int i = 0; i < size; i++) {
		u16 r = GetRandomIntBetween(0, (28 * 37));
		map[i] = r;
	}
}

TiledWorld::TiledWorld(u32 sizeW, u32 sizeH, u32 numLayers)
	:m_mapHeightTiles(sizeH),
	m_mapWidthTiles(sizeW),
	m_numLayers(numLayers)
{
	using namespace std;
	const u32 mapsize = sizeW * sizeH;
	glGenTextures(m_numLayers, m_layerTextureHandles);

	for (int i = 0; i < numLayers; i++) {
		m_layersData[i] = make_unique<u16[]>(mapsize);
		//GetRandomTileMap(m_layersData[i].get(), mapsize);
		memset(m_layersData[i].get(), 0, mapsize);
		m_layersVisible[i] = true;
		ProcedurallyGenerate(i);

		glBindTexture(GL_TEXTURE_2D, m_layerTextureHandles[i]);

		// must set these two or it won't work
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, sizeW, sizeH, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, m_layersData[i].get());
	}
}

void TiledWorld::IterateTileLayers(IterateTileLayersFunc iterationFunc) const
{
	for (int i = 0; i < m_numLayers; i++) {
		iterationFunc(m_layerTextureHandles[i], m_layersData[i].get(), m_layersVisible[i]);
	}
}

void TiledWorld::SetTile(u32 x, u32 y, u32 z, u32 newTileIndex)
{
	auto flatIndex = (y * m_mapWidthTiles) + x;
	if (flatIndex < 0 || flatIndex > m_mapHeightTiles * m_mapWidthTiles) {
		return;
	}
	m_layersData[z].get()[flatIndex] = newTileIndex;
	glTextureSubImage2D(m_layerTextureHandles[z], 0, x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_SHORT, &newTileIndex);
}

u32 TiledWorld::GetTile(u32 x, u32 y, u32 z)
{

	auto flatIndex = (y * m_mapWidthTiles) + x;
	if (flatIndex >= m_mapHeightTiles * m_mapWidthTiles) {
		return 0;
	}
	return m_layersData[z].get()[flatIndex];
}

u32 SelectWeighted(const f32* weights, u32 numWeights) {
	f32 randOneToZero = GetRandomFloatZeroToOne();
	f32 accumulatedWeight = 0.0f;
	for (int i = 0; i < numWeights; i++) {
		accumulatedWeight += weights[i];
		if (accumulatedWeight >= randOneToZero) {
			return i;
		}
	}
	return 0;
}
void TiledWorld::ProcedurallyGenerate(u32 layer)
{
	const u16 MUD_CENTERS[] = {893,894,974,1009,};
	const u16 GRASS_CENTERS[] = { 1019,889,890 };
	const u16 GRASS_CENTER_MUD_L = 963;
	const u16 GRASS_CENTER_MUD_R = 965;
	const u16 GRASS_CENTER_MUD_T = 927;
	const u16 GRASS_CENTER_MUD_B = 1001;

	const u16 GRASS_CENTER_MUD_BR = 1002;
	const u16 GRASS_CENTER_MUD_BL = 1000;
	const u16 GRASS_CENTER_MUD_TL = 926;
	const u16 GRASS_CENTER_MUD_TR = 928;

	const f32 MUD_CENTER_PROBABILITY[] = {0.3,0.3,0.3,0.1};

	if (layer != 0) {
		return;
	}

	u32 layerSize = m_mapWidthTiles * m_mapHeightTiles;

	for (u32 i = 0; i < layerSize; i++) {
		m_layersData[layer][i] = MUD_CENTERS[SelectWeighted(MUD_CENTER_PROBABILITY, 4)];
	}

}
