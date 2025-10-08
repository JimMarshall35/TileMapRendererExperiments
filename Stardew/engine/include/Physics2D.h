#ifndef PHYSICS2D_H
#define PHYSICS2D_H
#include "HandleDefs.h"
#include <cglm/cglm.h>
#include "DynArray.h"

typedef vec2 Physics2DPoint;

struct Physics2DRect
{
    vec2 tl;
    vec2 br;
};

struct Physics2DPoly
{
    VECTOR(Physics2DPoint) pPoints;
};

struct Physics2DCircle
{
    vec2 center;
    float radius;
};

enum PhysicsBodyType
{
    PBT_Rect,
    PBT_Circle,
    PBT_Ellipse,
    PBT_Poly
};

struct PhysicsShape2D
{
    enum PhysicsBodyType type;
    union
    {
        struct Physics2DCircle circle;
        struct Physics2DPoly poly;
        struct Physics2DRect rect;
    }data;
};

struct KinematicBodyOptions
{
    u32 bLockRotation : 1;
};

HPhysicsWorld Ph_GetPhysicsWorld(float gravityX, float gravityY, float pixelsPerMeter);

void Ph_PixelCoords2PhysicsCoords(vec2 inPixelCoords, vec2 outPhysicsCoords);

void Ph_PhysicsCoords2PixelCoords(vec2 inPhysicsCoords, vec2 outPixelCoords);

HStaticBody Ph_GetStaticBody2D(struct PhysicsShape2D* pShape);

struct PhysicsShape2D* Ph_GetStaticBodyShape2D(HStaticBody hStatic);

HKinematicBody GetKinematicBody(struct PhysicsShape2D* pShape, struct KinematicBodyOptions* pOptions);

#endif
