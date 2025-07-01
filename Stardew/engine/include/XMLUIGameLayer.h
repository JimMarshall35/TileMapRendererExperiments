#ifndef XMLUI_GAME_LAYER_H
#define XMLUI_GAME_LAYER_H
#include <stdbool.h>
#include "HandleDefs.h"
#include "DynArray.h"
#include "Widget.h"
#include "DrawContext.h"
#include <libxml/tree.h>
 

struct XMLUIData;
struct GameFrameworkLayer;

struct XMLUIGameLayerOptions
{
	DrawContext* pDc;
	const char* xmlPath;
	bool bLoadImmediately; // eg, when XMLUIGameLayer_Get is called, do we load the xml file?
};

typedef HWidget(*AddChildFn)(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUILayerData);

#define XML_UI_MAX_PATH 256
#define MAX_FOCUSED_WIDGETS 16

typedef struct XMLUIData
{
	HWidget rootWidget;
	char xmlFilePath[XML_UI_MAX_PATH];
	const char* xmlData;
	bool bLoaded;
	hAtlas atlas;
	VECTOR(struct WidgetVertex) pWidgetVertices;
	HUIVertexBuffer hVertexBuffer;
	int hViewModel; // reference to lua table
	HWidget focusedWidgets[MAX_FOCUSED_WIDGETS];
	int nFocusedWidgets;
}XMLUIData;



void XMLUIGameLayer_Get(struct GameFrameworkLayer* pLayer, struct XMLUIGameLayerOptions* pOptions);


#endif
