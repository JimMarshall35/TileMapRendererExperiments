#pragma once
#include "BasicTypedefs.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>

#define MAX_NUM_Z_LAYERS 128

using ArrayTexture2DHandle = u32;
class Camera2D;


struct NewRendererInitialisationInfo {
	u32 tilemapSizeX;
	u32 tilemapSizeY;
	u32 chunkSizeX;
	u32 chunkSizeY;
	u32 windowWidth;
	u32 windowHeight;
	u32 numLayers;
};
class NewRenderer
{
public:
	NewRenderer(const NewRendererInitialisationInfo& info);
	void DrawChunk(const glm::ivec2& chunkWorldMapOffset, const glm::vec2& pos, const glm::vec2& scale, float rotation, ArrayTexture2DHandle texArray, const Camera2D& camArray) const;
private:
	std::unique_ptr<u16[]> m_worldTexturesBytes[MAX_NUM_Z_LAYERS];
	u32 m_worldTexturesHandles[MAX_NUM_Z_LAYERS];
	Shader m_tileShader;
	glm::ivec2 m_tilemapChunkSize;
	glm::ivec2 m_worldMapSize;
	u32 m_windowWidth, m_windowHeight;
	u32 m_vao;
	u32 m_numLayers;
};

