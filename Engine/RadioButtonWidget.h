#ifndef RADIOBUTTONWIDGET_H
#define RADIOBUTTONWIDGET_H

#include "HandleDefs.h"
#include <stdbool.h>

HWidget RadioButtonWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);

void RadioButton_SetSelected(HWidget hRadioButton, bool bSelected);

#endif // !RADIOBUTTONWIDGET_H
