#include "TiledWorld.h"
#include <glad/glad.h>

inline static u16 GetRandomIntBetween(u16 min, u16 max) {
	return (u16)rand() % (max - min + 1) + min;
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
		GetRandomTileMap(m_layersData[i].get(), mapsize);
		//memset(m_layersData[i].get(), 0, mapsize);

		m_layersDrawEnabled[i] = true;

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
		iterationFunc(m_layerTextureHandles[i], m_layersData[i].get());
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
