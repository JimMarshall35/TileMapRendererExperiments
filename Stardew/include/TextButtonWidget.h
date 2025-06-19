#ifndef TEXTBUTTONWIDGET_H
#define TEXTBUTTONWIDGET_H

#include "HandleDefs.h"

struct XMLUIData;
struct xml_node;

HWidget TextButtonWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);


#endif // !TEXTBUTTONWIDGET_H

