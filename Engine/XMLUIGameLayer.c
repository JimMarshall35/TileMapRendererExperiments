#include "XMLUIGameLayer.h"
#include "DrawContext.h"
#include "InputContext.h"
#include "GameFramework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "IntTypes.h"
#include "DynArray.h"
#include "ObjectPool.h"
#include "Widget.h"
#include "xml.h"

#define XML_UI_MAX_PATH 256


typedef struct
{
	int rootWidget;
	const char xmlFilePath[XML_UI_MAX_PATH];
	const char* xmlData;
	bool bLoaded;
}XMLUIData;




static void Update(struct GameFrameworkLayer* pLayer, float deltaT)
{

}

static void Draw(struct GameFrameworkLayer* pLayer, DrawContext* dc)
{

}

static void Input(struct GameFrameworkLayer* pLayer, InputContext* ctx)
{

}



static void OnPush(struct GameFrameworkLayer* pLayer, DrawContext* drawContext, InputContext* inputContext)
{
	
}



static void OnPop(struct GameFrameworkLayer* pLayer, DrawContext* drawContext, InputContext* inputContext)
{
	
}

void LoadUIData(XMLUIData* pUIData)
{
	assert(!pUIData->bLoaded);
	FILE* fp = fopen(pUIData->xmlFilePath, "r");
	struct xml_document* pXMLDoc = xml_open_document(fp);
	
	if (pXMLDoc)
	{
		struct xml_node* root = xml_document_root(pXMLDoc);
		size_t children = xml_node_children(root);
		for (int i = 0; i < children; i++)
		{
			struct xml_node* child = xml_node_child(root, i);
		}
		printf("pXMLDoc\n");
	}
}


void XMLUIGameLayer_Get(struct GameFrameworkLayer* pLayer, const struct XMLUIGameLayerOptions* pOptions)
{
	pLayer->userData = malloc(sizeof(XMLUIData));
	if (!pLayer->userData) { printf("XMLUIGameLayer_Get: no memory"); return; }
	XMLUIData* pUIData = (XMLUIData*)pLayer->userData;
	
	memset(pLayer->userData, 0, sizeof(XMLUIData));
	
	strcpy(pUIData->xmlFilePath, pOptions->xmlPath);
	pUIData->rootWidget = NewRootWidget();
	pLayer->draw = &Draw;
	pLayer->update = &Update;
	pLayer->input = &Input;
	pLayer->onPop = &OnPop;
	pLayer->onPush = &OnPush;
	pLayer->flags = 0;
	pLayer->flags |= EnableDrawFn | EnableInputFn | EnableUpdateFn | EnableOnPop | EnableOnPush;
	if (pOptions->bLoadImmediately)
	{
		LoadUIData(pUIData);
	}
}










