#pragma once
#include "BasicTypedefs.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>
#include "IRenderer.h"

#define MAX_NUM_Z_LAYERS 128

using ArrayTexture2DHandle = u32;
class Camera2D;
class TiledWorld;
class MetaAtlas;


struct NewRendererInitialisationInfo {
	u32 windowWidth;
	u32 windowHeight;
};
class NewRenderer : public IRenderer
{
public:
	NewRenderer(const NewRendererInitialisationInfo& info);
	void DrawChunk(const glm::ivec2& chunkWorldMapOffset, const glm::vec2& pos, const glm::vec2& scale, float rotation, const TiledWorld& world, ArrayTexture2DHandle texArray, const Camera2D& camArray, const glm::ivec2& chunkSize, float opacity=1.0f) const;
	void DrawMetaSprite(u32 metaSpriteHandle, const glm::vec2& pos, const glm::vec2& scale, float rotation, const MetaAtlas& atlas, ArrayTexture2DHandle texArray, const Camera2D& cam, float opacity = 1.0f) const;
private:
	Shader m_tileShader;
	u32 m_windowWidth, m_windowHeight;
	u32 m_vao;

	// Inherited via IRenderer
	virtual void SetWindowWidthAndHeight(u32 width, u32 height) override;
};

