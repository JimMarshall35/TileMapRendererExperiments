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

struct TextButtonWidgetData
{
	struct TextWidgetData textWidgetData;
	struct BackgroundBoxWidgetData backgroundBoxWidgetData;
	struct WidgetPadding textPadding;
};

static void MakeDefaultTextButtonWidgetData(struct TextButtonWidgetData* pData)
{
	pData->textPadding.paddingBottom = 10;
	pData->textPadding.paddingTop = 10;
	pData->textPadding.paddingLeft = 10;
	pData->textPadding.paddingRight = 10;

}

static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return 0.0f;
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
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

static void* OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	return NULL;
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

	CreateTextWidgetData(&pData->textWidgetData, pXMLNode, pUILayerData);
	CreateBackgroundBoxWidgetData(pWidget, &pData->backgroundBoxWidgetData, pXMLNode, pUILayerData);

	
}

HWidget TextButtonWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoTextButtonWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}
