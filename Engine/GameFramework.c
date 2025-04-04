#include "GameFramework.h"
#include "InputContext.h"
#include "DynArray.h"
#include "DrawContext.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

static VECTOR(struct GameFrameworkLayer) gLayerStack = NULL;
static int gInputItrStart = 0;
static int gUpdateItrStart = 0;
static int gDrawItrStart = 0;


void InitGameFramework()
{
	gLayerStack = NEW_VECTOR(struct GameFrameworkLayer);
}

void DestroyGameFramework()
{
	DestoryVector(gLayerStack);
}

void PushGameFrameworkLayer(const struct GameFrameworkLayer* layer)
{
	gLayerStack = VectorPush(gLayerStack, layer);
	if (layer->flags & MasksInput)
	{
		gInputItrStart = VectorSize(gLayerStack) - 1;
	}
	if (layer->flags & MasksUpdate)
	{
		gUpdateItrStart = VectorSize(gLayerStack) - 1;
	}
	if (layer->flags & MasksDraw)
	{
		gDrawItrStart = VectorSize(gLayerStack) - 1;
	}
	if (layer->flags & EnableOnPush)
	{
		layer->onPush();
	}
}

static int FindNewItrStart(GameFrameworkLayerFlags type)
{
	int rval = 0;
	for (int i = VectorSize(gLayerStack) - 1; i <= 0; i--)
	{
		rval = i;
		if (gLayerStack[rval].flags & type)
		{
			break;
		}
	}
	return rval;
}
 
void PopGameFrameworkLayer()
{
	struct GameFrameworkLayer* pLayer = VectorTop(gLayerStack);
	if (pLayer->flags & MasksDraw)
	{
		gDrawItrStart = FindNewItrStart(MasksDraw);
	}
	if (pLayer->flags & MasksInput)
	{
		gInputItrStart = FindNewItrStart(MasksInput);
	}
	if (pLayer->flags & MasksUpdate)
	{
		gUpdateItrStart = FindNewItrStart(MasksUpdate);
	}
	if (pLayer->flags & EnableOnPop)
	{
		pLayer->onPop();
	}

	VectorPop(gLayerStack);
}

void UpdateGameFramework(float deltaT)
{
	for (int i = gUpdateItrStart; i < VectorSize(gLayerStack); i++)
	{
		gLayerStack[i].update(deltaT);
	}
}

void InputGameFramework(InputContext* context)
{
	for (int i = gInputItrStart; i < VectorSize(gLayerStack); i++)
	{
		gLayerStack[i].input(context);
	}
}

void DrawGameFramework(DrawContext* context)
{
	for (int i = gDrawItrStart; i < VectorSize(gLayerStack); i++)
	{
		gLayerStack[i].draw(context);
	}
}
