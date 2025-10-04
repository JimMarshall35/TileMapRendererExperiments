#include "Game2DLayer.h"
#include "GameFramework.h"
#include "Atlas.h"
#include <stdlib.h>
#include <string.h>
#include "BinarySerializer.h"
#include "DrawContext.h"
#include "AssertLib.h"
#include "DynArray.h"
#include <cglm/cglm.h>
#include "GameFrameworkEvent.h"
#include "Scripting.h"
#include "XMLUIGameLayer.h"

#define FREE_LOOK_ZOOM_IN_BINDING_NAME "zoomIn"
#define FREE_LOOK_ZOOM_OUT_BINDING_NAME "zoomOut"
#define FREE_LOOK_XPOS_BINDING_NAME "moveRight"
#define FREE_LOOK_XNEG_BINDING_NAME "moveLeft"
#define FREE_LOOK_YPOS_BINDING_NAME "moveDown"
#define FREE_LOOK_YNEG_BINDING_NAME "moveUp"
#define FREE_LOOK_CURSOR_X_BINDING_NAME "CursorPosX"
#define FREE_LOOK_CURSOR_Y_BINDING_NAME "CursorPosY"
#define FREE_LOOK_CURSOR_SELECT_BINDING_NAME "select"

float gDebugWSX = 0;
float gDebugWSY = 0;
static void GetViewportWorldspaceTLBR(vec2 outTL, vec2 outBR, struct Transform2D* pCam, int windowW, int windowH)
{
	outTL[0] = pCam->position[0];
	outTL[1] = pCam->position[1];

	outTL[0] = -outTL[0];
	outTL[1] = -outTL[1];

	outBR[0] = outTL[0] + windowW / pCam->scale[0];
	outBR[1] = outTL[1] + windowH / pCam->scale[0];

}

static void LoadTilesUncompressedV1(struct TileMapLayer* pLayer, struct BinarySerializer* pBS)
{
	int allocSize = pLayer->heightTiles * pLayer->widthTiles * sizeof(TileIndex);
	pLayer->Tiles = malloc(allocSize);
	BS_BytesRead(pBS, allocSize, (char*)pLayer->Tiles);
}

static void LoadTilesRLEV1(struct TileMapLayer* pTileMap, struct BinarySerializer* pBS)
{
	EASSERT(false);
}


static void LoadTilemapV1(struct TileMap* pTileMap, struct BinarySerializer* pBS)
{
	u32 numLayers = 0;
	BS_DeSerializeU32(&numLayers, pBS);
	pTileMap->layers = VectorResize(pTileMap->layers, numLayers);
	for (int i = 0; i < numLayers; i++)
	{
		struct TileMapLayer layer;
		u32 width, height, x, y, compression, tw, th;
		BS_DeSerializeU32(&width, pBS);
		BS_DeSerializeU32(&height, pBS);
		BS_DeSerializeU32(&x, pBS);
		BS_DeSerializeU32(&y, pBS);
		BS_DeSerializeU32(&tw, pBS);
		BS_DeSerializeU32(&th, pBS);
		BS_DeSerializeU32(&compression, pBS);
		layer.widthTiles = width;
		layer.heightTiles = height;
		layer.transform.position[0] = x;
		layer.transform.position[1] = y;
		layer.tileWidthPx = tw;
		layer.tileHeightPx = th;

		switch (compression)
		{
		case 1:         // RLE
			LoadTilesRLEV1(&layer, pBS);
			break;
		case 2:         // uncompressed
			LoadTilesUncompressedV1(&layer, pBS);
			break;
		default:
			printf("unexpected value for compression enum %i\n", compression);
			break;
		}
		pTileMap->layers = VectorPush(pTileMap->layers, &layer);
	}
}

static void LoadTilemap(struct TileMap* pTileMap, const char* tilemapFilePath, DrawContext* pDC, hAtlas atlas)
{
	pTileMap->layers = NEW_VECTOR(struct TileMapLayer);
	struct BinarySerializer bs;
	memset(&bs, 0, sizeof(struct BinarySerializer));
	BS_CreateForLoad(tilemapFilePath, &bs);
	u32 version = 0;
	BS_DeSerializeU32(&version, &bs);
	switch (version)
	{
	case 1:
		LoadTilemapV1(pTileMap, &bs);
		break;
	default:
		printf("Unexpected tilemap file version %u\n", version);
		break;
	}
	BS_Finish(&bs);
}

