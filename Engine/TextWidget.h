#ifndef  TEXTWIDGET_H
#define TEXTWIDGET_H

#include "HandleDefs.h"
#include "DynArray.h"

struct TextWidgetData
{
	char* content;
	HFont font;
	hAtlas atlas;
	float r, g, b, a;
};

void CreateTextWidgetData(struct TextWidgetData* pData, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);

HWidget TextWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);

void* OutputTextWidgetVerts(float left, float top, const struct WidgetPadding* padding, struct TextWidgetData* pData, VECTOR(struct WidgetVertex) pOutVerts);


#endif // ! TEXTWIDGET_H
