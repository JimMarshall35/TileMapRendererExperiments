#include "XMLUIGameLayer.h"
#include "DrawContext.h"
#include "InputContext.h"
#include "GameFramework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "IntTypes.h"
#include "DynArray.h"
#include "ObjectPool.h"
#include "Widget.h"
#include "Widget.h"
#include "StaticWidget.h"
#include "StackPanelWidget.h"
#include "Atlas.h"
#include "RootWidget.h"
#include "main.h"
#include "TextWidget.h"
#include "BackgroundBoxWidget.h"
#include "Scripting.h"
#include "Geometry.h"
#include "AssertLib.h"
#include "TextButtonWidget.h"
#include "RadioButtonWidget.h"
#include "RadioGroupWidget.h"
#include "SliderWidget.h"
#include "CanvasWidget.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

struct NameConstructorPair
{
	const char* name;
	AddChildFn pCtor;
};

struct NameConstructorPair gNodeNameTable[] =
{
	{"stackpanel",    &StackPanelWidgetNew},     // done
	{"static",        &StaticWidgetNew},         // done
	{"text",          &TextWidgetNew},           // done
	{"backgroundbox", &BackgroundBoxWidgetNew},  // done
	{"textButton",    &TextButtonWidgetNew},     // done
	{"radioGroup",    &RadioGroupWidgetNew},     // done
	{"radioButton",   &RadioButtonWidgetNew},    // done
	{"slider",        &SliderWidgetNew},         // done
	{"canvas",        &CanvasWidgetNew} 
};

AddChildFn LookupWidgetCtor(const char* widgetName)
{
	for (int i = 0; sizeof(gNodeNameTable) / sizeof(struct NameConstructorPair); i++)
	{
		if (strcmp(widgetName, gNodeNameTable[i].name) == 0)
		{
			return gNodeNameTable[i].pCtor;
		}
	}
	return NULL;
}


static void Update(struct GameFrameworkLayer* pLayer, float deltaT)
{
	
}

static void UpdateRootWidget(XMLUIData* pData, DrawContext* dc)
{
	VectorClear(pData->pWidgetVertices);
	struct UIWidget* pRootWidget = UI_GetWidget(pData->rootWidget);
	pRootWidget->fnLayoutChildren(pRootWidget, NULL);
	pData->pWidgetVertices = pRootWidget->fnOutputVertices(pRootWidget, pData->pWidgetVertices);
	dc->UIVertexBufferData(pData->hVertexBuffer, pData->pWidgetVertices, sizeof(struct WidgetVertex) * VectorSize(pData->pWidgetVertices));
	SetRootWidgetIsDirty(pData->rootWidget, false);
}

static void Draw(struct GameFrameworkLayer* pLayer, DrawContext* dc)
{
	XMLUIData* pData = pLayer->userData;
	struct UIWidget* pRootWidget = UI_GetWidget(pData->rootWidget);
	if (!pRootWidget)
	{
		printf("something wrong\n");
		return;
	}

	if (GetRootWidgetIsDirty(pData->rootWidget))
	{
		UpdateRootWidget(pData, dc);
	}
	int size = VectorSize(pData->pWidgetVertices);
	//printf("%i\n", size);

	dc->DrawUIVertexBuffer(pData->hVertexBuffer, size);
}

static HMouseAxisBinding gMouseX = NULL_HANDLE;
static HMouseAxisBinding gMouseY = NULL_HANDLE;
static HMouseButtonBinding gMouseBtnLeft = NULL_HANDLE;

static void* BuildWidgetsHoverred(VECTOR(HWidget) outWidgets, HWidget hWidget, float mouseX, float mouseY)
{
	GeomRect hitbox;
	UI_GetHitBox(hitbox, hWidget);
	if (Ge_PointInAABB(mouseX, mouseY, hitbox))
	{
		outWidgets = VectorPush(outWidgets, &hWidget);
		
		struct UIWidget* pWidget = UI_GetWidget(hWidget);
		HWidget hChild = pWidget->hFirstChild;
		while (hChild != NULL_HWIDGET)
		{
			outWidgets = BuildWidgetsHoverred(outWidgets, hChild, mouseX, mouseY);
			struct UIWidget* pChild = UI_GetWidget(hChild);
			hChild = pChild->hNext;
		}
	}
	return outWidgets;

}

