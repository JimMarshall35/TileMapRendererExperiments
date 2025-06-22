#include "Widget.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "Scripting.h"
#include "AssertLib.h"

OBJECT_POOL(struct UIWidget) gWidgetPool = NULL;



#define WIDGET_POOL_BOUNDS_CHECK(handle, rVal) OBJ_POOL_BOUNDS_CHECK(handle, rVal, gWidgetPool)
#define WIDGET_POOL_BOUNDS_CHECK_NO_RETURN(handle) OBJ_POOL_BOUNDS_CHECK_NO_RETURN(handle, gWidgetPool)

static void MakeBlankScriptingCallbacks(struct LuaWidgetCallbacks* pCallbacks)
{
	memset(pCallbacks, 0, sizeof(struct LuaWidgetCallbacks));
}

HWidget UI_NewBlankWidget()
{
	HWidget widget = 0;
	gWidgetPool = GetObjectPoolIndex(gWidgetPool, &widget);
	struct UIWidget* pWidget = &gWidgetPool[widget];
	memset(pWidget, 0, sizeof(struct UIWidget));
	pWidget->hFirstChild = NULL_HWIDGET;
	pWidget->hNext = NULL_HWIDGET;
	pWidget->hPrev = NULL_HWIDGET;
	pWidget->hParent = NULL_HWIDGET;
	pWidget->numBindings = 0;
	pWidget->width.type = WD_Auto;
	pWidget->height.type = WD_Auto;
	MakeBlankScriptingCallbacks(&pWidget->scriptCallbacks);
	return widget;
}

size_t UI_CountWidgetChildrenPtr(struct UIWidget* pWidget)
{
	size_t count = 0;
	HWidget h = pWidget->hFirstChild;
	while (h != NULL_HWIDGET)
	{
		struct UIWidget* pChild = UI_GetWidget(h);
		count++;
		h = pChild->hNext;
	}
	return count;
}

size_t UI_CountWidgetChildren(HWidget hWidget)
{
	WIDGET_POOL_BOUNDS_CHECK(hWidget, 0);
	struct UIWidget* pWidget = UI_GetWidget(hWidget);
	return UI_CountWidgetChildrenPtr(pWidget);
}

struct UIWidget* UI_GetWidget(HWidget hWidget)
{
	if (hWidget < 0)
	{
		return NULL;
	}
	return &gWidgetPool[hWidget];
}

void UI_AddChild(HWidget hParent, HWidget hChild)
{
	WIDGET_POOL_BOUNDS_CHECK_NO_RETURN(hParent)
	WIDGET_POOL_BOUNDS_CHECK_NO_RETURN(hChild)
	struct UIWidget* pParent = UI_GetWidget(hParent);
	struct UIWidget* pChild = UI_GetWidget(hChild);
	pChild->hParent = hParent;
	if (pParent->hFirstChild == NULL_HWIDGET)
	{
		pParent->hFirstChild = hChild;
		return;
	}
	
	struct UIWidget* pLastChild = UI_GetWidget(pParent->hFirstChild);
	HWidget hLastWidget = hChild;
	while (pLastChild->hNext != NULL_HWIDGET)
	{
		hLastWidget = pLastChild->hNext;
		pLastChild = UI_GetWidget(pLastChild->hNext);
	}
	pLastChild->hNext = hChild;
	pChild->hPrev = hLastWidget;

}

void UI_Init()
{
	gWidgetPool = NEW_OBJECT_POOL(struct UIWidget, 256);
}

void UI_DestroyWidget(HWidget widget)
{
	WIDGET_POOL_BOUNDS_CHECK_NO_RETURN(widget)
	struct UIWidget* pWidget = UI_GetWidget(widget);
	if (pWidget->hNext != NULL_HWIDGET)
	{
		UI_GetWidget(pWidget->hNext)->hPrev = pWidget->hPrev;
	}
	if (pWidget->hPrev != NULL_HWIDGET)
	{
		UI_GetWidget(pWidget->hPrev)->hNext = pWidget->hNext;
	}
	if (widget >= 0 && widget < ObjectPoolCapacity(gWidgetPool))
	{
		HWidget child = gWidgetPool[widget].hFirstChild;
		while (child != NULL_HWIDGET)
		{
			gWidgetPool[widget].fnOnDestroy(&gWidgetPool[widget]);
			UI_DestroyWidget(child);
			child = gWidgetPool[child].hNext;
		}
		FreeObjectPoolIndex(gWidgetPool, widget);
	}
	else
	{
		printf("DestroyWidget: widget %i out of range", widget);
	}
}