static void PublishDebugMessage(struct GameLayer2DData* pData)
{
	vec2 tl, br;
	GetViewportWorldspaceTLBR(tl, br, &pData->camera, pData->windowW, pData->windowH);
	sprintf(pData->debugMsg, "Cam: x:%.2f y:%.2f zoom:%.2f tlx:%.2f tly:%.2f brx:%.2f bry:%.2f",
		gDebugWSX, gDebugWSY, pData->camera.scale[0],
		tl[0], tl[1],
		br[0], br[1]
	);
	struct ScriptCallArgument arg;
	arg.type = SCA_string;
	arg.val.string = pData->debugMsg;
	struct LuaListenedEventArgs args = { .numArgs = 1, .args = &arg };
	Ev_FireEvent("DebugMessage", &args);
}

static void Update(struct GameFrameworkLayer* pLayer, float deltaT)
{
	struct GameLayer2DData* pData = pLayer->userData;
	if (pData->bDebugLayerAttatched)
	{
		PublishDebugMessage(pData);
	}
}

void OutputSpriteVertices(
	AtlasSprite* pSprite,
	VECTOR(Worldspace2DVert)* pOutVert,
	VECTOR(VertIndexT)* pOutInd,
	VertIndexT* pNextIndex,
	int col,
	int row
)
{
	VECTOR(Worldspace2DVert) outVert = *pOutVert;
	VECTOR(VertIndexT) outInd = *pOutInd;

	VertIndexT base = *pNextIndex;
	*pNextIndex += 4;
	ivec2 dims = {
		pSprite->widthPx,
		pSprite->heightPx
	};
	ivec2 topLeft = {
		col * pSprite->widthPx,
		row * pSprite->heightPx
	};
	ivec2 bottomRight;
	glm_ivec2_add(topLeft, dims, bottomRight);

	ivec2 topRight = {
		topLeft[0] + pSprite->widthPx,
		topLeft[1]
	};

	ivec2 bottomLeft = {
		topLeft[0],
		topLeft[1] + pSprite->heightPx
	};
	Worldspace2DVert vert = {
		topLeft[0], topLeft[1],
		pSprite->topLeftUV_U, pSprite->topLeftUV_V
	};

	// top left
	VertIndexT tl = base;
	outVert = VectorPush(outVert, &vert);
	
	vert.x = topRight[0]; 
	vert.y = topRight[1];
	vert.u = pSprite->bottomRightUV_U;
	vert.v = pSprite->topLeftUV_V;
	
	// top right
	VertIndexT tr = base + 1;
	outVert = VectorPush(outVert, &vert);

	vert.x = bottomLeft[0];
	vert.y = bottomLeft[1];
	vert.u = pSprite->topLeftUV_U;
	vert.v = pSprite->bottomRightUV_V;

	// bottom left
	VertIndexT bl = base + 2;
	outVert = VectorPush(outVert, &vert);

	vert.x = bottomRight[0];
	vert.y = bottomRight[1];
	vert.u = pSprite->bottomRightUV_U;
	vert.v = pSprite->bottomRightUV_V;

	// bottom right
	VertIndexT br = base + 3;
	outVert = VectorPush(outVert, &vert);

	outInd = VectorPush(outInd, &tl);
	outInd = VectorPush(outInd, &tr);
	outInd = VectorPush(outInd, &bl);
	outInd = VectorPush(outInd, &tr);
	outInd = VectorPush(outInd, &br);
	outInd = VectorPush(outInd, &bl);

	*pOutVert = outVert;
	*pOutInd = outInd;
}

