#include "DefaultEntityFunctions.h"
#include "Game2DLayer.h"
#include "Components.h"
#include "Entities.h"
#include "Sprite.h"

void Entity2DOnInit(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer)
{
    Co_InitComponents(pEnt, pLayer);
}

void Entity2DUpdateFn(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT)
{
    Co_UpdateComponents(pEnt, pLayer, deltaT);
}

void Entity2DUpdatePostPhysicsFn(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT)
{
    Co_Entity2DUpdatePostPhysicsFn(pEnt, pLayer, deltaT);
}

void Entity2DDrawFn(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, struct Transform2D* pCam, VECTOR(Worldspace2DVert)* outVerts, VECTOR(VertIndexT)* outIndices, VertIndexT* pNextIndex)
{
    Co_DrawComponents(pEnt, pLayer, pCam, outVerts, outIndices, pNextIndex);
}

void Entity2DInputFn(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, InputContext* context)
{
    Co_InputComponents(pEnt, pLayer, context);
}

void Entity2DOnDestroyFn(struct Entity2D* pEnt)
{
    Co_DestroyComponents(pEnt);
}


void Entity2DGetBoundingBoxFn(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, vec2 outTL, vec2 outBR)
{
    vec2 bbtl  = {99999999999, 9999999999999};
    vec2 bbbr  = {-99999999999, -9999999999999};
    bool bSet = false;
    for(int i=0; i < pEnt->numComponents; i++)
    {
        struct Component2D* pComponent = &pEnt->components[i];
        if(pComponent->type == ETE_Sprite)
        {
            vec2 tl, br;
            bSet = true;
            SpriteComp_GetBoundingBox(pEnt, &pComponent->data.sprite, pLayer, tl, br);
            if(tl[0] < bbtl[0])
            {
                bbtl[0] = tl[0];
            }
            if(tl[1] < bbtl[1])
            {
                bbtl[1] = tl[1];
            }
            if(br[0] > bbbr[0])
            {
                br[0] = bbbr[0];
            }
            if(br[1] > bbbr[1])
            {
                br[1] = bbbr[1];
            }
        }
    }
    if(!bSet)
    {
        outTL[0] = 0;
        outTL[1] = 0;
        outBR[0] = 0;
        outBR[1] = 0;
    }
    else
    {
        outTL[0] = bbtl[0];
        outTL[1] = bbtl[1];
        outBR[0] = bbbr[0];
        outBR[1] = bbbr[1];
    }
}