#ifndef XMLUI_GAME_LAYER_H
#define XMLUI_GAME_LAYER_H
#include <stdbool.h>

struct XMLUIGameLayerOptions
{
	const char* xmlPath;
	bool bLoadImmediately; // eg, when XMLUIGameLayer_Get is called, do we load the xml file?
};

void XMLUIGameLayer_Get(struct GameFrameworkLayer* pLayer, const struct XMLUIGameLayerOptions* pOptions);


#endif