#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "OpenGlRenderer.h"
#include "GLTextureHelper.h"
#include "Tile.h"
#include "Camera2D.h"


OpenGlRenderer::OpenGlRenderer(u32 windowWidth, u32 windowHeight)
	:_windowWidth(windowWidth), _windowHeight(windowHeight)
{
	_tileMapShader = Shader("shaders\\TileMapVert.glsl", "shaders\\TileMapFrag.glsl", "shaders\\TileMapGeom.glsl");
}

bool OpenGlRenderer::GPULoadAtlasTexture(const unsigned char* data, unsigned int width, unsigned int height, unsigned int* id)
{
	_atlasTexture = OpenGlGPULoadTexture(data, width, height, id);
	return _atlasTexture;
}


static void TileToUBOStruct(const Tile& tile, glm::vec4& output) {
	output[0] = tile.UTopLeft;
	output[1] = tile.VTopLeft;
	output[2] = tile.UBottomRight;
	output[3] = tile.VBottomRight;
}

static void FillInUboStructsArray(const std::vector<Tile>& tiles, glm::vec4* output) {
	u32 i = 0;
	for (const auto& tile : tiles) {
		TileToUBOStruct(tile, output[i++]);
	}
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
	// Todo: gracefully handle when too many tiles are trying to be loaded for the ubo 
	if (_tilesDataUboHandle != 0) {
		std::cout << "somethings wrong - you already have the UBO." << std::endl;
		return false;
	}
	auto uboUvs = std::make_unique<glm::vec4[]>(tiles.size());
	FillInUboStructsArray(tiles, uboUvs.get());
	_tileMapShader.use();
	auto programHandle = _tileMapShader.ID;
	GLuint blockIndex = glGetUniformBlockIndex(programHandle, "TilesDataBlock");
	GLint blockSize;
	glGetActiveUniformBlockiv(programHandle, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	auto blockBuffer = std::make_unique<GLbyte[]>(blockSize);
#define NUM_TILE_ATTRIBUTES 1
	const GLchar* names[] = { "uvCoordsTopLeftBottomRight"};
	GLuint indices[NUM_TILE_ATTRIBUTES];
	glGetUniformIndices(programHandle, NUM_TILE_ATTRIBUTES, names, indices);
	GLint offset[1];
	glGetActiveUniformsiv(programHandle, NUM_TILE_ATTRIBUTES, indices, GL_UNIFORM_OFFSET, offset);
	auto size = tiles.size() * sizeof(glm::vec4);
	memcpy(blockBuffer.get() + offset[0], uboUvs.get(), size);

	glGenBuffers(1, &_tilesDataUboHandle);
	glBindBuffer(GL_UNIFORM_BUFFER, _tilesDataUboHandle);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer.get(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, _tilesDataUboHandle);
	return true;
}

void OpenGlRenderer::SetWindowWidthAndHeight(u32 width, u32 height)
{
	_windowWidth = width;
	_windowHeight = height;
}

VertexBufferHandle OpenGlRenderer::UploadTilemapVerticesBuffer(const u32* vertices, u32 tilesWidth, u32 tilesHeight) const
{
	u32 vbo, vao;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, tilesWidth * tilesHeight * sizeof(u32), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(u32), (void*)0);
	glEnableVertexAttribArray(0);

	return vao;
}

void OpenGlRenderer::DrawTileMap(
	VertexBufferHandle vertexBufferHandle,
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
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, _tilesDataUboHandle);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _atlasTexture);
	glBindVertexArray(vertexBufferHandle);
	glDrawArrays(GL_POINTS, 0, numVertices);
	
}
