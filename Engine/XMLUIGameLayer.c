#include "XMLUIGameLayer.h"
#include "DrawContext.h"
#include "InputContext.h"
#include "GameFramework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "IntTypes.h"
#include "DynArray.h"
#include "ObjectPool.h"
#define XML_UI_MAX_PATH 256



struct UIWidget;

typedef float(*GetUIWidgetDimensionFn)(struct UIWidget* pWidget, struct UIWidget* pParent);
typedef void(*LayoutChildrenFn)(struct UIWidget* pWidget, struct UIWidget* pParent);
typedef void(*OnDestroyWidget)(struct UIWidget* pWidget);

typedef int HWidget;
#define NULL_HWIDGET -1

struct UIWidget
{
	int hNext;
	int hPrev;
	int hParent;
	int hFirstChild;
	void* pImplementationData;
	GetUIWidgetDimensionFn fnGetWidth;
	GetUIWidgetDimensionFn fnGetHeight;
	LayoutChildrenFn fnLayoutChildren;
	OnDestroyWidget fnOnDestroy;
	float top;
	float left;
};

typedef struct 
{
	int rootWidget;
	const char xmlFilePath[XML_UI_MAX_PATH];
	const char* xmlData;
	bool bLoaded;
}XMLUIData;


OBJECT_POOL(struct UIWidget) gWidgetPool = NULL;

HWidget NewWidget(
	HWidget firstChild,
	HWidget paremt,
	HWidget next,
	HWidget prev,
	void* pImplementationData,
	GetUIWidgetDimensionFn fnGetWidth,
	GetUIWidgetDimensionFn fnGetHeight,
	LayoutChildrenFn fnLayoutChildren,
	OnDestroyWidget fnOnDestroy,
	float top,
	float left)
{
	HWidget widget = 0;
	gWidgetPool = GetObjectPoolIndex(gWidgetPool, widget);
	struct UIWidget* pWidget = &gWidgetPool[widget];
	pWidget->hParent = paremt;
	pWidget->hFirstChild = firstChild;
	pWidget->hNext = next;
	pWidget->hPrev = prev;
	pWidget->pImplementationData = pImplementationData;
	pWidget->fnGetHeight = fnGetHeight;
	pWidget->fnGetWidth = fnGetWidth;
	pWidget->fnLayoutChildren = fnLayoutChildren;
	pWidget->top = top;
	pWidget->left = left;
	pWidget->fnOnDestroy = fnOnDestroy;

	return widget;
}

struct RootWidgetData
{
	int windowW;
	int windowH;
};

float RootWidget_GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return ((struct RootWidgetData*)pWidget->pImplementationData)->windowW;
}

float RootWidget_GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return ((struct RootWidgetData*)pWidget->pImplementationData)->windowH;
}

void RootWidget_LayoutChildren(struct UIWidget* pThis, struct UIWidget* pParent)
{
	if (pThis->hFirstChild != NULL_HWIDGET)
	{
		struct UIWidget* pWidget = &gWidgetPool[pThis->hFirstChild];
		while (pWidget)
		{
			pWidget->fnLayoutChildren(pWidget, pThis);
			if (pWidget->hNext != NULL_HWIDGET)
			{
				pWidget = &gWidgetPool[pWidget->hNext];
			}
		}
	}
}

void RootWidget_OnDestroy(struct UIWidget* pThis)
{
	free(pThis->pImplementationData);
}

HWidget NewRootWidget()
{
	struct RootWidsgetData* pRootWidgetData = malloc(sizeof(struct RootWidgetData));
	return NewWidget(NULL_HWIDGET, NULL_HWIDGET, NULL_HWIDGET, NULL_HWIDGET,
		pRootWidgetData,
		&RootWidget_GetWidth,
		&RootWidget_GetHeight,
		&RootWidget_LayoutChildren,
		&RootWidget_OnDestroy,
		0, 0);
}

void DestroyWidget(HWidget widget)
{
	if (widget >= 0 && widget < ObjectPoolCapacity(gWidgetPool))
	{
		HWidget child = gWidgetPool[widget].hFirstChild;
		while (child != NULL_HWIDGET)
		{
			DestroyWidget(child);
			child = gWidgetPool[child].hNext;
		}
		FreeObjectPoolIndex(gWidgetPool, widget);
	}
	else
	{
		printf("DestroyWidget: widget %i out of range", widget);
	}
}

static void Update(float deltaT)
{

}

static void Draw(DrawContext* dc)
{

}

static void Input(InputContext* ctx)
{

}

static void OneTimeInit()
{
	gWidgetPool = NEW_OBJECT_POOL(struct UIWidget, 256);
}

static bool OneTimeInitNeeded()
{
	return gWidgetPool == NULL;

}

static void OnPush(DrawContext* drawContext, InputContext* inputContext)
{
	if (OneTimeInitNeeded())
	{
		OneTimeInit();
	}
	printf("on push");
}

static void OnPop(DrawContext* drawContext, InputContext* inputContext)
{
	printf("on pop");
}


void XMLUIGameLayer_Get(struct GameFrameworkLayer* pLayer, const struct XMLUIGameLayerOptions* pOptions)
{
	pLayer->userData = malloc(sizeof(XMLUIData));
	if (!pLayer->userData) printf("XMLUIGameLayer_Get: no memory"); return;
	XMLUIData* pUIData = (XMLUIData*)pLayer->userData;
	
	memset(pLayer->userData, 0, sizeof(XMLUIData));
	
	strcpy(pUIData->xmlFilePath, pOptions->xmlPath);
	pUIData->rootWidget = NewWidget(NULL_HWIDGET, NULL_HWIDGET, NULL_HWIDGET, NULL_HWIDGET, pUIData,)
	pLayer->draw = &Draw;
	pLayer->update = &Update;
	pLayer->input = &Input;
	pLayer->onPop = &OnPop;
	pLayer->onPush = &OnPush;
	pLayer->flags = 0;
	pLayer->flags |= EnableDrawFn | EnableInputFn | EnableUpdateFn | EnableOnPop | EnableOnPush;
}
