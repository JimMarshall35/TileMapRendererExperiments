#include "StackPanelWidget.h"
#include "Widget.h"
#include <stdlib.h>
#include "AssertLib.h"
#include <string.h>
#include <libxml/tree.h>


void StackPanel_PopulateDataFromXML(xmlNode* pXMLNode, struct StackPanelWidgetData* pData)
{
	xmlChar* attribute = NULL;
	if(attribute = xmlGetProp(pXMLNode, "orientation"))
	{
		if (strcmp(attribute, "horizontal") == 0)
		{
			pData->orientation = SPO_Horizontal;
		}
		else if (strcmp(attribute, "vertical") == 0)
		{
			pData->orientation = SPO_Vertical;
		}
		else
		{
			pData->orientation = SPO_Vertical;
		}
	}
}

float StackPanel_GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent, struct StackPanelWidgetData* pStackPanelData)
{
	float width = 0.0;

	if (pWidget->hFirstChild == NULL_HWIDGET)
	{
		return width;
	}
	switch (pStackPanelData->orientation)
	{
	case SPO_Horizontal:
	{
		struct UIWidget* pChild = UI_GetWidget(pWidget->hFirstChild);
		while (pChild)
		{
			width += pChild->fnGetWidth(pChild, pWidget);
			if (pChild->hNext == NULL_HWIDGET)
			{
				pChild = NULL;
			}
			else
			{
				pChild = UI_GetWidget(pChild->hNext);
			}
		}
		break;
	}
	case SPO_Vertical:
	{
		float widest = 0.0;
		struct UIWidget* pChild = UI_GetWidget(pWidget->hFirstChild);
		while (pChild)
		{
			float thisChildW = pChild->fnGetWidth(pChild, pWidget);
			if (thisChildW > widest)
			{
				widest = thisChildW;
			}
			if (pChild->hNext == NULL_HWIDGET)
			{
				pChild = NULL;
			}
			else
			{
				pChild = UI_GetWidget(pChild->hNext);
			}
		}
		width = widest;
		break;
	}
	}
	return width + pWidget->padding.paddingLeft + pWidget->padding.paddingRight;
}

float StackPanel_GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent, struct StackPanelWidgetData* pStackPanelData)
{
	float height = 0.0;

	if (pWidget->hFirstChild == NULL_HWIDGET)
	{
		return height;
	}
	switch (pStackPanelData->orientation)
	{
	case SPO_Horizontal:
	{
		float tallest = 0.0;
		struct UIWidget* pChild = UI_GetWidget(pWidget->hFirstChild);
		while (pChild)
		{
			float thisChildH = pChild->fnGetHeight(pChild, pWidget);
			if (thisChildH > tallest)
			{
				tallest = thisChildH;
			}
			if (pChild->hNext == NULL_HWIDGET)
			{
				pChild = NULL;
			}
			else
			{
				pChild = UI_GetWidget(pChild->hNext);
			}
		}
		height = tallest;
		break;
	}
	case SPO_Vertical:
	{
		struct UIWidget* pChild = UI_GetWidget(pWidget->hFirstChild);
		while (pChild)
		{
			height += pChild->fnGetHeight(pChild, pWidget);
			if (pChild->hNext == NULL_HWIDGET)
			{
				pChild = NULL;
			}
			else
			{
				pChild = UI_GetWidget(pChild->hNext);
			}
		}
		break;
	}
	}
	return height + pWidget->padding.paddingTop + pWidget->padding.paddingBottom;
}

