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
#include "RootWidget.h"
#include "Scripting.h"

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
	HWidget rootWidget;
	bool bMouseDown;
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
	vec2 a2 = {
		left,
		top
	};
	
	switch (pData->orientation)
	{
	case SO_Vertical:
		{
			AtlasSprite* pSprite = At_GetSprite(pData->railStaticData.sprite, pData->railStaticData.atlas);
			vec2 tl = { 0,0 };
			vec2 br = { pSprite->widthPx, pSprite->heightPx / 3 };
			vec2 addition = { 0, pSprite->heightPx / 3 };
			PopulateWidgetQuad(&pOutQuads[0], pSprite, tl, br);
			glm_vec2_add(tl, addition, tl);
			glm_vec2_add(br, addition, br);
			PopulateWidgetQuad(&pOutQuads[1], pSprite, tl, br);
			glm_vec2_add(tl, addition, tl);
			glm_vec2_add(br, addition, br);
			PopulateWidgetQuad(&pOutQuads[2], pSprite, tl, br);
			
			vec2 trans = {
				0,
				pData->lengthPx - pSprite->heightPx / 3
			};
			
			TranslateWidgetQuad(trans, &pOutQuads[2]);
			pOutQuads[1].v[VL_TL].y = pOutQuads[0].v[VL_BL].y;
			pOutQuads[1].v[VL_TR].y = pOutQuads[0].v[VL_BR].y;
			pOutQuads[1].v[VL_BL].y = pOutQuads[2].v[VL_TL].y;
			pOutQuads[1].v[VL_BR].y = pOutQuads[2].v[VL_TR].y;
			for (int i = 0; i < 3; i++)
			{
				TranslateWidgetQuad(a2, &pOutQuads[i]);
			}
			break;
		}
	case SO_Horizontal:
		{
			AtlasSprite* pSprite = At_GetSprite(pData->railStaticData.sprite, pData->railStaticData.atlas);
			vec2 tl = { 0,0 };
			vec2 br = { pSprite->widthPx/3, pSprite->heightPx };
			vec2 addition = { pSprite->widthPx / 3, 0 };
			PopulateWidgetQuad(&pOutQuads[0], pSprite, tl, br);
			glm_vec2_add(tl, addition, tl);
			glm_vec2_add(br, addition, br);
			PopulateWidgetQuad(&pOutQuads[1], pSprite, tl, br);
			glm_vec2_add(tl, addition, tl);
			glm_vec2_add(br, addition, br);
			PopulateWidgetQuad(&pOutQuads[2], pSprite, tl, br);
			vec2 trans = {
				pData->lengthPx - pSprite->widthPx / 3,
				0
			};
		
			TranslateWidgetQuad(trans, &pOutQuads[2]);
			pOutQuads[1].v[VL_TL].x = pOutQuads[0].v[VL_TR].x;
			pOutQuads[1].v[VL_BL].x = pOutQuads[0].v[VL_BR].x;
			pOutQuads[1].v[VL_TR].x = pOutQuads[2].v[VL_TL].x;
			pOutQuads[1].v[VL_BR].x = pOutQuads[2].v[VL_BL].x;
			for (int i = 0; i < 3; i++)
			{
				TranslateWidgetQuad(a2, &pOutQuads[i]);
			}
			break;
		}
	}
}

static void PopulateSliderQuad(float left, float top, struct WidgetQuad* pOutQuad, struct SliderData* pData)
{
	EASSERT(pData->fVal >= pData->fMinVal && pData->fVal <= pData->fMaxVal);
	AtlasSprite* pSprite = At_GetSprite(pData->sliderStaticData.sprite, pData->sliderStaticData.atlas);
	float fraction = (pData->fVal - pData->fMinVal) / (pData->fMaxVal - pData->fMinVal);
	PopulateWidgetQuadWholeSprite(pOutQuad, pSprite);
	vec2 t0 = {
		left,
		top
	};
	TranslateWidgetQuad(t0, pOutQuad);
	switch (pData->orientation)
	{
	case SO_Vertical:
	{
		float maxLen = pData->lengthPx - pSprite->heightPx;
		vec2 t = {
			0,
			maxLen* fraction
		};
		TranslateWidgetQuad(t, pOutQuad);
	}
		break;
	case SO_Horizontal:
	{
		float maxLen = pData->lengthPx - pSprite->widthPx;
		vec2 t = {
			maxLen* fraction,
			0
		};
		TranslateWidgetQuad(t, pOutQuad);
	}
		break;
	default:
		EASSERT(false);
		break;
	}
}

