#include "Widget.h"
#include <stdbool.h>
#include <string.h>
#include "xml.h"

OBJECT_POOL(struct UIWidget) gWidgetPool = NULL;



#define WIDGET_POOL_BOUNDS_CHECK(handle, rVal) OBJ_POOL_BOUNDS_CHECK(handle, rVal, gWidgetPool)
#define WIDGET_POOL_BOUNDS_CHECK(handle) OBJ_POOL_BOUNDS_CHECK(handle, gWidgetPool)


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

HWidget UI_NewBlankWidget()
{
	HWidget widget = 0;
	gWidgetPool = GetObjectPoolIndex(gWidgetPool, &widget);
	struct UIWidget* pWidget = &gWidgetPool[widget];
	memset(pWidget, 0, sizeof(struct UIWidget));
	return widget;
}

struct UIWidget* UI_GetWidget(HWidget hWidget)
{
	WIDGET_POOL_BOUNDS_CHECK(hWidget, NULL)
	return &gWidgetPool[hWidget];
}

void UI_AddChild(HWidget hParent, HWidget hChild)
{
	WIDGET_POOL_BOUNDS_CHECK(hParent)
	WIDGET_POOL_BOUNDS_CHECK(hChild)
	struct UIWidget* pParent = UI_GetWidget(hParent);
	struct UIWidget* pChild = UI_GetWidget(hChild);
	if (pParent->hFirstChild == NULL_HWIDGET)
	{
		pParent->hFirstChild = hChild;
		return;
	}
	
	struct UIWidget* pLastChild = UI_GetWidget(pParent->hFirstChild);
	HWidget hLastWidget = hChild;
	while (pLastChild->hNext != NULL_HWIDGET)
	{
		hLastWidget = pLastChild->hNext;
		pLastChild = UI_GetWidget(pLastChild->hNext);
	}
	pLastChild->hNext = hChild;
	pChild->hPrev = hLastWidget;

}

void UI_Init()
{
	gWidgetPool = NEW_OBJECT_POOL(struct UIWidget, 256);
}

void UI_DestroyWidget(HWidget widget)
{
	WIDGET_POOL_BOUNDS_CHECK(widget)
	struct UIWidget* pWidget = UI_GetWidget(widget);
	if (pWidget->hNext != NULL_HWIDGET)
	{
		UI_GetWidget(pWidget->hNext)->hPrev = pWidget->hPrev;
	}
	if (pWidget->hPrev != NULL_HWIDGET)
	{
		UI_GetWidget(pWidget->hPrev)->hNext = pWidget->hNext;
	}
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

void UI_ParseWidgetDimsAttribute(struct xml_string* contents, struct WidgetDim* outWidgetDims)
{
	char attributeBuffer[256];

	int len = xml_string_length(contents);

	xml_string_copy(contents, attributeBuffer, len);
	attributeBuffer[len] = '\0';
	if (strcmp(attributeBuffer, "auto") == 0)
	{
		outWidgetDims->type = WD_Auto;
	}
	if (strcmp(attributeBuffer, "stretch") == 0)
	{
		outWidgetDims->type = WD_Stretch;
	}
}

void UI_ParseWidgetPaddingAttributes(struct xml_node* pInNode, struct WidgetPadding* outWidgetPadding)
{
}

void UI_ParseHorizontalAlignementAttribute(struct xml_string* contents, enum WidgetHorizontalAlignment* outAlignment)
{
	char attributeBuffer[256];

	int len = xml_string_length(contents);

	xml_string_copy(contents, attributeBuffer, len);
	attributeBuffer[len] = '\0';
	if (strcmp(attributeBuffer, "left") == 0)
	{
		*outAlignment = WHA_Left;
	}
	if (strcmp(attributeBuffer, "middle") == 0)
	{
		*outAlignment = WHA_Middle;
	}
	if (strcmp(attributeBuffer, "right") == 0)
	{
		*outAlignment = WHA_Right;
	}
}

float UI_ResolveWidgetDimPxls(const struct WidgetDim* dim)
{
	// TODO: STUB!
	// IMPLEMENT DIFFERENT TYPES OF DIMENSION
	return dim->data;
}
