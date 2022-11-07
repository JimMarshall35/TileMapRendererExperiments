#include "MetaAtlas.h"
#include <glad/glad.h>
#include <iostream>
#include "flecs.h"
#include "MetaSpriteComponent.h"

MetaAtlas::MetaAtlas(u32 width, u32 height)
	:m_atlasData(std::make_unique<u16[]>(width * height)),
	m_atlasHeight(height),
	m_atlasWidth(width),
	m_atlasSize(width * height)
{
	memset(m_atlasData.get(), 0, m_atlasSize * sizeof(u16));
	CreateGlTextureFromAtlas();
}


MetaSpriteHandle MetaAtlas::LoadMetaSprite(const MetaSpriteDescription& description)
{
	u32 rVal = m_numLoadedMetasprites;
	if (rVal + 1 == MAX_NUM_METASPRITES) {
		std::cerr << "MAX_NUM_METASPRITES exceeded consider increasing\n";
		return -1;
	}
	if (description.spriteTilesWidth > m_atlasWidth || description.spriteTilesHeight > m_atlasWidth) {
		ExpandAtlas(description.spriteTilesWidth, description.spriteTilesHeight);
	}

	auto& descriptionDest = m_loadedMetaspriteDescriptions[m_numLoadedMetasprites];
	descriptionDest.name = description.name;
	descriptionDest.numTiles = description.numTiles;
	descriptionDest.spriteTilesHeight = description.spriteTilesHeight;
	descriptionDest.spriteTilesWidth = description.spriteTilesWidth;
	memcpy(descriptionDest.tiles, description.tiles, sizeof(u16) * description.numTiles);
	// do the actual loading here, texturesubimage2d into the texture, saving offset to m_loadedMetaSpriteOffsets[m_numLoadedMetasprites]

	if (m_currentY + description.spriteTilesHeight > m_nextY) {
		m_nextY = m_currentY + description.spriteTilesHeight;
	}

	if (m_nextX + description.spriteTilesWidth >= m_atlasWidth) {
		m_nextX = 0;
		m_currentY = m_nextY;
		if (m_nextY + description.spriteTilesHeight >= m_atlasHeight) {
			ExpandAtlas();
			// TODO: need to reset the x 
		}
	}

	auto writeX = m_nextX;
	auto writeY = m_currentY;

	m_loadedMetaSpriteOffsets[rVal] = glm::ivec2(writeX, writeY);
	
	int onTile = 0;
	for (int i = writeY; i<writeY+descriptionDest.spriteTilesHeight; i++) {
		for (int j = writeX; j < writeX + descriptionDest.spriteTilesWidth; j++) {
			glTextureSubImage2D(m_atlasTextureHandle, 0, j, i, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_SHORT, &description.tiles[onTile++]);
		}
	}

	m_nextX = m_nextX + description.spriteTilesWidth;

	
	// finally increment m_numLoadedMetasprites
	++m_numLoadedMetasprites;

	return rVal;
}

void MetaAtlas::AddMetaSpriteComponentToEntity(flecs::entity& entity, MetaSpriteHandle handle)
{
	assert(handle >= 0);
	assert(handle < MAX_NUM_METASPRITES);
	assert(handle < m_numLoadedMetasprites);
	entity.add<MetaSpriteComponent>();
	//entity.set<MetaSpriteComponent>({ handle, &m_loadedMetaspriteDescriptions[handle] });
}

void MetaAtlas::GetSprites(const MetaSpriteDescription** sprites, u32* numsprites)
{
	*sprites = m_loadedMetaspriteDescriptions;
	*numsprites = m_numLoadedMetasprites;
}

u32 MetaAtlas::GetAtlasTextureHandle() const
{
	return m_atlasTextureHandle;
}

const glm::ivec2* MetaAtlas::GetOffset(MetaSpriteHandle handle) const
{
	if (handle < 0 || handle >= m_numLoadedMetasprites) {
		std::cerr << "invalid handle " << handle << "\n";
		return nullptr;
	}
	return &m_loadedMetaSpriteOffsets[handle];
}

const MetaSpriteDescription* MetaAtlas::getDescription(MetaSpriteHandle handle) const
{
	if (handle < 0 || handle >= m_numLoadedMetasprites) {
		std::cerr << "invalid handle " << handle << "\n";
		return nullptr;
	}
	return &m_loadedMetaspriteDescriptions[handle];
}


void MetaAtlas::ExpandAtlas(u32 tooBigSpriteSizeW, u32 tooBigSpriteSizeH)
{
	if (tooBigSpriteSizeW || tooBigSpriteSizeH) {
		// a sprite is being added which is too big for the atlas, double atlas width until sprite can fit
		do {
			DoubleAtlasDims();
		} while (tooBigSpriteSizeH > m_atlasHeight / 2 || tooBigSpriteSizeW > m_atlasWidth / 2);
	}
	else {
		// one too many sprites have been added, double atlas height and width
		DoubleAtlasDims();
	}
}

void MetaAtlas::DoubleAtlasDims()
{
	auto copy = std::make_unique<u16[]>(m_atlasSize);
	memcpy(copy.get(), m_atlasData.get(), m_atlasSize * sizeof(u16));
	m_atlasData.reset();
	auto newAtlasHeight = m_atlasHeight * 2;
	auto newAtlasWidth = m_atlasWidth * 2;

	m_atlasData = std::make_unique<u16[]>(newAtlasHeight * newAtlasWidth);

	for (u32 i = 0; i < m_atlasHeight; i++) {
		for (u32 j = 0; j < m_atlasWidth; j++) {
			u32 sourceVal = copy[(i * m_atlasWidth) + j];
			m_atlasData[(i * newAtlasWidth) + j] = sourceVal;
		}
	}

	m_atlasHeight = newAtlasHeight;
	m_atlasWidth = newAtlasWidth;
	CreateGlTextureFromAtlas();
}

void MetaAtlas::CreateGlTextureFromAtlas()
{
	if (m_atlasTextureHandle != 0) {
		glDeleteTextures(1, &m_atlasTextureHandle);
	}
	glGenTextures(1, &m_atlasTextureHandle);
	glBindTexture(GL_TEXTURE_2D, m_atlasTextureHandle);

	// must set these two or it won't work
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, m_atlasWidth, m_atlasHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, m_atlasData.get());
}
