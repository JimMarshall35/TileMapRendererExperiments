#include "TextWidget.h"
#include "XMLUIGameLayer.h"
#include "xml.h"
#include "XMLHelpers.h"
#include "Widget.h"
#include "Atlas.h"
#include "AssertLib.h"
#include <stdio.h>
#include <string.h>
#include "WidgetVertexOutputHelpers.h"

void TextWidget_Destroy(struct TextWidgetData* pData)
{
	free(pData->content);
}


static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct TextWidgetData* pData = pWidget->pImplementationData;
	return Fo_StringWidth(pData->atlas, pData->font, pData->content) + pWidget->padding.paddingLeft + pWidget->padding.paddingRight;
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct TextWidgetData* pData = pWidget->pImplementationData;
	return Fo_StringHeight(pData->atlas, pData->font, pData->content) + pWidget->padding.paddingTop + pWidget->padding.paddingBottom;
}

static float LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return 0.0f;
}


static void OnDestroy(struct UIWidget* pWidget)
{
	struct TextWidgetData* pData = pWidget->pImplementationData;
	TextWidget_Destroy(pData);
	free(pData);
}

static void OnDebugPrint(int indentLvl, struct UIWidget* pWidget, PrintfFn printfFn)
{
	struct TextWidgetData* pTextWidgetData = pWidget->pImplementationData;
	for (int i = 0; i < indentLvl; i++)
	{
		printfFn("\t");
	}
	printfFn("Text. imageName = %s, font = %i, atlas = %i, ",
		pTextWidgetData->content,
		pTextWidgetData->font,
		pTextWidgetData->atlas);
	UI_DebugPrintCommonWidgetInfo(pWidget, printfFn);
	printfFn("\n");

}

void* TextWidget_OutputVerts(float left, float top, const struct WidgetPadding* padding, struct TextWidgetData* pData, VECTOR(struct WidgetVertex) pOutVerts)
{
	//struct TextWidgetData* pData = pThisWidget->pImplementationData;
	float maxYBearing = Fo_GetMaxYBearing(pData->atlas, pData->font, pData->content);
	vec2 pen = { left + padding->paddingLeft, top + maxYBearing + padding->paddingTop };
	size_t len = strlen(pData->content);
	for (int i = 0; i < len; i++)
	{
		char c = pData->content[i];
		AtlasSprite* pAtlasSprite = Fo_GetCharSprite(pData->atlas, pData->font, c);
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
		struct WidgetQuad quad;
		PopulateWidgetQuadWholeSprite(&quad, pAtlasSprite);
		SetWidgetQuadColour(&quad, pData->r, pData->g, pData->b, pData->a);

		// topleft
		glm_vec2_add(bearing, pen, output);
		TranslateWidgetQuad(output, &quad);
		pOutVerts = OutputWidgetQuad(pOutVerts, &quad);

		pen[0] += advance;
	}

	return pOutVerts;
}


static void* OnOutputVerts(struct UIWidget* pThisWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	pOutVerts = TextWidget_OutputVerts(pThisWidget->left, pThisWidget->top, &pThisWidget->padding, pThisWidget->pImplementationData, pOutVerts);
	pOutVerts = UI_Helper_OnOutputVerts(pThisWidget, pOutVerts);
	return pOutVerts;
}

static void ParseColourAttribute(char* inText, struct TextWidgetData* pOutWidgetData)
{
	char* tok = strtok(inText, ",");
	int onToken = 0;
	while (tok)
	{
		int i = atoi(tok);
		EASSERT(i < 256);
		switch (onToken++)
		{
		case 0:
			pOutWidgetData->r = (float)i / 255.0f;
			break;
		case 1:
			pOutWidgetData->g = (float)i / 255.0f;
			break;
		case 2:
			pOutWidgetData->b = (float)i / 255.0f;
			break;
		case 3:
			pOutWidgetData->a = (float)i / 255.0f;
			break;
		default:
			printf("ParseColourAttribute: invalid number of tokens: %i", onToken);
		}
		tok = strtok(NULL, ",");
	}
}

static void ParseSizeAttribute(char* inText, struct TextWidgetData* pOutWidgetData)
{
	char* endPtr;
	float val = strtof(inText, &endPtr);
	if (strcmp(endPtr, "pxls") == 0)
	{
		val = At_PixelsToPts(val);
	}
	pOutWidgetData->fSizePts = val;
}

void TextWidget_FromXML(struct TextWidgetData* pData, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	pData->atlas = pUILayerData->atlas;
	char attribName[64];
	char attribContent[64];
	char fontName[64];

	memset(attribName, 0, 64);

	struct xml_string* pString = xml_node_content(pXMLNode);
	int l = xml_string_length(pString);
	if (pData->content)
	{
		free(pData->content);
		pData->content = NULL;
	}
	EASSERT(pData->content == NULL);
	pData->content = malloc(l + 1);
	xml_string_copy(pString, pData->content, l);
	pData->content[l] = '\0';

	bool bFontSet = false;
	bool bFontSizeSet = false;

	for (int i = 0; i < xml_node_attributes(pXMLNode); i++)
	{
		XML_AttributeNameToBuffer(pXMLNode, attribName, i, 64);
		XML_AttributeContentToBuffer(pXMLNode, attribContent, i, 64);

		if (strcmp(attribName, "font") == 0)
		{
			strcpy(fontName, attribContent);
			bFontSet = true;
		}
		else if (strcmp(attribName, "colour") == 0)
		{
			ParseColourAttribute(attribContent, pData);
		}
		else if (strcmp(attribName, "fontSize") == 0)
		{
			ParseSizeAttribute(attribContent, pData);
			bFontSizeSet = true;
		}
	}
	if (bFontSet && bFontSizeSet)
	{
		HFont font = Fo_FindFont(pUILayerData->atlas, fontName, pData->fSizePts);
		if (font == NULL_HANDLE)
		{
			printf("font '%s' could not be found\n", attribContent);
		}
		pData->font = font;
	}
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
	memset(pWidget->pImplementationData, 0, sizeof(struct TextWidgetData));

	struct TextWidgetData* pData = pWidget->pImplementationData;

	TextWidget_FromXML(pData, pXMLNode, pUILayerData);

}

HWidget TextWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoTextWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}