static void Input(struct GameFrameworkLayer* pLayer, InputContext* ctx)
{
	static VECTOR(HWidget) pWidgetsHovverred = NULL;
	static VECTOR(HWidget) pWidgetsHovverredLastFrame = NULL;
	static VECTOR(HWidget) pWidgetsEntered = NULL;
	static VECTOR(HWidget) pWidgetsLeft = NULL;
	static VECTOR(HWidget) pWidgetsRemained = NULL;

	static bool bLastLeftClick = false;
	static bool bThisLeftClick = false;

	if (!pWidgetsHovverred)
	{
		pWidgetsHovverred = NEW_VECTOR(HWidget);
	}
	if (!pWidgetsHovverredLastFrame)
	{
		pWidgetsHovverredLastFrame = NEW_VECTOR(HWidget);
	}
	if (!pWidgetsEntered)
	{
		pWidgetsEntered = NEW_VECTOR(HWidget);
	}
	if (!pWidgetsLeft)
	{
		pWidgetsLeft = NEW_VECTOR(HWidget);
	}
	if (!pWidgetsRemained)
	{
		pWidgetsRemained = NEW_VECTOR(HWidget);
	}

	pWidgetsHovverredLastFrame = VectorClear(pWidgetsHovverredLastFrame);
	for (int i = 0; i < VectorSize(pWidgetsHovverred); i++)
	{
		pWidgetsHovverredLastFrame = VectorPush(pWidgetsHovverredLastFrame, &pWidgetsHovverred[i]);
	}

	pWidgetsHovverred = VectorClear(pWidgetsHovverred);
	pWidgetsEntered = VectorClear(pWidgetsEntered);
	pWidgetsLeft = VectorClear(pWidgetsLeft);
	pWidgetsRemained = VectorClear(pWidgetsRemained);

	XMLUIData* pUIData = pLayer->userData;
	float w, h;

	if (gMouseX == NULL_HANDLE)
	{
		gMouseX = In_FindMouseAxisMapping(ctx, "CursorPosX");
	}
	if (gMouseY == NULL_HANDLE)
	{
		gMouseY = In_FindMouseAxisMapping(ctx, "CursorPosY");
	}
	if (gMouseBtnLeft == NULL_HANDLE)
	{
		gMouseBtnLeft = In_FindMouseBtnMapping(ctx, "select");
	}

	bThisLeftClick = In_GetMouseButtonValue(ctx, gMouseBtnLeft);

	vec2 mousePos = { In_GetMouseAxisValue(ctx, gMouseX), In_GetMouseAxisValue(ctx, gMouseY) };


	pWidgetsHovverred = BuildWidgetsHoverred(pWidgetsHovverred, pUIData->rootWidget, mousePos[0], mousePos[1]);

	// any widgets started to be hoverred?
	for (int i = 0; i < VectorSize(pWidgetsHovverred); i++)
	{
		HWidget hHovvered = pWidgetsHovverred[i];
		bool bWasHoverredLastFrame = false;
		for (int j = 0; j < VectorSize(pWidgetsHovverredLastFrame); j++)
		{
			if (hHovvered == pWidgetsHovverredLastFrame[j])
			{
				bWasHoverredLastFrame = true;
				break;
			}
		}
		if (!bWasHoverredLastFrame)
		{
			pWidgetsEntered = VectorPush(pWidgetsEntered, &hHovvered);
		}
		else
		{
			pWidgetsRemained = VectorPush(pWidgetsRemained, &hHovvered);
		}
	}

	// any widgets stopped being hovvered?
	for (int i = 0; i < VectorSize(pWidgetsHovverredLastFrame); i++)
	{
		HWidget hHovvered = pWidgetsHovverredLastFrame[i];
		bool bHoverredThisFrame = false;
		for (int j = 0; j < VectorSize(pWidgetsHovverred); j++)
		{
			if (hHovvered == pWidgetsHovverred[j])
			{
				bHoverredThisFrame = true;
				break;;
			}
		}
		if (!bHoverredThisFrame)
		{
			pWidgetsLeft = VectorPush(pWidgetsLeft, &hHovvered);
		}
	}

	struct WidgetMouseInfo info =
	{
		.x = mousePos[0],
		.y = mousePos[1],
		.button = 0
	};
	
	bool bSendLMouseDown = false;
	bool bSendLMouseUp = false;
	if (bThisLeftClick && !bLastLeftClick)
	{
		info.button = 0;
		bSendLMouseDown = true;
	}
	else if (!bThisLeftClick && bLastLeftClick)
	{
		info.button = 0;
		bSendLMouseUp = true;
	}

	bLastLeftClick = bThisLeftClick;


	for (int i = 0; i < VectorSize(pWidgetsEntered); i++)
	{
		HWidget hWidget = pWidgetsEntered[i];
		struct UIWidget* pWidget = UI_GetWidget(hWidget);
		UI_SendWidgetMouseEvent(pWidget, WC_OnMouseEnter, &info);
	}
	for (int i = 0; i < VectorSize(pWidgetsLeft); i++)
	{
		HWidget hWidget = pWidgetsLeft[i];
		struct UIWidget* pWidget = UI_GetWidget(hWidget);
		UI_SendWidgetMouseEvent(pWidget, WC_OnMouseLeave, &info);
	}
	for (int i = 0; i < VectorSize(pWidgetsRemained); i++)
	{
		HWidget hWidget = pWidgetsRemained[i];
		struct UIWidget* pWidget = UI_GetWidget(hWidget);
		if (bSendLMouseDown)
		{
			EASSERT(!bSendLMouseUp);
			UI_SendWidgetMouseEvent(pWidget, WC_OnMouseDown, &info);
		}
		if (bSendLMouseUp)
		{
			EASSERT(!bSendLMouseDown);
			UI_SendWidgetMouseEvent(pWidget, WC_OnMouseUp, &info);
		}
		UI_SendWidgetMouseEvent(pWidget, WC_OnMouseMove, &info);
	}
}

