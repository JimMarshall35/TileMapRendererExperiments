#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include "HandleDefs.h"

/*

A widget with fixed dimensions, optionally set in terms of its parents dims.
Arranges its children according to docking position


*/

typedef struct _xmlNode xmlNode;
struct XMLUIData;

HWidget CanvasWidgetNew(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUILayerData);


#endif // !CANVASWIDGET_H