void StackPanel_LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent, struct StackPanelWidgetData* pStackPanelData, float w, float h)
{
	if (pWidget->hFirstChild == NULL_HWIDGET)
	{
		return;
	}

	float top = pWidget->top + pWidget->padding.paddingTop;
	float left = pWidget->left + pWidget->padding.paddingLeft;
	//float w = GetWidth(pWidget, pParent);
	//float h = GetHeight(pWidget, pParent);


	switch (pStackPanelData->orientation)
	{
	case SPO_Horizontal:
	{
		struct UIWidget* pChild = UI_GetWidget(pWidget->hFirstChild);
		while (pChild)
		{
			float childHeight = pChild->fnGetHeight(pChild, pWidget);
			if (childHeight == h)
			{
				pChild->left = left;
				pChild->top = top;
				left += pChild->fnGetWidth(pChild, pWidget);
			}
			else
			{
				EASSERT(childHeight < h);
				switch(pChild->verticalAlignment)
				{
				case WVA_Middle:
					pChild->top = top + (h - childHeight) * 0.5;
					break;
				case WVA_Bottom:
					pChild->top = top + (h - childHeight);
					break;
				case WVA_Top:
					pChild->top = top;
					break;
				default:
					EASSERT(false);
					break;
				}
				pChild->left = left;
				left += pChild->fnGetWidth(pChild, pWidget);

			}

			if (pChild->hNext == NULL_HWIDGET)
			{
				pChild = NULL;
			}
			else
			{
				pChild = UI_GetWidget(pChild->hNext);
			}
		}

		break;
	}
	case SPO_Vertical:
	{
		struct UIWidget* pChild = UI_GetWidget(pWidget->hFirstChild);
		while (pChild)
		{
			float childWidth = pChild->fnGetWidth(pChild, pWidget);
			if (childWidth == w)
			{
				pChild->left = left;
				pChild->top = top;
				top += pChild->fnGetHeight(pChild, pWidget);
			}
			else
			{
				EASSERT(childWidth < w);
				// TODO: move horizontal + vertical alignment directly into widget class, query here, centre alignment implemented currently
				switch(pChild->horizontalAlignment)
				{
				case WHA_Left:
					pChild->left = left;
					break;
				case WHA_Middle:
					pChild->left = left + (w - childWidth) * 0.5f;
					break;
				case WHA_Right:
					pChild->left = left + (w - childWidth);
					break;
				}
				pChild->top = top;
				top += pChild->fnGetHeight(pChild, pWidget);
			}

			if (pChild->hNext == NULL_HWIDGET)
			{
				pChild = NULL;
			}
			else
			{
				pChild = UI_GetWidget(pChild->hNext);
			}
		}
		break;
	}
	}
	UI_Helper_OnLayoutChildren(pWidget, pParent);
}

static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return StackPanel_GetWidth(pWidget, pParent, pWidget->pImplementationData);
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return StackPanel_GetHeight(pWidget, pParent, pWidget->pImplementationData);
}

static void LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct StackPanelWidgetData* pStackPanelData = pWidget->pImplementationData;
	StackPanel_LayoutChildren(pWidget, pParent, pStackPanelData, GetWidth(pWidget, pParent), GetHeight(pWidget, pParent));
}

static void OnDestroy(struct UIWidget* pWidget)
{
	assert(pWidget->pImplementationData);
	free(pWidget->pImplementationData);
}

static void OnDebugPrint(int indentLvl, struct UIWidget* pWidget, PrintfFn printfFn)
{
	for (int i = 0; i < indentLvl; i++)
	{
		printfFn("\t");
	}

	struct StackPanelWidgetData* pData = pWidget->pImplementationData;
	printfFn("StackPanel. orientation = %s, ", pData->orientation == SPO_Horizontal ? "Horizontal" : "Vertical");
	UI_DebugPrintCommonWidgetInfo(pWidget, printfFn);
	printfFn("\n");
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
	pOutVerts = UI_Helper_OnOutputVerts(pWidget, pOutVerts);
	return pOutVerts;
}

static void MakeWidgetIntoStackPanel(HWidget hWidget, xmlNode* pXMLNode)
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
	pWidget->pImplementationData = malloc(sizeof(struct StackPanelWidgetData));
	memset(pWidget->pImplementationData, 0, sizeof(struct StackPanelWidgetData));
	struct StackPanelWidgetData* pData = pWidget->pImplementationData;
	StackPanel_PopulateDataFromXML(pXMLNode, pData);
}

HWidget StackPanelWidgetNew(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUIData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoStackPanel(hWidget, pXMLNode);
	return hWidget;
}