void UI_ParseWidgetDimsAttribute(const char* attributeContent, struct WidgetDim* outWidgetDims)
{
	if (strcmp(attributeContent, "auto") == 0)
	{
		outWidgetDims->type = WD_Auto;
		return;
	}
	if (strcmp(attributeContent, "*") == 0)
	{
		outWidgetDims->type = WD_Stretch;
		return;
	}

	char dimType[16];
	float dimVal = 0.0f;
	int numMatched = sscanf(attributeContent, "%f%s", &dimVal, dimType);
	switch (numMatched)
	{
	case 1:
		outWidgetDims->type = WD_Pixels;
		break;
	case 2:
		if (strcmp(dimType, "px") == 0)
		{
			outWidgetDims->type = WD_Pixels;
		}
		else if (strcmp(dimType, "*") == 0)
		{
			outWidgetDims->type = WD_StretchFraction;
		}
		else if (strcmp(dimType, "%") == 0)
		{
			outWidgetDims->type = WD_Percentage;
		}
		break;
	default:
		printf("invalid widget dim value %s\n", attributeContent);
		EASSERT(false);
		break;
	}
	outWidgetDims->data = dimVal;
}

void UI_ParseWidgetPaddingAttributes(xmlNode* pInNode, struct WidgetPadding* outWidgetPadding)
{
	xmlChar* attribute = NULL;
	if(attribute = xmlGetProp(pInNode, "paddingTop"))
	{
		outWidgetPadding->paddingTop = (float)atof(attribute);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pInNode, "paddingBottom"))
	{
		outWidgetPadding->paddingBottom = (float)atof(attribute);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pInNode, "paddingLeft"))
	{
		outWidgetPadding->paddingLeft = (float)atof(attribute);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pInNode, "paddingRight"))
	{
		outWidgetPadding->paddingRight = (float)atof(attribute);
		xmlFree(attribute);
	}

}

void UI_ParseHorizontalAlignementAttribute(xmlChar* contents, enum WidgetHorizontalAlignment* outAlignment)
{
	if (strcmp(contents, "left") == 0)
	{
		*outAlignment = WHA_Left;
	}
	else if (strcmp(contents, "middle") == 0)
	{
		*outAlignment = WHA_Middle;
	}
	else if (strcmp(contents, "right") == 0)
	{
		*outAlignment = WHA_Right;
	}
	else
	{
		*outAlignment = WHA_Middle;
	}
}

void UI_ParseVerticalAlignementAttribute(xmlChar* contents, enum WidgetVerticalAlignment* outAlignment)
{
	if (strcmp(contents, "top") == 0)
	{
		*outAlignment = WVA_Top;
	}
	else if (strcmp(contents, "middle") == 0)
	{
		*outAlignment = WVA_Middle;
	}
	else if (strcmp(contents, "bottom") == 0)
	{
		*outAlignment = WVA_Bottom;
	}
	else
	{
		*outAlignment = WVA_Middle;
	}
}

struct WidgetDim* GetWidgetWidthDim(struct UIWidget* pWidget) { return &pWidget->width; }
struct WidgetDim* GetWidgetHeightDim(struct UIWidget* pWidget) { return &pWidget->height; }

/// <summary>
/// helper that finds all child widgets with width type WD_ParentFraction
/// and returns the total of their "*" values
/// </summary>
/// <param name="pWidgetParent"></param>
/// <returns></returns>
static void GetTotalFractionAmongChildren(struct UIWidget* pWidgetParent, float* outV, WidgetDimGetterFn getter)
{
	*outV = 0.0f;
	HWidget hChild = pWidgetParent->hFirstChild;
	while (hChild != NULL_HWIDGET)
	{
		struct UIWidget* pChild = UI_GetWidget(hChild);
		const struct WidgetDim* dim = getter(pChild);

		EASSERT(dim->type == WD_StretchFraction);
		*outV += dim->data;
		hChild = pChild->hNext;
	}
}

