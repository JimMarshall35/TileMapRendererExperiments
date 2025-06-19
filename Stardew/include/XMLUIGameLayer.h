#ifndef XMLUI_GAME_LAYER_H
#define XMLUI_GAME_LAYER_H
#include <stdbool.h>
#include "HandleDefs.h"
#include "DynArray.h"
#include "Widget.h"
#include "DrawContext.h"

struct xml_node;
struct XMLUIData;
struct GameFrameworkLayer;

struct XMLUIGameLayerOptions
{
	DrawContext* pDc;
	const char* xmlPath;
	bool bLoadImmediately; // eg, when XMLUIGameLayer_Get is called, do we load the xml file?
};

typedef HWidget(*AddChildFn)(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);

#define XML_UI_MAX_PATH 256

typedef struct XMLUIData
{
	HWidget rootWidget;
	const char xmlFilePath[XML_UI_MAX_PATH];
	const char* xmlData;
	bool bLoaded;
	hAtlas atlas;
	VECTOR(struct WidgetVertex) pWidgetVertices;
	HUIVertexBuffer hVertexBuffer;
	int hViewModel; // reference to lua table
}XMLUIData;



void XMLUIGameLayer_Get(struct GameFrameworkLayer* pLayer, struct XMLUIGameLayerOptions* pOptions);


#endif