static void LoadUIData(XMLUIData* pUIData, DrawContext* pDC);


static void OnPush(struct GameFrameworkLayer* pLayer, DrawContext* drawContext, InputContext* inputContext)
{
	XMLUIData* pData = pLayer->userData;
	pData->pWidgetVertices = NEW_VECTOR(struct WidgetVertex);
	if (!pData->bLoaded)
	{
		LoadUIData(pData, drawContext);
	}
	hTexture hAtlasTex = At_GetAtlasTexture(pData->atlas);
	drawContext->SetCurrentAtlas(hAtlasTex);
}

static void FreeWidgetTree(HWidget root)
{
	struct UIWidget* pWidget = UI_GetWidget(root);
	if(!pWidget)
	{
		return;
	}
	
	HWidget h = pWidget->hFirstChild;
	while(h != NULL_HWIDGET)
	{
		HWidget oldH = h;
		pWidget = UI_GetWidget(h);
			
		h = pWidget->hNext;
		FreeWidgetTree(oldH);
	}
	UI_DestroyWidget(root);
}

static void OnPop(struct GameFrameworkLayer* pLayer, DrawContext* drawContext, InputContext* inputContext)
{
	XMLUIData* pData = pLayer->userData;
	
	DestoryVector(pData->pWidgetVertices);
	drawContext->DestroyVertexBuffer(pData->hVertexBuffer);
	FreeWidgetTree(pData->rootWidget);
	if (pData->hViewModel)
	{
		Sc_DeleteTableInReg(pData->hViewModel);
	}
}

void AddNodeChildren(HWidget widget, xmlNode* pNode, XMLUIData* pUIData)
{
	for (xmlNode* pChild = pNode->children; pChild; pChild = pChild->next)
	{
		if (pChild->type != XML_ELEMENT_NODE)
		{
			continue;
		}
		AddChildFn pCtor = LookupWidgetCtor(pChild->name);
		if (!pCtor)
		{
			// log error
			printf("error occured\n");
			return;
		}
		HWidget childWidget = pCtor(widget, pChild, pUIData);

		struct UIWidget* pWiddget = UI_GetWidget(childWidget);
		pWiddget->scriptCallbacks.viewmodelTable = pUIData->hViewModel;
		UI_WidgetCommonInit(pChild, pWiddget);

		UI_AddChild(widget, childWidget);
		
		AddNodeChildren(childWidget, pChild, pUIData);
	}
}