static void OutputTilemapLayerVertices(
	hAtlas atlas,
	struct TileMapLayer* pLayer,
	VECTOR(Worldspace2DVert)* outVerts,
	VECTOR(VertIndexT)* outInds,
	VertIndexT* pNextIndex,
	vec2 viewportTL,
	vec2 viewportBR
)
{
	VECTOR(Worldspace2DVert) outVert = *outVerts;
	VECTOR(VertIndexT) outInd = *outInds;

	/*
		Only draw those tiles that are in the viewport:
		TODO: make this work for layers that are transformed
	*/

	int startCol = ((int)viewportTL[0]) / pLayer->tileWidthPx;
	startCol = startCol < 0 ? 0 : startCol;
	int endCol = ((int)viewportBR[0]) / pLayer->tileWidthPx;
	endCol++;
	endCol = endCol > pLayer->widthTiles ? pLayer->widthTiles : endCol;


	int startRow = ((int)viewportTL[1]) / pLayer->tileHeightPx;
	startRow = startRow < 0 ? 0 : startRow;
	int endRow = ((int)viewportBR[1]) / pLayer->tileHeightPx;
	endRow++;
	endRow = endRow > pLayer->heightTiles ? pLayer->heightTiles : endRow;

	for (int row = startRow; row < endRow; row++)
	{
		for (int col = startCol; col < endCol; col++)
		{
			TileIndex tile = pLayer->Tiles[row * pLayer->widthTiles + col];
			if (tile == 0)
			{
				continue;
			}
			hSprite sprite = At_TilemapIndexToSprite(atlas, tile);
			AtlasSprite* pSprite = At_GetSprite(sprite, atlas);
			OutputSpriteVertices(pSprite, &outVert, &outInd, pNextIndex, col, row);
		}
	}

	*outVerts = outVert;
	*outInds = outInd;
}

static void OutputTilemapVertices(
	struct TileMap* pData, 
	struct Transform2D* pCam, 
	VECTOR(Worldspace2DVert)* outVerts,
	VECTOR(VertIndexT)* outIndices,
	struct GameLayer2DData* pLayerData
)
{
	VECTOR(Worldspace2DVert) verts = *outVerts;
	VECTOR(VertIndexT) inds = *outIndices;
	
	vec2 tl, br;
	GetViewportWorldspaceTLBR(tl, br, pCam, pLayerData->windowW, pLayerData->windowH);

	VertIndexT nextIndexVal = 0;
	for (int i = 0; i < VectorSize(pData->layers); i++)
	{
		OutputTilemapLayerVertices(pLayerData->hAtlas, pData->layers + i, &verts, &inds, &nextIndexVal, tl, br);
	}

	*outVerts = verts;
	*outIndices = inds;
}

static void Draw(struct GameFrameworkLayer* pLayer, DrawContext* context)
{
	struct GameLayer2DData* pData = pLayer->userData;
	At_SetCurrent(pData->hAtlas, context);
	pData->pWorldspaceVertices = VectorClear(pData->pWorldspaceVertices);
	pData->pWorldspaceIndices = VectorClear(pData->pWorldspaceIndices);
	OutputTilemapVertices(&pData->tilemap, &pData->camera, &pData->pWorldspaceVertices, &pData->pWorldspaceIndices, pData);
	context->WorldspaceVertexBufferData(pData->vertexBuffer, pData->pWorldspaceVertices, VectorSize(pData->pWorldspaceVertices), pData->pWorldspaceIndices, VectorSize(pData->pWorldspaceIndices));
	mat4 view;
	glm_mat4_identity(view);
	// TODO: set here based on camera
	vec3 translate = {
		pData->camera.position[0],
		pData->camera.position[1],
		0.0f
	};
	vec3 scale = {
		pData->camera.scale[0],
		pData->camera.scale[1],
		1.0f
	};

	glm_scale(view, scale);
	glm_translate(view, translate);

	context->DrawWorldspaceVertexBuffer(pData->vertexBuffer, VectorSize(pData->pWorldspaceIndices), view);
}

static void CenterCameraAt(float worldspaceX, float worldspaceY, struct Transform2D* pCam, int winWidth, int winHeight)
{
	vec2 tl, br;
	GetViewportWorldspaceTLBR(tl, br, pCam, winWidth, winHeight);
	pCam->position[0] = -(worldspaceX - (br[0] - tl[0]) / 2.0f);
	pCam->position[1] = -(worldspaceY - (br[1] - tl[1]) / 2.0f);
}

