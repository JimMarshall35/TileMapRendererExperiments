#include "TextEntryWidget.h"
#include "Widget.h"
#include <libxml/tree.h>
#include "XMLUIGameLayer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Atlas.h"
#include "AssertLib.h"
#include "TextWidget.h"
#include "CanvasWidget.h"

struct TextEntryWidgetData
{
	struct TextWidgetData textWidget;
	struct CanvasData canvasWidget;
	
};

static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return UI_ResolveWidthDimPxls(pWidget, &pWidget->width) + pWidget->padding.paddingLeft + pWidget->padding.paddingRight;
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct TextEntryWidgetData* pData = (struct TextEntryWidgetData*)pWidget->pImplementationData;
	return pData->textWidget.fSizePts * 1.33333333 + pWidget->padding.paddingTop + pWidget->padding.paddingBottom;
}

static void LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
}

static void OnDestroy(struct UIWidget* pWidget)
{
	free(pWidget->pImplementationData);
}

static void* FakeChildOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	return TextWidget_OutputVerts(pWidget->left, pWidget->top, &pWidget->padding, pWidget->pImplementationData, pOutVerts);
}

static void* OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	HWidget hFake = UI_GetScratchWiget();
	struct UIWidget* fakeChild = UI_GetWidget(hFake);

	memcpy(fakeChild, pWidget, sizeof(struct UIWidget));
	fakeChild->hNext = NULL_HANDLE;
	fakeChild->fnOutputVertices = &FakeChildOutputVerts;
	fakeChild->top += pWidget->padding.paddingTop;
	fakeChild->left += pWidget->padding.paddingLeft;
	memset(&fakeChild->padding, 0, sizeof(struct WidgetPadding));
	
	pWidget->hFirstChild = hFake;
	pOutVerts = CanvasWidget_OnOutputVerts(pWidget, pOutVerts);
	pWidget->hFirstChild = NULL_HANDLE;
	return pOutVerts;
}

static void OnPropertyChanged(struct UIWidget* pThisWidget, struct WidgetPropertyBinding* pBinding)
{

}

static void RecieveKeystrokeCallback(struct UIWidget* pWidget, int keystroke)
{
	printf("%c\n",(char)keystroke);
}

static void AllocateStringContents(int maxStringLength, struct TextEntryWidgetData* pData)
{
	EASSERT(pData->textWidget.content);
	char* newAlloc = malloc(maxStringLength + 1);
	if(strlen(pData->textWidget.content) >= maxStringLength)
	{
		
		memcpy(newAlloc, pData->textWidget.content, maxStringLength);
		newAlloc[maxStringLength] = '\0';
	}
	else
	{
		strcpy(newAlloc, pData->textWidget.content);
	}
	free(pData->textWidget.content);
	pData->textWidget.content = newAlloc;
}

static void MakeWidgetIntoTextEntryWidget(HWidget hWidget, xmlNode* pXMLNode, struct XMLUIData* pUILayerData)
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
	pWidget->fnOutputVertices = &OnOutputVerts;
	pWidget->fnOnBoundPropertyChanged = &OnPropertyChanged;
	pWidget->fnRecieveKeystroke = &RecieveKeystrokeCallback;
	pWidget->bAcceptsFocus = true;
	pWidget->pImplementationData = malloc(sizeof(struct TextEntryWidgetData));
	memset(pWidget->pImplementationData, 0, sizeof(struct TextEntryWidgetData));
	struct TextEntryWidgetData* pData = pWidget->pImplementationData;
	TextWidget_FromXML(&pData->textWidget, pXMLNode, pUILayerData);
}

HWidget TextEntryWidgetNew(HWidget hParent, xmlNode* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoTextEntryWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}