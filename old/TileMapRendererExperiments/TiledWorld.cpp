#include "TiledWorld.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "ITiledWorldPopulater.h"


TiledWorld::TiledWorld(ITiledWorldPopulater* populater)
	:m_mapHeightTiles(populater->GetHeightTiles()),
	m_mapWidthTiles(populater->GetWidthTiles()),
	m_numLayers(populater->GetRequiredNumLayers()),
	m_populater(populater)
{
	using namespace std;
	const u32 mapsize = m_mapWidthTiles * m_mapHeightTiles;
	
	for (int i = 0; i < m_numLayers; i++) {
		TileLayer& tileLayer = m_tileLayers[i];
		glGenTextures(1, &tileLayer.Handle);

		tileLayer.Data = make_unique<u16[]>(mapsize);
		//GetRandomTileMap(m_layersData[i].get(), mapsize);
		memset(tileLayer.Data.get(), 0, mapsize);
		tileLayer.Visible = true;
		//ProcedurallyGenerate(i);
		m_populater->PopulateLayer(i, tileLayer.Data.get(), m_mapWidthTiles, m_mapHeightTiles, tileLayer.Name);
		glBindTexture(GL_TEXTURE_2D, tileLayer.Handle);

		// must set these two or it won't work
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, m_mapWidthTiles, m_mapHeightTiles, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, tileLayer.Data.get());
	}
}

void TiledWorld::IterateTileLayers(IterateTileLayersFunc iterationFunc) const
{
	for (int i = 0; i < m_numLayers; i++) {
		const TileLayer& tileLayer = m_tileLayers[i];
		iterationFunc(tileLayer.Handle, tileLayer.Data.get(), tileLayer.Visible);
	}
}

void TiledWorld::SetTile(u32 x, u32 y, u32 z, u32 newTileIndex)
{
	auto flatIndex = (y * m_mapWidthTiles) + x;
	if (flatIndex < 0 || flatIndex > m_mapHeightTiles * m_mapWidthTiles) {
		return;
	}
	TileLayer& tileLayer = m_tileLayers[z];
	tileLayer.Data.get()[flatIndex] = newTileIndex;
	glTextureSubImage2D(tileLayer.Handle, 0, x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_SHORT, &newTileIndex);
}

u32 TiledWorld::GetTile(u32 x, u32 y, u32 z)
{

	auto flatIndex = (y * m_mapWidthTiles) + x;
	if (flatIndex >= m_mapHeightTiles * m_mapWidthTiles) {
		return 0;
	}
	return m_tileLayers[z].Data.get()[flatIndex];
}

