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
#include "Widget.h"
#include "StaticWidget.h"
#include "StackPanelWidget.h"

#define XML_UI_MAX_PATH 256


typedef struct
{
	int rootWidget;
	const char xmlFilePath[XML_UI_MAX_PATH];
	const char* xmlData;
	bool bLoaded;
}XMLUIData;


struct NameConstructorPair
{
	const char* name;
	AddChildFn pCtor;
};

struct NameConstructorPair gNodeNameTable[] =
{
	{"stackpanel", &StackPanelWidgetNew},
	{"static",     &StaticWidgetNew}
};

AddChildFn LookupWidgetCtor(const char* widgetName)
{
	for (int i = 0; sizeof(gNodeNameTable) / sizeof(struct NameConstructorPair); i++)
	{
		if (strcmp(widgetName, gNodeNameTable[i].name) == 0)
		{
			return gNodeNameTable[i].pCtor;
		}
	}
	return NULL;
}

HWidget GetWidgetFromNode(struct xml_node* pNode)
{

}


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

void AddNodeChildren(HWidget widget, struct xml_node* pNode)
{
	size_t children = xml_node_children(pNode);
	for (int i = 0; i < children; i++)
	{
		struct xml_node* pChild = xml_node_child(pNode, i);
		struct xml_string* pString = xml_node_name(pChild);
		char* pBuf =  malloc(xml_string_length(pString) + 1);
		memset(pBuf, 0, xml_string_length(pString) + 1);
		xml_string_copy(pString, pBuf, xml_string_length(pString));

		AddChildFn pCtor = LookupWidgetCtor(pBuf);
		if (!pCtor)
		{
			// log error
			return;
		}

		HWidget childWidget = pCtor(widget, pChild);
		UI_AddChild(widget, childWidget);
		
		AddNodeChildren(childWidget, pChild);
	}

}


void LoadUIData(XMLUIData* pUIData)
{
	assert(!pUIData->bLoaded);
	FILE* fp = fopen(pUIData->xmlFilePath, "r");
	struct xml_document* pXMLDoc = xml_open_document(fp);
	
	if (pXMLDoc)
	{
		struct xml_node* root = xml_document_root(pXMLDoc);
		AddNodeChildren(pUIData->rootWidget, root);
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










