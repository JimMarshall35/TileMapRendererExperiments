#pragma once
#include "IRenderer.h"
#include "BasicTypedefs.h"
#include "Shader.h"
class OpenGlRenderer : public IRenderer
{
public:
	OpenGlRenderer(u32 windowWidth, u32 windowHeight);
	// Inherited via IRenderer
	virtual bool GPULoadAtlasTexture(const unsigned char* data, unsigned int width, unsigned int height, unsigned int* id) override;
	// Inherited via IRenderer
	virtual void DrawTileMap(
		VertexBufferHandle vertexBufferHandle,
		u32 numVertices,
		const glm::ivec2& mapSize,
		const glm::vec2& pos,
		const glm::vec2& scale,
		float rotation,
		const Camera2D& cam)const override;
	// Inherited via IRenderer
	virtual bool GPULoadTileData(const std::vector<Tile>& tiles) override;
	// Inherited via IRenderer
	virtual void SetWindowWidthAndHeight(u32 width, u32 height) override;
	// Inherited via IRenderer
	virtual VertexBufferHandle UploadTilemapVerticesBuffer(const u32* vertices, u32 tilesWidth, u32 tilesHeight) const override ;

private:
	u32 _atlasTexture;
	u32 _windowWidth;
	u32 _windowHeight;


	Shader _tileMapShader;
	u32 _tilesDataUboHandle = 0;





};