struct UIWidget* FindResolveableDimensionAncestor(struct UIWidget* pWidgetParent, WidgetDimGetterFn getter)
{
	HWidget hParent = pWidgetParent->hParent;
	while (hParent != NULL_HWIDGET)
	{
		struct UIWidget* pParent = UI_GetWidget(hParent);
		EASSERT(pParent);
		const struct WidgetDim* pDim = getter(pParent);
		if (pDim->type == WD_Percentage || pDim->type == WD_StretchFraction || pDim->type == WD_Pixels)
		{
			return pParent;
		}
		hParent = pParent->hParent;
	}
	EASSERT(false); // root widget must be of of fixed size
	return NULL;
}

float UI_ResolveWidgetDimPxls(struct UIWidget* pWidget, WidgetDimGetterFn getter, GetUIWidgetDimensionFn autoFn)
{
	const struct WidgetDim* dim = getter(pWidget);
	switch (dim->type)
	{
	case WD_Auto:            
	{
		printf("fjdifjdifd");
		return autoFn(pWidget, UI_GetWidget(pWidget->hParent));
	}
	case WD_Pixels:          return dim->data;
	case WD_StretchFraction:
	{
		float wFraction;
		//EASSERT(pWidgetParent->width.type != WD_Auto);
		struct UIWidget*  pResolvableWidgetParent = FindResolveableDimensionAncestor(pWidget, getter);
		struct UIWidget* pActualWidgetParent = UI_GetWidget(pWidget->hParent);
		GetTotalFractionAmongChildren(pActualWidgetParent, &wFraction, getter);
		float parentW = UI_ResolveWidgetDimPxls(pResolvableWidgetParent, getter, autoFn);
		return (dim->data / wFraction) * parentW;
	}
	case WD_Percentage:
	{
		struct UIWidget* pWidgetParent = FindResolveableDimensionAncestor(pWidget, getter);
		float parentW = UI_ResolveWidgetDimPxls(pWidgetParent, getter, autoFn);
		return dim->data * parentW;
	}
	default:
		break;
	}
	return dim->data;
}

bool UI_ParseWidgetDockPoint(xmlNode* pInNode, struct UIWidget* outWidget)
{
	xmlChar* attribute = NULL;
	if(attribute = xmlGetProp(pInNode, "dockPoint"))
	{
		if (strcmp(attribute, "topLeft") == 0)
		{
			outWidget->dockPoint = WDP_TopLeft;
			return true;
		}
		else if (strcmp(attribute, "topMiddle") == 0)
		{
			outWidget->dockPoint = WDP_TopMiddle;
			return true;
		}
		else if (strcmp(attribute, "topRight") == 0)
		{
			outWidget->dockPoint = WDP_TopRight;
			return true;
		}
		else if (strcmp(attribute, "middleRight") == 0)
		{
			outWidget->dockPoint = WDP_MiddleRight;
			return true;
		}
		else if (strcmp(attribute, "bottomRight") == 0)
		{
			outWidget->dockPoint = WDP_BottomRight;
			return true;
		}
		else if (strcmp(attribute, "bottomMiddle") == 0)
		{
			outWidget->dockPoint = WDP_BottomMiddle;
			return true;
		}
		else if (strcmp(attribute, "bottomLeft") == 0)
		{
			outWidget->dockPoint = WDP_BottomLeft;
			return true;
		}
		else if (strcmp(attribute, "middleLeft") == 0)
		{
			outWidget->dockPoint = WDP_MiddleLeft;
			return true;
		}
		else if (strcmp(attribute, "centre") == 0)
		{
			outWidget->dockPoint = WDP_Centre;
			return true;
		}
		xmlFree(attribute);
	}
	return false;
}

static void ParseLuaCallbacks(xmlNode* pInNode, struct UIWidget* outWidget)
{
	xmlChar* attribute = NULL;
	if(attribute = xmlGetProp(pInNode, "onMouseEnter"))
	{
		outWidget->scriptCallbacks.Callbacks[WC_OnMouseEnter].bActive = true;
		strcpy(
			outWidget->scriptCallbacks.Callbacks[WC_OnMouseEnter].name,
			attribute
		);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pInNode, "onMouseLeave"))
	{
		outWidget->scriptCallbacks.Callbacks[WC_OnMouseLeave].bActive = true;
		strcpy(
			outWidget->scriptCallbacks.Callbacks[WC_OnMouseLeave].name,
			attribute
		);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pInNode, "onMouseMove"))
	{
		outWidget->scriptCallbacks.Callbacks[WC_OnMouseMove].bActive = true;
		strcpy(
			outWidget->scriptCallbacks.Callbacks[WC_OnMouseMove].name,
			attribute
		);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pInNode, "onMouseDown"))
	{
		outWidget->scriptCallbacks.Callbacks[WC_OnMouseDown].bActive = true;
		strcpy(
			outWidget->scriptCallbacks.Callbacks[WC_OnMouseDown].name,
			attribute
		);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pInNode, "onMouseUp"))
	{
		outWidget->scriptCallbacks.Callbacks[WC_OnMouseUp].bActive = true;
		strcpy(
			outWidget->scriptCallbacks.Callbacks[WC_OnMouseUp].name,
			attribute
		);
		xmlFree(attribute);
	}
}

