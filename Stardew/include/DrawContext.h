#ifndef DRAWCONTEXT_H
#define DRAWCONTEXT_H

#include "Atlas.h"
#include "HandleDefs.h"

typedef void(*SetCurrentAtlasFn)(hTexture atlas);
typedef HUIVertexBuffer(*NewUIVertexBufferFn)(int size);
typedef void(*UIVertexBufferDataFn)(HUIVertexBuffer hBuf, void* src, size_t size);
typedef void(*DrawUIVertexBufferFn)(HUIVertexBuffer hBuf, size_t vertexCount);
typedef void(*DestroyUIVertexBufferFn)(HUIVertexBuffer hBuf);
typedef hTexture(*UploadTextureFn)(void* src, int channels, int pxWidth, int pxHeight);
typedef void(*DestroyTextureFn)(hTexture tex);

typedef struct DrawContext
{
	int screenWidth;
	int screenHeight;
	NewUIVertexBufferFn NewUIVertexBuffer;
	UIVertexBufferDataFn UIVertexBufferData;
	DrawUIVertexBufferFn DrawUIVertexBuffer;
	DestroyUIVertexBufferFn DestroyVertexBuffer;
	SetCurrentAtlasFn SetCurrentAtlas;
	UploadTextureFn UploadTexture;
	DestroyTextureFn DestroyTexture;
}DrawContext;

DrawContext Dr_InitDrawContext();
void Dr_OnScreenDimsChange(DrawContext* pCtx, int newW, int newH);

#endif