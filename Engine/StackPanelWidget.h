#ifndef STACKPANEL_WIDGET_H
#define STACKPANEL_WIDGET_H
#include "HandleDefs.h"

struct xml_node;

HWidget StackPanelWidgetNew(HWidget hParent, struct xml_node* pXMLNode);

#define STACKPANEL_XML_NAME "stackpanel"

#endif