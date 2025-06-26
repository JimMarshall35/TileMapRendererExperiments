#ifndef RADIOGROUPWIDGET_H
#define RADIOGROUPWIDGET_H

#include "HandleDefs.h"

struct XMLUIData;
struct UIWidget;
typedef struct _xmlNode xmlNode;

HWidget RadioGroupWidgetNew(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUILayerData);
void RadioGroup_ChildSelected(HWidget hRadioGroup, struct UIWidget* pRadioButtonChild);


#endif // !RADIOGROUPWIDGET_H
