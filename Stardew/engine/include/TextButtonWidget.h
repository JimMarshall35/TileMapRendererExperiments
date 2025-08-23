#ifndef TEXTBUTTONWIDGET_H
#define TEXTBUTTONWIDGET_H

#include "HandleDefs.h"

struct XMLUIData;
struct DataNode;

HWidget TextButtonWidgetNew(HWidget hParent, struct DataNode* pXMLNode, struct XMLUIData* pUILayerData);


#endif // !TEXTBUTTONWIDGET_H

