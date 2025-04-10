#include "RootWidget.h"
#include "Widget.h"
        

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
		while (pWidget)
		{
			pWidget->fnLayoutChildren(pWidget, pThis);
			if (pWidget->hNext != NULL_HWIDGET)
			{
				pWidget = UI_GetWidget(pWidget->hNext);
			}
		}
	}
}

void RootWidget_OnDestroy(struct UIWidget* pThis)
{
	free(pThis->pImplementationData);
}

HWidget NewRootWidget()
{
	struct RootWidgetData* pRootWidgetData = malloc(sizeof(struct RootWidgetData));
	return UI_NewWidget(NULL_HWIDGET, NULL_HWIDGET, NULL_HWIDGET, NULL_HWIDGET,
		pRootWidgetData,
		&RootWidget_GetWidth,
		&RootWidget_GetHeight,
		&RootWidget_LayoutChildren,
		&RootWidget_OnDestroy,
		0, 0);
}

