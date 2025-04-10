#ifndef WIDGET_H
#define WIDGET_H

#include "ObjectPool.h"

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

struct UIWidget* UI_GetWidget(HWidget hWidget);


#endif
