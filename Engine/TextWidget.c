#include "TextWidget.h"
#include "XMLUIGameLayer.h"
#include "xml.h"
#include "Widget.h"
#include "Atlas.h"
#include "AssertLib.h"

struct TextWidgetData
{
	char* content;
	HFont font;
	hAtlas atlas;
};

static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct TextWidgetData* pData = pWidget->pImplementationData;
	return Fo_StringWidth(pData->atlas, pData->font, pData->content);
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct TextWidgetData* pData = pWidget->pImplementationData;
	return Fo_StringHeight(pData->atlas, pData->font, pData->content);
}

static float LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return 0.0f;
}

static void OnDestroy(struct UIWidget* pWidget)
{
	struct TextWidgetData* pData = pWidget->pImplementationData;
	free(pData->content);
}

static void OnDebugPrint(int indentLvl, struct UIWidget* pWidget, PrintfFn printfFn)
{

}

static void* OnOutputVerts(struct UIWidget* pThisWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	struct TextWidgetData* pData = pThisWidget->pImplementationData;
	float maxYBearing = Fo_GetMaxYBearing(pData->atlas, pData->font, pData->content);
	vec2 pen = { pThisWidget->left, pThisWidget->top + maxYBearing };
	size_t len = strlen(pData->content);
	for (int i = 0; i < len; i++)
	{
		char c = pData->content[i];
		AtlasSprite* pAtlasSprite = Fo_GetCharSprite(pData->atlas, pData->font, c);
		struct WidgetVertex v;
		vec2 bearing = { 0,0 };
		float advance = 0.0f;
		vec2 output = { 0,0 };
		vec2 bearingApplied = { 0,0 };

		if (!pAtlasSprite)
		{
			printf("can't get atlas sprite, file TextWidget.c");
			EASSERT(false);
			return pOutVerts;
		}

		EVERIFY(Fo_TryGetCharBearing(pData->atlas, pData->font, c, bearing));
		bearing[1] *= -1.0f; // FT coordinate system for bearing has increasing Y as up, in our game coordinate system that is decreasing y 
		EVERIFY(Fo_TryGetCharAdvance(pData->atlas, pData->font, c, &advance));
		
		// topleft
		glm_vec2_add(bearing, pen, output);
		bearingApplied[0] = output[0]; bearingApplied[1] = output[1];

		v.u = pAtlasSprite->topLeftUV_U;
		v.v = pAtlasSprite->topLeftUV_V;
		v.x = output[0];
		v.y = output[1];
		pOutVerts = VectorPush(pOutVerts, &v);
		
		// top right
		vec2 toAdd = { pAtlasSprite->widthPx, 0.0 };
		glm_vec2_add(toAdd, bearingApplied, output);
		v.u = pAtlasSprite->bottomRightUV_U;
		v.v = pAtlasSprite->topLeftUV_V;
		v.x = output[0];
		v.y = output[1];
		pOutVerts = VectorPush(pOutVerts, &v);
		
		// bottomleft
		toAdd[0] = 0.0f;
		toAdd[1] = pAtlasSprite->heightPx;
		glm_vec2_add(toAdd, bearingApplied, output);
		v.u = pAtlasSprite->topLeftUV_U;
		v.v = pAtlasSprite->bottomRightUV_V;
		v.x = output[0];
		v.y = output[1];
		pOutVerts = VectorPush(pOutVerts, &v);
		
		// TRIANGLE 2

		// topRight
		toAdd[0] = pAtlasSprite->widthPx;
		toAdd[1] = 0.0;
		glm_vec2_add(toAdd, bearingApplied, output);
		v.u = pAtlasSprite->bottomRightUV_U;
		v.v = pAtlasSprite->topLeftUV_V;
		v.x = output[0];
		v.y = output[1];
		pOutVerts = VectorPush(pOutVerts, &v);

		//// bottomRight
		toAdd[0] = pAtlasSprite->widthPx;
		toAdd[1] = pAtlasSprite->heightPx;
		glm_vec2_add(toAdd, bearingApplied, output);
		v.u = pAtlasSprite->bottomRightUV_U;
		v.v = pAtlasSprite->bottomRightUV_V;
		v.x = output[0];
		v.y = output[1];
		pOutVerts = VectorPush(pOutVerts, &v);

		//// bottomLeft
		toAdd[0] = 0;
		toAdd[1] = pAtlasSprite->heightPx;
		glm_vec2_add(toAdd, bearingApplied, output);
		v.u = pAtlasSprite->topLeftUV_U;
		v.v = pAtlasSprite->bottomRightUV_V;
		v.x = output[0];
		v.y = output[1];
		pOutVerts = VectorPush(pOutVerts, &v);


		pen[0] += advance;
	}
	pOutVerts = UI_Helper_OnOutputVerts(pThisWidget, pOutVerts);

	return pOutVerts;
}

static void MakeWidgetIntoTextWidget(HWidget hWidget, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
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
	pWidget->pImplementationData = malloc(sizeof(struct TextWidgetData));
	memset(pWidget->pImplementationData, 9, sizeof(struct TextWidgetData));

	struct TextWidgetData* pData = pWidget->pImplementationData;
	pData->atlas = pUILayerData->atlas;
	char attribName[128];
	char attribContent[256];
	memset(attribName, 0, 128);

	bool bContentSet = false;
	bool bFontSet = false;
	for (int i = 0; i < xml_node_attributes(pXMLNode); i++)
	{
		struct xml_string* pName = xml_node_attribute_name(pXMLNode, i);
		int nameLen = xml_string_length(pName);
		xml_string_copy(pName, attribName, nameLen);
		attribName[nameLen] = '\0';
		
		struct xml_string* pContent = xml_node_attribute_content(pXMLNode, i);
		int contentLen = xml_string_length(pContent);
		xml_string_copy(pContent, attribContent, contentLen);
		attribContent[contentLen] = '\0';
		
		if (strcmp(attribName, "content") == 0)
		{
			pData->content = malloc(strlen(attribName + 1));
			strcpy(pData->content, attribContent);
			bContentSet = true;
		}
		else if (strcmp(attribName, "font") == 0)
		{
			HFont font = Fo_FindFont(pUILayerData->atlas, attribContent);
			if (font == NULL_HANDLE)
			{
				printf("font '%s' could not be found\n", attribContent);
			}
			pData->font = font;
			bFontSet = true;
		}
	}
	if (!bFontSet || !bContentSet)
	{
		printf("invalid text widget node!\n");
	}

}

HWidget TextWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoTextWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}