static void LoadAtlasSprite(xmlNode* pChild, int onChild)
{
	bool bNameset = false;
	bool bPathSet = false;
	bool bTopSet = false;
	bool bLeftSet = false;
	bool bWidthSet = false;
	bool bHeightSet = false;
	bool bAllSet = true;
	int top = 0;
	int left = 0;
	int width = 0;
	int height = 0;

	xmlChar* spritePath = NULL;
	xmlChar* spriteName = NULL;
	xmlChar* attribute = NULL;
	if(attribute = xmlGetProp(pChild, "source"))
	{
		spritePath = attribute;
		bPathSet = true;
	}
	if(attribute = xmlGetProp(pChild, "name"))
	{
		spriteName = attribute;
		bNameset = true;
	}
	if(attribute = xmlGetProp(pChild, "top"))
	{
		top = atoi(attribute);
		xmlFree(attribute);
		bTopSet = true;
	}
	if(attribute = xmlGetProp(pChild, "left"))
	{
		left = atoi(attribute);
		xmlFree(attribute);
		bLeftSet = true;
	}
	if(attribute = xmlGetProp(pChild, "width"))
	{
		width = atoi(attribute);
		xmlFree(attribute);
		bWidthSet = true;
	}
	if(attribute = xmlGetProp(pChild, "height"))
	{
		height = atoi(attribute);
		xmlFree(attribute);
		bHeightSet = true;
	}
	if (!bPathSet)
	{
		printf("%s atlas child %i path not set\n", __FUNCTION__, onChild);
		bAllSet = false;
	}
	if (!bNameset)
	{
		printf("%s atlas child %i name not set\n", __FUNCTION__, onChild);
		bAllSet = false;
	}
	if (!bTopSet)
	{
		printf("%s atlas child %i top not set\n", __FUNCTION__, onChild);
		bAllSet = false;
	}
	if (!bLeftSet)
	{
		printf("%s atlas child %i left not set\n", __FUNCTION__, onChild);
		bAllSet = false;
	}
	if (!bWidthSet)
	{
		printf("%s atlas child %i top not set\n", __FUNCTION__, onChild);
		bAllSet = false;
	}
	if (!bHeightSet)
	{
		printf("%s atlas child %i left not set\n", __FUNCTION__, onChild);
		bAllSet = false;
	}
	if (bAllSet)
	{
		printf("adding sprite %s\n", spritePath);
		At_AddSprite(spritePath, top, left, width, height, spriteName);
		EASSERT(spritePath);
		EASSERT(spriteName);
		xmlFree(spritePath);
		xmlFree(spriteName);

		printf("done\n");
	}
}

static void LoadAtlasFont(xmlNode* pChild)
{
	struct FontAtlasAdditionSpec faas;
	memset(&faas, 0, sizeof(struct FontAtlasAdditionSpec));
	
	xmlChar* attribute = NULL;
	if(attribute = xmlGetProp(pChild, "source"))
	{
		strcpy(faas.path, attribute);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pChild, "name"))
	{
		strcpy(faas.name, attribute);
		xmlFree(attribute);
	}
	if(attribute = xmlGetProp(pChild, "options"))
	{
		if (strcmp(attribute, "normal") == 0)
		{
			faas.fontOptions = FS_Normal;
		}
		else if (strcmp(attribute, "italic"))
		{
			faas.fontOptions = FS_Italic;
		}
		else if (strcmp(attribute, "bold") == 0)
		{
			faas.fontOptions = FS_Bold;
		}
		else if (strcmp(attribute, "underline") == 0)
		{
			faas.fontOptions = FS_Underline;
		}
		xmlFree(attribute);
	}
	for (xmlNode* pChildChild = pChild->children; pChildChild; pChildChild = pChildChild->next)
	{
		if (pChildChild->type != XML_ELEMENT_NODE)
		{
			continue;
		}
		if(strcmp(pChildChild->name, "size") == 0)
		{
			struct FontSize fs;
			memset(&fs, 0, sizeof(struct FontSize));
			bool bTypeSet = false;
			bool bValSet = false;
			if(attribute = xmlGetProp(pChildChild, "type"))
			{
				if(strcmp(attribute, "pts") == 0)
				{
					fs.type = FOS_Pts;
					bTypeSet = true;
				}
				else if (strcmp(attribute, "pxls") == 0)
				{
					fs.type = FOS_Pixels;
					bTypeSet = true;
				}
				xmlFree(attribute);
			}
			if (attribute = xmlGetProp(pChildChild, "val"))
			{
				fs.val = atof(attribute);
				xmlFree(attribute);
				bValSet = true;
			}
			if (bValSet && bTypeSet)
			{
				faas.fontSizes[faas.numFontSizes++] = fs;
			}
		}
	}
	printf("adding font %s\n", faas.path);
	At_AddFont(&faas);
	printf("done\n");
}

