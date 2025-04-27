#include "SliderWidget.h"
#include "Widget.h"
#include "xml.h"
#include "XMLHelpers.h"
#include "XMLUIGameLayer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Atlas.h"
#include "AssertLib.h"
#include "StaticWidget.h"
#include "Atlas.h"
#include "WidgetVertexOutputHelpers.h"

enum SliderOrientation
{
	SO_Vertical,
	SO_Horizontal
};

struct SliderData
{
	enum SliderOrientation orientation;
	struct StaticWidgetData railStaticData;
	struct StaticWidgetData sliderStaticData;
	float fVal;
	float fMaxVal;
	float fMinVal;
	float lengthPx;
};

static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct SliderData* pData = pWidget->pImplementationData;
	switch (pData->orientation)
	{
	case SO_Horizontal:
		return pData->lengthPx + pWidget->padding.paddingLeft + pWidget->padding.paddingRight;
	case SO_Vertical:
	{
		AtlasSprite* pSprite = At_GetSprite(pData->sliderStaticData.sprite, pData->sliderStaticData.atlas);
		return pSprite->widthPx * pData->sliderStaticData.scale.scaleX + pWidget->padding.paddingLeft + pWidget->padding.paddingRight;
	}
	default:
		EASSERT(false);
		break;
	}
	return 0.0f;
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct SliderData* pData = pWidget->pImplementationData;
	switch (pData->orientation)
	{
	case SO_Horizontal:
	{
		AtlasSprite* pSprite = At_GetSprite(pData->sliderStaticData.sprite, pData->sliderStaticData.atlas);
		return pSprite->heightPx * pData->sliderStaticData.scale.scaleY + pWidget->padding.paddingTop + pWidget->padding.paddingBottom;

	}
	case SO_Vertical:
	{
		return pData->lengthPx + pWidget->padding.paddingTop + pWidget->padding.paddingBottom;

	}
	default:
		EASSERT(false);
		break;
	}

	return 0.0f;
}

static void LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
}

static void OnDestroy(struct UIWidget* pWidget)
{
	free(pWidget->pImplementationData);
}

static void OnDebugPrint(int indentLvl, struct UIWidget* pWidget, PrintfFn printfFn)
{
}

static void Populate3PanelRailQuads(float left, float top, struct WidgetQuad* pOutQuads, struct SliderData* pData)
{
	switch (pData->orientation)
	{
	case SO_Vertical:
		{
			break;
		}
	case SO_Horizontal:
		{
			break;
		}
	}
}

static void PopulateSliderQuad(float left, float top, struct WidgetQuad* pOutQuads, struct SliderData* pData)
{

}

static void* OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	struct SliderData* pData = pWidget->pImplementationData;
	struct WidgetQuad rail[3];
	struct WidgetQuad slider;
	float top = pWidget->top + pWidget->padding.paddingTop;
	float left = pWidget->top + pWidget->padding.paddingLeft;
	Populate3PanelRailQuads(left, top, rail, pData);
	PopulateSliderQuad(left, top, &slider, pData);
	return NULL;
}

static void OnPropertyChanged(struct UIWidget* pThisWidget, struct WidgetPropertyBinding* pBinding)
{

}


static void MouseButtonDownCallback(struct UIWidget* pWidget, float x, float y, int btn)
{
}

static void MouseButtonUpCallback(struct UIWidget* pWidget, float x, float y, int btn)
{
	
}

static void MouseLeaveCallback(struct UIWidget* pWidget, float x, float y)
{
	
}

static void MouseMoveCallback(struct UIWidget* pWidget, float x, float y)
{
	
}

