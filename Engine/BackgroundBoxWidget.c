#include "BackgroundBoxWidget.h"
#include "Widget.h"
#include "xml.h"
#include "XMLUIGameLayer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Atlas.h"
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
}

static void OnDestroy(struct UIWidget* pWidget)
{
	free(pWidget->pImplementationData);
}

typedef enum
{
	VL_TL,
	VL_TR,
	VL_BL,
	VL_BR,
	VT_NUM
}WidgetQuadVertexLocation;

struct WidgetQuad
{
	struct WidgetVertex v[4];
};

static void PopulateWidgetQuad(struct WidgetQuad* pQuad, AtlasSprite* pSprt, vec2 subSpriteTL, vec2 subSpriteBR)
{
	float subSpriteHeight = subSpriteBR[1] - subSpriteTL[1];
	float subSpriteWidth = subSpriteBR[0] - subSpriteTL[0];
	
	float uPerPxl = (pSprt->bottomRightUV_U - pSprt->topLeftUV_U) / (float)pSprt->widthPx;
	float vPerPxl = (pSprt->topLeftUV_V - pSprt->bottomRightUV_V) / (float)pSprt->heightPx;
	vec2 subSpriteTopLefUV = { 
		pSprt->topLeftUV_U + subSpriteTL[0] * uPerPxl,
		pSprt->topLeftUV_V - subSpriteTL[1] * vPerPxl
	};
	vec2 subSpriteBottomRightUV = {
		subSpriteTopLefUV[0] + uPerPxl * subSpriteWidth,
		subSpriteTopLefUV[1] - vPerPxl * subSpriteHeight
	};

	pQuad->v[VL_TL].x = 0.0f;
	pQuad->v[VL_TL].y = 0.0f;
	pQuad->v[VL_TL].u = subSpriteTopLefUV[0];
	pQuad->v[VL_TL].v = subSpriteTopLefUV[1];
	
	pQuad->v[VL_TR].x = subSpriteWidth;
	pQuad->v[VL_TR].y = 0.0f;
	pQuad->v[VL_TR].u = subSpriteBottomRightUV[0];
	pQuad->v[VL_TR].v = subSpriteTopLefUV[1];

	pQuad->v[VL_BR].x = subSpriteWidth;
	pQuad->v[VL_BR].y = subSpriteHeight;
	pQuad->v[VL_BR].u = subSpriteBottomRightUV[0];
	pQuad->v[VL_BR].v = subSpriteBottomRightUV[1];

	pQuad->v[VL_BL].x = 0.0f;
	pQuad->v[VL_BL].y = subSpriteHeight;
	pQuad->v[VL_BL].u = subSpriteTopLefUV[0];
	pQuad->v[VL_BL].v = subSpriteBottomRightUV[1];
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

static void* OutputWidgetQuad(VECTOR(struct WidgetVertex) pOutVerts, const struct WidgetQuad* pQuad)
{
	const int numIndices = 6;
	const WidgetQuadVertexLocation indices[6] = {
		VL_TL,
		VL_TR,
		VL_BL,
		VL_TR,
		VL_BR,
		VL_BL
	};
	for (int i = 0; i < numIndices; i++)
	{
		int index = indices[i];
		pOutVerts = VectorPush(pOutVerts, &pQuad->v[index]);
	}
	return pOutVerts;
}

static void TranslateWidgetQuad(vec2 vector, struct WidgetQuad* pOutQuad)
{
	for (int i = 0; i < 4; i++)
	{
		pOutQuad->v[i].x += vector[0];
		pOutQuad->v[i].y += vector[1];
	}
}

// keeps TL where it is and sets a new size, expanding to the right and downwards
static SizeWidgetQuad(vec2 size, struct WidgetQuad* pOutQuad)
{
	pOutQuad->v[VL_TR].x = pOutQuad->v[VL_TL].x + size[0];
	pOutQuad->v[VL_BR].x = pOutQuad->v[VL_BL].x + size[0];

	pOutQuad->v[VL_BL].y = pOutQuad->v[VL_TL].y + size[1];
	pOutQuad->v[VL_BR].y = pOutQuad->v[VL_TR].y + size[1];
}

static float WidgetQuadWidth(const struct WidgetQuad* pInQuad)
{
	return pInQuad->v[VL_TR].x - pInQuad->v[VL_TL].x;
}

static void* OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	struct BackgroundBoxWidgetData* pBBoxData = pWidget->pImplementationData;
	AtlasSprite* pAtlasSprite = At_GetSprite(pBBoxData->sprite, pBBoxData->atlas);

	float widgetWidth = GetWidth(pWidget, NULL);
	float widgetHeight = GetHeight(pWidget, NULL);

	if (widgetWidth <= pAtlasSprite->widthPx && widgetHeight > pAtlasSprite->heightPx)
	{
		return OnOutputVerts_3StripsHorizontal(pWidget, pOutVerts);
	}
	else if (widgetWidth > pAtlasSprite->widthPx && widgetHeight <= pAtlasSprite->heightPx)
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
	
	// top row
	translation[0] = widthOver3;
	translation[1] = 0.0f;
	TranslateWidgetQuad(translation, &quads[1]);
	float borderPieceNewWidth = widgetWidth - widthOver3 * 2;
	vec2 horizontalBorderSize = { borderPieceNewWidth, heightOver3 };
	SizeWidgetQuad(horizontalBorderSize, &quads[1]);

	translation[0] = widthOver3 + borderPieceNewWidth;
	translation[1] = 0.0f;
	TranslateWidgetQuad(translation, &quads[2]);

	// middle row
	translation[0] = 0.0f;
	translation[1] = heightOver3;
	TranslateWidgetQuad(translation, &quads[3]);
	float borderPieceNewHeight = widgetHeight - heightOver3 * 2;
	vec2 verticalBorderSize = { widthOver3, borderPieceNewHeight };
	SizeWidgetQuad(verticalBorderSize, &quads[3]);

	translation[0] = widthOver3;
	translation[1] = heightOver3;
	TranslateWidgetQuad(translation, &quads[4]);
	vec2 middlePieceSize = { borderPieceNewWidth, borderPieceNewHeight };
	SizeWidgetQuad(middlePieceSize, &quads[4]);

	translation[0] = widthOver3 + borderPieceNewWidth;
	translation[1] = heightOver3;
	TranslateWidgetQuad(translation, &quads[5]);
	SizeWidgetQuad(verticalBorderSize, &quads[5]);

	// bottom row
	translation[0] = 0.0f;
	translation[1] = heightOver3 + borderPieceNewHeight;
	TranslateWidgetQuad(translation, &quads[6]);

	translation[0] = widthOver3;
	translation[1] = heightOver3 + borderPieceNewHeight;
	TranslateWidgetQuad(translation, &quads[7]);
	SizeWidgetQuad(horizontalBorderSize, &quads[7]);

	translation[0] = widthOver3 + borderPieceNewWidth;
	translation[1] = heightOver3 + borderPieceNewHeight;
	TranslateWidgetQuad(translation, &quads[8]);

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
			pWidgetData->sprite = At_FindSprite(pWidgetData->imageName, pWidgetData->atlas);
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
