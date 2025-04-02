#ifndef GAMEFRAMEWORK_H
#define GAMEFRAMEWORK_H
#include "InputContext.h"
#include "DrawContext.h"

typedef void (*UpdateFn)(float deltaT);
typedef void (*DrawFn)(DrawContext* context);
typedef void (*InputFn)(InputContext* context);

typedef enum
{
	EnableUpdateFn = 1,
	EnableDrawFn = 2,
	EnableInputFn = 8,
	MasksDraw = 16,
	MasksUpdate = 32,
	MasksInput = 64
}GameFrameworkLayerFlags;

typedef struct
{
	UpdateFn update;
	DrawFn draw;
	InputFn input;
	unsigned int flags;
}GameFrameworkLayer;

void InitGameFramework();
void DestroyGameFramework();

void PushGameFrameworkLayer(const GameFrameworkLayer* layer);
void PopGameFrameworkLayer();

void UpdateGameFramework(float deltaT);
void InputGameFramework(InputContext* context);
void DrawGameFramework(DrawContext* context);

#endif