void LoadAtlas(XMLUIData* pUIData, xmlNode* child0, DrawContext* pDC)
{
	printf("loading atlas\n");
	At_BeginAtlas();

	int onChild = 0;
	for (xmlNode* pChild = child0->children; pChild; pChild = pChild->next)
	{
        if (pChild->type != XML_ELEMENT_NODE)
		{
			continue;
		}
		
		if (strcmp(pChild->name, "sprite") == 0)
		{
			LoadAtlasSprite(pChild, onChild);
		}
		else if (strcmp(pChild->name, "font") == 0)
		{
			LoadAtlasFont(pChild);
		}
		onChild++;
	}
	pUIData->atlas = At_EndAtlas(pDC);
}

static bool TryLoadViewModel(XMLUIData* pUIData, xmlNode* pScreenNode)
{
	bool rVal = false;
	bool bVMFileSet = false;
	bool bVMFunctionSet = false;
	char* pFilePath = NULL;
	char* pFnName = NULL;
	xmlChar* attribute = NULL;
	if(attribute = xmlGetProp(pScreenNode, "viewmodelFile"))
	{
		pFilePath = attribute;
		bVMFileSet = true;
	}
	if(attribute = xmlGetProp(pScreenNode, "viewmodelFunction"))
	{
		pFnName = attribute;
		bVMFunctionSet = true;
	}

	if (bVMFileSet && bVMFunctionSet)
	{
		printf("opening viewmodel file %s\n", pFilePath);
		// instantiate viewmodel lua object and store in registry
		Sc_OpenFile(pFilePath);
		printf("done\n");
		pUIData->hViewModel = Sc_CallGlobalFuncReturningTableAndStoreResultInReg(pFnName, NULL, 0);
		// tag the viewmodel table with a ptr to the XMLUIDataPtr so it can set the widget tree flag to dirty
		Sc_AddLightUserDataValueToTable(pUIData->hViewModel, "XMLUIDataPtr", pUIData);
	}
	else
	{
		printf("TryLoadViewModel, either file or function (or both) not set. file: %i function name: %i\n", bVMFileSet, bVMFunctionSet);
	}

	if(pFnName)
	{
		xmlFree(pFnName);
	}
	if(pFilePath)
	{
		xmlFree(pFilePath);
	}

	return rVal;
}

static void InitializeWidgets(HWidget root)
{
	struct UIWidget* pWidget = UI_GetWidget(root);
	if (pWidget->fnOnWidgetInit)
	{
		pWidget->fnOnWidgetInit(pWidget);
	}
	HWidget child = pWidget->hFirstChild;
	while (child != NULL_HWIDGET)
	{
		InitializeWidgets(child);
		struct UIWidget* pWidget = UI_GetWidget(child);
		child = pWidget->hNext;
	}
}

xmlNode* GetNthChild(xmlNode* node, unsigned int index)
{
	int onChild = 0;
	assert(xmlChildElementCount(node) > index);
	xmlNode* pChild = xmlFirstElementChild(node);
	while(pChild)
	{
		if (pChild->type != XML_ELEMENT_NODE)
		{
			pChild = pChild->next;
			continue;
		}
		if(onChild++ == index)
		{
			return pChild;
		}
		pChild = pChild->next;
	}
}

