#ifndef BACKGROUNDBOXWIDGET_H
#define BACKGROUNDBOXWIDGET_H

#include "HandleDefs.h"
#include "Widget.h"

struct BackgroundBoxWidgetData
{
	char* imageName;
	struct WidgetScale scale;
	hSprite sprite;
	hAtlas atlas;
};

void CreateBackgroundBoxWidgetData(struct UIWidget* pWidget, struct BackgroundBoxWidgetData* pWidgetData, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);
HWidget BackgroundBoxWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);


#endif // !BACKGROUNDBOXWIDGET_H

