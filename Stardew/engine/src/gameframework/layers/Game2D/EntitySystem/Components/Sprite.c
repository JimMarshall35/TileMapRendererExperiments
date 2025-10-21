#include "Sprite.h"
#include "Entities.h"
#include "GameFramework.h"
#include "InputContext.h"
#include "DrawContext.h"
#include "Atlas.h"
#include "Game2DLayer.h"

void SpriteComp_OnInit(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT)
{

}

void SpriteComp_Update(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT)
{
}

void SpriteComp_UpdatePostPhysics(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT)
{
    
}

void OutputSpriteVerticesBase(
	AtlasSprite* pSprite,
	VECTOR(Worldspace2DVert)* pOutVert,
	VECTOR(VertIndexT)* pOutInd,
	VertIndexT* pNextIndex,
	vec2 tlPos,
	vec2 brPos
)
{
	VECTOR(Worldspace2DVert) outVert = *pOutVert;
	VECTOR(VertIndexT) outInd = *pOutInd;

	VertIndexT base = *pNextIndex;
	*pNextIndex += 4;
	ivec2 dims = {
		brPos[0] - tlPos[0],
		brPos[1] - tlPos[1]
	};

	vec2 trPos = {brPos[0], tlPos[1]};
	vec2 blPos = {tlPos[0], brPos[1]};

	Worldspace2DVert vert = {
		tlPos[0], tlPos[1],
		pSprite->topLeftUV_U, pSprite->topLeftUV_V
	};

	// top left
	VertIndexT tl = base;
	outVert = VectorPush(outVert, &vert);
	
	vert.x = trPos[0]; 
	vert.y = trPos[1];
	vert.u = pSprite->bottomRightUV_U;
	vert.v = pSprite->topLeftUV_V;
	
	// top right
	VertIndexT tr = base + 1;
	outVert = VectorPush(outVert, &vert);

	vert.x = blPos[0];
	vert.y = blPos[1];
	vert.u = pSprite->topLeftUV_U;
	vert.v = pSprite->bottomRightUV_V;

	// bottom left
	VertIndexT bl = base + 2;
	outVert = VectorPush(outVert, &vert);

	vert.x = brPos[0];
	vert.y = brPos[1];
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


void SpriteComp_Draw(
    struct Sprite* pSpriteComp,
    struct Entity2D* pEnt,
    struct GameFrameworkLayer* pLayer,
    struct Transform2D* pCam,
    VECTOR(Worldspace2DVert)* outVerts,
    VECTOR(VertIndexT)* outIndices,
    VertIndexT* pNextIndex
)
{
    vec2 tl, br;
    SpriteComp_GetBoundingBox(pEnt, pSpriteComp, pLayer, tl, br);
    struct GameLayer2DData* pLayerData = pLayer->userData;
    AtlasSprite* pSprite = At_GetSprite(pSpriteComp->sprite, pLayerData->hAtlas);
    OutputSpriteVerticesBase(pSprite, outVerts, outIndices, pNextIndex, tl, br);
}

void SpriteComp_Input(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, InputContext* context)
{

}

void SpriteComp_OnDestroy(struct Entity2D* pEnt)
{

}

void SpriteComp_GetBoundingBox(struct Entity2D* pEnt, struct Sprite* pSpriteComp, struct GameFrameworkLayer* pLayer, vec2 outTL, vec2 outBR)
{
    struct GameLayer2DData* pLayerData = pLayer->userData;
    AtlasSprite* pSprite = At_GetSprite(pSpriteComp->sprite, pLayerData->hAtlas);
    vec2 tl = {pEnt->transform.position[0], pEnt->transform.position[1]};
    vec2 br;
    vec2 size = {
        pSprite->widthPx  * pEnt->transform.scale[0] * pSpriteComp->transform.scale[0],
        pSprite->heightPx * pEnt->transform.scale[1] * pSpriteComp->transform.scale[1]
    };
    glm_vec2_add(pEnt->transform.position, size, br);
    glm_vec2_add(tl, pSpriteComp->transform.position, tl);
    glm_vec2_add(br, pSpriteComp->transform.position, br);

	outTL[0] = tl[0];
	outTL[1] = tl[1];
	outBR[0] = br[0];
	outBR[1] = br[1];
}

