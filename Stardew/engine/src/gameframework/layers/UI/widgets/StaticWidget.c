#include "StaticWidget.h"
#include "XMLUIGameLayer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Atlas.h"
#include "AssertLib.h"
#include "WidgetVertexOutputHelpers.h"
#include <libxml/tree.h>



float StaticWidget_GetWidth(struct StaticWidgetData* pStaticData, struct WidgetPadding* pPadding)
{
	AtlasSprite* pAtlasSprite = At_GetSprite(pStaticData->sprite, pStaticData->atlas);
	if (!pAtlasSprite)
	{
		return 0;
	}
	return pAtlasSprite->widthPx * pStaticData->scale.scaleX + pPadding->paddingLeft + pPadding->paddingRight;
}

float StaticWidget_GetHeight(struct StaticWidgetData* pStaticData, struct WidgetPadding* pPadding)
{
	AtlasSprite* pAtlasSprite = At_GetSprite(pStaticData->sprite, pStaticData->atlas);
	if (!pAtlasSprite)
	{
		return 0;
	}

	return pAtlasSprite->heightPx * pStaticData->scale.scaleY + pPadding->paddingTop + pPadding->paddingBottom;
}

void StaticWidget_Destroy(struct StaticWidgetData* pStaticData)
{
	assert(pStaticData->imageName);
	free(pStaticData->imageName);
}

void* StaticWidget_OnOutputVerts(struct StaticWidgetData* pStaticData, float left, float top, struct WidgetPadding* pPadding, VECTOR(struct WidgetVertex) pOutVerts)
{
	AtlasSprite* pSprite = At_GetSprite(pStaticData->sprite, pStaticData->atlas);
	float width = pSprite->widthPx;
	float height = pSprite->heightPx;

	struct WidgetQuad quad;
	vec2 translate = {
		left + pPadding->paddingLeft,
		top + pPadding->paddingTop
	};
	PopulateWidgetQuadWholeSprite(&quad, pSprite);
	TranslateWidgetQuad(translate, &quad);
	ScaleWidgetQuad(pStaticData->scale.scaleX, pStaticData->scale.scaleY, &quad);
	pOutVerts = OutputWidgetQuad(pOutVerts, &quad);
	return pOutVerts;
}

void StaticWidget_MakeFromXML(struct StaticWidgetData* pWidgetData, xmlNode* pXMLNode, struct XMLUIData* pUILayerData)
{
	xmlChar* attribute = NULL;
	if(attribute = xmlGetProp(pXMLNode, "sprite"))
	{
		int len = strlen(attribute);
		if (pWidgetData->imageName)
		{
			free(pWidgetData->imageName);
		}
		pWidgetData->imageName = malloc(len + 1);
		strcpy(pWidgetData->imageName, attribute);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pXMLNode, "scaleX"))
	{
		pWidgetData->scale.scaleX = atof(attribute);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pXMLNode, "scaleY"))
	{
		pWidgetData->scale.scaleY = atof(attribute);
		xmlFree(attribute);
	}
	if (pWidgetData->imageName)
	{
		pWidgetData->sprite = At_FindSprite(pWidgetData->imageName, pUILayerData->atlas);
		pWidgetData->atlas = pUILayerData->atlas;
	}
}

static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct StaticWidgetData* pStaticData = pWidget->pImplementationData;
	return StaticWidget_GetWidth(pStaticData, &pWidget->padding);
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct StaticWidgetData* pStaticData = pWidget->pImplementationData;
	return StaticWidget_GetHeight(pStaticData, &pWidget->padding);
}

static void LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	// shouldnt really have children
	assert(pWidget->hFirstChild == NULL_HWIDGET);
}

static void OnDestroy(struct UIWidget* pWidget)
{
	StaticWidget_Destroy(pWidget->pImplementationData);
	free(pWidget->pImplementationData);
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
	pOutVerts = StaticWidget_OnOutputVerts(pStaticData, pWidget->left, pWidget->top, &pWidget->padding, pOutVerts);
	pOutVerts = UI_Helper_OnOutputVerts(pWidget, pOutVerts);
	return pOutVerts;
}

static void MakeWidgetIntoStatic(HWidget hWidget, xmlNode* pXMLNode, struct XMLUIData* pUILayerData)
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
	pWidget->fnOutputVertices = &OnOutputVerts;
	pWidget->pImplementationData = malloc(sizeof(struct StaticWidgetData));
	struct StaticWidgetData* pWidgetData = pWidget->pImplementationData;
	memset(pWidgetData, 0, sizeof(struct StaticWidgetData));

	pWidgetData->scale.scaleX = 1.0f;
	pWidgetData->scale.scaleY = 1.0f;
	StaticWidget_MakeFromXML(pWidgetData, pXMLNode, pUILayerData);
}

HWidget StaticWidgetNew(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoStatic(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}
