#include "Widget.h"
#include <stdbool.h>
#include <string.h>
#include "xml.h"
#include <stdlib.h>

OBJECT_POOL(struct UIWidget) gWidgetPool = NULL;



#define WIDGET_POOL_BOUNDS_CHECK(handle, rVal) OBJ_POOL_BOUNDS_CHECK(handle, rVal, gWidgetPool)
#define WIDGET_POOL_BOUNDS_CHECK(handle) OBJ_POOL_BOUNDS_CHECK(handle, gWidgetPool)

HWidget UI_NewBlankWidget()
{
	HWidget widget = 0;
	gWidgetPool = GetObjectPoolIndex(gWidgetPool, &widget);
	struct UIWidget* pWidget = &gWidgetPool[widget];
	memset(pWidget, 0, sizeof(struct UIWidget));
	pWidget->hFirstChild = NULL_HWIDGET;
	pWidget->hNext = NULL_HWIDGET;
	pWidget->hPrev = NULL_HWIDGET;
	pWidget->hParent = NULL_HWIDGET;
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
	pChild->hParent = hParent;
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
	int attribs = xml_node_attributes(pInNode);
	char attribNameBuf[128];
	char attribValBuf[128];
	memset(outWidgetPadding, 0, sizeof(struct WidgetPadding));
	for (int i = 0; i < attribs; i++)
	{
		struct xml_string* pAttribName = xml_node_attribute_name(pInNode, i);
		int attribNameLen = xml_string_length(pAttribName);
		xml_string_copy(pAttribName, attribNameBuf, attribNameLen);
		attribNameBuf[attribNameLen] = '\0';

		struct xml_string* pAttribVal = xml_node_attribute_content(pInNode, i);
		int attribValLen = xml_string_length(pAttribVal);
		xml_string_copy(pAttribVal, attribValBuf, attribValLen);
		attribValBuf[attribValLen] = '\0';

		if (strcmp(attribNameBuf, "paddingTop") == 0)
		{
			outWidgetPadding->paddingTop = (float)atof(attribValBuf);
		}
		else if (strcmp(attribNameBuf, "paddingBottom") == 0)
		{
			outWidgetPadding->paddingBottom = (float)atof(attribValBuf);
		}
		else if (strcmp(attribNameBuf, "paddingLeft") == 0)
		{
			outWidgetPadding->paddingLeft = (float)atof(attribValBuf);
		}
		else if (strcmp(attribNameBuf, "paddingRight") == 0)
		{
			outWidgetPadding->paddingRight = (float)atof(attribValBuf);
		}
	}
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
	else if (strcmp(attributeBuffer, "middle") == 0)
	{
		*outAlignment = WHA_Middle;
	}
	else if (strcmp(attributeBuffer, "right") == 0)
	{
		*outAlignment = WHA_Right;
	}
	else
	{
		*outAlignment = WHA_Middle;
	}
}

void UI_ParseVerticalAlignementAttribute(struct xml_string* contents, enum WidgetVerticalAlignment* outAlignment)
{
	char attributeBuffer[256];

	int len = xml_string_length(contents);

	xml_string_copy(contents, attributeBuffer, len);
	attributeBuffer[len] = '\0';
	if (strcmp(attributeBuffer, "top") == 0)
	{
		*outAlignment = WVA_Top;
	}
	else if (strcmp(attributeBuffer, "middle") == 0)
	{
		*outAlignment = WVA_Middle;
	}
	else if (strcmp(attributeBuffer, "bottom") == 0)
	{
		*outAlignment = WVA_Bottom;
	}
	else
	{
		*outAlignment = WVA_Middle;
	}
	
}

float UI_ResolveWidgetDimPxls(const struct WidgetDim* dim)
{
	// TODO: STUB!
	// IMPLEMENT DIFFERENT TYPES OF DIMENSION
	return dim->data;
}

