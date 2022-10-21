#include "NewRenderer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "Camera2D.h"

#define TILE_NUM_INDICES 6

inline static u16 GetRandomIntBetween(u16 min, u16 max) {
	return (u16)rand() % (max - min + 1) + min;
}


static void GetRandomTileMap(u16* map, u32 size) {
	for (int i = 0; i < size; i++) {
		u16 r = GetRandomIntBetween(0, (28*37));
		map[i] = r;
	}
}
NewRenderer::NewRenderer(const NewRendererInitialisationInfo& info)
	:m_tileShader("shaders\\TilemapVert2.glsl", "shaders\\TilemapFrag2.glsl"),
	m_worldMapSize(info.tilemapSizeX, info.tilemapSizeY),
	m_tilemapChunkSize(info.chunkSizeX, info.chunkSizeY),
	m_windowWidth(info.windowWidth),
	m_windowHeight(info.windowHeight),
	m_numLayers(info.numLayers)
{
	using namespace std;
	const u32 mapsize = info.tilemapSizeX * info.tilemapSizeY;
	glGenTextures(m_numLayers, m_worldTexturesHandles);

	for (int i = 0; i < info.numLayers; i++) {
		m_worldTexturesBytes[i] = make_unique<u16[]>(mapsize);
		if (i > 0) {
			GetRandomTileMap(m_worldTexturesBytes[i].get(), mapsize);
		}
		else {
			memset(m_worldTexturesBytes[i].get(), 0, mapsize);
		}


		glBindTexture(GL_TEXTURE_2D, m_worldTexturesHandles[i]);

		// must set these two or it won't work
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, info.tilemapSizeX, info.tilemapSizeY, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, m_worldTexturesBytes[i].get());
	}
	

	glGenVertexArrays(1, &m_vao);
}

void NewRenderer::DrawChunk(
	const glm::ivec2& chunkWorldMapOffset,
	const glm::vec2& pos,
	const glm::vec2& scale,
	float rotation,
	ArrayTexture2DHandle texArray,
	const Camera2D& cam
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
	m_tileShader.SetIVec2("chunkSize", m_tilemapChunkSize);
	m_tileShader.setInt("masterTileTexture", 0);
	m_tileShader.setInt("atlasSampler", 1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);
	glBindVertexArray(m_vao);

	for (int i = 0; i < m_numLayers; i++) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_worldTexturesHandles[i]);

		glDrawArrays(GL_TRIANGLES, 0, m_tilemapChunkSize.x * m_tilemapChunkSize.y * TILE_NUM_INDICES);
	}
}
