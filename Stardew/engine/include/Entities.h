#ifndef ENTITIES_H
#define ENTITIES_H

#include "Game2DLayer.h"
#include "HandleDefs.h"

#include <box2d/box2d.h>

struct Entity2D;

typedef void (*Entity2DOnInitFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT);
typedef void (*Entity2DUpdateFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT);
typedef void (*Entity2DUpdatePostPhysicsFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT);
typedef void (*Entity2DDrawFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, DrawContext* context);
typedef void (*Entity2DInputFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, InputContext* context);
typedef void (*Entity2DOnDestroyFn)(struct Entity2D* pEnt);


typedef void(*EntityDeserializeFn)(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData);
typedef void(*EntitySerializeFn)(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData);

typedef void(*RegisterGameEntitiesFn)(void);

struct EntitySerializerPair
{
    EntityDeserializeFn deserialize;
    EntitySerializeFn serialize;
};


typedef i32 EntityType;

#define MAX_COMPONENTS 16

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

struct Component2D
{
    enum ComponentType type;
    union
    {
        struct Sprite
        {
            hSprite sprite;
        }sprite;

        struct StaticCollider
        {
            HStaticBody id;
        }staticCollider;

        struct KinematicCollider
        {
            HKinematicBody id;
        }kinematicCollider;

        struct TextSprite
        {
            char* content;
            HFont font;
            float fSizePts;
            hAtlas atlas;
            float r, g, b, a;
        }textSprite;
        
        struct AnimatedSprite
        {
            float timer;
            hSprite* pSprites;
            int numSprites;
            int onSprite;
            float fps;
            bool bRepeat;
        }spriteAnimator;
    }data;
};

enum EngineBuiltinEntityType
{
    EBET_StaticColliderRect,
    EBET_StaticColliderCircle,
    EBET_Last
};

struct BinarySerializer;

void Et2D_RegisterEntityType(u32 typeID, struct EntitySerializerPair* pair);

void Et2D_Init(RegisterGameEntitiesFn registerGameEntities);

HEntity2D Et2D_AddEntity(struct Entity2D* pEnt);

/* both serialize and deserialize */
void Et2D_SerializeEntities(struct BinarySerializer* bs, struct GameLayer2DData* pData);

void Et2D_DeserializeCommon(struct BinarySerializer* bs, struct Entity2D* pOutEnt);
void Et2D_SerializeCommon(struct BinarySerializer* bs, struct Entity2D* pInEnt);

struct Entity2D
{

    Entity2DOnInitFn init;
    Entity2DUpdateFn update;
    Entity2DUpdatePostPhysicsFn postPhys;
    Entity2DDrawFn draw;
    Entity2DInputFn input;
    Entity2DOnDestroyFn onDestroy;

    struct Transform2D transform;
    EntityType type;
    void* pData;
    HEntity2D nextSibling;
    HEntity2D previousSibling;

    int numComponents;
    struct Component2D components[MAX_COMPONENTS];
};

#endif