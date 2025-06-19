#ifndef RADIOGROUPWIDGET_H
#define RADIOGROUPWIDGET_H

#include "HandleDefs.h"

struct xml_node;
struct XMLUIData;
struct UIWidget;

HWidget RadioGroupWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);
void RadioGroup_ChildSelected(HWidget hRadioGroup, struct UIWidget* pRadioButtonChild);


#endif // !RADIOGROUPWIDGET_H
