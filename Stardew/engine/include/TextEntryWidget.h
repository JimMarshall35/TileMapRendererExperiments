#ifndef TEXTENTRYWIDGET_H
#define TEXTENTRYWIDGET_H

#include "HandleDefs.h"

/*

A widget for entering text


*/
typedef struct _xmlNode xmlNode;
struct XMLUIData;

HWidget TextEntryWidgetNew(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUILayerData);


#endif // !TEXTENTRYWIDGET_H
