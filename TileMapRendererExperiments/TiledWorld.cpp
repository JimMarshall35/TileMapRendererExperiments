#include "TiledWorld.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "ITiledWorldPopulater.h"


TiledWorld::TiledWorld(u32 sizeW, u32 sizeH, u32 numLayers, ITiledWorldPopulater* populater)
	:m_mapHeightTiles(sizeH),
	m_mapWidthTiles(sizeW),
	m_numLayers(numLayers),
	m_populater(populater)
{
	using namespace std;
	const u32 mapsize = sizeW * sizeH;
	glGenTextures(m_numLayers, m_layerTextureHandles);

	for (int i = 0; i < numLayers; i++) {
		m_layersData[i] = make_unique<u16[]>(mapsize);
		//GetRandomTileMap(m_layersData[i].get(), mapsize);
		memset(m_layersData[i].get(), 0, mapsize);
		m_layersVisible[i] = true;
		//ProcedurallyGenerate(i);
		m_populater->PopulateLayer(i, m_layersData[i].get(), m_mapWidthTiles, m_mapHeightTiles);
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

