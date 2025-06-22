#ifndef STATIC_WIDGET_H
#define STATIC_WIDGET_H

#include "HandleDefs.h"
#include "DynArray.h"
#include "Widget.h"
typedef struct _xmlNode xmlNode;

struct StaticWidgetData
{
	char* imageName;
	struct WidgetScale scale;
	hSprite sprite;
	hAtlas atlas;
};

struct XMLUIData;

HWidget StaticWidgetNew(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUILayerData);

void* StaticWidget_OnOutputVerts(struct StaticWidgetData* pStaticData, float left, float top, struct WidgetPadding* pPadding, VECTOR(struct WidgetVertex) pOutVerts);
void StaticWidget_Destroy(struct StaticWidgetData* pStaticData);
float StaticWidget_GetHeight(struct StaticWidgetData* pStaticData, struct WidgetPadding* pPadding);
float StaticWidget_GetWidth(struct StaticWidgetData* pStaticData, struct WidgetPadding* pPadding);
void StaticWidget_MakeFromXML(struct StaticWidgetData* pWidgetData, xmlNode* pXMLNode, struct XMLUIData* pUILayerData);

#endif // !STATIC_WIDGET_H
