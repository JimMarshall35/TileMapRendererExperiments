#include "CanvasWidget.h"
#include "Widget.h"
#include "xml.h"
#include "XMLUIGameLayer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Atlas.h"
#include "AssertLib.h"
#include "RootWidget.h"

static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return UI_ResolveWidgetDimPxls(pWidget, &GetWidgetWidthDim, pWidget->fnGetWidth);
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return UI_ResolveWidgetDimPxls(pWidget, &GetWidgetHeightDim, pWidget->fnGetHeight);
}

static void LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	RootWidget_LayoutChildren(pWidget, pParent);
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
	pOutVerts = UI_Helper_OnOutputVerts(pWidget, pOutVerts);
	return pOutVerts;
}

static void OnPropertyChanged(struct UIWidget* pThisWidget, struct WidgetPropertyBinding* pBinding)
{

}

static void MakeWidgetIntoCanvasWidget(HWidget hWidget, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
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
	pWidget->pImplementationData = NULL;
}

HWidget CanvasWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoCanvasWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}