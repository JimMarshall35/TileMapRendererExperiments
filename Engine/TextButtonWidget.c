#include "TextButtonWidget.h"
#include "Widget.h"
#include "xml.h"
#include "XMLUIGameLayer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Atlas.h"
#include "AssertLib.h"
#include "TextWidget.h"
#include "BackgroundBoxWidget.h"
#include "RootWidget.h"

struct TextButtonWidgetData
{
	struct TextWidgetData textWidgetData;
	struct BackgroundBoxWidgetData backgroundBoxWidgetData;
	struct BackgroundBoxWidgetData pressedBackgroundBoxWidgetData;
	struct WidgetPadding textPadding;
	bool bPressed;
	HWidget RootWidget;
};

static void MakeDefaultTextButtonWidgetData(struct TextButtonWidgetData* pData)
{
	pData->textPadding.paddingBottom = 20;
	pData->textPadding.paddingTop = 20;
	pData->textPadding.paddingLeft = 20;
	pData->textPadding.paddingRight = 20;
	const char* defaultBackgroundWidget = "defaultButton";
	const char* defaultPressedBackgroundWidget = "defaultPressedButton";

	// background widget

	pData->backgroundBoxWidgetData.scale.scaleX = 1.0f;
	pData->backgroundBoxWidgetData.scale.scaleY = 1.0f;

	pData->pressedBackgroundBoxWidgetData.scale.scaleX = 1.0f;
	pData->pressedBackgroundBoxWidgetData.scale.scaleY = 1.0f;

	pData->backgroundBoxWidgetData.imageName = malloc(strlen(defaultBackgroundWidget) + 1);
	strcpy(pData->backgroundBoxWidgetData.imageName, defaultBackgroundWidget);
	pData->backgroundBoxWidgetData.sprite = At_FindSprite(pData->backgroundBoxWidgetData.imageName, pData->backgroundBoxWidgetData.atlas);

	pData->pressedBackgroundBoxWidgetData.imageName = malloc(strlen(defaultPressedBackgroundWidget) + 1);
	strcpy(pData->pressedBackgroundBoxWidgetData.imageName, defaultPressedBackgroundWidget);
	pData->pressedBackgroundBoxWidgetData.sprite = At_FindSprite(pData->pressedBackgroundBoxWidgetData.imageName, pData->pressedBackgroundBoxWidgetData.atlas);


	// text widget
	
	pData->textWidgetData.content = malloc(strlen("Button") + 1);
	strcpy(pData->textWidgetData.content, "Button");
	
	pData->textWidgetData.r = 0.0f;
	pData->textWidgetData.g = 0.0f;
	pData->textWidgetData.b = 0.0f;
	pData->textWidgetData.a = 1.0f;


	pData->bPressed = false;
}

static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct TextButtonWidgetData* pWD = pWidget->pImplementationData;
	struct TextWidgetData* pData = &pWD->textWidgetData;
	return Fo_StringWidth(pData->atlas, pData->font, pData->content) +
		pWD->textPadding.paddingLeft +
		pWD->textPadding.paddingRight +
		pWidget->padding.paddingLeft +
		pWidget->padding.paddingRight;
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct TextButtonWidgetData* pWD = pWidget->pImplementationData;
	struct TextWidgetData* pData = &pWD->textWidgetData;
	return Fo_StringHeight(pData->atlas, pData->font, pData->content) +
		pWD->textPadding.paddingTop +
		pWD->textPadding.paddingBottom +
		pWidget->padding.paddingTop +
		pWidget->padding.paddingBottom;
}

static void LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	EASSERT(pWidget->hFirstChild == NULL_HWIDGET);
}

static void OnDestroy(struct UIWidget* pWidget)
{
	free(pWidget->pImplementationData);
}

static void OnDebugPrint(int indentLvl, struct UIWidget* pWidget, PrintfFn printfFn)
{
}

static void* OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	struct TextButtonWidgetData* pWD = pWidget->pImplementationData;
	struct TextWidgetData* pTextWidgetData = &pWD->textWidgetData;
	struct BackgroundBoxWidgetData* pBBWD = pWD->bPressed ? &pWD->pressedBackgroundBoxWidgetData : &pWD->backgroundBoxWidgetData;
	pOutVerts = OutputBackgroundBoxVerts(
		pBBWD,
		pOutVerts,
		GetWidth(pWidget, NULL),
		GetHeight(pWidget, NULL),
		&pWidget->padding,
		pWidget->left,
		pWidget->top
	);
	pOutVerts = OutputTextWidgetVerts(pWidget->left, pWidget->top, &pWD->textPadding, pTextWidgetData, pOutVerts);
	
	pOutVerts = UI_Helper_OnOutputVerts(pWidget, pOutVerts);
	return pOutVerts;
}


static void MouseButtonDownCallback(struct UIWidget* pWidget, float x, float y, int btn)
{
	struct TextButtonWidgetData* pWD = pWidget->pImplementationData;
	pWD->bPressed = true;
	SetRootWidgetIsDirty(pWD->RootWidget, true);
}

static void MouseButtonUpCallback(struct UIWidget* pWidget, float x, float y, int btn)
{
	struct TextButtonWidgetData* pWD = pWidget->pImplementationData;
	pWD->bPressed = false;
	SetRootWidgetIsDirty(pWD->RootWidget, true);

}

static void MouseLeaveCallback(struct UIWidget* pWidget, float x, float y)
{
	struct TextButtonWidgetData* pWD = pWidget->pImplementationData;
	pWD->bPressed = false;
	SetRootWidgetIsDirty(pWD->RootWidget, true);

}

static void SetCMouseCallbacks(struct UIWidget* pWidget)
{
	pWidget->cCallbacks.Callbacks[WC_OnMouseDown].type = WC_OnMouseDown;
	pWidget->cCallbacks.Callbacks[WC_OnMouseDown].callback.mouseBtnFn = &MouseButtonDownCallback;

	pWidget->cCallbacks.Callbacks[WC_OnMouseUp].type = WC_OnMouseUp;
	pWidget->cCallbacks.Callbacks[WC_OnMouseUp].callback.mouseBtnFn = &MouseButtonUpCallback;

	pWidget->cCallbacks.Callbacks[WC_OnMouseLeave].type = WC_OnMouseLeave;
	pWidget->cCallbacks.Callbacks[WC_OnMouseLeave].callback.mouseBtnFn = &MouseLeaveCallback;


}


static void MakeWidgetIntoTextButtonWidget(HWidget hWidget, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
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
	pWidget->pImplementationData = malloc(sizeof(struct TextButtonWidgetData));
	memset(pWidget->pImplementationData, 0, sizeof(struct TextButtonWidgetData));
	struct TextButtonWidgetData* pData = pWidget->pImplementationData;
	pData->RootWidget = pUILayerData->rootWidget;
	MakeDefaultTextButtonWidgetData(pData);
	CreateTextWidgetData(&pData->textWidgetData, pXMLNode, pUILayerData);
	CreateBackgroundBoxWidgetData(pWidget, &pData->backgroundBoxWidgetData, pXMLNode, pUILayerData);
	SetCMouseCallbacks(pWidget);
}

HWidget TextButtonWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoTextButtonWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}