bool UI_ParseWidgetDockPoint(struct xml_node* pInNode, struct UIWidget* outWidget)
{
	int numAttribs = xml_node_attributes(pInNode);
	char attribNameBuf[256];
	char attribValBuf[256];
	outWidget->dockPoint = 0;
	for (int i = 0; i < numAttribs; i++)
	{
		struct xml_string* pName = xml_node_attribute_name(pInNode, i);
		struct xml_string* pVal = xml_node_attribute_content(pInNode, i);
		int namelen = xml_string_length(pName);
		int vallen = xml_string_length(pVal);
		xml_string_copy(pName, attribNameBuf, namelen);
		xml_string_copy(pVal, attribValBuf, vallen);
		attribNameBuf[namelen] = '\0';
		attribValBuf[vallen] = '\0';
		if (strcmp(attribNameBuf, "dockPoint") == 0)
		{
			if (strcmp(attribValBuf, "topLeft") == 0)
			{
				outWidget->dockPoint = WDP_TopLeft;
				return true;
			}
			else if (strcmp(attribValBuf, "topMiddle") == 0)
			{
				outWidget->dockPoint = WDP_TopMiddle;
				return true;
			}
			else if (strcmp(attribValBuf, "topRight") == 0)
			{
				outWidget->dockPoint = WDP_TopRight;
				return true;
			}
			else if (strcmp(attribValBuf, "middleRight") == 0)
			{
				outWidget->dockPoint = WDP_MiddleRight;
				return true;
			}
			else if (strcmp(attribValBuf, "bottomRight") == 0)
			{
				outWidget->dockPoint = WDP_BottomRight;
				return true;
			}
			else if (strcmp(attribValBuf, "bottomMiddle") == 0)
			{
				outWidget->dockPoint = WDP_BottomMiddle;
				return true;
			}
			else if (strcmp(attribValBuf, "bottomLeft") == 0)
			{
				outWidget->dockPoint = WDP_BottomLeft;
				return true;
			}
			else if (strcmp(attribValBuf, "middleLeft") == 0)
			{
				outWidget->dockPoint = WDP_MiddleLeft;
				return true;
			}
			else if (strcmp(attribValBuf, "centrw") == 0)
			{
				outWidget->dockPoint = WDP_Centre;
				return true;
			}
		}
	}
	return false;
}

void UI_WidgetCommonInit(struct xml_node* pInNode, struct UIWidget* outWidget)
{
	UI_ParseWidgetPaddingAttributes(pInNode, &outWidget->padding);
	UI_ParseWidgetDockPoint(pInNode, outWidget);
}

static const char* GetDockingPointName(WidgetDockPoint dockingPoint)
{
	switch (dockingPoint)
	{
	case WDP_TopLeft: return "TopLeft";
	case WDP_TopMiddle: return "TopMiddle";
	case WDP_TopRight: return "TopRight";
	case WDP_MiddleRight: return "MiddleRight";
	case WDP_BottomRight: return "BottomRight";
	case WDP_BottomMiddle:  return "BottomMiddle";
	case WDP_BottomLeft:  return "BottomLeft";
	case WDP_MiddleLeft:  return "MiddleLeft";
	default: return "Unknown";
	}
}

void UI_DebugPrintCommonWidgetInfo(const struct UIWidget* inWidget, PrintfFn pPrintfFn)
{
	pPrintfFn("paddingTop = %.1f, paddingBottom = %.1f, paddingLeft = %.1f, paddingRight = %.1f, left = %.1f, top = %.1f, dockPoint = %s",
		inWidget->padding.paddingTop,
		inWidget->padding.paddingBottom,
		inWidget->padding.paddingLeft,
		inWidget->padding.paddingRight,
		inWidget->left,
		inWidget->top,
		GetDockingPointName(inWidget->dockPoint));
}

void* UI_Helper_OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	HWidget child = pWidget->hFirstChild;
	while (child != NULL_HWIDGET)
	{
		struct UIWidget* pChildWidget = UI_GetWidget(child);
		if (pChildWidget->fnOutputVertices)
		{
			pOutVerts = pChildWidget->fnOutputVertices(pChildWidget, pOutVerts);
		}
		child = pChildWidget->hNext;
	}
	return pOutVerts;
}
