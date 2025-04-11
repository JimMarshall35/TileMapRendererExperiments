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

static void GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{

}

static void GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{

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