static void ParseWidgetDims(xmlNode* pInNode, struct UIWidget* outWidget)
{
	xmlChar* attribute = NULL;
	if(attribute = xmlGetProp(pInNode, "width"))
	{
		UI_ParseWidgetDimsAttribute(attribute, &outWidget->width);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pInNode, "height"))
	{
		UI_ParseWidgetDimsAttribute(attribute, &outWidget->height);
		xmlFree(attribute);
	}
}

static void ParseWidgetOffsets(xmlNode* pInNode, struct UIWidget* outWidget)
{
	xmlChar* attribute = NULL;
	if(attribute = xmlGetProp(pInNode, "x"))
	{
		outWidget->offsetX = atof(attribute);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pInNode, "y"))
	{
		outWidget->offsetY = atof(attribute);
		xmlFree(attribute);
	}
}

void UI_WidgetCommonInit(xmlNode* pInNode, struct UIWidget* outWidget)
{
	UI_ParseWidgetPaddingAttributes(pInNode, &outWidget->padding);
	UI_ParseWidgetDockPoint(pInNode, outWidget);
	ParseLuaCallbacks(pInNode, outWidget);
	ParseWidgetDims(pInNode, outWidget);
	ParseWidgetOffsets(pInNode, outWidget);
}

static const char* GetDockingPointName(WidgetDockPoint dockingPoint)
{
	switch (dockingPoint)
	{
	case WDP_TopLeft: return "TopLeft";
	case WDP_TopMiddle: return "TopMiddle";
	case WDP_TopRight: return "TopRight";
	case WDP_MiddleRight: return "MiddleRight";
	case WDP_BottomRight: return "BottomRight";
	case WDP_BottomMiddle:  return "BottomMiddle";
	case WDP_BottomLeft:  return "BottomLeft";
	case WDP_MiddleLeft:  return "MiddleLeft";
	default: return "Unknown";
	}
}

void UI_DebugPrintCommonWidgetInfo(const struct UIWidget* inWidget, PrintfFn pPrintfFn)
{
	pPrintfFn("paddingTop = %.1f, paddingBottom = %.1f, paddingLeft = %.1f, paddingRight = %.1f, left = %.1f, top = %.1f, dockPoint = %s",
		inWidget->padding.paddingTop,
		inWidget->padding.paddingBottom,
		inWidget->padding.paddingLeft,
		inWidget->padding.paddingRight,
		inWidget->left,
		inWidget->top,
		GetDockingPointName(inWidget->dockPoint));
}

void* UI_Helper_OnOutputVerts(struct UIWidget* pWidget, VECTOR(struct WidgetVertex) pOutVerts)
{
	HWidget child = pWidget->hFirstChild;
	while (child != NULL_HWIDGET)
	{
		struct UIWidget* pChildWidget = UI_GetWidget(child);
		if (pChildWidget->fnOutputVertices)
		{
			pOutVerts = pChildWidget->fnOutputVertices(pChildWidget, pOutVerts);
		}
		child = pChildWidget->hNext;
	}
	return pOutVerts;
}

void UI_Helper_OnLayoutChildren(struct UIWidget* pWidget, struct UIWidget* pParent)
{
	if (pWidget->hFirstChild == NULL_HWIDGET)
	{
		return;
	}
	struct UIWidget* pChild = UI_GetWidget(pWidget->hFirstChild);
	while (pChild)
	{
		pChild->fnLayoutChildren(pChild, pWidget);
		pChild = UI_GetWidget(pChild->hNext);
	}
}

