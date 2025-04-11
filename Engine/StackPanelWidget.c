#include "StackPanelWidget.h"
#include "Widget.h"
#include <stdlib.h>
#include <assert.h>
#include "xml.h"

typedef enum
{
	SPO_Horizontal,
	SPO_Vertical
}StackPanelOrientation;




struct StackPanelWidgetData
{
	StackPanelOrientation orientation;
	struct WidgetPadding padding;
	WidgetDockPoint dockPoint;
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
	struct StackPanelWidgetData* pStackPanelData = pWidget->pImplementationData;

	if (pWidget->hFirstChild == NULL_HWIDGET)
	{
		return;
	}

	float top = pWidget->top;
	float left = pWidget->left;
	float w = GetWidth(pWidget, pParent);
	float h = GetHeight(pWidget, pParent);


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
					left += GetWidth(pChild, pWidget);
				}
				else
				{
					assert(childHeight < h);
					// TODO: move horizontal + vertical alignment directly into widget class, query here, centre alignment implemented currently
					pChild->left = left;
					pChild->top = top + (h - childHeight) * 0.5;
					left += GetWidth(pChild, pWidget);
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
					top += GetHeight(pChild, pWidget);
				}
				else
				{
					assert(childWidth < w);
					// TODO: move horizontal + vertical alignment directly into widget class, query here, centre alignment implemented currently
					pChild->left = left + (w - childWidth) * 0.5f;
					pChild->top = top;
					top += GetHeight(pChild, pWidget);
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

}

static void OnDestroy(struct UIWidget* pWidget)
{
	assert(pWidget->pImplementationData);
	free(pWidget->pImplementationData);
}

static void MakeWidgetIntoStackPanel(HWidget hWidget, struct xml_node* pXMLNode)
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
	memset(pWidget->pImplementationData, 0, sizeof(struct StackPanelWidgetData));
	struct StackPanelWidgetData* pData = pWidget->pImplementationData;
	int numAttributes = xml_node_attributes(pXMLNode);
	char attribNameBuf[256];
	char attribValBuf[256];
	for (int i = 0; i < numAttributes; i++)
	{
		struct xml_string* pAttribName = xml_node_attribute_name(pXMLNode, i);
		int len = xml_string_length(pAttribName);
		xml_string_copy(pAttribName, attribNameBuf, len);
		attribNameBuf[len] = 0;
		if (strcmp(attribNameBuf, "orientation") == 0)
		{
			struct xml_string* pAttribVal = xml_node_attribute_content(pXMLNode, i);
			int attribValLen = xml_string_length(pAttribVal);
			xml_string_copy(pAttribVal, attribValBuf, attribValLen);
			attribValBuf[attribValLen] = 0;
			if (strcmp(attribValBuf, "horizontal") == 0)
			{
				pData->orientation = SPO_Horizontal;
			}
			else if(strcmp(attribValBuf, "vertical") == 0)
			{
				pData->orientation = SPO_Vertical;
			}
			else
			{
				pData->orientation = SPO_Vertical;
			}
		}
		else if (strcmp(attribNameBuf, "dockPoint"))
		{
			struct xml_string* pAttribVal = xml_node_attribute_content(pXMLNode, i);
			int attribValLen = xml_string_length(pAttribVal);
			xml_string_copy(pAttribVal, attribValBuf, attribValLen);
			attribValBuf[attribValLen] = 0;
			if (strcmp(attribValBuf, "topLeft") == 0)
			{
			}
			else if (strcmp(attribValBuf, "topMiddle") == 0)
			{
			}
			else if (strcmp(attribValBuf, "topRight") == 0)
			{
			}
			else if (strcmp(attribValBuf, "middleRight") == 0)
			{
			}
			else if (strcmp(attribValBuf, "bottomRight") == 0)
			{
			}
			else if (strcmp(attribValBuf, "bottomMiddle") == 0)
			{
			}
			else if (strcmp(attribValBuf, "bottomLeft") == 0)
			{
			}
			else if (strcmp(attribValBuf, "middleLeft") == 0)
			{
			}
			else if (strcmp(attribValBuf, "centre") == 0)
			{
			}
			else
			{
				pData->orientation = SPO_Vertical;
			}

		}
	}
	UI_ParseWidgetPaddingAttributes(pXMLNode, &pData->padding);
	
}

HWidget StackPanelWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUIData)
{
	
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoStackPanel(hWidget, pXMLNode);
	return hWidget;
}
