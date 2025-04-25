#include "BackgroundBoxWidget.h"
#include "xml.h"
#include "XMLUIGameLayer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "WidgetVertexOutputHelpers.h"
#include "AssertLib.h"
#include "Scripting.h"



static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	EASSERT(UI_CountWidgetChildrenPtr(pWidget) == 1);
	if (pWidget->hFirstChild != NULL_HWIDGET)
	{
		struct UIWidget* pFirstChild = UI_GetWidget(pWidget->hFirstChild);
		return pFirstChild->fnGetWidth(pFirstChild, pWidget) + pWidget->padding.paddingLeft + pWidget->padding.paddingRight;
	}
	return 0.0f;
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	EASSERT(UI_CountWidgetChildrenPtr(pWidget) == 1);
	if (pWidget->hFirstChild != NULL_HWIDGET)
	{
		struct UIWidget* pFirstChild = UI_GetWidget(pWidget->hFirstChild);
		return pFirstChild->fnGetHeight(pFirstChild, pWidget) + pWidget->padding.paddingTop + pWidget->padding.paddingBottom;
	}
	return 0.0f;
}

static void LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct UIWidget* pChild = UI_GetWidget(pWidget->hFirstChild);
	if (pChild)
	{
		pChild->top = pWidget->top + pWidget->padding.paddingTop;
		pChild->left = pWidget->left + pWidget->padding.paddingLeft;
		pChild->fnLayoutChildren(pChild, pWidget);
	}
}

static void OnDestroy(struct UIWidget* pWidget)
{
	free(pWidget->pImplementationData);
}



static void OnDebugPrint(int indentLvl, struct UIWidget* pWidget, PrintfFn printfFn)
{
}

void* OnOutputVerts_3StripsHorizontal(
	struct BackgroundBoxWidgetData* pBBoxData,
	VECTOR(struct WidgetVertex) pOutVerts,
	float totalWidth,
	float totalHeight,
	const struct WidgetPadding* padding,
	float left, float top
)
{
	AtlasSprite* pAtlasSprite = At_GetSprite(pBBoxData->sprite, pBBoxData->atlas);
	const struct WidgetScale* pScale = &pBBoxData->scale;
	struct WidgetQuad quads[3]; // in row first order, ie topleft, topmcentre, topright, middleleft, middlecentre, ect...
	float height = (float)pAtlasSprite->heightPx;
	float widthOver3 = (float)pAtlasSprite->widthPx / 3.0f;
	float widgetWidth = totalWidth - padding->paddingLeft - padding->paddingRight;//GetWidth(pWidget, NULL) - pWidget->padding.paddingLeft - pWidget->padding.paddingRight;
	float widgetHeight = totalHeight - padding->paddingTop - padding->paddingBottom;

	vec2 tl, br;
	tl[0] = 0.0f;
	tl[1] = 0.0f;
	br[0] = widthOver3;
	br[1] = height;
	for (int row = 0; row < 3; row++)
	{
		PopulateWidgetQuad(&quads[row], pAtlasSprite, tl, br);
		tl[0] += widthOver3;
		br[0] += widthOver3;
	}

	vec2 translation = { left + padding->paddingLeft, top + padding->paddingTop };
	for (int i = 0; i < 3; i++)
	{
		TranslateWidgetQuad(translation, &quads[i]);
	}

	vec2 size = {
		widthOver3 * pScale->scaleX,
		height * pScale->scaleY,

	};
	SizeWidgetQuad(size, &quads[0]);
	SizeWidgetQuad(size, &quads[1]);
	SizeWidgetQuad(size, &quads[2]);

	quads[1].v[VL_TR].x += widgetWidth - size[0];
	quads[1].v[VL_BR].x += widgetWidth - size[0];
	quads[1].v[VL_TL].x += size[0];
	quads[1].v[VL_BL].x += size[0];

	translation[1] = 0.0f;
	translation[0] = widgetWidth - size[0];
	TranslateWidgetQuad(translation, &quads[2]);
	for (int i = 0; i < 3; i++)
	{
		pOutVerts = OutputWidgetQuad(pOutVerts, &quads[i]);
	}
	return pOutVerts;
}

void* OnOutputVerts_3StripsVertical(
	struct BackgroundBoxWidgetData* pBBoxData,
	VECTOR(struct WidgetVertex) pOutVerts,
	float totalWidth,
	float totalHeight,
	const struct WidgetPadding* padding,
	float left, float top
)
{
	return pOutVerts;
}

