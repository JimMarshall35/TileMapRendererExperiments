#include "StaticWidget.h"
#include "Widget.h"
#include "xml.h"
#include "XMLUIGameLayer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Atlas.h"
#include <assert.h>

struct StaticWidgetData
{
	char* imageName;
	struct WidgetPadding padding;
	struct WidgetScale scale;
	/*struct WidgetDim width;
	struct WidgetDim height;*/
	hSprite sprite;
	hAtlas atlas;
};

static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct StaticWidgetData* pStaticData = pWidget->pImplementationData;
	AtlasSprite* pAtlasSprite = At_GetSprite(pStaticData->sprite, pStaticData->atlas);
	if (!pAtlasSprite)
	{
		return 0;
	}
	return pAtlasSprite->widthPx * pStaticData->scale.scaleX + pStaticData->padding.paddingLeft + pStaticData->padding.paddingRight;
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct StaticWidgetData* pStaticData = pWidget->pImplementationData;
	AtlasSprite* pAtlasSprite = At_GetSprite(pStaticData->sprite, pStaticData->atlas);
	if (!pAtlasSprite)
	{
		return 0;
	}
	return pAtlasSprite->heightPx * pStaticData->scale.scaleY + pStaticData->padding.paddingTop + pStaticData->padding.paddingBottom;
}

static void LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	// shouldnt really have children
	assert(pWidget->hFirstChild == NULL_HWIDGET);
}

static void OnDestroy(struct UIWidget* pWidget)
{
	struct StaticWidgetData* pStaticData = pWidget->pImplementationData;
	assert(pStaticData->imageName);
	free(pStaticData->imageName);
	free(pStaticData);
}


static void MakeWidgetIntoStatic(HWidget hWidget, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	struct UIWidget* pWidget = UI_GetWidget(hWidget);

	pWidget->hNext = -1;
	pWidget->hPrev = -1;
	pWidget->hParent = -1;
	pWidget->hFirstChild = -1;
	pWidget->fnGetHeight = &GetHeight;
	pWidget->fnGetWidth = &GetWidth;
	pWidget->fnLayoutChildren = &LayoutChildren;
	pWidget->fnOnDestroy = &OnDestroy;
	pWidget->pImplementationData = malloc(sizeof(struct StaticWidgetData));
	memset(pWidget->pImplementationData, 0, sizeof(struct StaticWidgetData));
	struct StaticWidgetData* pWidgetData = pWidget->pImplementationData;
	memset(pWidgetData, 0, sizeof(struct StaticWidgetData));
	size_t numAttributes = xml_node_attributes(pXMLNode);
	char attributeBuffer[256];
	for (int i = 0; i < numAttributes; i++)
	{
		struct xml_string* name = xml_node_attribute_name(pXMLNode, i);
		int nameLen = xml_string_length(name);
		if (nameLen >= 256)
		{
			printf("function %s, namelen > 256. namelen is %i", __FUNCTION__, nameLen);
			continue;
		}
		memset(attributeBuffer, 0, 256);
		xml_string_copy(name, attributeBuffer, nameLen);
		if (strcmp(attributeBuffer, "sprite") == 0)
		{
			struct xml_string* contents = xml_node_attribute_content(pXMLNode, i);
			pWidgetData->imageName = malloc(xml_string_length(contents) + 1);
			xml_string_copy(contents, pWidgetData->imageName, xml_string_length(contents));
			pWidgetData->imageName[xml_string_length(contents)] = '\0';
		}
		//else if (strcmp(attributeBuffer, "width"))
		//{
		//	struct xml_string* contents = xml_node_attribute_content(pXMLNode, i);
		//	UI_ParseWidgetDimsAttribute(contents, &pWidgetData->width);

		//}
		//else if (strcmp(attributeBuffer, "height"))
		//{
		//	struct xml_string* contents = xml_node_attribute_content(pXMLNode, i);
		//	UI_ParseWidgetDimsAttribute(contents, &pWidgetData->height);
		//}
	}
	if (pWidgetData->imageName)
	{
		pWidgetData->sprite = At_FindSprite(pWidgetData->imageName, pUILayerData->atlas);
		pWidgetData->atlas = pUILayerData->atlas;
	}
}

HWidget StaticWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoStatic(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}
