#ifndef GAMEFRAMEWORK_H
#define GAMEFRAMEWORK_H
#include "InputContext.h"
#include "DrawContext.h"

typedef void (*UpdateFn)(float deltaT);
typedef void (*DrawFn)(DrawContext* context);
typedef void (*InputFn)(InputContext* context);
typedef void (*OnPushFn)(DrawContext* drawContext, InputContext* inputContext);
typedef void (*OnPopFn)(DrawContext* drawContext, InputContext* inputContext);

typedef enum
{
	EnableUpdateFn = 1,
	EnableDrawFn = 2,
	EnableInputFn = 8,
	EnableOnPush = 16,
	EnableOnPop = 32,
	MasksDraw = 64,
	MasksUpdate = 128,
	MasksInput = 256
}GameFrameworkLayerFlags;

#define GF_ANYMASKMASK (MasksInput | MasksUpdate | MasksDraw)

struct GameFrameworkLayer
{
	UpdateFn update;
	DrawFn draw;
	InputFn input;
	OnPushFn onPush;
	OnPopFn onPop;
	unsigned int flags;
	void* userData; // this is the game freamework users responsiblity to alloc and free
};

void GF_InitGameFramework();
void GF_DestroyGameFramework();

void GF_PushGameFrameworkLayer(const struct GameFrameworkLayer* layer);
void GF_PopGameFrameworkLayer();

void GF_EndFrame(DrawContext* drawContext, InputContext* inputContext);

void GF_UpdateGameFramework(float deltaT);
void GF_InputGameFramework(InputContext* context);
void GF_DrawGameFramework(DrawContext* context);

#endif
