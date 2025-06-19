#pragma once
#include "IRenderer.h"
#include "BasicTypedefs.h"
#include "Shader.h"
class OpenGlRenderer
{
public:
	OpenGlRenderer(u32 windowWidth, u32 windowHeight);
	// Inherited via IRenderer
	bool GPULoadAtlasTexture(const unsigned char* data, unsigned int width, unsigned int height, unsigned int* id);
	// Inherited via IRenderer
	void DrawTileMap(
		VertexBufferObjectHandle vertexBufferHandle,
		u32 numVertices,
		const glm::ivec2& mapSize,
		const glm::vec2& pos,
		const glm::vec2& scale,
		float rotation,
		const Camera2D& cam)const;
	// Inherited via IRenderer
	bool GPULoadTileData(const std::vector<Tile>& tiles);
	// Inherited via IRenderer
	void SetWindowWidthAndHeight(u32 width, u32 height);
	// Inherited via IRenderer
	void UploadTilemapVerticesBuffer(const u32* vertices, u32 tilesWidth, u32 tilesHeight,
		VertexBufferObjectHandle& vbo, VertexAttributeObjectHandle& vao) const;

private:
	u32 _atlasUvsTexture = 0;
	u32 _atlasTexture = 0;
	u32 _windowWidth;
	u32 _windowHeight;


	Shader _tileMapShader;

};