static void GetCamWorldspaceCenter(struct Transform2D* pCam, int winWidth, int winHeight, vec2 outCenter)
{
	vec2 tl, br;
	vec2 add;
	GetViewportWorldspaceTLBR(tl, br, pCam, winWidth, winHeight);
	add[0] = (br[0] - tl[0]) / 2.0f;
	add[1] = (br[1] - tl[1]) / 2.0f;
	glm_vec2_add(tl, add, outCenter);
}

static void GetWorldspacePos(int screenPosX, int screenPosY, int screenW, int screenH, struct Transform2D* pCam, vec2 outWorldspace)
{
	vec2 tl, br;
	GetViewportWorldspaceTLBR(tl, br, pCam, screenW, screenH);
	float fX = (float)screenPosX / (float)screenW;
	float fY = (float)screenPosY / (float)screenH;
	outWorldspace[0] = tl[0] + fX * (br[0] - tl[0]);
	outWorldspace[1] = tl[1] + fY * (br[1] - tl[1]);
}

static void ScreenSpaceToWorldSpaceTransVector(vec2 screenSpaceTranslateVector, int screenW, int screenH, struct Transform2D* pCam, vec2 outWorldspace)
{
	vec2 tl, br;
	GetViewportWorldspaceTLBR(tl, br, pCam, screenW, screenH);
	vec2 norm = {
		screenSpaceTranslateVector[0] / screenW,
		screenSpaceTranslateVector[1] / screenH
	};
	outWorldspace[0] = norm[0] * (br[0] - tl[0]);
	outWorldspace[1] = norm[1] * (br[1] - tl[1]);
}

static void Input(struct GameFrameworkLayer* pLayer, InputContext* context)
{
	struct GameLayer2DData* pData = pLayer->userData;

	const float FREE_LOOK_ZOOM_FACTOR = 0.99;
	const float FREE_LOOK_MOVEMENT_SPEED = 0.3;
	bool bZoomIn = In_GetButtonValue(context, pData->freeLookZoomInBinding);
	bool bZoomOut = In_GetButtonValue(context, pData->freeLookZoomOutBinding);
	bool bXPos = In_GetButtonValue(context, pData->freeLookZoomMoveXPosBinding);
	bool bXNeg = In_GetButtonValue(context, pData->freeLookZoomMoveXNegBinding);

	bool bYPos = In_GetButtonValue(context, pData->freeLookZoomMoveYPosBinding);
	bool bYNeg = In_GetButtonValue(context, pData->freeLookZoomMoveYNegBinding);

	vec2 movementVector = { 0,0 };
	if (bXPos)
	{
		vec2 dir = { 1,0 };
		glm_vec2_add(movementVector, dir, movementVector);
	}
	if (bXNeg)
	{
		vec2 dir = { -1,0 };
		glm_vec2_add(movementVector, dir, movementVector);
	}
	if (bYPos)
	{
		vec2 dir = { 0,-1 };
		glm_vec2_add(movementVector, dir, movementVector);
	}
	if (bYNeg)
	{
		vec2 dir = { 0,1 };
		glm_vec2_add(movementVector, dir, movementVector);
	}
	glm_vec2_normalize(movementVector);
	movementVector[0] = movementVector[0] * FREE_LOOK_MOVEMENT_SPEED;
	movementVector[1] = movementVector[1] * FREE_LOOK_MOVEMENT_SPEED;

	glm_vec2_add(movementVector, pData->camera.position, pData->camera.position);

	if (bZoomOut)//bZoomIn)
	{
		pData->camera.scale[0] *= 0.9f;
		pData->camera.scale[1] *= 0.9f;
	}
	if (bZoomIn)//bZoomOut)
	{
		pData->camera.scale[0] *= 1.1f;
		pData->camera.scale[1] *= 1.1f;
	}

	// move around with mouse
	bool bSelectVal = In_GetButtonValue(context, pData->freeLookSelectButtonBinding);
	static bool bLastSelect = false;
	static ivec2 dragAnchorScreenSpace;
	static vec2 dragAnchorWorldSpace;
	if(bSelectVal && !bLastSelect)
	{
		//press 
		int screenX = In_GetAxisValue(context, pData->freeLookCursorXAxisBinding);
		int screenY = In_GetAxisValue(context, pData->freeLookCursorYAxisBinding);
		GetWorldspacePos(screenX, screenY, pData->windowW, pData->windowH, &pData->camera, dragAnchorWorldSpace);
		gDebugWSX = dragAnchorWorldSpace[0];
		gDebugWSY = dragAnchorWorldSpace[1];
		dragAnchorScreenSpace[0] = screenX;
		dragAnchorScreenSpace[1] = screenY;
		//CenterCameraAt(gDebugWSX, gDebugWSY, &pData->camera, pData->windowW, pData->windowH); // to test
		GetCamWorldspaceCenter(&pData->camera, pData->windowW, pData->windowH, dragAnchorWorldSpace);
	}
	if(bSelectVal)
	{
		// drag	
		ivec2 ss;
		ivec2 sstrans;
		vec2 sstransf;
		vec2 wstrans;
		vec2 camCenter;
		ss[0] = In_GetAxisValue(context, pData->freeLookCursorXAxisBinding);
		ss[1] = In_GetAxisValue(context, pData->freeLookCursorYAxisBinding);
		glm_ivec2_sub(ss, dragAnchorScreenSpace, sstrans);
		sstransf[0] = (float)sstrans[0];
		sstransf[1] = (float)sstrans[1];
		ScreenSpaceToWorldSpaceTransVector(sstransf, pData->windowW, pData->windowH, &pData->camera, wstrans);
		glm_vec2_scale(wstrans, -1.0f, wstrans);
		glm_vec2_add(dragAnchorWorldSpace, wstrans, camCenter);
		CenterCameraAt(camCenter[0], camCenter[1], &pData->camera, pData->windowW, pData->windowH); // to test

	}

	bLastSelect = bSelectVal;
}

