#ifndef XMLUI_GAME_LAYER_H
#define XMLUI_GAME_LAYER_H

#include <libxml/tree.h>


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "HandleDefs.h"
#include "DynArray.h"
#include "Widget.h"
#include "DrawContext.h"
#include "TimerPool.h"

struct XMLUIData;
struct GameFrameworkLayer;
struct DataNode;

struct XMLUIGameLayerOptions
{
	DrawContext* pDc;
	const char* xmlPath;
	bool bLoadImmediately; // eg, when XMLUIGameLayer_Get is called, do we load the xml file?
};

typedef HWidget(*AddChildFn)(HWidget hParent, struct DataNode* pDataNode, struct XMLUIData* pUILayerData);

#define XML_UI_MAX_PATH 256
#define MAX_FOCUSED_WIDGETS 16


/*
	its a "request" because if an ancenstor of the requesting widget 
	also requests then the request is overruled
*/
struct WidgetChildrenChangeRequest
{
	/* Registry index of the viewmodel table the function will be called on */
	int regIndex;

	/* Function that will generate new children for the widget */
	const char* funcName;

	/* Widget that will have new children */
	HWidget hWidget;
};


typedef struct XMLUIData
{
	HWidget rootWidget;
	char xmlFilePath[XML_UI_MAX_PATH];
	const char* xmlData;
	bool bLoaded;
	hAtlas atlas;
	VECTOR(WidgetVertex) pWidgetVertices;
	HUIVertexBuffer hVertexBuffer;
	int hViewModel; // reference to lua table
	HWidget focusedWidgets[MAX_FOCUSED_WIDGETS];
	int nFocusedWidgets;
	struct SDTimerPool timerPool;
	VECTOR(struct WidgetChildrenChangeRequest) pChildrenChangeRequests;
}XMLUIData;



void XMLUIGameLayer_Get(struct GameFrameworkLayer* pLayer, struct XMLUIGameLayerOptions* pOptions);

#ifdef __cplusplus
}
#endif

#endif
