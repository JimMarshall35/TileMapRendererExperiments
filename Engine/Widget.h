#ifndef WIDGET_H
#define WIDGET_H

#include "ObjectPool.h"
#include "HandleDefs.h"
#include <stdbool.h>
#include "DynArray.h"

struct UIWidget;

struct xml_string;
struct xml_node;

struct WidgetVertex
{
	float x, y;
	float u, v;
	float r, g, b, a;
};

typedef void(*PrintfFn)(const char* fmt, ...);

typedef float(*GetUIWidgetDimensionFn)(struct UIWidget* pWidget, struct UIWidget* pParent);
typedef void(*LayoutChildrenFn)(struct UIWidget* pWidget, struct UIWidget* pParent);
typedef void(*OnDestroyWidgetFn)(struct UIWidget* pWidget);
typedef void(*OnDebugPrintFn)(int indentLvl, struct UIWidget* pWidget, PrintfFn printfFn);
typedef void* (*OutputWidgetVerticesFn)(struct UIWidget* pThisWidget, VECTOR(struct WidgetVertex) pOutVerts);


struct WidgetPadding
{
	float paddingTop;
	float paddingBottom;
	float paddingLeft;
	float paddingRight;
};

struct WidgetScale
{
	float scaleX;
	float scaleY;
};

enum WidgetDimType
{
	WD_Auto,
	WD_Stretch,
	FD_Pixels,
	FD_ScreenFraction,
};

typedef enum WidgetHorizontalAlignment
{
	WHA_Left,
	WHA_Middle,
	WHA_Right
}WidgetHorizontalAlignment;

typedef enum WidgetVerticalAlignment
{
	WVA_Top,
	WVA_Middle,
	WVA_Bottom
}WidgetVerticalAlignment;

typedef enum WidgetDockPoint
{
	WDP_TopLeft,
	WDP_TopMiddle,
	WDP_TopRight,
	WDP_MiddleRight,
	WDP_BottomRight,
	WDP_BottomMiddle,
	WDP_BottomLeft,
	WDP_MiddleLeft,
	WDP_Centre
}WidgetDockPoint;

struct WidgetDim
{
	enum WidgetDimType type;
	float data;
};


struct UIWidget
{
	HWidget hNext;
	HWidget hPrev;
	HWidget hParent;
	HWidget hFirstChild;
	void* pImplementationData;
	GetUIWidgetDimensionFn fnGetWidth;
	GetUIWidgetDimensionFn fnGetHeight;
	LayoutChildrenFn fnLayoutChildren;
	OnDestroyWidgetFn fnOnDestroy;
	OnDebugPrintFn fnOnDebugPrint;
	OutputWidgetVerticesFn fnOutputVertices;
	float top;
	float left;
	WidgetDockPoint dockPoint;
	struct WidgetPadding padding;
};


void UI_Init();


void UI_DestroyWidget(HWidget widget);


HWidget UI_NewBlankWidget();

size_t UI_CountWidgetChildrenPtr(struct UIWidget* pWidget);

size_t UI_CountWidgetChildren(HWidget pWidget);

struct UIWidget* UI_GetWidget(HWidget hWidget);

void UI_AddChild(HWidget hParent, HWidget hChild);

void UI_ParseWidgetDimsAttribute(struct xml_string* contents, struct WidgetDim* outWidgetDims);

void UI_ParseWidgetPaddingAttributes(struct xml_node* pInNode, struct WidgetPadding* outWidgetPadding);

void UI_ParseHorizontalAlignementAttribute(struct xml_string* contents, enum WidgetHorizontalAlignment* outAlignment);

void UI_ParseVerticalAlignementAttribute(struct xml_string* contents, enum WidgetVerticalAlignment* outAlignment);

float UI_ResolveWidgetDimPxls(const struct WidgetDim* dim);

bool UI_ParseWidgetDockPoint(struct xml_node* pInNode, struct UIWidget* outWidget);

void UI_WidgetCommonInit(struct xml_node* pInNode, struct UIWidget* outWidget);

void UI_DebugPrintCommonWidgetInfo(const struct UIWidget* inWidget, PrintfFn pPrintfFn);

void* UI_Helper_OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts);

void UI_Helper_OnLayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent);

#endif
