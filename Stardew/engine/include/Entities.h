#ifndef ENTITIES_H
#define ENTITIES_H

#include "Game2DLayer.h"
#include "HandleDefs.h"

#include <box2d/box2d.h>

struct Entity2D;

typedef void (*OnEntityInitFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT);
typedef void (*UpdateFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT);
typedef void (*UpdatePostPhysicsFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT);
typedef void (*DrawFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, DrawContext* context);
typedef void (*InputFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, InputContext* context);


typedef i32 EntityType;

/* types built into the engine */
enum ComponentType
{
    ETE_Sprite,
    ETE_StaticCollider,
    ETE_DynamicCollider,
    ETE_TextSprite,
    ETE_SpriteAnimator,
    ETE_Last
};

struct Component
{
    enum ComponentType type;
    union
    {
        struct SpriteData
        {
            hSprite sprite;
        }sprite;

        struct StaticCollider
        {
            b2BodyId id;
        }staticCollider;
        
    }data;
};

struct EntityDef
{
    OnEntityInitFn init;
    UpdateFn update;
    UpdatePostPhysicsFn postPhys;
    DrawFn draw;
    InputFn input;

};

void Et2D_RegisterEntityType(EntityType typeIndex, const char* typeName, struct EntityDef* def);

struct Entity2D
{
    struct Transform2D transform;
    EntityType type;
    void* pData;
    HEntity2D hFirstChild;
    HEntity2D nextSibling;
    HEntity2D previousSibling;
};

#endif