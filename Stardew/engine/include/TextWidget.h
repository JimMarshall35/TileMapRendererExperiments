#ifndef  TEXTWIDGET_H
#define TEXTWIDGET_H

#include "HandleDefs.h"
#include "DynArray.h"

typedef struct _xmlNode xmlNode;

struct TextWidgetData
{
	char* content;
	HFont font;
	float fSizePts;
	hAtlas atlas;
	float r, g, b, a;
};


struct XMLUIData;
struct WidgetPadding;
struct WidgetVertex;

void TextWidget_FromXML(struct TextWidgetData* pData, xmlNode* pXMLNode, struct XMLUIData* pUILayerData);

HWidget TextWidgetNew(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUILayerData);

void* TextWidget_OutputVerts(float left, float top, const struct WidgetPadding* padding, struct TextWidgetData* pData, VECTOR(struct WidgetVertex) pOutVerts);

void TextWidget_Destroy(struct TextWidgetData* pData);

#endif // ! TEXTWIDGET_H
