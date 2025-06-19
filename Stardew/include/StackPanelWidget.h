#ifndef STACKPANEL_WIDGET_H
#define STACKPANEL_WIDGET_H
#include "HandleDefs.h"

struct xml_node;

typedef enum
{
	SPO_Horizontal,
	SPO_Vertical
}StackPanelOrientation;

struct StackPanelWidgetData
{
	StackPanelOrientation orientation;
};

struct UIWidget;
struct xml_node;
struct XMLUIData;

HWidget StackPanelWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUIData);

void StackPanel_PopulateDataFromXML(struct xml_node* pXMLNode, struct StackPanelWidgetData* pData);
float StackPanel_GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent, struct StackPanelWidgetData* pStackPanelData);
float StackPanel_GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent, struct StackPanelWidgetData* pStackPanelData);
void StackPanel_LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent, struct StackPanelWidgetData* pStackPanelData, float w, float h);



#define STACKPANEL_XML_NAME "stackpanel"

#endif
