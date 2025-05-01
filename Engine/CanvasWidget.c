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
#include "Geometry.h"
#include "WidgetVertexOutputHelpers.h"


struct CanvasData
{
	float scrollX;
	float scrollY;
	GeomRect contentBB;
};

static float BBWidth(GeomRect r)
{
	return r[2] - r[0];

}

static float BBHeight(GeomRect r)
{
	return r[3] - r[1];
}


static float GetWidth(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return UI_ResolveWidgetDimPxls(pWidget, &GetWidgetWidthDim, pWidget->fnGetWidth);
}

static float GetHeight(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	return UI_ResolveWidgetDimPxls(pWidget, &GetWidgetHeightDim, pWidget->fnGetHeight);
}

/// <summary>
/// STORES RESULT IN tl1, br1
/// </summary>
static void UnionRects(vec2 tl1, vec2 br1, vec2 tl2, vec2 br2)
{
	if (tl2[0] < tl1[0])
	{
		tl1[0] = tl2[0];
	}
	if (tl2[1] < tl1[1])
	{
		tl1[1] = tl2[1];
	}
	if (br2[0] > br1[0])
	{
		br1[0] = br2[0];
	}
	if (br2[1] > br1[1])
	{
		br1[1] = br2[1];
	}
}

static void GetChildrenBoundingBox(struct UIWidget* pWidget, vec2 tl, vec2 br)
{
	bool bSet = false;
	HWidget hChild = pWidget->hFirstChild;
	while (hChild != NULL_HWIDGET)
	{

		vec2 tl0, br0;
		struct UIWidget* pChild = UI_GetWidget(hChild);
		tl0[0] = pChild->left;
		tl0[1] = pChild->top;
		vec2 dims;
		UI_GetWidgetSize(hChild, &dims[0], &dims[1]);
		glm_vec2_add(tl0, dims, br0);
		if (!bSet)
		{
			memcpy(tl, tl0, sizeof(vec2));
			memcpy(br, br0, sizeof(vec2));
			bSet = true;
			hChild = pChild->hNext;
			continue; // nothing to see here
		}
		UnionRects(tl, br, tl0, br0);
	}
}

static void LayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	struct CanvasData* pData = pWidget->pImplementationData;
	RootWidget_LayoutChildren(pWidget, pParent, pData->scrollX, pData->scrollY);
	GetChildrenBoundingBox(pWidget, pData->contentBB, &pData->contentBB[2]);
}

static void OnDestroy(struct UIWidget* pWidget)
{
	free(pWidget->pImplementationData);
}

static void OnDebugPrint(int indentLvl, struct UIWidget* pWidget, PrintfFn printfFn)
{
}

bool ContentExceedsSize(struct CanvasData* pCanvasData, struct UIWidget* pWidget)
{
	float w = pWidget->fnGetWidth(pWidget, UI_GetWidget(pWidget->hParent)) - (pWidget->padding.paddingRight + pWidget->padding.paddingLeft);
	float h = pWidget->fnGetHeight(pWidget, UI_GetWidget(pWidget->hParent)) - (pWidget->padding.paddingTop + pWidget->padding.paddingBottom);
	
	float contentW = BBWidth(pCanvasData->contentBB);
	float contentH = BBHeight(pCanvasData->contentBB);
	return contentW > w || contentH > h;
}

void GetClipRegion(GeomRect rect, struct UIWidget* pWidget)
{
	float w = pWidget->fnGetWidth(pWidget, UI_GetWidget(pWidget->hParent)) - (pWidget->padding.paddingRight + pWidget->padding.paddingLeft);
	float h = pWidget->fnGetHeight(pWidget, UI_GetWidget(pWidget->hParent)) - (pWidget->padding.paddingTop + pWidget->padding.paddingBottom);
	rect[0] = pWidget->left + pWidget->padding.paddingLeft;
	rect[1] = pWidget->top + pWidget->padding.paddingTop;
	rect[2] = rect[0] + w;
	rect[3] = rect[1] + h;

}

static void* OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	struct CanvasData* pCanvasData = pWidget->pImplementationData;
	if (ContentExceedsSize(pCanvasData, pWidget))
	{
		GeomRect region;
		GetClipRegion(region, pWidget);
		SetClipRect(region);
	}

	pOutVerts = UI_Helper_OnOutputVerts(pWidget, pOutVerts);
	
	UnsetClipRect();
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
	pWidget->pImplementationData = malloc(sizeof(struct CanvasData));
	memset(pWidget->pImplementationData, 0, sizeof(struct CanvasData));
}

HWidget CanvasWidgetNew(HWidget hParent, struct xml_node* pXMLNode, struct XMLUIData* pUILayerData)
{
	HWidget hWidget = UI_NewBlankWidget();
	MakeWidgetIntoCanvasWidget(hWidget, pXMLNode, pUILayerData);
	return hWidget;
}