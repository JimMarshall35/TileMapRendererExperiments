#include "StaticWidget.h"
#include "Widget.h"
#include "xml.h"
#include "XMLUIGameLayer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Atlas.h"
#include "AssertLib.h"

struct StaticWidgetData
{
	char* imageName;
	struct WidgetScale scale;
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
	return pAtlasSprite->widthPx * pStaticData->scale.scaleX + pWidget->padding.paddingLeft + pWidget->padding.paddingRight;
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct StaticWidgetData* pStaticData = pWidget->pImplementationData;
	AtlasSprite* pAtlasSprite = At_GetSprite(pStaticData->sprite, pStaticData->atlas);
	if (!pAtlasSprite)
	{
		return 0;
	}
	return pAtlasSprite->heightPx * pStaticData->scale.scaleY + pWidget->padding.paddingTop + pWidget->padding.paddingBottom;
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

static void OnDebugPrint(int indentLvl, struct UIWidget* pWidget, PrintfFn printfFn)
{

	struct StaticWidgetData* pStaticData = pWidget->pImplementationData;
	for (int i = 0; i < indentLvl; i++)
	{
		printfFn("\t");
	}
	printfFn("Static. imageName = %s, scaleX = %.2f, scaleY = %.2f, sprite = %i, atlas = %i, ",
		pStaticData->imageName,
		pStaticData->scale.scaleX,
		pStaticData->scale.scaleY,
		pStaticData->sprite,
		pStaticData->atlas);
	UI_DebugPrintCommonWidgetInfo(pWidget, printfFn);
	printfFn("\n");
}

static void* OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	struct StaticWidgetData* pStaticData = pWidget->pImplementationData;
	AtlasSprite* pSprite = At_GetSprite(pStaticData->sprite, pStaticData->atlas);
	float width = pSprite->widthPx;//GetWidth(pWidget, UI_GetWidget(pWidget->hParent));
	float height = pSprite->heightPx;//GetHeight(pWidget, UI_GetWidget(pWidget->hParent));

	struct WidgetVertex v;
	v.r = 1.0f; v.g = 1.0f; v.b = 1.0f; v.a = 1.0f;
	
	// topleft
	v.x = pWidget->left;
	v.y = pWidget->top;
	v.u = pSprite->topLeftUV_U;
	v.v = pSprite->topLeftUV_V;
	pOutVerts = VectorPush(pOutVerts, &v);
	
	// topRight
	v.x = pWidget->left + width * pStaticData->scale.scaleX;
	v.y = pWidget->top;
	v.u = pSprite->bottomRightUV_U;
	v.v = pSprite->topLeftUV_V;
	pOutVerts = VectorPush(pOutVerts, &v);

	// bottomLeft
	v.x = pWidget->left;
	v.y = pWidget->top + height * pStaticData->scale.scaleY;
	v.u = pSprite->topLeftUV_U;
	v.v = pSprite->bottomRightUV_V;
	pOutVerts = VectorPush(pOutVerts, &v);

	// TRIANGLE 2

	// topRight
	v.x = pWidget->left + width * pStaticData->scale.scaleX;
	v.y = pWidget->top;
	v.u = pSprite->bottomRightUV_U;
	v.v = pSprite->topLeftUV_V;
	pOutVerts = VectorPush(pOutVerts, &v);

	// bottomRight
	v.x = pWidget->left + width * pStaticData->scale.scaleX;
	v.y = pWidget->top + height * pStaticData->scale.scaleY;
	v.u = pSprite->bottomRightUV_U;
	v.v = pSprite->bottomRightUV_V;
	pOutVerts = VectorPush(pOutVerts, &v);

	// bottomLeft
	v.x = pWidget->left;
	v.y = pWidget->top + height * pStaticData->scale.scaleY;
	v.u = pSprite->topLeftUV_U;
	v.v = pSprite->bottomRightUV_V;
	pOutVerts = VectorPush(pOutVerts, &v);

	pOutVerts = UI_Helper_OnOutputVerts(pWidget, pOutVerts);

	return pOutVerts;
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
	pWidget->fnOnDebugPrint = &OnDebugPrint;
	pWidget->fnOutputVertices = &OnOutputVerts;
	pWidget->pImplementationData = malloc(sizeof(struct StaticWidgetData));
	memset(pWidget->pImplementationData, 0, sizeof(struct StaticWidgetData));
	struct StaticWidgetData* pWidgetData = pWidget->pImplementationData;
	memset(pWidgetData, 0, sizeof(struct StaticWidgetData));
	pWidgetData->scale.scaleX = 1.0f;
	pWidgetData->scale.scaleY = 1.0f;

	pWidgetData->atlas = pUILayerData->atlas;
	size_t numAttributes = xml_node_attributes(pXMLNode);
	char attributeNameBuffer[128];
	char attributeContentBuffer[128];
	for (int i = 0; i < numAttributes; i++)
	{
		struct xml_string* name = xml_node_attribute_name(pXMLNode, i);
		struct xml_string* content = xml_node_attribute_content(pXMLNode, i);
		int nameLen = xml_string_length(name);
		int contentLen = xml_string_length(content);
		if (nameLen >= 128)
		{
			printf("function %s, namelen > 128. namelen is %i", __FUNCTION__, nameLen);
			continue;
		}
		if (contentLen >= 128)
		{
			printf("function %s, content > 128. contentlen is %i", __FUNCTION__, nameLen);
			continue;
		}
		xml_string_copy(name, attributeNameBuffer, nameLen);
		attributeNameBuffer[nameLen] = '\0';
		xml_string_copy(content, attributeContentBuffer, contentLen);
		attributeContentBuffer[contentLen] = '\0';
		if (strcmp(attributeNameBuffer, "sprite") == 0)
		{
			struct xml_string* contents = xml_node_attribute_content(pXMLNode, i);
			pWidgetData->imageName = malloc(xml_string_length(contents) + 1);
			xml_string_copy(contents, pWidgetData->imageName, xml_string_length(contents));
			pWidgetData->imageName[xml_string_length(contents)] = '\0';
			pWidgetData->sprite = At_FindSprite(pWidgetData->imageName, pWidgetData->atlas);
		}
		else if (strcmp(attributeNameBuffer, "scaleX") == 0)
		{
			pWidgetData->scale.scaleX = atof(attributeContentBuffer);
		}
		else if (strcmp(attributeNameBuffer, "scaleY") == 0)
		{
			pWidgetData->scale.scaleY = atof(attributeContentBuffer);
		}
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
