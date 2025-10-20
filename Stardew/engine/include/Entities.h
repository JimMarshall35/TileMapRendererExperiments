#ifndef ENTITIES_H
#define ENTITIES_H

#include "Game2DLayer.h"
#include "DynArray.h"
#include "DrawContext.h"
#include "Physics2D.h"
#include <box2d/box2d.h>
#include <stdbool.h>

struct Entity2D;
struct Entity2DCollection;


typedef void (*Entity2DOnInitFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer);
typedef void (*Entity2DUpdateFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT);
typedef void (*Entity2DUpdatePostPhysicsFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT);
typedef void (*Entity2DDrawFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, struct Transform2D* pCam, VECTOR(Worldspace2DVert)* outVerts, VECTOR(VertIndexT)* outIndices, VertIndexT* pNextIndex);
typedef void (*Entity2DInputFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, InputContext* context);
typedef void (*Entity2DOnDestroyFn)(struct Entity2D* pEnt);
typedef void (*Entity2DGetBoundingBoxFn)(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, vec2 outTL, vec2 outBR);


typedef void(*EntityDeserializeFn)(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData);
typedef void(*EntitySerializeFn)(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData);

typedef void(*RegisterGameEntitiesFn)(void);

struct EntitySerializerPair
{
    EntityDeserializeFn deserialize;
    EntitySerializeFn serialize;
};

// static HEntity2D gEntityListHead = NULL_HANDLE;
// static HEntity2D gEntityListTail = NULL_HANDLE;
// static int gNumEnts = 0;

// static OBJECT_POOL(struct Entity2D) pEntityPool = NULL;



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

struct Sprite
{
    hSprite sprite;
    struct Transform2D transform;
};

struct StaticCollider
{
    HStaticBody id;
    struct PhysicsShape2D shape;
};

struct KinematicCollider
{
    HKinematicBody id;
    struct PhysicsShape2D shape;
};

struct TextSprite
{
    char* content;
    HFont font;
    float fSizePts;
    hAtlas atlas;
    float r, g, b, a;
};

struct AnimatedSprite
{
    float timer;
    hSprite* pSprites;
    int numSprites;
    int onSprite;
    float fps;
    bool bRepeat;
};

struct Component2D
{
    enum ComponentType type;
    union
    {
        struct Sprite sprite;

        struct StaticCollider staticCollider;

        struct KinematicCollider kinematicCollider;

        struct TextSprite textSprite;
        
        struct AnimatedSprite spriteAnimator;
    }data;
    
};

enum EngineBuiltinEntityType
{
    EBET_StaticColliderRect,
    EBET_StaticColliderCircle,
    EBET_StaticColliderPoly,
    EBET_StaticColliderEllipse,
    EBET_Last
};

struct BinarySerializer;

void Et2D_RegisterEntityType(u32 typeID, struct EntitySerializerPair* pair);

void Et2D_Init(RegisterGameEntitiesFn registerGameEntities);

HEntity2D Et2D_AddEntity(struct Entity2DCollection* pCollection, struct Entity2D* pEnt);

void Et2D_DestroyEntity(struct Entity2DCollection* pCollection, HEntity2D hEnt);

struct Entity2D* Et2D_GetEntity(struct Entity2DCollection* pCollection, HEntity2D hEnt);

/* both serialize and deserialize */

/* return value == should continue iterating */
typedef bool(*Entity2DIterator)(struct Entity2D* pEnt, int i, void* pUser);


void Et2D_IterateEntities(struct Entity2DCollection* pCollection, Entity2DIterator itr, void* pUser);
void Et2D_SerializeEntities(struct Entity2DCollection* pCollection, struct BinarySerializer* bs, struct GameLayer2DData* pData);

void Et2D_DeserializeCommon(struct BinarySerializer* bs, struct Entity2D* pOutEnt);
void Et2D_SerializeCommon(struct BinarySerializer* bs, struct Entity2D* pInEnt);

void Et2D_InitCollection(struct Entity2DCollection* pCollection);
void Et2D_DestroyCollection(struct Entity2DCollection* pCollection);

struct Entity2D
{

    Entity2DOnInitFn init;
    Entity2DUpdateFn update;
    Entity2DUpdatePostPhysicsFn postPhys;
    Entity2DDrawFn draw;
    Entity2DInputFn input;
    Entity2DOnDestroyFn onDestroy;
    Entity2DGetBoundingBoxFn getBB;

    struct Transform2D transform;
    EntityType type;
    
    union
    {
        /* data */
        void* pData;
        HGeneric hData;
    }user;
    
    
    /* just for convenience - this entities handle */
    HEntity2D thisEntity;

    HEntity2D nextSibling;
    HEntity2D previousSibling;

    int numComponents;
    struct Component2D components[MAX_COMPONENTS];

    bool bKeepInQuadtree;

    /* 
        Which object layer of the scene is it in? 
        Effects the order they are drawn in
    */
    int inDrawLayer;
};

/*
    Default base implementations, override with own behavior and then call these at the end
*/
void Entity2DOnInit(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer);
void Entity2DUpdate(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT);
void Entity2DUpdatePostPhysics(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT);
void Entity2DDraw(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, struct Transform2D* pCam, VECTOR(Worldspace2DVert)* outVerts, VECTOR(VertIndexT)* outIndices, VertIndexT* pNextIndex);
void Entity2DInput(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, InputContext* context);
void Entity2DOnDestroy(struct Entity2D* pEnt);
void Entity2DGetBoundingBox(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, vec2 outTL, vec2 outBR);




#endif
