#include "BackgroundBoxWidget.h"
#include "xml.h"
#include "XMLUIGameLayer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "WidgetVertexOutputHelpers.h"
#include "AssertLib.h"

struct BackgroundBoxWidgetData
{
	char* imageName;
	struct WidgetScale scale;
	hSprite sprite;
	hAtlas atlas;
};

static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	EASSERT(UI_CountWidgetChildrenPtr(pWidget) == 1);
	if (pWidget->hFirstChild != NULL_HWIDGET)
	{
		struct UIWidget* pFirstChild = UI_GetWidget(pWidget->hFirstChild);
		return pFirstChild->fnGetWidth(pFirstChild, pWidget);
	}
	return 0.0f;
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	EASSERT(UI_CountWidgetChildrenPtr(pWidget) == 1);
	if (pWidget->hFirstChild != NULL_HWIDGET)
	{
		struct UIWidget* pFirstChild = UI_GetWidget(pWidget->hFirstChild);
		return pFirstChild->fnGetHeight(pFirstChild, pWidget);
	}
	return 0.0f;
}

static void LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct UIWidget* pChild = UI_GetWidget(pWidget->hFirstChild);
	if (pChild)
	{
		pChild->top = pWidget->top;
		pChild->left = pWidget->left;
	}
	pChild->fnLayoutChildren(pChild, pWidget);
}

static void OnDestroy(struct UIWidget* pWidget)
{
	free(pWidget->pImplementationData);
}



static void OnDebugPrint(int indentLvl, struct UIWidget* pWidget, PrintfFn printfFn)
{
}

static void* OnOutputVerts_3StripsHorizontal(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	return pOutVerts;
}

static void* OnOutputVerts_3StripsVertical(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	return pOutVerts;
}



static void* OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	struct BackgroundBoxWidgetData* pBBoxData = pWidget->pImplementationData;
	AtlasSprite* pAtlasSprite = At_GetSprite(pBBoxData->sprite, pBBoxData->atlas);
	const struct WidgetScale* pScale = &pBBoxData->scale;
	float widgetWidth = GetWidth(pWidget, NULL);
	float widgetHeight = GetHeight(pWidget, NULL);

	if (widgetWidth <= pAtlasSprite->widthPx * pBBoxData->scale.scaleX && widgetHeight > pAtlasSprite->heightPx * pBBoxData->scale.scaleY)
	{
		return OnOutputVerts_3StripsHorizontal(pWidget, pOutVerts);
	}
	else if (widgetWidth > pAtlasSprite->widthPx * pBBoxData->scale.scaleX && widgetHeight <= pAtlasSprite->heightPx * pBBoxData->scale.scaleY)
	{
		return OnOutputVerts_3StripsVertical(pWidget, pOutVerts);
	}

	// widget is bigger in both dimensions than the sprite - do 9 panel scaling.
	/*  
	 https://en.wikipedia.org/wiki/9-slice_scaling#:~:text=9%2Dslice%20scaling%20(also%20known,a%20grid%20of%20nine%20parts.
	                        ________
         _ _ _             |_|____|_|
		|_|_|_|   ->       | |    | |
	    |_|_|_|            | |    | |
		|_|_|_|            |_|____|_|
	                       |_|____|_|
	
	
	*/
	struct WidgetQuad quads[9]; // in row first order, ie topleft, topmcentre, topright, middleleft, middlecentre, ect...
	float widthOver3 = (float)pAtlasSprite->widthPx / 3.0f;
	float heightOver3 = (float)pAtlasSprite->heightPx / 3.0f;
	vec2 tl, br;

	tl[0] = 0;
	tl[1] = 0;
	br[0] = widthOver3;
	br[1] = heightOver3;
	int i = 0;
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			PopulateWidgetQuad(&quads[i++], pAtlasSprite, tl, br);

			tl[0] += widthOver3;
			br[0] += widthOver3;
		}
		tl[0] = 0;
		br[0] = widthOver3;
		tl[1] += heightOver3;
		br[1] += heightOver3;
	}

	vec2 translation = { pWidget->left, pWidget->top};
	for (int i = 0; i < 9; i++)
	{
		TranslateWidgetQuad(translation, &quads[i]);
	}

	vec2 size = {
		widthOver3  * pScale->scaleX,
		heightOver3 * pScale->scaleY,

	};
	SizeWidgetQuad(size, &quads[0]);
	
	// top row
	translation[0] = size[0];
	translation[1] = 0.0f;
	TranslateWidgetQuad(translation, &quads[1]);
	float borderPieceNewWidth = widgetWidth - size[0] * 2;
	vec2 horizontalBorderSize = { borderPieceNewWidth, size[1]};
	SizeWidgetQuad(horizontalBorderSize, &quads[1]);

	translation[0] = size[0] + borderPieceNewWidth;
	translation[1] = 0.0f;
	TranslateWidgetQuad(translation, &quads[2]);
	SizeWidgetQuad(size, &quads[2]);

	// middle row
	translation[0] = 0.0f;
	translation[1] = size[1];
	TranslateWidgetQuad(translation, &quads[3]);
	float borderPieceNewHeight = widgetHeight - size[1] * 2;
	vec2 verticalBorderSize = { size[0], borderPieceNewHeight};
	SizeWidgetQuad(verticalBorderSize, &quads[3]);

	translation[0] = size[0];
	translation[1] = size[1];
	TranslateWidgetQuad(translation, &quads[4]);
	vec2 middlePieceSize = { borderPieceNewWidth, borderPieceNewHeight };
	SizeWidgetQuad(middlePieceSize, &quads[4]);

	translation[0] = size[0] + borderPieceNewWidth;
	translation[1] = size[1];
	TranslateWidgetQuad(translation, &quads[5]);
	SizeWidgetQuad(verticalBorderSize, &quads[5]);

	// bottom row
	translation[0] = 0.0f;
	translation[1] = size[1] + borderPieceNewHeight;
	TranslateWidgetQuad(translation, &quads[6]);
	SizeWidgetQuad(size, &quads[6]);

	translation[0] = size[0];
	translation[1] = size[1] + borderPieceNewHeight;
	TranslateWidgetQuad(translation, &quads[7]);
	SizeWidgetQuad(horizontalBorderSize, &quads[7]);

	translation[0] = size[0] + borderPieceNewWidth;
	translation[1] = size[1] + borderPieceNewHeight;
	TranslateWidgetQuad(translation, &quads[8]);
	SizeWidgetQuad(size, &quads[8]);

	for (int i = 0; i < 9; i++)
	{
		pOutVerts = OutputWidgetQuad(pOutVerts, &quads[i]);
	}

	pOutVerts = UI_Helper_OnOutputVerts(pWidget, pOutVerts);
	return pOutVerts;
}

static void MakeWidgetIntoBackgroundBoxWidget(HWidget hWidget, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
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
	pWidget->pImplementationData = malloc(sizeof(struct BackgroundBoxWidgetData));
	memset(pWidget->pImplementationData, 0, sizeof(struct BackgroundBoxWidgetData));

	struct BackgroundBoxWidgetData* pWidgetData = pWidget->pImplementationData;

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

HWidget BackgroundBoxWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoBackgroundBoxWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}