void UI_SendWidgetMouseEvent(struct UIWidget* pWidget, enum WidgetCallbackTypes type, struct WidgetMouseInfo* pMouseInfo)
{
	if (pWidget->cCallbacks.Callbacks[type].callback.bActive)
	{
		switch (type)
		{
		case WC_OnMouseLeave:
		case WC_OnMouseEnter:
		case WC_OnMouseMove:
			pWidget->cCallbacks.Callbacks[type].callback.mousePosFn(pWidget, pMouseInfo->x, pMouseInfo->y);
			break;
		case WC_OnMouseDown:
		case WC_OnMouseUp:
			pWidget->cCallbacks.Callbacks[type].callback.mouseBtnFn(pWidget, pMouseInfo->x, pMouseInfo->y, pMouseInfo->button);
			break;
		
		}
	}
	if (!pWidget->scriptCallbacks.Callbacks[type].bActive)
	{
		return;
	}
	int numArguments = 0;
	struct ScriptCallArgument arguments[8];

	switch (type)
	{
	case WC_OnMouseLeave:
	case WC_OnMouseEnter:
	case WC_OnMouseMove:
	{
		struct ScriptCallArgument* pX = &arguments[numArguments++];
		struct ScriptCallArgument* pY = &arguments[numArguments++];
		pX->type = SCA_number;
		pX->val.number = pMouseInfo->x;
		pY->type = SCA_number;
		pY->val.number = pMouseInfo->y;
	}
		break;
	case WC_OnMouseDown:
	case WC_OnMouseUp:
	{
		struct ScriptCallArgument* pX = &arguments[numArguments++];
		struct ScriptCallArgument* pY = &arguments[numArguments++];
		struct ScriptCallArgument* pMouseButton = &arguments[numArguments++];

		pX->type = SCA_number;
		pX->val.number = pMouseInfo->x;
		pY->type = SCA_number;
		pY->val.number = pMouseInfo->y;
		pMouseButton->type = SCA_number;
		pMouseButton->val.number = pMouseInfo->button;
	}
		break;
	}

	Sc_CallFuncInRegTableEntryTable(
		pWidget->scriptCallbacks.viewmodelTable,
		pWidget->scriptCallbacks.Callbacks[type].name,
		&arguments[0], 
		numArguments,
		0
	);
}

void UI_GetWidgetSize(HWidget hWidget, float* pOutW, float* pOutH)
{
	WIDGET_POOL_BOUNDS_CHECK_NO_RETURN(hWidget);

	struct UIWidget* pWidget = UI_GetWidget(hWidget);
	*pOutW = pWidget->fnGetWidth(pWidget, NULL);
	*pOutH = pWidget->fnGetHeight(pWidget, NULL);
}

void UI_GetWidgetPadding(HWidget hWidget, float* pOutPaddingTop, float* pOutPaddingBottom, float* pOutPaddingLeft, float* pOutPaddingRight)
{
	WIDGET_POOL_BOUNDS_CHECK_NO_RETURN(hWidget);

	struct UIWidget* pWidget = UI_GetWidget(hWidget);
	*pOutPaddingTop = pWidget->padding.paddingTop;
	*pOutPaddingBottom = pWidget->padding.paddingBottom;
	*pOutPaddingLeft = pWidget->padding.paddingLeft;
	*pOutPaddingRight = pWidget->padding.paddingRight;
}

void UI_GetWidgetTopLeft(HWidget hWidget, float* pOutLeft, float* pOutTop)
{
	WIDGET_POOL_BOUNDS_CHECK_NO_RETURN(hWidget);
	struct UIWidget* pWidget = UI_GetWidget(hWidget);
	*pOutLeft = pWidget->left;
	*pOutTop = pWidget->top;
}	

void UI_GetHitBox(GeomRect outRect, HWidget hWidget)
{
	float padTop, padBot, padLeft, padRight;
	float top, left;
	float widgth, height;
	UI_GetWidgetPadding(hWidget, &padTop, &padBot, &padLeft, &padRight);
	UI_GetWidgetTopLeft(hWidget, &left, &top);
	UI_GetWidgetSize(hWidget, &widgth, &height);
	widgth -= padLeft + padRight;
	height -= padTop + padBot;
	outRect[GR_TLX] = left + padLeft;
	outRect[GR_TLY] = top + padTop;
	outRect[GR_BRX] = outRect[GR_TLX] + widgth;
	outRect[GR_BRY] = outRect[GR_TLY] + height;
}

bool UI_IsAttributeStringABindingExpression(const char* attributeValue)
{
	return attributeValue[0] == '{' && attributeValue[strlen(attributeValue) - 1] == '}';
}


