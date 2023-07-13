#include "NewRenderer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "Camera2D.h"
#include "TiledWorld.h"
#include "MetaAtlas.h"

#define TILE_NUM_INDICES 6
#define SCR_WIDTH 800
#define SCR_HEIGHT 1200

NewRenderer* NewRenderer::s_instance;

NewRendererInitialisationInfo::NewRendererInitialisationInfo(u32 width, u32 height)
	:windowHeight(height), windowWidth(width)
{

}

std::unique_ptr<NewRendererInitialisationInfo> NewRendererInitialisationInfo::NewRendererInitialisationInfoFactory()
{
	DI_FactoryImpl(NewRendererInitialisationInfo, SCR_WIDTH, SCR_HEIGHT)
}

NewRenderer::NewRenderer(const NewRendererInitialisationInfo& info)
	:m_tileShader("shaders\\TilemapVert2.glsl", "shaders\\TilemapFrag2.glsl"),
	m_singleLineShader("shaders\\SingleLineVert.glsl", "shaders\\SingleLineFrag.glsl"),
	m_windowWidth(info.windowWidth),
	m_windowHeight(info.windowHeight)
{
	glGenVertexArrays(1, &m_vao);
	InitSingleLineDrawing();
	s_instance = this;
}

NewRenderer::NewRenderer(NewRendererInitialisationInfo* info)
	:NewRenderer(*info)
{
	
}

DI_FactoryDecl(NewRenderer)(NewRendererInitialisationInfo* info)
{
	DI_FactoryImpl(NewRenderer, info)
}


void NewRenderer::DrawChunk(
	const glm::ivec2& chunkWorldMapOffset,
	const glm::vec2& pos,
	const glm::vec2& scale,
	float rotation,
	const TiledWorld& world,
	ArrayTexture2DHandle texArray,
	const Camera2D& cam,
	const glm::ivec2& chunkSize,
	float opacity
) const
{
	m_tileShader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	m_tileShader.setMat4("vpMatrix", cam.GetProjectionMatrix(m_windowWidth, m_windowHeight));
	m_tileShader.setMat4("modelMatrix", model);
	m_tileShader.SetIVec2("chunkOffset", chunkWorldMapOffset);
	m_tileShader.SetIVec2("chunkSize", chunkSize);
	m_tileShader.setInt("masterTileTexture", 0);
	m_tileShader.setInt("atlasSampler", 1);
	m_tileShader.setFloat("opacity", opacity);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);
	glBindVertexArray(m_vao);

	world.IterateTileLayers([this, chunkSize](const TextureHandle& texture, u16* data, bool visible) {
		if (!visible) {
			return;
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glDrawArrays(GL_TRIANGLES, 0, chunkSize.x * chunkSize.y * TILE_NUM_INDICES);
	});

}

void NewRenderer::DrawMetaSprite(u32 metaSpriteHandle, const glm::vec2& pos, const glm::vec2& scale, float rotation, const MetaAtlas& atlas, ArrayTexture2DHandle texArray, const Camera2D& cam, float opacity) const
{
	m_tileShader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	m_tileShader.setMat4("vpMatrix", cam.GetProjectionMatrix(m_windowWidth, m_windowHeight));
	m_tileShader.setMat4("modelMatrix", model);
	m_tileShader.SetIVec2("chunkOffset", *atlas.GetOffset(metaSpriteHandle));
	const auto& description = atlas.getDescription(metaSpriteHandle);
	m_tileShader.SetIVec2("chunkSize", glm::ivec2{description->spriteTilesWidth, description->spriteTilesHeight});
	m_tileShader.setInt("masterTileTexture", 0);
	m_tileShader.setInt("atlasSampler", 1);
	m_tileShader.setFloat("opacity", opacity);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);
	glBindVertexArray(m_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlas.GetAtlasTextureHandle());

	glDrawArrays(GL_TRIANGLES, 0, description->spriteTilesWidth * description->spriteTilesHeight * TILE_NUM_INDICES);

}

void NewRenderer::InitSingleLineDrawing()
{
	const unsigned int lineVerts[2] = {
		0,1
	};
	// line 
	glGenVertexArrays(1, &m_singleLineVAO);
	glGenBuffers(1, &m_singleLineVBO);
	glBindVertexArray(m_singleLineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_singleLineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVerts), lineVerts, GL_STATIC_DRAW);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(unsigned int), NULL);
	glEnableVertexAttribArray(0);
}

void NewRenderer::DrawLine(const glm::vec2& point1, const glm::vec2& point2, const glm::vec4& colour, float width, const Camera2D& cam) const
{
	m_singleLineShader.use();
	m_singleLineShader.setMat4("projection", cam.GetProjectionMatrix(m_windowWidth, m_windowHeight));
	m_singleLineShader.setVec4("Colour", colour);
	m_singleLineShader.setVec2("point1world", point1);
	m_singleLineShader.setVec2("point2world", point2);
	glLineWidth(width);
	glBindVertexArray(m_singleLineVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glLineWidth(1);
}


void NewRenderer::SetWindowWidthAndHeight(u32 width, u32 height)
{
	m_windowWidth = width;
	m_windowHeight = height;
}
