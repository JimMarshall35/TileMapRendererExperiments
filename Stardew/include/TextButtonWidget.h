#ifndef TEXTBUTTONWIDGET_H
#define TEXTBUTTONWIDGET_H

#include "HandleDefs.h"

struct XMLUIData;
typedef struct _xmlNode xmlNode;

HWidget TextButtonWidgetNew(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUILayerData);


#endif // !TEXTBUTTONWIDGET_H

