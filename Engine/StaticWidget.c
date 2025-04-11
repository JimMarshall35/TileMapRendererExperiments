#include "StaticWidget.h"
#include "Widget.h"
#include "xml.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


struct StaticWidgetData
{
	char* imageName;
	struct WidgetPadding padding;
	struct WidgetScale scale;
	struct WidgetDim width;
	struct WidgetDim height;
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



static void MakeWidgetIntoStatic(HWidget hWidget, struct xml_node* pXMLNode)
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
	pWidget->pImplementationData = malloc(sizeof(struct StaticWidgetData));
	memset(pWidget->pImplementationData, 0, sizeof(struct StaticWidgetData));
	struct StaticWidgetData* pWidgetData = pWidget->pImplementationData;
	size_t numAttributes = xml_node_attributes(pXMLNode);
	char attributeBuffer[256];
	for (int i = 0; i < numAttributes; i++)
	{
		struct xml_string* name = xml_node_attribute_name(pXMLNode, i);
		int nameLen = xml_string_length(name);
		if (nameLen >= 256)
		{
			printf("function %s, namelen > 256. namelen is %i", __FUNCTION__, nameLen);
			continue;
		}
		memset(attributeBuffer, 0, 256);
		xml_string_copy(name, attributeBuffer, nameLen);
		if (strcmp(attributeBuffer, "sprite") == 0)
		{
			struct xml_string* contents = xml_node_attribute_content(pXMLNode, i);
			pWidgetData->imageName = malloc(xml_string_length(contents) + 1);
			xml_string_copy(contents, pWidgetData->imageName, xml_string_length(contents));
			pWidgetData->imageName[xml_string_length(contents)] = '\0';
		}
		else if (strcmp(attributeBuffer, "width"))
		{
			struct xml_string* contents = xml_node_attribute_content(pXMLNode, i);
			UI_ParseWidgetDimsAttribute(contents, &pWidgetData->width);

		}
		else if (strcmp(attributeBuffer, "height"))
		{

		}
	}
}

HWidget StaticWidgetNew(HWidget hParent, struct xml_node* pXMLNode)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoStatic(hWidget, pXMLNode);
	return hWidget;
}