static void* OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	struct SliderData* pData = pWidget->pImplementationData;
	struct WidgetQuad rail[4];
	float top = pWidget->top + pWidget->padding.paddingTop;
	float left = pWidget->left + pWidget->padding.paddingLeft;
	Populate3PanelRailQuads(left, top, rail, pData);
	PopulateSliderQuad(left, top, &rail[3], pData);
	return OutputWidgetQuads(pOutVerts, rail, 4);
}

static void OnPropertyChanged(struct UIWidget* pThisWidget, struct WidgetPropertyBinding* pBinding)
{
	struct SliderData* pData = pThisWidget->pImplementationData;
	if (strcmp(pBinding->boundPropertyName, "val") == 0)
	{
		char* fnName = UI_MakeBindingGetterFunctionName(pBinding->name);
		Sc_CallFuncInRegTableEntryTable(pThisWidget->scriptCallbacks.viewmodelTable, fnName, NULL, 0, 1);
		free(fnName);
		pData->fVal = Sc_Float();
		Sc_ResetStack();
		SetRootWidgetIsDirty(pData->rootWidget, true);
	}
}

static void SetSliderPosition(struct UIWidget* pWidget, struct SliderData* pData, float x, float y)
{
	switch (pData->orientation)
	{
	case SO_Vertical:
	{
		float zeroY = pWidget->top + pWidget->padding.paddingTop;
		float maxY = pWidget->top + pWidget->padding.paddingTop + pData->lengthPx;
		float fraction = (y - zeroY) / (maxY - zeroY);
		pData->fVal = pData->fMinVal + fraction * (pData->fMaxVal - pData->fMinVal);
		break;
	}
	case SO_Horizontal:
	{
		float zeroX = pWidget->left + pWidget->padding.paddingLeft;
		float maxX = pWidget->left + pWidget->padding.paddingLeft + pData->lengthPx;
		float fraction = (x - zeroX) / (maxX - zeroX);
		pData->fVal = pData->fMinVal + fraction * (pData->fMaxVal - pData->fMinVal);
		break;
	}
	default:
		break;
	}
	struct WidgetPropertyBinding* pBinding = UI_FindBinding(pWidget, "val");
	if (pBinding)
	{
		char* setterName = UI_MakeBindingSetterFunctionName(pBinding->name);
		struct ScriptCallArgument arg;
		arg.type = SCA_number;
		arg.val.number = pData->fVal;
		Sc_CallFuncInRegTableEntryTable(pWidget->scriptCallbacks.viewmodelTable, setterName, &arg, 1, 0);
		free(setterName);

	}
	SetRootWidgetIsDirty(pData->rootWidget, true);
}

static void MouseButtonDownCallback(struct UIWidget* pWidget, float x, float y, int btn)
{
	struct SliderData* pData = pWidget->pImplementationData;
	pData->bMouseDown = true;
	SetSliderPosition(pWidget, pData, x, y);
}

static void MouseButtonUpCallback(struct UIWidget* pWidget, float x, float y, int btn)
{
	struct SliderData* pData = pWidget->pImplementationData;
	pData->bMouseDown = false;
}

static void MouseLeaveCallback(struct UIWidget* pWidget, float x, float y)
{
	struct SliderData* pData = pWidget->pImplementationData;
	pData->bMouseDown = false;
}

static void MouseMoveCallback(struct UIWidget* pWidget, float x, float y)
{
	struct SliderData* pData = pWidget->pImplementationData;
	if (pData->bMouseDown)
	{
		SetSliderPosition(pWidget, pData, x, y);
	}
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

static void ParseBindingEspressionAttribute(const char* attribName, const char* attribContent, struct UIWidget* pWidget, struct SliderData* pData, struct XMLUIData* pUILayerData)
{
	if (strcmp(attribName, "val") == 0)
	{
		UI_AddFloatPropertyBinding(pWidget, attribName, attribContent, &pData->fVal, pUILayerData->hViewModel);
	}
	else
	{
		printf("invalid property binding: %s\n", attribContent);
	}

}

static void MakeSliderFromXML(struct UIWidget* pWidget, struct SliderData* pData, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
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
		if (UI_IsAttributeStringABindingExpression(attribContent))
		{
			ParseBindingEspressionAttribute(attribName, attribContent, pWidget, pData, pUILayerData);
			continue;
		}
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
	pData->rootWidget = pUILayerData->rootWidget;

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
	struct SliderData* pData = pWidget->pImplementationData;
	MakeSliderFromXML(pWidget, pData, pXMLNode, pUILayerData);
}

HWidget SliderWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoSliderWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}