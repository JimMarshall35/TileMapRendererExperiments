#include "TextWidget.h"
#include "XMLUIGameLayer.h"
#include "xml.h"
#include "Widget.h"
#include "Atlas.h"

struct TextWidgetData
{
	char* content;
	HFont font;
};

static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return 0.0f;
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return 0.0f;
}

static float LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return 0.0f;
}

static void OnDestroy(struct UIWidget* pWidget)
{

}

static void OnDebugPrint(int indentLvl, struct UIWidget* pWidget, PrintfFn printfFn)
{

}

static void* OnOutputVerts(struct UIWidget* pThisWidget, VECTOR(struct WidgetVertex) pOutVerts)
{

}

static void MakeWidgetIntoTextWidget(HWidget hWidget, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
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
	pWidget->pImplementationData = malloc(sizeof(struct TextWidgetData));
	memset(pWidget->pImplementationData, 9, sizeof(struct TextWidgetData));

	struct TextWidgetData* pData = pWidget->pImplementationData;
	char attribName[128];
	char attribContent[256];
	memset(attribName, 0, 128);

	bool bContentSet = false;
	bool bFontSet = false;
	for (int i = 0; i < xml_node_attributes(pXMLNode); i++)
	{
		struct xml_string* pName = xml_node_attribute_name(pXMLNode, i);
		int nameLen = xml_string_length(pName);
		xml_string_copy(pName, attribName, nameLen);
		attribName[nameLen] = '\0';
		
		struct xml_string* pContent = xml_node_attribute_content(pXMLNode, i);
		int contentLen = xml_string_length(pName);
		xml_string_copy(pName, attribName, contentLen);
		attribContent[contentLen] = '\0';
		
		if (strcmp(attribName, "content") == 0)
		{
			pData->content = malloc(strlen(attribName + 1));
			strcpy(pData->content, attribContent);
			bContentSet = true;
		}
		else if (strcmp(attribName, "font") == 0)
		{
			HFont font = At_FindFont(pUILayerData->atlas, attribContent);
			if (font == NULL_HANDLE)
			{
				printf("font '%s' could not be found\n", attribContent);
			}
			pData->font = font;
			bFontSet = true;
		}
	}
	if (!bFontSet || !bContentSet)
	{
		printf("invalid text widget node!\n");
	}

}

HWidget TextWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoTextWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}