static void LoadLayerAssets(struct GameLayer2DData* pData, DrawContext* pDC)
{
	struct BinarySerializer bs;
	memset(&bs, 0, sizeof(struct BinarySerializer));
	BS_CreateForLoad(pData->atlasFilePath, &bs);
	At_SerializeAtlas(&bs, &pData->hAtlas, pDC);
	BS_Finish(&bs);
	LoadTilemap(&pData->tilemap, pData->tilemapFilePath, pDC, pData->hAtlas);
	pData->bLoaded = true;
}

static void BindFreeLookControls(InputContext* inputContext, struct GameLayer2DData* pData)
{
	pData->freeLookZoomInBinding = In_FindButtonMapping(inputContext, FREE_LOOK_ZOOM_IN_BINDING_NAME);
	pData->freeLookZoomOutBinding = In_FindButtonMapping(inputContext, FREE_LOOK_ZOOM_OUT_BINDING_NAME);
	pData->freeLookZoomMoveXPosBinding = In_FindButtonMapping(inputContext, FREE_LOOK_XPOS_BINDING_NAME);
	pData->freeLookZoomMoveXNegBinding = In_FindButtonMapping(inputContext, FREE_LOOK_XNEG_BINDING_NAME);
	pData->freeLookZoomMoveYPosBinding = In_FindButtonMapping(inputContext, FREE_LOOK_YPOS_BINDING_NAME);
	pData->freeLookZoomMoveYNegBinding = In_FindButtonMapping(inputContext, FREE_LOOK_YNEG_BINDING_NAME);

	pData->freeLookCursorXAxisBinding = In_FindAxisMapping(inputContext, FREE_LOOK_CURSOR_X_BINDING_NAME);
	pData->freeLookCursorYAxisBinding = In_FindAxisMapping(inputContext, FREE_LOOK_CURSOR_Y_BINDING_NAME);
	pData->freeLookSelectButtonBinding = In_FindButtonMapping(inputContext, FREE_LOOK_CURSOR_SELECT_BINDING_NAME);

	In_ActivateButtonBinding(pData->freeLookZoomInBinding, &pData->freeLookInputsMask);
	In_ActivateButtonBinding(pData->freeLookZoomOutBinding, &pData->freeLookInputsMask);
	In_ActivateButtonBinding(pData->freeLookZoomMoveXPosBinding, &pData->freeLookInputsMask);
	In_ActivateButtonBinding(pData->freeLookZoomMoveXNegBinding, &pData->freeLookInputsMask);
	In_ActivateButtonBinding(pData->freeLookZoomMoveYPosBinding, &pData->freeLookInputsMask);
	In_ActivateButtonBinding(pData->freeLookZoomMoveYNegBinding, &pData->freeLookInputsMask);
	In_ActivateButtonBinding(pData->freeLookSelectButtonBinding, &pData->freeLookInputsMask);
	In_ActivateAxisBinding(pData->freeLookCursorXAxisBinding, &pData->freeLookInputsMask);
	In_ActivateAxisBinding(pData->freeLookCursorYAxisBinding, &pData->freeLookInputsMask);
}

