#include "Widget.h"
#include <stdbool.h>

OBJECT_POOL(struct UIWidget) gWidgetPool = NULL;



#define WIDGET_POOL_BOUNDS_CHECK(handle, rVal) OBJ_POOL_BOUNDS_CHECK(handle, rVal, gWidgetPool)

HWidget UI_NewWidget(
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
	gWidgetPool = GetObjectPoolIndex(gWidgetPool, &widget);
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

struct UIWidget* UI_GetWidget(HWidget hWidget)
{
	WIDGET_POOL_BOUNDS_CHECK(hWidget, NULL)
	return &gWidgetPool[hWidget];
}

void UI_Init()
{
	gWidgetPool = NEW_OBJECT_POOL(struct UIWidget, 256);
}

void UI_DestroyWidget(HWidget widget)
{
	if (widget >= 0 && widget < ObjectPoolCapacity(gWidgetPool))
	{
		HWidget child = gWidgetPool[widget].hFirstChild;
		while (child != NULL_HWIDGET)
		{
			gWidgetPool[widget].fnOnDestroy(&gWidgetPool[widget]);
			UI_DestroyWidget(child);
			child = gWidgetPool[child].hNext;
		}
		FreeObjectPoolIndex(gWidgetPool, widget);
	}
	else
	{
		printf("DestroyWidget: widget %i out of range", widget);
	}
}