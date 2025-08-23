#ifndef TEXTENTRYWIDGET_H
#define TEXTENTRYWIDGET_H

#include "HandleDefs.h"

/*

A widget for entering text


*/
struct DataNode;
struct XMLUIData;

HWidget TextEntryWidgetNew(HWidget hParent, struct DataNode* pXMLNode, struct XMLUIData* pUILayerData);


#endif // !TEXTENTRYWIDGET_H
