#ifndef XMLUI_GAME_LAYER_H
#define XMLUI_GAME_LAYER_H
#include <stdbool.h>
#include "HandleDefs.h"

struct xml_node;

struct XMLUIGameLayerOptions
{
	const char* xmlPath;
	bool bLoadImmediately; // eg, when XMLUIGameLayer_Get is called, do we load the xml file?
};

typedef HWidget(*AddChildFn)(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData);

#define XML_UI_MAX_PATH 256

typedef struct XMLUIData
{
	int rootWidget;
	const char xmlFilePath[XML_UI_MAX_PATH];
	const char* xmlData;
	bool bLoaded;
	hAtlas atlas;
}XMLUIData;



void XMLUIGameLayer_Get(struct GameFrameworkLayer* pLayer, const struct XMLUIGameLayerOptions* pOptions);


#endif