static void ActivateFreeLookMode(InputContext* inputContext, struct GameLayer2DData* pData)
{
	In_SetMask(&pData->freeLookInputsMask, inputContext);
}

static void OnDebugLayerPushed(void* pUserData, void* pEventData)
{
	struct GameLayer2DData* pData = pUserData;
	pData->bDebugLayerAttatched = true;
}

static void OnPush(struct GameFrameworkLayer* pLayer, DrawContext* drawContext, InputContext* inputContext)
{
	struct GameLayer2DData* pData = pLayer->userData;
	BindFreeLookControls(inputContext, pData);
	ActivateFreeLookMode(inputContext, pData);
	if (!pData->bLoaded)
	{
		LoadLayerAssets(pData, drawContext);
	}
	Ev_SubscribeEvent("onDebugLayerPushed", &OnDebugLayerPushed, pData);
	XMLUI_PushGameFrameworkLayer("./Assets/debug_overlay.xml");
}

static void OnPop(struct GameFrameworkLayer* pLayer, DrawContext* drawContext, InputContext* inputContext)
{
	struct GameLayer2DData* pData = pLayer->userData;
	EASSERT(pData->pDebugListener);
	Ev_UnsubscribeEvent(pData->pDebugListener);
}

static void OnWindowDimsChange(struct GameFrameworkLayer* pLayer, int newW, int newH)
{
	struct GameLayer2DData* pData = pLayer->userData;
	pData->windowW = newW;
	pData->windowH = newH;
}

void Game2DLayer_Get(struct GameFrameworkLayer* pLayer, struct Game2DLayerOptions* pOptions, DrawContext* pDC)
{
	pLayer->userData = malloc(sizeof(struct GameLayer2DData));
	memset(pLayer->userData, 0, sizeof(struct GameLayer2DData));
	struct GameLayer2DData* pData = pLayer->userData;
	pData->tilemap.layers = NEW_VECTOR(struct TileMapLayer);

	EASSERT(strlen(pData->tilemapFilePath) < 128);
	EASSERT(strlen(pData->atlasFilePath) < 128);
	strcpy(pData->tilemapFilePath, pOptions->tilemapFilePath);
	strcpy(pData->atlasFilePath, pOptions->atlasFilePath);

	pLayer->update = &Update;
	pLayer->draw = &Draw;
	pLayer->input = &Input;
	pLayer->onPush = &OnPush;
	pLayer->onWindowDimsChanged = &OnWindowDimsChange;

	pData->camera.scale[0] = 1;
	pData->camera.scale[1] = 1;

	pData->vertexBuffer = pDC->NewWorldspaceVertBuffer(256);
	pData->pWorldspaceVertices = NEW_VECTOR(Worldspace2DVert);
	pData->pWorldspaceIndices = NEW_VECTOR(VertIndexT);

	pData->windowH = pDC->screenHeight;
	pData->windowW = pDC->screenWidth;

	if (pOptions->loadImmediatly)
	{
		LoadLayerAssets(pData, pDC);
	}
}
