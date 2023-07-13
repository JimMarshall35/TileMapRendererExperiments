#include "MetaAtlas.h"
#include <glad/glad.h>
#include <iostream>
#include <fstream>      // std::ifstream
#include "MetaSpriteComponent.h"
#include "jsonhelper.h"

MetaAtlas::MetaAtlas(u32 width, u32 height, ECS* ecs)
	:m_atlasData(std::make_unique<u16[]>(width * height)),
	m_atlasHeight(height),
	m_atlasWidth(width),
	m_atlasSize(width * height),
	m_ecs(ecs)
{
	memset(m_atlasData.get(), 0, m_atlasSize * sizeof(u16));
	CreateGlTextureFromAtlas();
	LoadFromFile("data\\metasprites.atlas");
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
	//descriptionDest.numTiles = description.numTiles;
	descriptionDest.tiles.resize(description.tiles.size());
	descriptionDest.spriteTilesHeight = description.spriteTilesHeight;
	descriptionDest.spriteTilesWidth = description.spriteTilesWidth;
	memcpy(descriptionDest.tiles.data(), description.tiles.data(), sizeof(u16) * description.tiles.size());
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
			glTextureSubImage2D(m_atlasTextureHandle, 0, j, i, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_SHORT, &description.tiles[onTile]);
			m_atlasData[(m_atlasWidth * i) + j] = description.tiles[onTile++];
		}
	}

	m_nextX = m_nextX + description.spriteTilesWidth;

	
	// finally increment m_numLoadedMetasprites
	++m_numLoadedMetasprites;

	return rVal;
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

const flecs::entity MetaAtlas::GetECSEntity(MetaSpriteHandle handle) const
{
	return m_loadedMetaSpriteDescriptionEntities[handle];
}

const MetaSpriteDescription* MetaAtlas::getDescription(MetaSpriteHandle handle) const
{
	if (handle < 0 || handle >= m_numLoadedMetasprites) {
		std::cerr << "invalid handle " << handle << "\n";
		return nullptr;
	}
	return &m_loadedMetaspriteDescriptions[handle];
}

u32 GetSizeNeededForDescription(const MetaSpriteDescription& m) {
	u32 totalSize = 0;
	size_t numTiles = m.tiles.size();
	totalSize += sizeof(numTiles);
	totalSize += sizeof(m.spriteTilesHeight);
	totalSize += sizeof(m.spriteTilesWidth);
	totalSize += sizeof(u16) * numTiles; // tiles
	totalSize += m.name.length() + 1;// + 1 for null character
	return totalSize;
}

u32 MetaAtlas::GetBufferSizeRequiredToSave() const
{
	u32 bufferSize = 0;
	bufferSize += sizeof(m_atlasWidth);
	bufferSize += sizeof(m_atlasHeight);
	bufferSize += m_atlasSize * sizeof(u16);
	for (int i = 0; i < m_numLoadedMetasprites; i++) {
		bufferSize += GetSizeNeededForDescription(m_loadedMetaspriteDescriptions[i]);
		bufferSize += sizeof(glm::ivec2);
	}
	bufferSize += sizeof(m_numLoadedMetasprites);
	bufferSize += sizeof(m_nextX);
	bufferSize += sizeof(m_nextY);
	bufferSize += sizeof(m_currentY);
	return bufferSize;
}



void WriteU32(u8*& writePtr, u32 val) {
	*(u32*)writePtr = val;
	writePtr += sizeof(u32);
}

void WriteU16Array(u8*& writePtr, const u16* val, u32 numU16s) {
	memcpy(writePtr, val, numU16s * sizeof(u16));
	writePtr += numU16s * sizeof(u16);
}

void WriteString(u8*& writePtr, std::string string) {
	for (int i = 0; i < string.length(); i++) {
		*(writePtr++) = string[i];
	}
	*(writePtr++) = '\0';
}

void WriteF32(u8*& writePtr, f32 val) {
	*(f32*)writePtr = val;
	writePtr += sizeof(f32);
}

void WriteIVec2(u8*& writePtr, const glm::ivec2& val) {
	WriteU32(writePtr, val.x);
	WriteU32(writePtr, val.y);
}



void ReadU32(u32& in, std::ifstream& fs) {
	fs.read((char*)&in, sizeof(u32));
}

void ReadI32(i32& in, std::ifstream& fs) {
	fs.read((char*)&in, sizeof(i32));
}

void ReadU16Array(u16* out, u32 arraySize, std::ifstream& fs) {
	fs.read((char*)out, arraySize * sizeof(u16));
}

void ReadF32(f32& in, std::ifstream& fs) {
	fs.read((char*)&in, sizeof(f32));
}

void ReadIVec2(glm::ivec2& in, std::ifstream& fs) {
	ReadI32((i32&)in.x, fs);
	ReadI32((i32&)in.y, fs);
}

