#ifndef  TEXTWIDGET_H
#define TEXTWIDGET_H

#include "HandleDefs.h"
#include "DynArray.h"

struct TextWidgetData
{
	char* content;
	HFont font;
	float fSizePts;
	hAtlas atlas;
	float r, g, b, a;
};

struct xml_node;
struct XMLUIData;
struct WidgetPadding;
struct WidgetVertex;

void TextWidget_FromXML(struct TextWidgetData* pData, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);

HWidget TextWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);

void* TextWidget_OutputVerts(float left, float top, const struct WidgetPadding* padding, struct TextWidgetData* pData, VECTOR(struct WidgetVertex) pOutVerts);

void TextWidget_Destroy(struct TextWidgetData* pData);

#endif // ! TEXTWIDGET_H
