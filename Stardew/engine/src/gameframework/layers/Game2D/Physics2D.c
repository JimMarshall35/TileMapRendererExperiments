#include "Physics2D.h"
#include "box2d/box2d.h"
#include "ObjectPool.h"
#include "Game2DLayer.h"
#include "AssertLib.h"

struct Phys2dWorld
{
    /* data */
    b2WorldId id;
    float gravX;
    float gravY;
    float pxlPerMeter;
};

struct StaticBody2D
{
    b2BodyId bodyID;
    b2Polygon box;
    b2ShapeDef shapedef;
    b2ShapeId shapeID;
};

static OBJECT_POOL(struct Phys2dWorld) gWorldDefPool = NULL;

static OBJECT_POOL(struct StaticBody2D) gStaticBodyPool = NULL;

static OBJECT_POOL(struct StaticBody2D) gKinematicBodyPool = NULL;


void Ph_DestroyPhysicsWorld(HPhysicsWorld world)
{
    b2DestroyWorld(gWorldDefPool[world].id);
    FreeObjectPoolIndex(gWorldDefPool, world);
}

void Ph_Init()
{
    gWorldDefPool = NEW_OBJECT_POOL(struct Phys2dWorld, 32);
    gStaticBodyPool = NEW_OBJECT_POOL(struct StaticBody2D, 256);
    gKinematicBodyPool = NEW_OBJECT_POOL(struct StaticBody2D, 256);
}

HPhysicsWorld Ph_GetPhysicsWorld(float gravityX, float gravityY, float pixelsPerMeter)
{
    HPhysicsWorld index = -1;
    gWorldDefPool = GetObjectPoolIndex(gWorldDefPool, &index);
    b2WorldDef def = b2DefaultWorldDef();
    def.gravity.x = gravityX;
    def.gravity.y = gravityY;
    gWorldDefPool[index].gravX = gravityX;
    gWorldDefPool[index].gravY = gravityY;
    gWorldDefPool[index].pxlPerMeter = pixelsPerMeter;
    gWorldDefPool[index].id = b2CreateWorld(&def);
    return index;
}

float Ph_GetPixelsPerMeter(HPhysicsWorld world)
{
    return gWorldDefPool[world].pxlPerMeter;
}

void Ph_PixelCoords2PhysicsCoords(HPhysicsWorld world, vec2 inPixelCoords, vec2 outPhysicsCoords)
{
    struct Phys2dWorld* pPool = &gWorldDefPool[world];
    outPhysicsCoords[0] = inPixelCoords[0] / pPool->pxlPerMeter;
    outPhysicsCoords[1] = inPixelCoords[1] / pPool->pxlPerMeter;
}

void Ph_PhysicsCoords2PixelCoords(HPhysicsWorld world, vec2 inPhysicsCoords, vec2 outPixelCoords)
{
    struct Phys2dWorld* pPool = &gWorldDefPool[world];
    outPixelCoords[0] = inPhysicsCoords[0] * pPool->pxlPerMeter;
    outPixelCoords[1] = inPhysicsCoords[1] * pPool->pxlPerMeter;
}

HStaticBody Ph_GetStaticBody2D(HPhysicsWorld world, struct PhysicsShape2D* pShape, struct Transform2D* pTransform)
{
    HStaticBody hStatic = -1;
    gStaticBodyPool = GetObjectPoolIndex(gStaticBodyPool, &hStatic);
    switch (pShape->type)
    {
    case PBT_Rect:
        {
            vec2 pixelsRectCenter = {
                pTransform->position[0] + pShape->data.rect.w / 2,
                pTransform->position[1] + pShape->data.rect.h / 2,
            };
            vec2 pixelDims = {
                pShape->data.rect.w,
                pShape->data.rect.h
            };
            vec2 physicsDims;
            vec2 physicsPos;
            Ph_PixelCoords2PhysicsCoords(world, pixelsRectCenter, physicsPos);
            Ph_PixelCoords2PhysicsCoords(world, pixelDims, physicsDims);
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.position = (b2Vec2){physicsPos[0], physicsPos[1]};
            b2BodyId id = b2CreateBody(gWorldDefPool[world].id, &bodyDef);

            gStaticBodyPool[hStatic].bodyID = id;

            gStaticBodyPool[hStatic].box = b2MakeBox(physicsDims[0] / 2.0f, physicsDims[1] / 2.0f);
            gStaticBodyPool[hStatic].shapedef = b2DefaultShapeDef();
            gStaticBodyPool[hStatic].shapeID = b2CreatePolygonShape(id, &gStaticBodyPool[hStatic].shapedef, &gStaticBodyPool[hStatic].box);
        }
        break;
    case PBT_Circle:
        EASSERT(false);
        break;
    case PBT_Ellipse:
        EASSERT(false);
        break;
    case PBT_Poly:
        EASSERT(false);
        break;
    default:
        break;
    }
}

HKinematicBody GetKinematicBody(HPhysicsWorld world, struct PhysicsShape2D* pShape, struct KinematicBodyOptions* pOptions, struct Transform2D* pTransform)
{

}