void ReadString(std::string& in, std::ifstream& fs) {
	char buffer[300];
	u32 writeIndex = 0;
	auto pos = fs.tellg();
	do {
		fs.read(&buffer[writeIndex++], sizeof(char));
	} while (buffer[writeIndex - 1] != '\0');

	in = std::string(buffer);
}

void MetaAtlas::SaveToFile(std::string path) const
{
	u32 size = GetBufferSizeRequiredToSave();
	auto buffer = std::make_unique<u8[]>(size);
	u8* writePtr = buffer.get();

	WriteU32(writePtr, m_atlasWidth);
	WriteU32(writePtr, m_atlasHeight);
	WriteU16Array(writePtr, m_atlasData.get(), m_atlasSize);
	WriteU32(writePtr, m_numLoadedMetasprites);
	for (int i = 0; i < m_numLoadedMetasprites; i++) {
		const auto& d = m_loadedMetaspriteDescriptions[i];
		const auto& o = m_loadedMetaSpriteOffsets[i];
		WriteIVec2(writePtr, o);
		WriteString(writePtr, d.name);
		WriteU32(writePtr, d.spriteTilesWidth);
		WriteU32(writePtr, d.spriteTilesHeight);
		WriteU32(writePtr, d.tiles.size());
		WriteU16Array(writePtr, d.tiles.data(), d.tiles.size());
	}
	WriteU32(writePtr, m_nextX);
	WriteU32(writePtr, m_nextY);
	WriteU32(writePtr, m_currentY);

	std::ofstream fs(path, std::ios::binary);
	fs.write((const char*)buffer.get(), size);
}

void MetaAtlas::LoadFromFile(std::string path)
{
	auto fs = std::ifstream(path, std::ios::binary);
	ReadU32(m_atlasWidth, fs);
	ReadU32(m_atlasHeight, fs);
	if (m_atlasData.get()) {
		m_atlasData.reset();
	}
	m_atlasSize = m_atlasWidth * m_atlasHeight;
	m_atlasData = std::make_unique<u16[]>(m_atlasSize);
	ReadU16Array(m_atlasData.get(), m_atlasSize, fs);

	ReadU32(m_numLoadedMetasprites, fs);
	auto pos = fs.tellg();
	for (int i = 0; i < m_numLoadedMetasprites; i++) {
		ReadIVec2(m_loadedMetaSpriteOffsets[i], fs);
		auto& d = m_loadedMetaspriteDescriptions[i];
		ReadString(d.name, fs);
		ReadU32(d.spriteTilesWidth, fs);
		ReadU32(d.spriteTilesHeight, fs);
		u32 size = 0;
		ReadU32(size, fs);
		d.tiles.resize(size);
		ReadU16Array(d.tiles.data(), d.tiles.size(), fs);
	}
	ReadU32(m_nextX, fs);
	ReadU32(m_nextY, fs);
	ReadU32(m_currentY, fs);
	CreateGlTextureFromAtlas();
	SaveAtlasAsECSEntities();
}

bool MetaAtlas::LoadFromJSON(const std::string& folder, const std::string& file, std::string& outErrorMsg)
{
	using namespace rapidjson;
	Document doc;
	if (!loadJSONFile(doc, (folder + "/" + file).c_str())) {
		outErrorMsg = "problem loading " + (folder + "\\" + file) + "\n";
		return false;
	}
	if(!checkJSONValue("metasprites", JSONTYPE::ARRAY, doc)){
		outErrorMsg = "metasprites field doesn't exist or isn't an array\n";
		return false;
	}
	const Value& metaSpritesArray = doc["metasprites"];
	for (int i = 0; i < metaSpritesArray.Size(); i++)
	{
		const Value& metaSprite = metaSpritesArray[i];
		std::string name = metaSprite["name"].GetString();
		u32 width = metaSprite["width"].GetUint();
		const Value& data = metaSprite["tiles"];
		u32 numTiles = data.Size();
		MetaSpriteDescription desc;
		desc.name = name;
		desc.spriteTilesWidth = width;
		desc.spriteTilesHeight = numTiles / width;
		desc.tiles.resize(numTiles);
		for (int j = 0; j < numTiles; j++) {
			desc.tiles[j] = (u16)data[j].GetUint();
		}
		MetaSpriteHandle hndl = LoadMetaSprite(desc);
	}
	SaveAtlasAsECSEntities();
}

void MetaAtlas::SaveAtlasAsECSEntities()
{
	flecs::world* world = m_ecs->GetWorld();

	world->each([world, this](flecs::entity_t e, MetaSpriteDescription& desc) {
		ecs_delete(world->c_ptr(), e);
	});
	for (u32 i = 0; i < m_numLoadedMetasprites; i++) {
		m_loadedMetaSpriteDescriptionEntities[i] = world->entity()
			.set([this, i](MetaSpriteDescription& desc) {
				desc = m_loadedMetaspriteDescriptions[i];
			});
	}
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