static void PopulateStaticInternal(
	struct xml_node* pXMLNode,
	struct StaticWidgetData* pWidgetData,
	struct XMLUIData* pUILayerData,
	const char* spriteAttribName,
	const char* scaleXAttribName,
	const char* scaleYAttribName)
{
	size_t numAttributes = xml_node_attributes(pXMLNode);
	char attributeNameBuffer[128];
	char attributeContentBuffer[128];
	for (int i = 0; i < numAttributes; i++)
	{
		XML_AttributeNameToBuffer(pXMLNode, attributeNameBuffer, i, 128);
		XML_AttributeContentToBuffer(pXMLNode, attributeContentBuffer, i, 128);
		if (strcmp(attributeNameBuffer, spriteAttribName) == 0)
		{
			struct xml_string* contents = xml_node_attribute_content(pXMLNode, i);
			if (pWidgetData->imageName)
			{
				free(pWidgetData->imageName);
			}
			pWidgetData->imageName = malloc(xml_string_length(contents) + 1);
			xml_string_copy(contents, pWidgetData->imageName, xml_string_length(contents));
			pWidgetData->imageName[xml_string_length(contents)] = '\0';
			pWidgetData->sprite = At_FindSprite(pWidgetData->imageName, pWidgetData->atlas);
		}
		else if (strcmp(attributeNameBuffer, scaleXAttribName) == 0)
		{
			pWidgetData->scale.scaleX = atof(attributeContentBuffer);
		}
		else if (strcmp(attributeNameBuffer, scaleYAttribName) == 0)
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

static void MakeSliderFromXML(struct SliderData* pData, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	PopulateStaticInternal(pXMLNode, &pData->railStaticData, pUILayerData,
		"railSprite",
		"railScaleX",
		"railScaleY");
	PopulateStaticInternal(pXMLNode, &pData->sliderStaticData, pUILayerData,
		"sliderSprite",
		"sliderScaleX",
		"sliderScaleY");

	int numAttribs = xml_node_attributes(pXMLNode);
	char attribName[64];
	char attribContent[64];
	for (int i = 0; i < numAttribs; i++)
	{
		XML_AttributeNameToBuffer(pXMLNode, attribName, i, 64);
		XML_AttributeContentToBuffer(pXMLNode, attribContent, i, 64);
		if (strcmp(attribName, "orientation") == 0)
		{
			if (strcmp(attribContent, "horizontal") == 0)
			{
				pData->orientation = SO_Horizontal;
			}
			else if (strcmp(attribContent, "vertical") == 0)
			{
				pData->orientation = SO_Vertical;
			}
			else
			{
				printf("invalid slider orientation %s\n", attribContent);
			}
		}
	}
}

static void MakeDefaultSliderWidget(struct SliderData* pData, struct XMLUIData* pUILayerData)
{
	pData->orientation = SO_Horizontal;

	// slider and rail sprites
	pData->sliderStaticData.imageName = malloc(strlen("defaultSliderHorizontal") + 1);
	strcpy(pData->sliderStaticData.imageName, "defaultSliderHorizontal");
	pData->railStaticData.imageName = malloc(strlen("defaultRailHorizontal") + 1);
	strcpy(pData->railStaticData.imageName, "defaultRailHorizontal");

	pData->sliderStaticData.scale.scaleX = 1.0f;
	pData->sliderStaticData.scale.scaleY = 1.0f;
	pData->railStaticData.scale.scaleX = 1.0f;
	pData->railStaticData.scale.scaleY = 1.0f;

	pData->sliderStaticData.atlas = pUILayerData->atlas;
	pData->railStaticData.atlas = pUILayerData->atlas;

	pData->sliderStaticData.sprite = At_FindSprite(pData->sliderStaticData.imageName, pUILayerData->atlas);
	pData->railStaticData.sprite = At_FindSprite(pData->railStaticData.imageName, pUILayerData->atlas);

	// slider data
	pData->lengthPx = 200.0f;
	pData->fMaxVal = 200.0f;
	pData->fMinVal = 0.0f;
	pData->fVal = 0.0f;

}

static void MakeWidgetIntoSliderWidget(HWidget hWidget, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
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
	pWidget->pImplementationData = malloc(sizeof(struct SliderData));

	pWidget->cCallbacks.Callbacks[WC_OnMouseDown].type = WC_OnMouseDown;
	pWidget->cCallbacks.Callbacks[WC_OnMouseDown].callback.mouseBtnFn = &MouseButtonDownCallback;
	
	pWidget->cCallbacks.Callbacks[WC_OnMouseUp].type = WC_OnMouseUp;
	pWidget->cCallbacks.Callbacks[WC_OnMouseUp].callback.mouseBtnFn = &MouseButtonUpCallback;

	pWidget->cCallbacks.Callbacks[WC_OnMouseLeave].type = WC_OnMouseLeave;
	pWidget->cCallbacks.Callbacks[WC_OnMouseLeave].callback.mouseBtnFn = &MouseLeaveCallback;

	pWidget->cCallbacks.Callbacks[WC_OnMouseMove].type = WC_OnMouseMove;
	pWidget->cCallbacks.Callbacks[WC_OnMouseMove].callback.mouseBtnFn = &MouseMoveCallback;

	memset(pWidget->pImplementationData, 0, sizeof(struct SliderData));
	MakeDefaultSliderWidget(pWidget->pImplementationData, pUILayerData);
}

HWidget SliderWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoSliderWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}