static void LoadUIData(XMLUIData* pUIData, DrawContext* pDC)
{
	assert(!pUIData->bLoaded);
	pUIData->bLoaded = true;
	xmlDoc* pXMLDoc = xmlReadFile(pUIData->xmlFilePath, NULL, 0);

	char nodeNameArr[128];

	if (pXMLDoc)
	{
		printf("pXMLDoc is valid\n");
		xmlNode* root = xmlDocGetRootElement(pXMLDoc);
		unsigned long numchildren = xmlChildElementCount(root);
		if (numchildren != 2)
		{
			printf("%s root should have 2 kids\n", __FUNCTION__);
			xmlFreeDoc(pXMLDoc);
			return;
		}
		xmlNode* child0 = GetNthChild(root, 0);
		xmlNode* child1 = GetNthChild(root, 1);
		
		bool bDoneAtlas = false;
		bool bDoneScreen = false;
		
		if (strcmp(child0->name, "atlas") == 0)
		{
			bDoneAtlas = true;
			LoadAtlas(pUIData, child0, pDC);
		}
		else if (strcmp(child0->name, "screen") == 0)
		{
			bDoneScreen = true;
			TryLoadViewModel(pUIData, child0);
			AddNodeChildren(pUIData->rootWidget, child0, pUIData);
		}

		if (strcmp(child1->name, "atlas") == 0 && !bDoneAtlas)
		{
			bDoneAtlas = true;
			LoadAtlas(pUIData, child1, pDC);
		}
		else if (strcmp(child1->name, "screen") == 0 && !bDoneScreen)
		{
			bDoneScreen = true;
			TryLoadViewModel(pUIData, child1);
			AddNodeChildren(pUIData->rootWidget, child1, pUIData);
		}
		if (!bDoneAtlas || !bDoneScreen)
		{
			printf("%s ui xml file doesn't have both screen and atlas components\n", __FUNCTION__);
		}
		//xml_document_free(pXMLDoc, true);
		xmlFreeDoc(pXMLDoc);

		struct UIWidget* pWidget = UI_GetWidget(pUIData->rootWidget);
		pWidget->scriptCallbacks.viewmodelTable = pUIData->hViewModel;

		InitializeWidgets(pUIData->rootWidget);

		pUIData->hVertexBuffer = pDC->NewUIVertexBuffer(2048);
	}
	
}

static void OnWindowSizeChanged(struct GameFrameworkLayer* pLayer, int newW, int newH)
{
	XMLUIData* pData = pLayer->userData;
	RootWidget_OnWindowSizeChanged(pData->rootWidget, newW, newH);

	struct UIWidget* pWidget = UI_GetWidget(pData->rootWidget);
	SetRootWidgetIsDirty(pData->rootWidget, true);

	pWidget->fnOnDebugPrint(0, pWidget,&printf);

}


void XMLUIGameLayer_Get(struct GameFrameworkLayer* pLayer, struct XMLUIGameLayerOptions* pOptions)
{
	pLayer->userData = malloc(sizeof(XMLUIData));
	if (!pLayer->userData) { printf("XMLUIGameLayer_Get: no memory"); return; }
	XMLUIData* pUIData = (XMLUIData*)pLayer->userData;
	
	memset(pLayer->userData, 0, sizeof(XMLUIData));
	
	strcpy(pUIData->xmlFilePath, pOptions->xmlPath);
	pUIData->rootWidget = NewRootWidget();
	RootWidget_OnWindowSizeChanged(pUIData->rootWidget, Mn_GetScreenWidth(), Mn_GetScreenHeight());
	pLayer->draw = &Draw;
	pLayer->update = &Update;
	pLayer->input = &Input;
	pLayer->onPop = &OnPop;
	pLayer->onPush = &OnPush;
	pLayer->onWindowDimsChanged = &OnWindowSizeChanged;
	pLayer->flags = 0;
	pLayer->flags |= EnableDrawFn | EnableInputFn | EnableUpdateFn | EnableOnPop | EnableOnPush;
	if (pOptions->bLoadImmediately)
	{
		LoadUIData(pUIData, pOptions->pDc);
	}
}










