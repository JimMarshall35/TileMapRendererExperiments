#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include "HandleDefs.h"

/*

A widget with fixed dimensions, optionally set in terms of its parents dims.
Arranges its children according to docking position


*/

struct xml_node;
struct XMLUIData;

HWidget CanvasWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);


#endif // !CANVASWIDGET_H
