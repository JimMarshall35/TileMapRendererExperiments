#ifndef  TEXTWIDGET_H
#define TEXTWIDGET_H

#include "HandleDefs.h"

struct TextWidgetData
{
	char* content;
	HFont font;
	hAtlas atlas;
	float r, g, b, a;
};

void CreateTextWidgetData(struct TextWidgetData* pData, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);

HWidget TextWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);

#endif // ! TEXTWIDGET_H