void* OutputBackgroundBoxVerts(
	struct BackgroundBoxWidgetData* pBBoxData,
	VECTOR(struct WidgetVertex) pOutVerts,
	float totalWidth,
	float totalHeight,
	const struct WidgetPadding* padding,
	float left, float top
)
{
	AtlasSprite* pAtlasSprite = At_GetSprite(pBBoxData->sprite, pBBoxData->atlas);
	const struct WidgetScale* pScale = &pBBoxData->scale;
	float widgetWidth = totalWidth - padding->paddingLeft - padding->paddingRight;//GetWidth(pWidget, NULL) - pWidget->padding.paddingLeft - pWidget->padding.paddingRight;
	float widgetHeight = totalHeight - padding->paddingTop - padding->paddingBottom;

	if (widgetWidth <= pAtlasSprite->widthPx * pBBoxData->scale.scaleX && widgetHeight > pAtlasSprite->heightPx * pBBoxData->scale.scaleY)
	{
		return OnOutputVerts_3StripsVertical(pBBoxData, pOutVerts, totalWidth, totalHeight, padding, left, top);
	}
	else if (widgetWidth > pAtlasSprite->widthPx * pBBoxData->scale.scaleX && widgetHeight <= pAtlasSprite->heightPx * pBBoxData->scale.scaleY)
	{
		return OnOutputVerts_3StripsHorizontal(pBBoxData, pOutVerts, totalWidth, totalHeight, padding, left, top);
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

	vec2 translation = { left + padding->paddingLeft, top + padding->paddingTop };
	for (int i = 0; i < 9; i++)
	{
		TranslateWidgetQuad(translation, &quads[i]);
	}

	vec2 size = {
		widthOver3 * pScale->scaleX,
		heightOver3 * pScale->scaleY,

	};
	SizeWidgetQuad(size, &quads[0]);

	// top row
	translation[0] = size[0];
	translation[1] = 0.0f;
	TranslateWidgetQuad(translation, &quads[1]);
	float borderPieceNewWidth = widgetWidth - size[0] * 2;
	vec2 horizontalBorderSize = { borderPieceNewWidth, size[1] };
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
	vec2 verticalBorderSize = { size[0], borderPieceNewHeight };
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

	return pOutVerts;
}

static void* OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	float totalW = GetWidth(pWidget, NULL);
	float totalH = GetHeight(pWidget, NULL);
	pOutVerts = OutputBackgroundBoxVerts(
		pWidget->pImplementationData,
		pOutVerts,
		totalW,
		totalH,
		&pWidget->padding,
		pWidget->left,
		pWidget->top
	);
	pOutVerts = UI_Helper_OnOutputVerts(pWidget, pOutVerts);
}


static void OnPropertyChanged(struct UIWidget* pThisWidget, struct WidgetPropertyBinding* pBinding)
{
	struct BackgroundBoxWidgetData* pData = pThisWidget->pImplementationData;
	if (strcmp(pBinding->boundPropertyName, "sprite") == 0)
	{
		char* fnName = UI_MakeBindingGetterFunctionName(pBinding->name);
		Sc_CallFuncInRegTableEntryTable(pThisWidget->scriptCallbacks.viewmodelTable, fnName, NULL, 0, 1);
		EASSERT(pData->imageName);
		free(pData->imageName);
		size_t len = Sc_StackTopStringLen();
		pData->imageName = malloc(len + 1);
		Sc_StackTopStrCopy(pData->imageName);
		pData->sprite = At_FindSprite(pData->imageName, pData->atlas);

	}
	//pData->imageName
}

void ParseBindingEspressionAttribute(const char* pAttributeName, const char* pAttributeContent, struct UIWidget* pWidget, struct BackgroundBoxWidgetData* pWidgetData, struct XMLUIData* pUILayerData)
{

	if (strcmp(pAttributeName, "sprite") == 0)
	{
		if (pWidgetData->imageName)
		{
			free(pWidgetData->imageName);
			pWidgetData->imageName = NULL;
		}
		EASSERT(pWidgetData->imageName == NULL);
		UI_AddStringPropertyBinding(pWidget, pAttributeName, pAttributeContent, &pWidgetData->imageName, pUILayerData->hViewModel);
		pWidgetData->sprite = At_FindSprite(pWidgetData->imageName, pWidgetData->atlas);
	}
	else
	{
		printf("invalid property binding: %s\n", pAttributeContent);
	}
}

static void ParseLiteralBackgroundBoxData(struct BackgroundBoxWidgetData* pWidgetData, const char* attributeNameBuffer, const char* attributeContentBuffer)
{
	if (strcmp(attributeNameBuffer, "sprite") == 0)
	{
		if (pWidgetData->imageName)
		{
			free(pWidgetData->imageName);
			pWidgetData->imageName = NULL;
		}
		EASSERT(pWidgetData->imageName == NULL);
		pWidgetData->imageName = malloc(strlen(attributeContentBuffer) + 1);
		strcpy(pWidgetData->imageName, attributeContentBuffer);
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

void CreateBackgroundBoxWidgetData(struct UIWidget* pWidget, struct BackgroundBoxWidgetData* pWidgetData, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
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

		if (UI_IsAttributeStringABindingExpression(attributeContentBuffer))
		{
			ParseBindingEspressionAttribute(attributeNameBuffer, attributeContentBuffer, pWidget, pWidgetData, pUILayerData);
			continue;
		}

		ParseLiteralBackgroundBoxData(pWidgetData, attributeNameBuffer, attributeContentBuffer);
	}
	if (pWidgetData->imageName)
	{
		pWidgetData->sprite = At_FindSprite(pWidgetData->imageName, pUILayerData->atlas);
		pWidgetData->atlas = pUILayerData->atlas;
	}
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
	pWidget->fnOnBoundPropertyChanged = &OnPropertyChanged;
	pWidget->pImplementationData = malloc(sizeof(struct BackgroundBoxWidgetData));
	memset(pWidget->pImplementationData, 0, sizeof(struct BackgroundBoxWidgetData));

	struct BackgroundBoxWidgetData* pWidgetData = pWidget->pImplementationData;

	CreateBackgroundBoxWidgetData(pWidget, pWidgetData, pXMLNode, pUILayerData);
}

HWidget BackgroundBoxWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoBackgroundBoxWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}
