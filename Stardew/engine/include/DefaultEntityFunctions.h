#ifndef DEFAULTENTITYFUNCTIONS_H
#define DEFAULTENTITYFUNCTIONS_H
#include "DrawContext.h"
#include "DynArray.h"

struct Entity2D;
struct GameFrameworkLayer;
struct DrawContext;
typedef struct DrawContext DrawContext;
struct InputContext;
typedef struct InputContext InputContext;
struct Transform2D;

void Entity2DOnInit(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer);
void Entity2DUpdateFn(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT);
void Entity2DUpdatePostPhysicsFn(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT);
void Entity2DDrawFn(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, struct Transform2D* pCam, VECTOR(Worldspace2DVert)* outVerts, VECTOR(VertIndexT)* outIndices, VertIndexT* pNextIndex);
void Entity2DInputFn(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, InputContext* context);
void Entity2DOnDestroyFn(struct Entity2D* pEnt);
void Entity2DGetBoundingBoxFn(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, vec2 outTL, vec2 outBR);

#endif