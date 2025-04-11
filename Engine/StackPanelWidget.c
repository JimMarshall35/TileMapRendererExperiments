#include "StackPanelWidget.h"
#include "Widget.h"
#include <stdlib.h>

typedef enum
{
	SPO_Horizontal,
	SPO_Vertical
}StackPanelOrientation;



struct StackPanelWidgetData
{
	StackPanelOrientation orientation;
	struct WidgetPadding padding;
};

static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct StackPanelWidgetData* pStackPanelData = pWidget->pImplementationData;
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
	return width;
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct StackPanelWidgetData* pStackPanelData = pWidget->pImplementationData;
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
	return height;
}

static void LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{

}

static void OnDestroy(struct UIWidget* pWidget)
{

}

static void MakeWidgetIntotStackPanel(HWidget hWidget, struct xml_node* pXMLNode)
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
	pWidget->pImplementationData = malloc(sizeof(struct StackPanelWidgetData));
	
}

HWidget StackPanelWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUIData)
{
	
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntotStackPanel(hWidget, pXMLNode);
	return hWidget;
}
