#include "RootWidget.h"
#include "Widget.h"
#include "main.h"
#include <string.h>        
#include <stdlib.h>

float RootWidget_GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return ((struct RootWidgetData*)pWidget->pImplementationData)->windowW;
}

float RootWidget_GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return ((struct RootWidgetData*)pWidget->pImplementationData)->windowH;
}

void RootWidget_LayoutChildren(struct UIWidget* pThis, struct UIWidget* pParent)
{
	if (pThis->hFirstChild != NULL_HWIDGET)
	{
		struct UIWidget* pWidget = UI_GetWidget(pThis->hFirstChild);
		struct RootWidgetData* pRootData = pThis->pImplementationData;
		float windowW = (float)pRootData->windowW;
		float windowH = (float)pRootData->windowH;
		while (pWidget)
		{
			float width = pWidget->fnGetWidth(pWidget, pThis);
			float height = pWidget->fnGetHeight(pWidget, pThis);

			switch (pWidget->dockPoint)
			{
			case WDP_TopLeft:
				pWidget->top = 0;
				pWidget->left = 0;
				break;
			case WDP_TopMiddle:
				pWidget->top = 0;
				pWidget->left = (windowW / 2) - (width / 2);
				break;
			case WDP_TopRight:
				pWidget->top = 0;
				pWidget->left = windowW - width;
				break;
			case WDP_MiddleRight:
				pWidget->top = (windowH / 2.0f) - (height / 2.0f);
				pWidget->left = windowW - width;
				break;
			case WDP_BottomRight:
				pWidget->top = windowH - height;
				pWidget->left = windowW - width;
				break;
			case WDP_BottomMiddle:
				pWidget->top = windowH - height;
				pWidget->left = windowW / 2.0f - width / 2.0f;
				break;
			case WDP_BottomLeft:
				pWidget->top = windowH - height;
				pWidget->left = 0;
				break;
			case WDP_MiddleLeft:
				pWidget->top = (windowH / 2.0f) - (height / 2.0f);
				pWidget->left = 0;
				break;
			case WDP_Centre:
				pWidget->top = (windowH / 2.0f) - (height / 2.0f);
				pWidget->left = (windowW / 2.0f) - (width / 2.0f);
				break;
			}
			pWidget->fnLayoutChildren(pWidget, pThis);
			if (pWidget->hNext != NULL_HWIDGET)
			{
				pWidget = UI_GetWidget(pWidget->hNext);
			}
			else
			{
				pWidget = NULL;
			}
		}
	}
}

void RootWidget_OnDestroy(struct UIWidget* pThis)
{
	free(pThis->pImplementationData);
}

static void DebugPrintWidget(int indentLvl, struct UIWidget* pWidget, PrintfFn printfFn)
{
	struct RootWidgetData* pRootWidgetData = pWidget->pImplementationData;
	for (int i = 0; i < indentLvl; i++)
	{
		printfFn("\t");
	}
	printfFn("Root width = %i, height = %i\n", pRootWidgetData->windowW, pRootWidgetData->windowH);
	HWidget child = pWidget->hFirstChild;
	while (child != NULL_HWIDGET)
	{
		struct UIWidget* pChildWidget = UI_GetWidget(child);
		if (pChildWidget->fnOnDebugPrint)
		{
			pChildWidget->fnOnDebugPrint(indentLvl + 1, pChildWidget, printfFn);
		}
		child = pChildWidget->hNext;
	}
}

static void* OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	return UI_Helper_OnOutputVerts(pWidget, pOutVerts);
}

void MakeIntoRootWidget(HWidget widget)
{
	struct UIWidget* pWidget = UI_GetWidget(widget);
	struct RootWidgetData* pRootWidgetData = malloc(sizeof(struct RootWidgetData));
	memset(pRootWidgetData, 0, sizeof(struct RootWidgetData));
	pRootWidgetData->windowW = Mn_GetScreenWidth();
	pRootWidgetData->windowH = Mn_GetScreenHeight();
	pRootWidgetData->bIsDirty = true;

	pWidget->pImplementationData = pRootWidgetData;
	pWidget->fnGetWidth = &RootWidget_GetWidth;
	pWidget->fnGetHeight = &RootWidget_GetHeight;
	pWidget->fnLayoutChildren = &RootWidget_LayoutChildren;
	pWidget->fnOnDestroy = &RootWidget_OnDestroy;
	pWidget->fnOnDebugPrint = &DebugPrintWidget;
	pWidget->fnOutputVertices = &OnOutputVerts;
	pWidget->left = 0.0f;
	pWidget->top = 0.0f;
}

HWidget NewRootWidget()
{
	HWidget w = UI_NewBlankWidget();
	MakeIntoRootWidget(w);
	return w;
}

void RootWidget_OnWindowSizeChanged(HWidget rootWidget, int newW, int newH)
{
	struct UIWidget* pWidget = UI_GetWidget(rootWidget);
	struct RootWidgetData* pRootData = pWidget->pImplementationData;
	pRootData->windowH = newH;
	pRootData->windowW = newW;
}
bool GetRootWidgetIsDirty(HWidget hRootWidget)
{
	struct UIWidget* pWidget = UI_GetWidget(hRootWidget);
	struct RootWidgetData* pRootData = pWidget->pImplementationData;
	return pRootData->bIsDirty;
}

// if dirty, a whole tree rebuild will be triggered
void SetRootWidgetIsDirty(HWidget hRootWidget, bool bIsDirty)
{
	struct UIWidget* pWidget = UI_GetWidget(hRootWidget);
	struct RootWidgetData* pRootData = pWidget->pImplementationData;
	pRootData->bIsDirty = bIsDirty;
}

