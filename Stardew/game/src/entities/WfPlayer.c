#include "WfPlayer.h"
#include "BinarySerializer.h"
#include "Game2DLayer.h"
#include "Entities.h"
#include "ObjectPool.h"
#include <string.h>

#define WALKING_UP_MALE "walk-base-male-up"
#define WALKING_DOWN_MALE "walk-base-male-down"
#define WALKING_LEFT_MALE "walk-base-male-left"
#define WALKING_RIGHT_MALE "walk-base-male-right"

#define WALKING_UP_FEMALE "walk-base-female-up"
#define WALKING_DOWN_FEMALE "walk-base-female-down"
#define WALKING_LEFT_FEMALE "walk-base-female-left"
#define WALKING_RIGHT_FEMALE "walk-base-female-right"

#define PLAYER_SPRITE_COMPONENT_INDEX 0
#define PLAYER_GROUND_COLLIDER_COMPONENT_INDEX 1

struct WfPlayerEntData
{
    vec2 groundColliderCenter2EntTransform;
};

static OBJECT_POOL(struct WfPlayerEntData) gPlayerEntDataPool = NULL;

void WfInitPlayer()
{
    gPlayerEntDataPool = NEW_OBJECT_POOL(struct WfPlayerEntData, 4);
}

static void OnInitPlayer(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer)
{
    Entity2DOnInit(pEnt,pLayer);
}

static void OnDestroyPlayer(struct Entity2D* pEnt, struct GameFrameworkLayer* pData)
{
    Entity2DOnDestroy(pEnt, pData);
}

static void OnUpdatePlayer(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT)
{
    Entity2DUpdate(pEnt, pLayer, deltaT);
}

static void OnInputPlayer(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, InputContext* context)
{
    Entity2DInput(pEnt, pLayer, context);
}

void WfDeSerializePlayerEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData)
{
}

void WfSerializePlayerEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData)
{
}


void WfMakeIntoPlayerEntity(struct Entity2D* pEnt, struct GameLayer2DData* pData, vec2 spawnAtGroundPos)
{
    pEnt->nextSibling = NULL_HANDLE;
    pEnt->previousSibling = NULL_HANDLE;
    GetObjectPoolIndex(gPlayerEntDataPool, &pEnt->user.hData);
    gPlayerEntDataPool[pEnt->user.hData].groundColliderCenter2EntTransform[0] = -32;
    gPlayerEntDataPool[pEnt->user.hData].groundColliderCenter2EntTransform[1] = -60;
    pEnt->numComponents = 0;
    /*
        Animated Sprite
    */
    struct Component2D* pComponent1 = &pEnt->components[pEnt->numComponents++];
    pComponent1->type = ETE_SpriteAnimator;
    struct AnimatedSprite* pAnimatedSprite = &pComponent1->data.spriteAnimator;
    memset(pAnimatedSprite, 0, sizeof(struct AnimatedSprite));
    pAnimatedSprite->animationName = WALKING_DOWN_MALE;
    pAnimatedSprite->bRepeat = true;
    pAnimatedSprite->transform.scale[0] = 1.0f;
    pAnimatedSprite->transform.scale[1] = 1.0f;
    pAnimatedSprite->bIsAnimating = true;

    /*
        Ground Collider
    */
    struct Component2D* pComponent2 = &pEnt->components[pEnt->numComponents++];
    pComponent2->type = ETE_DynamicCollider;
    pComponent2->data.kinematicCollider.shape.type = PBT_Circle;
    pComponent2->data.kinematicCollider.shape.data.circle.center[0] = spawnAtGroundPos[0];
    pComponent2->data.kinematicCollider.shape.data.circle.center[1] = spawnAtGroundPos[1];
    pComponent2->data.kinematicCollider.shape.data.circle.radius = 4;

    glm_vec2_add(spawnAtGroundPos, gPlayerEntDataPool[pEnt->user.hData].groundColliderCenter2EntTransform, pEnt->transform.position);
    pEnt->transform.scale[0] = 1.0;
    pEnt->transform.scale[1] = 1.0;
    

    pEnt->inDrawLayer = 0;

    Et2D_PopulateCommonHandlers(pEnt);
    pEnt->init = &OnInitPlayer;
    pEnt->update = &OnUpdatePlayer;
    pEnt->bKeepInQuadtree = false;
    pEnt->bKeepInDynamicList = true;
}