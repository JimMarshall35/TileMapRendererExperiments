#include "GuiSpriteLoader.h"
#include "GLTextureHelper.h"
#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"


#define NUM_CHANNELS 4

GuiSpriteLoader::GuiSpriteLoader() 
{

}

bool GuiSpriteLoader::LoadWholeImageFileSprite(const std::string& folder, const std::string& file, const std::string& name, i32& outHandle)
{
	std::string path = folder + "/" + file;
	int img_w, img_h;
	int n;
	u8* data = stbi_load(path.c_str(), &img_w, &img_h, &n, NUM_CHANNELS);
	bool rval = LoadSubImageSprite(
		folder, file, name,
		glm::vec2{0,0}, glm::vec2{img_w, img_h},
		outHandle,
		data, img_w, img_h
	);
	stbi_image_free(data);
	return rval;

}

u32 GuiSpriteLoader::GetGuiSpriteTextureHandle(i32  handle) const
{
	if (handle < m_sprites.vectorsSize && handle >= 0){
		if (m_sprites.isLoaded[handle]){
			return m_sprites.textureHandles[handle];
		}
		else{
			std::cerr << "[GuiSpriteLoader::GetGuiSprite] sprite at index " << handle << " not loaded\n";
		}
	}
	else{
		std::cerr << "[GuiSpriteLoader::GetGuiSprite] handle " << handle << " is not a valid index into m_sprites, which has size " << m_sprites.vectorsSize << "\n";
	}
	
}

i32 GuiSpriteLoader::GetHandleByName(const std::string& name) const
{
	for (int i = 0; i < m_sprites.vectorsSize; i++) {
		if (m_sprites.isLoaded[i] && m_sprites.names[i] == name) {
			return i;
		}
	}
	return -1;
}

void GuiSpriteLoader::FreeTexture(i32 handle)
{
	if (handle < m_sprites.vectorsSize && handle >= 0) {
		if (m_sprites.isLoaded[handle]) {
			glDeleteTextures(1, &m_sprites.textureHandles[handle]);
			m_deadHandles.push_back(handle);
			m_sprites.pixelData[handle].reset(nullptr);
			m_sprites.isLoaded[handle] = false;
		}
		else {
			std::cerr << "[GuiSpriteLoader::FreeTexture] handle " << handle << " is not loaded - it's already been freed\n";
		}
	}
	else {
		std::cerr << "[GuiSpriteLoader::FreeTexture] handle " << handle << " is not a valid index into m_sprites, which has size " << m_sprites.vectorsSize << "\n";
	}
}

bool GuiSpriteLoader::LoadSubImageSprite(const std::string& folder, const std::string& file, const std::string& name, const glm::vec2& topLeftPx, const glm::vec2& bottomRightPx, i32& outHandle, u8* dataIn, u32 widthPx, u32 heightPx)
{
	std::string path = folder + "/" + file;
	int img_w, img_h;
	int n;
	u8* data;

	if (!dataIn) {
		data = stbi_load(path.c_str(), &img_w, &img_h, &n, NUM_CHANNELS);
		// Early return if the file can't be opened
		if (data == NULL) {
			std::cerr << "can't open file " << path << std::endl;
			return false;
		}
	}
	else {
		data = dataIn;
		img_w = widthPx;
		img_h = heightPx;
	}
	
	u32 handle;
	if (m_deadHandles.size() > 0) {
		handle = m_deadHandles.back();
		m_deadHandles.pop_back();
		
	}
	else {
		m_sprites.textureHandles.push_back(0);
		m_sprites.sizesInPixels.push_back(glm::vec2{ 0,0 });
		m_sprites.isLoaded.push_back(true);
		m_sprites.names.push_back("");
		m_sprites.pixelData.push_back(std::unique_ptr<u8[]>());
		++m_sprites.vectorsSize;
		handle = m_sprites.vectorsSize - 1;
	}

	outHandle = handle;

	m_sprites.names[handle] = name;
	glm::vec2 size = glm::vec2{ bottomRightPx.x - topLeftPx.x, bottomRightPx.y - topLeftPx.y };
	m_sprites.sizesInPixels[handle] = size;

	
	m_sprites.pixelData[handle].reset(nullptr);
	
	u32 sizeBytes = img_w * img_h * NUM_CHANNELS;
	m_sprites.pixelData[handle] = std::make_unique<u8[]>(sizeBytes);

	u32 sourceRowLength = img_w * NUM_CHANNELS;
	size_t startOffset = ((topLeftPx.y * sourceRowLength) + (topLeftPx.x * NUM_CHANNELS));
	u8* startReadPtr = data + startOffset;

	u32 destRowLength = size.x * NUM_CHANNELS;
	u8* writePtr = m_sprites.pixelData[handle].get();
	for (int i = 0; i < size.y; i++) {
		memcpy(writePtr, startReadPtr, destRowLength);
		startReadPtr += sourceRowLength;
		writePtr += destRowLength;
	}


	u32* textureHandlePtr = &(m_sprites.textureHandles[handle]);
	glGenTextures(1, textureHandlePtr);

	glBindTexture(GL_TEXTURE_2D, *textureHandlePtr);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // GL_NEAREST is the better filtering option for this game
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR); // as gives better more "pixelated" (less "smoothed out") textures
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_sprites.pixelData[handle].get());
	glGenerateMipmap(GL_TEXTURE_2D);

	m_sprites.isLoaded[handle] = true;
	if (!dataIn) {
		stbi_image_free(data);
	}
	
	
	return true;
}