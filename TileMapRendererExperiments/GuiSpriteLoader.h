#pragma once
#include <memory>
#include <string>
#include <vector>
#include "BasicTypedefs.h"
#include <glm/glm.hpp>

struct GuiSprites {
	// struct of arrays for better cache usage
	std::vector<u32> textureHandles;
	std::vector<glm::vec2> sizesInPixels;
	std::vector<bool> isLoaded;
	std::vector<std::string> names;
	std::vector<std::unique_ptr<u8[]>> pixelData;
	u32 vectorsSize = 0;
};



/*
	load free form sprites, one opengl texture per sprite
*/
class GuiSpriteLoader{
public:
	GuiSpriteLoader();
	bool LoadWholeImageFileSprite(const std::string& folder, const std::string& file, const std::string& name, i32& outHandle);
	bool LoadSubImageSprite(const std::string& folder, const std::string& file, const std::string& name, const glm::vec2& topLeftPx, const glm::vec2& bottomRightPx, i32& outHandle, u8* data = nullptr, u32 widthPx = 0, u32 heightPx = 0);
	u32 GetGuiSpriteTextureHandle(i32  handle) const;
	i32 GetHandleByName(const std::string& name) const;
	void FreeTexture(i32 handle);
private:
	std::vector<i32> m_deadHandles;
	GuiSprites m_sprites;
};