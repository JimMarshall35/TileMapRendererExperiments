#include "GameFramework.h"
#include "InputContext.h"
#include "DynArray.h"
#include "DrawContext.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

static GameFrameworkLayer* gLayerStack = NULL;

void InitGameFramework()
{
	gLayerStack = VECTOR(GameFrameworkLayer);
}

void DestroyGameFramework()
{
	DestoryVector(gLayerStack);
}

void PushGameFrameworkLayer(const GameFrameworkLayer* layer)
{
	gLayerStack = VectorPush(gLayerStack, layer);
}
 
void PopGameFrameworkLayer()
{
	VectorPop(gLayerStack);
}

void UpdateGameFramework(float deltaT)
{
	for (int i = VectorSize(gLayerStack) - 1; i >= 0; i++)
	{
		if (gLayerStack[i].flags & EnableUpdateFn)
		{
			gLayerStack[i].update(deltaT);
		}
		if (gLayerStack[i].flags & MasksUpdate)
		{
			break;
		}
	}
}

void InputGameFramework(InputContext* context)
{
	for (int i = VectorSize(gLayerStack) - 1; i >= 0; i++)
	{
		if (gLayerStack[i].flags & EnableInputFn)
		{
			gLayerStack[i].input(context);
		}
		if (gLayerStack[i].flags & MasksInput)
		{
			break;
		}
	}
}

void DrawGameFramework(DrawContext* context)
{
	for (int i = VectorSize(gLayerStack) - 1; i >= 0; i++)
	{
		if (gLayerStack[i].flags & EnableDrawFn)
		{
			gLayerStack[i].draw(context);
		}
		if (gLayerStack[i].flags & MasksDraw)
		{
			break;
		}
	}
}
