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
#include "Atlas.h"


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

void AddNodeChildren(HWidget widget, struct xml_node* pNode, XMLUIData* pUIData)
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

		HWidget childWidget = pCtor(widget, pChild, pUIData);
		UI_AddChild(widget, childWidget);
		
		AddNodeChildren(childWidget, pChild, pUIData);
	}

}

void LoadAtlas(XMLUIData* pUIData, struct xml_node* child0)
{
	At_BeginAtlas();

	char attributeNameBuf[256];
	char spriteName[256];
	char spritePath[256];
	char numberBuf[256];

	int top = 0;
	int left = 0;
	int width = 0;
	int height = 0;
	int numChildren = xml_node_children(child0);
	for (int i = 0; i < numChildren; i++)
	{
		struct xml_node* pChild = xml_node_child(child0, i);
		int numAttributes = xml_node_attributes(pChild);
		bool bNameset = false;
		bool bPathSet = false;
		bool bTopSet = false;
		bool bLeftSet = false;
		bool bWidthSet = false;
		bool bHeightSet = false;
		bool bAllSet = true;

		for (int j = 0; j < numAttributes; j++)
		{
			struct xml_string* pAttr = xml_node_attribute_name(pChild, j);
			int attrNameLen = xml_string_length(pAttr);
			xml_string_copy(pAttr, attributeNameBuf, attrNameLen);
			attributeNameBuf[attrNameLen] = '\0';
			if (strcmp(attributeNameBuf, "source") == 0)
			{
				struct xml_string* pSrcVal = xml_node_attribute_content(pChild, j);
				int len = xml_string_length(pSrcVal);
				xml_string_copy(pSrcVal, spritePath, len);
				spritePath[len] = '\0';
				bPathSet = true;
			}
			else if (strcmp(attributeNameBuf, "name") == 0)
			{
				struct xml_string* pSrcVal = xml_node_attribute_content(pChild, j);
				int len = xml_string_length(pSrcVal);
				xml_string_copy(pSrcVal, spriteName, len);
				spriteName[len] = '\0';
				bNameset = true;
			}
			else if (strcmp(attributeNameBuf, "top") == 0)
			{
				struct xml_string* pSrcVal = xml_node_attribute_content(pChild, j);
				int len = xml_string_length(pSrcVal);
				xml_string_copy(pSrcVal, numberBuf, len);
				numberBuf[len] = '\0';
				top = atoi(numberBuf);
				bTopSet = true;
			}
			else if (strcmp(attributeNameBuf, "left") == 0)
			{
				struct xml_string* pSrcVal = xml_node_attribute_content(pChild, j);
				int len = xml_string_length(pSrcVal);
				xml_string_copy(pSrcVal, numberBuf, len);
				numberBuf[len] = '\0';
				left = atoi(numberBuf);
				bLeftSet = true;
			}
			else if (strcmp(attributeNameBuf, "width") == 0)
			{
				struct xml_string* pSrcVal = xml_node_attribute_content(pChild, j);
				int len = xml_string_length(pSrcVal);
				xml_string_copy(pSrcVal, numberBuf, len);
				numberBuf[len] = '\0';
				width = atoi(numberBuf);
				bWidthSet = true;
			}
			else if (strcmp(attributeNameBuf, "height") == 0)
			{
				struct xml_string* pSrcVal = xml_node_attribute_content(pChild, j);
				int len = xml_string_length(pSrcVal);
				xml_string_copy(pSrcVal, numberBuf, len);
				numberBuf[len] = '\0';
				height = atoi(numberBuf);
				bHeightSet = true;
			}
		}
		if (!bPathSet)
		{
			printf("%s atlas child %i path not set\n", __FUNCTION__, i);
			bAllSet = false;
		}
		if (!bNameset)
		{
			printf("%s atlas child %i name not set\n", __FUNCTION__, i);
			bAllSet = false;
		}
		if (!bTopSet)
		{
			printf("%s atlas child %i top not set\n", __FUNCTION__, i);
			bAllSet = false;
		}
		if (!bLeftSet)
		{
			printf("%s atlas child %i left not set\n", __FUNCTION__, i);
			bAllSet = false;
		}
		if (!bWidthSet)
		{
			printf("%s atlas child %i top not set\n", __FUNCTION__, i);
			bAllSet = false;
		}
		if (!bHeightSet)
		{
			printf("%s atlas child %i left not set\n", __FUNCTION__, i);
			bAllSet = false;
		}
		if (bAllSet)
		{
			At_AddSprite(spritePath, top, left, width, height, spriteName);
		}
	}

	pUIData->atlas = At_EndAtlas();
}

void LoadUIData(XMLUIData* pUIData)
{
	assert(!pUIData->bLoaded);
	FILE* fp = fopen(pUIData->xmlFilePath, "r");
	struct xml_document* pXMLDoc = xml_open_document(fp);
	
	char nodeNameArr[256];
	if (pXMLDoc)
	{
		struct xml_node* root = xml_document_root(pXMLDoc);
		int numchildren = xml_node_children(root);
		if (numchildren != 2)
		{
			printf("%s root should have 2 kids", __FUNCTION__);
			return;
		}
		struct xml_node* child0 = xml_node_child(root, 0);
		struct xml_node* child1 = xml_node_child(root, 1);
		struct xml_string* c0Name = xml_node_name(child0);
		struct xml_string* c1Name = xml_node_name(child1);
		int c0Length = xml_string_length(c0Name);
		int c1Length = xml_string_length(c1Name);
		
		bool bDoneAtlas = false;
		bool bDoneScreen = false;
		
		xml_string_copy(c0Name, nodeNameArr, c0Length);
		nodeNameArr[c0Length] = '\0';
		if (strcmp(nodeNameArr, "atlas") == 0)
		{
			bDoneAtlas = true;
			LoadAtlas(pUIData, child0);
		}
		else if (strcmp(nodeNameArr, "screen") == 0)
		{
			bDoneScreen = true;
			AddNodeChildren(pUIData->rootWidget, child0, pUIData);
		}

		xml_string_copy(c1Name, nodeNameArr, c1Length);
		nodeNameArr[c1Length] = '\0';
		if (strcmp(nodeNameArr, "atlas") == 0 && !bDoneAtlas)
		{
			bDoneAtlas = true;
			LoadAtlas(pUIData, child1);
		}
		else if (strcmp(nodeNameArr, "screen") == 0 && !bDoneScreen)
		{
			bDoneScreen = true;
			AddNodeChildren(pUIData->rootWidget, child1, pUIData);
		}


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










