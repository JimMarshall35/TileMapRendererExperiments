#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include "HandleDefs.h"

/*

A widget with fixed dimensions, optionally set in terms of its parents dims.
Arranges its children according to docking position


*/
#include <stdbool.h>
#include "Geometry.h"
#include "SliderWidget.h"

struct CanvasData
{
	float scrollX;
	float scrollY;
	GeomRect contentBB;

	struct SliderData sliderH;
	struct SliderData sliderV;
	vec2 sliderHTopLeft;
	vec2 sliderVTopLeft;

	bool bHSliderActive;
	bool bVSliderActive;
};

typedef struct _xmlNode xmlNode;
struct XMLUIData;
struct UIWidget;
struct WidgetVertex;

HWidget CanvasWidgetNew(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUILayerData);
void* CanvasWidget_OnOutputVerts(struct UIWidget* pWidget, struct WidgetVertex* pOutVerts);


#endif // !CANVASWIDGET_H
