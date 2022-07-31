#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "OpenGlRenderer.h"
#include "GLTextureHelper.h"
#include "Tile.h"
#include "Camera2D.h"

#define ATLAS_TEXTURE_UNIT GL_TEXTURE0
#define ATLAS_UVS_TEXTURE_UNIT GL_TEXTURE1



OpenGlRenderer::OpenGlRenderer(u32 windowWidth, u32 windowHeight)
	:_windowWidth(windowWidth), _windowHeight(windowHeight),
	_tileMapShader("shaders\\TileMapVert.glsl", "shaders\\TileMapFrag.glsl", "shaders\\TileMapGeom.glsl")
{
	//_tileMapShader = Shader("shaders\\TileMapVert.glsl", "shaders\\TileMapFrag.glsl", "shaders\\TileMapGeom.glsl");
}

bool OpenGlRenderer::GPULoadAtlasTexture(const unsigned char* data, unsigned int width, unsigned int height, unsigned int* id)
{
	_atlasTexture = OpenGlGPULoadTexture(data, width, height, id);
	return _atlasTexture;
}

static void FillInTextureArray(const std::vector<Tile>& tiles, f32* output) {
	u32 i = 0;
	for (const auto& tile : tiles) {
		output[i++] = tile.UTopLeft;
		output[i++] = tile.VTopLeft;
		output[i++] = tile.UBottomRight;
		output[i++] = tile.VBottomRight;
	}
}

u32 MakeTextureFromTilesData(const std::vector<Tile>& tiles) {
	u32 length = tiles.size() * 4;
	auto textureData = std::vector<float>(length);
	FillInTextureArray(tiles, textureData.data());
	u32 id;
	OpenGlGPULoadUVsTexture(textureData.data(), tiles.size(), &id);
	return id;
}

/// <summary>
/// load an opengl uniform buffer object (Ubo) from a list of Tile structs.
/// Vertices in the shader index into this buffer to get their uv coordinates.
/// Should be called once when new tiles have been loaded.
/// </summary>
/// <param name="tiles">A List of Tile objects containing uv coords for top left and bottom right corners of each tile</param>
/// <returns></returns>
bool OpenGlRenderer::GPULoadTileData(const std::vector<Tile>& tiles)
{
	if (_atlasUvsTexture != 0) {
		std::cout << "somethings wrong - you already loaded the atlas uvs texture." << std::endl;
		return false;
	}
	_atlasUvsTexture = MakeTextureFromTilesData(tiles);
	return true;
}

void OpenGlRenderer::SetWindowWidthAndHeight(u32 width, u32 height)
{
	_windowWidth = width;
	_windowHeight = height;
}

void OpenGlRenderer::UploadTilemapVerticesBuffer(const u32* vertices, u32 tilesWidth, u32 tilesHeight, VertexBufferObjectHandle& vbo, VertexAttributeObjectHandle& vao) const
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, tilesWidth * tilesHeight * sizeof(u32), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(u32), (void*)0);
	glEnableVertexAttribArray(0);
}

void OpenGlRenderer::DrawTileMap(
	VertexBufferObjectHandle vertexBufferHandle,
	u32 numVertices,
	const glm::ivec2& mapSize,
	const glm::vec2& pos,
	const glm::vec2& scale,
	float rotation,
	const Camera2D& cam) const
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	_tileMapShader.use();
	_tileMapShader.setMat4("projection", cam.GetProjectionMatrix(_windowWidth,_windowHeight));
	_tileMapShader.setMat4("model", model);
	_tileMapShader.SetIVec2("mapSize", mapSize);
	_tileMapShader.setInt("atlasSampler", 0);
	_tileMapShader.setInt("uvsSampler", 1);

	glActiveTexture(ATLAS_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, _atlasTexture);
	glActiveTexture(ATLAS_UVS_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_1D, _atlasUvsTexture);

	glBindVertexArray(vertexBufferHandle);
	glDrawArrays(GL_POINTS, 0, numVertices);
	
}