char* UI_MakeBindingGetterFunctionName(const char* inBindingName)
{
	char* fnName = malloc(strlen(inBindingName) + 1 + 4);
	char* writePtr = fnName;
	*writePtr++ = 'G';
	*writePtr++ = 'e';
	*writePtr++ = 't';
	*writePtr++ = '_';
	strcpy(writePtr, inBindingName);
	return fnName;
}

char* UI_MakeBindingSetterFunctionName(const char* inBindingName)
{
	char* fnName = malloc(strlen(inBindingName) + 1 + 4);
	char* writePtr = fnName;
	*writePtr++ = 'S';
	*writePtr++ = 'e';
	*writePtr++ = 't';
	*writePtr++ = '_';
	strcpy(writePtr, inBindingName);
	return fnName;
}

struct WidgetPropertyBinding* UI_FindBinding(struct UIWidget* pWidget, const char* bindingName)
{
	for (int i = 0; i < pWidget->numBindings; i++)
	{
		if (strcmp(pWidget->bindings[i].boundPropertyName, bindingName) == 0)
		{
			return &pWidget->bindings[i];
		}
	}
	return NULL;
}


static char* PopulateBinding(struct WidgetPropertyBinding* pBinding, char* inBindingExpression, char* inBoundPropertyName)
{
	inBindingExpression[strlen(inBindingExpression) - 1] = '\0';
	inBindingExpression++;
	strcpy(pBinding->name, inBindingExpression);
	strcpy(pBinding->boundPropertyName, inBoundPropertyName);
	return inBindingExpression;
}

void UI_AddStringPropertyBinding(struct UIWidget* pWidget, char* inBoundPropertyName, char* inBindingExpression, char** pOutData, int viewmodelTableIndex)
{
	if (pWidget->numBindings >= MAX_NUM_BINDINGS)
	{
		printf("MAX_NUM_BINDINGS exceeded\n");
		EASSERT(false);
		return;
	}
	struct WidgetPropertyBinding* pBinding = &pWidget->bindings[pWidget->numBindings++];
	inBindingExpression = PopulateBinding(pBinding, inBindingExpression, inBoundPropertyName);

	pBinding->type = WBT_String;
	pBinding->data.str = NULL;
	char* fnName = UI_MakeBindingGetterFunctionName(inBindingExpression);

	Sc_CallFuncInRegTableEntryTable(viewmodelTableIndex, fnName, NULL, 0, 1);
	size_t strLen =  Sc_StackTopStringLen();
	*pOutData = malloc(strLen + 1);
	Sc_StackTopStrCopy(*pOutData);
	Sc_ResetStack();
	free(fnName);
}

void UI_AddIntPropertyBinding(struct UIWidget* pWidget, char* inBoundPropertyName, char* inBindingExpression, int* pOutData, int viewmodelTableIndex)
{
	if (pWidget->numBindings >= MAX_NUM_BINDINGS)
	{
		printf("MAX_NUM_BINDINGS exceeded\n");
		EASSERT(false);
		return;
	}
	struct WidgetPropertyBinding* pBinding = &pWidget->bindings[pWidget->numBindings++];
	inBindingExpression = PopulateBinding(pBinding, inBindingExpression, inBoundPropertyName);

	pBinding->type = WBT_Int;
	pBinding->data.str = NULL;
	char* fnName = UI_MakeBindingGetterFunctionName(inBindingExpression);

	Sc_CallFuncInRegTableEntryTable(viewmodelTableIndex, fnName, NULL, 0, 1);
	int i = Sc_Int();
	Sc_ResetStack();
	*pOutData = i;
}

void UI_AddFloatPropertyBinding(struct UIWidget* pWidget, char* inBoundPropertyName, char* inBindingExpression, float* pOutData, int viewmodelTableIndex)
{
	if (pWidget->numBindings >= MAX_NUM_BINDINGS)
	{
		printf("MAX_NUM_BINDINGS exceeded\n");
		EASSERT(false);
		return;
	}
	struct WidgetPropertyBinding* pBinding = &pWidget->bindings[pWidget->numBindings++];
	inBindingExpression = PopulateBinding(pBinding, inBindingExpression, inBoundPropertyName);

	pBinding->type = WBT_Float;
	pBinding->data.str = NULL;
	char* fnName = UI_MakeBindingGetterFunctionName(inBindingExpression);

	Sc_CallFuncInRegTableEntryTable(viewmodelTableIndex, fnName, NULL, 0, 1);
	float i = Sc_Float();
	Sc_ResetStack();
	*pOutData = i;

}
