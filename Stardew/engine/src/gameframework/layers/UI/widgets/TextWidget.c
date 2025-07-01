#include "TextWidget.h"
#include "XMLUIGameLayer.h"
#include "Widget.h"
#include "Atlas.h"
#include "AssertLib.h"
#include <stdio.h>
#include <string.h>
#include "WidgetVertexOutputHelpers.h"
#include <libxml/tree.h>

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

static void LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{

}


static void OnDestroy(struct UIWidget* pWidget)
{
	struct TextWidgetData* pData = pWidget->pImplementationData;
	TextWidget_Destroy(pData);
	free(pData);
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

static bool IsWhitespaceChar(xmlChar character)
{
	return 
		character == ' '  ||
		character == '\n' ||
		character == '\t' ||
		character == '\r';
}

/*
	params:
		inString - input string to strip
		outStrippedStart - output ptr to starting character
	returns:
		length after stripping start and end whitespace
*/
static int GetWhitespaceStrippedLengthAndStart(xmlChar* inString, xmlChar** outStrippedStart)
{
	xmlChar* strippedStart = inString;
	xmlChar* strippedEnd = NULL;
	
	// find start ptr
	while(IsWhitespaceChar(*strippedStart)) 
	{
		strippedStart++;
	}
	*outStrippedStart = strippedStart;

	// find end
	int len = strlen(inString);
	strippedEnd = inString + (len - 1);
	while(IsWhitespaceChar(*strippedEnd))
	{
		strippedEnd--;
	}
	return (strippedEnd - strippedStart) + 1;
}

/*
	returns a string that is the input string with trailing and leading whitespace stripped.
	Callers responsibility to free with free()
*/
static char* GetWhitespaceStrippedString(xmlChar* inString)
{
	xmlChar* pStart = NULL;
	int strippedLen = GetWhitespaceStrippedLengthAndStart(inString, &pStart);
	char* outStr = malloc(strippedLen + 1);
	memcpy(outStr, pStart, strippedLen);
	outStr[strippedLen] = '\0';
	return outStr;
}

void TextWidget_FromXML(struct TextWidgetData* pData, xmlNode* pXMLNode, struct XMLUIData* pUILayerData)
{
	pData->atlas = pUILayerData->atlas;
	char attribName[64];
	char attribContent[64];
	char* fontName = NULL;

	memset(attribName, 0, 64);

	xmlChar* content = xmlNodeGetContent(pXMLNode);
	int len = strlen(content);
	if(pData->content)
	{
		free(pData->content);
		pData->content = NULL;
	}
	pData->content = GetWhitespaceStrippedString(content);

	xmlFree(content);

	bool bFontSet = false;
	bool bFontSizeSet = false;

	xmlChar* attribute = NULL;
	if(attribute = xmlGetProp(pXMLNode, "font"))
	{
		fontName = attribute;
		bFontSet = true;
	}
	if(attribute = xmlGetProp(pXMLNode, "colour"))
	{
		ParseColourAttribute(attribute, pData);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pXMLNode, "fontSize"))
	{
		ParseSizeAttribute(attribute, pData);
		bFontSizeSet = true;
		xmlFree(attribute);
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
	if(fontName)
	{
		xmlFree(fontName);
	}
}

static void MakeWidgetIntoTextWidget(HWidget hWidget, xmlNode* pXMLNode, struct XMLUIData* pUILayerData)
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
	pWidget->pImplementationData = malloc(sizeof(struct TextWidgetData));
	memset(pWidget->pImplementationData, 0, sizeof(struct TextWidgetData));

	struct TextWidgetData* pData = pWidget->pImplementationData;

	TextWidget_FromXML(pData, pXMLNode, pUILayerData);

}

HWidget TextWidgetNew(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoTextWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}
