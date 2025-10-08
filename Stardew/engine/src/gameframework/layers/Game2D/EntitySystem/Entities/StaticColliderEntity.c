#include "StaticColliderEntity.h"
#include "BinarySerializer.h"
#include "Physics2D.h"
#include "AssertLib.h"



void DeSerialize2DRectStaticColliderEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData)
{
    float w, h;
    BS_DeSerializeFloat(&w, bs);
    BS_DeSerializeFloat(&h, bs);
    vec2 tl = {
        pOutEnt->transform.position[0],
        pOutEnt->transform.position[1],
    };
    vec2 add = { w, h };
    vec2 br;
    glm_vec2_add(tl, add, br);
    struct PhysicsShape2D shape = {
        .type = PBT_Rect,
        .data.rect = {
            .tl = { tl[0], tl[1] },
            .br = { br[0], br[1] }
        }
    };
    struct Component2D cmp = 
    {
        .type = ETE_StaticCollider,
        .data.staticCollider.id = Ph_GetStaticBody2D(&shape)
    };
    pOutEnt->components[pOutEnt->numComponents++] = cmp;
}

void Serialize2DRectStaticColliderEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData)
{
    if(pInEnt->numComponents != 1)
    {
        EASSERT(false);
        printf("SerializeStaticColliderEntity, collider doesn't have exactly 1 component\n");
        return;
    }
    if(pInEnt->components[0].type != ETE_StaticCollider)
    {
        EASSERT(false);
        printf("SerializeStaticColliderEntity, component isn't of type static collider\n");
        return;
    }
    struct PhysicsShape2D* pShape = Ph_GetStaticBodyShape2D(pInEnt->components[0].data.staticCollider.id);
    float w = pShape->data.rect.br[0] - pShape->data.rect.tl[0];
    float h = pShape->data.rect.br[1] - pShape->data.rect.tl[1];
    BS_SerializeFloat(w, bs);
    BS_SerializeFloat(h, bs);
}

static struct EntitySerializerPair gPairRect = 
{
    .deserialize = &DeSerialize2DRectStaticColliderEntity,
    .serialize = &Serialize2DRectStaticColliderEntity,
};

struct EntitySerializerPair Et2D_Get2DRectStaticColliderSerializerPair()
{
    return gPairRect;
}

void DeSerialize2DCircleStaticColliderEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData)
{
    float r;
    BS_DeSerializeFloat(&r, bs);
    struct PhysicsShape2D shape = {
        .type = PBT_Circle,
        .data.circle = {
            .center = { pOutEnt->transform.position[0], pOutEnt->transform.position[1] },
            .radius = r
        }
    };
    struct Component2D cmp = 
    {
        .type = ETE_StaticCollider,
        .data.staticCollider.id = Ph_GetStaticBody2D(&shape)
    };
    pOutEnt->components[pOutEnt->numComponents++] = cmp;
}

void Serialize2DCircleStaticColliderEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData)
{
    if(pInEnt->numComponents != 1)
    {
        EASSERT(false);
        printf("SerializeStaticColliderEntity, collider doesn't have exactly 1 component\n");
        return;
    }
    if(pInEnt->components[0].type != ETE_StaticCollider)
    {
        EASSERT(false);
        printf("SerializeStaticColliderEntity, component isn't of type static collider\n");
        return;
    }
    struct PhysicsShape2D* pShape = Ph_GetStaticBodyShape2D(pInEnt->components[0].data.staticCollider.id);
    float r = pShape->data.circle.radius;
    BS_SerializeFloat(r, bs);
}

static struct EntitySerializerPair gPairCircle = 
{
    .deserialize = &DeSerialize2DCircleStaticColliderEntity,
    .serialize = &Serialize2DCircleStaticColliderEntity,
};

struct EntitySerializerPair Et2D_Get2DCircleStaticColliderSerializerPair()
{
    return gPairCircle;
}

