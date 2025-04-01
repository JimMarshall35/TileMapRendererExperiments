#ifndef GAMEFRAMEWORK_H
#define GAMEFRAMEWORK_H


typedef void (*UpdateFn)(float deltaT);
typedef void (*DrawFn)(struct DrawContext* context);
typedef void (*InputFn)(struct InputContext* context);

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
void InputGameFramework(struct InputContext* context);
void DrawGameFramework(struct DrawContext* context);

#endif
