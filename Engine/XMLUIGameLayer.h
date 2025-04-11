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

typedef HWidget(*AddChildFn)(HWidget hParent, struct xml_node* pXMLNode);

void XMLUIGameLayer_Get(struct GameFrameworkLayer* pLayer, const struct XMLUIGameLayerOptions* pOptions);


#endif