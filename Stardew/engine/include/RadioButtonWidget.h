#ifndef RADIOBUTTONWIDGET_H
#define RADIOBUTTONWIDGET_H

#include "HandleDefs.h"
#include <stdbool.h>

typedef struct _xmlNode xmlNode;
struct XMLUIData;

HWidget RadioButtonWidgetNew(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUILayerData);

void RadioButton_SetSelected(HWidget hRadioButton, bool bSelected);

#endif // !RADIOBUTTONWIDGET_H
