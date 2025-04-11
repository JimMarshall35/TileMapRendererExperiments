#ifndef WIDGET_H
#define WIDGET_H

#include "ObjectPool.h"
#include "HandleDefs.h"

struct UIWidget;

struct xml_string;
struct xml_node;

typedef float(*GetUIWidgetDimensionFn)(struct UIWidget* pWidget, struct UIWidget* pParent);
typedef void(*LayoutChildrenFn)(struct UIWidget* pWidget, struct UIWidget* pParent);
typedef void(*OnDestroyWidget)(struct UIWidget* pWidget);


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
	WidgetHorizontalAlignment horizontalAlignment;
	WidgetVerticalAlignment verticalAlignment;
};


void UI_Init();


void UI_DestroyWidget(HWidget widget);

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
	float left);

HWidget UI_NewBlankWidget();

struct UIWidget* UI_GetWidget(HWidget hWidget);

void UI_AddChild(HWidget hParent, HWidget hChild);

void UI_ParseWidgetDimsAttribute(struct xml_string* contents, struct WidgetDim* outWidgetDims);

void UI_ParseWidgetPaddingAttributes(struct xml_node* pInNode, struct WidgetPadding* outWidgetPadding);

void UI_ParseHorizontalAlignementAttribute(struct xml_string* contents, enum WidgetHorizontalAlignment* outAlignment);

void UI_ParseVerticalAlignementAttribute(struct xml_string* contents, enum WidgetVerticalAlignment* outAlignment);

float UI_ResolveWidgetDimPxls(const struct WidgetDim* dim);

#endif
