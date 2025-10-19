#include "Entities.h"
#include "ObjectPool.h"
#include "AssertLib.h"
#include "BinarySerializer.h"
#include "StaticColliderEntity.h"
#include <string.h>

static HEntity2D gEntityListHead = NULL_HANDLE;
static HEntity2D gEntityListTail = NULL_HANDLE;
static int gNumEnts = 0;

static OBJECT_POOL(struct Entity2D) pEntityPool = NULL;
static VECTOR(struct EntitySerializerPair) pSerializers = NULL;


void Entity2DOnInit(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer)
{
    Co_InitComponents(pEnt, pLayer);
    if(pEnt->bKeepInQuadtree)
    {

    }
}

void Entity2DUpdate(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT)
{
    Co_UpdateComponents(pEnt, pLayer, deltaT);
}

void Entity2DUpdatePostPhysics(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, float deltaT)
{
    Co_Entity2DUpdatePostPhysicsFn(pEnt, pLayer, deltaT);
}

void Entity2DDraw(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, struct Transform2D* pCam, VECTOR(Worldspace2DVert)* outVerts, VECTOR(VertIndexT)* outIndices, VertIndexT* pNextIndex)
{
    Co_DrawComponents(pEnt, pLayer, pCam, outVerts, outIndices, pNextIndex);
}

void Entity2DInput(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, InputContext* context)
{
    Co_InputComponents(pEnt, pLayer, context);
}

void Entity2DOnDestroy(struct Entity2D* pEnt)
{
    Co_DestroyComponents(pEnt);
}


void Entity2DGetBoundingBox(struct Entity2D* pEnt, struct GameFrameworkLayer* pLayer, vec2 outTL, vec2 outBR)
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


void Et2D_Init(RegisterGameEntitiesFn registerGameEntities)
{
    pEntityPool = NEW_OBJECT_POOL(struct Entity2D, 512);
    pSerializers = NEW_VECTOR(struct EntitySerializerPair);
    pSerializers = VectorClear(pSerializers);

    struct EntitySerializerPair scCtorRect = Et2D_Get2DRectStaticColliderSerializerPair();
    Et2D_RegisterEntityType(EBET_StaticColliderRect, &scCtorRect);
    struct EntitySerializerPair scCtorCircle = Et2D_Get2DCircleStaticColliderSerializerPair();
    Et2D_RegisterEntityType(EBET_StaticColliderCircle, &scCtorCircle);


    struct EntitySerializerPair scCtorEllipse = Et2D_Get2DEllipseStaticColliderSerializerPair();
    struct EntitySerializerPair scCtorPoly  = Et2D_Get2DPolygonStaticColliderSerializerPair();

    Et2D_RegisterEntityType(EBET_StaticColliderPoly, &scCtorPoly);
    Et2D_RegisterEntityType(EBET_StaticColliderEllipse, &scCtorEllipse);

    if(registerGameEntities)
    {
        registerGameEntities();
    }
}

void Et2D_DestroyEntity(HEntity2D hEnt)
{
    struct Entity2D* pEnt = &pEntityPool[hEnt];

    if(gEntityListHead == hEnt)
    {
        gEntityListTail = pEnt->nextSibling;
    }
    if(gEntityListTail == hEnt)
    {
        gEntityListHead = pEnt->previousSibling;
    }

    if(pEnt->nextSibling != NULL_HANDLE)
    {
        struct Entity2D* pNext = &pEntityPool[pEnt->nextSibling];
        pNext->previousSibling = pEnt->previousSibling;

    }
    if(pEnt->previousSibling != NULL_HANDLE)
    {
        struct Entity2D* pPrev = &pEntityPool[pEnt->previousSibling];
        pPrev->nextSibling = pEnt->nextSibling;
    }

    pEnt->onDestroy(pEnt);
    gNumEnts--;
    FreeObjectPoolIndex(pEntityPool, hEnt);
}

HEntity2D Et2D_AddEntity(struct Entity2D* pEnt)
{
    HEntity2D hEnt = NULL_HANDLE;
    pEnt->nextSibling = NULL_HANDLE;
    pEnt->previousSibling = NULL_HANDLE;
    pEntityPool = GetObjectPoolIndex(pEntityPool, &hEnt);
    EASSERT(hEnt != NULL_HANDLE);
    memcpy(&pEntityPool[hEnt], pEnt, sizeof(struct Entity2D));
    pEnt = &pEntityPool[hEnt];
    if(gEntityListHead == NULL_HANDLE)
    {
        gEntityListHead = hEnt;
        gEntityListTail = hEnt;
    }
    else
    {
        struct Entity2D* pLast = &pEntityPool[gEntityListTail];
        pLast->nextSibling = hEnt;
        pEnt->previousSibling = gEntityListTail;
        gEntityListTail = hEnt;
    }
    gNumEnts++;
    return hEnt;
}

static void DeserializeEntityV1(struct BinarySerializer* bs, struct GameLayer2DData* pData)
{
    u32 entityType;
    BS_DeSerializeU32(&entityType, bs);
    struct Entity2D ent;
    memset(&ent, 0, sizeof(struct Entity2D));
    ent.nextSibling = NULL_HANDLE;
    ent.previousSibling = NULL_HANDLE;
    ent.inDrawLayer = -1;
    ent.type = entityType;

    Et2D_DeserializeCommon(bs, &ent);

    if(ent.type < VectorSize(pSerializers))
    {
        pSerializers[ent.type].deserialize(bs, &ent, pData);
    }
    else 
    {
        printf("DESERIALIZE: Entity Serializer type %i out of range\n", ent.type);
    }
    Et2D_AddEntity(&ent);
}

static void LoadEntitiesV1(struct BinarySerializer* bs, struct GameLayer2DData* pData)
{
    u32 numEntities = 0;
    BS_DeSerializeU32(&numEntities, bs);
    for(int i=0; i<numEntities; i++)
    {
        DeserializeEntityV1(bs, pData);
    }
}

static void LoadEntities(struct BinarySerializer* bs, struct GameLayer2DData* pData)
{
    EASSERT(!bs->bSaving);
    u32 version = 0;
    BS_DeSerializeU32(&version, bs);
    switch(version)
    {
    case 1:
        LoadEntitiesV1(bs, pData);
        break;
    default:
        printf("E2D unsupported version %i\n", version);
        EASSERT(false);
        break;
    }
}

static void SaveEntities(struct BinarySerializer* bs, struct GameLayer2DData* pData)
{
    EASSERT(bs->bSaving);
    BS_SerializeU32(gNumEnts, bs);
    HEntity2D hOn = gEntityListHead;
    while(hOn != NULL_HANDLE)
    {
        struct Entity2D* pOn = &pEntityPool[hOn];
        BS_SerializeU32(pOn->type, bs);
        Et2D_SerializeCommon(bs, pOn);
        if(pOn->type < VectorSize(pSerializers))
        {
            pSerializers[pOn->type].serialize(bs, pOn, pData);
        }
        else 
        {
            printf("Entity Serializer type %i out of range\n", pOn->type);
        }
        hOn = pOn->nextSibling;
    }
}

/* both serialize and deserialize */
void Et2D_SerializeEntities(struct BinarySerializer* bs, struct GameLayer2DData* pData)
{
    if(bs->bSaving)
    {
        SaveEntities(bs, pData);
    }
    else
    {
        LoadEntities(bs, pData);
    }
}

void Et2D_RegisterEntityType(u32 typeID, struct EntitySerializerPair* pair)
{
    //pSerializers = (pSerializers, pair);
    int size = VectorSize(pSerializers);
    EASSERT(size == typeID);
    pSerializers = VectorPush(pSerializers, pair);
}

void Et2D_DeserializeCommon(struct BinarySerializer* bs, struct Entity2D* pOutEnt)
{
    u32 version = 0;
    BS_DeSerializeI32(&version, bs);
    switch(version)
    {
    case 1:
        BS_DeSerializeFloat(&pOutEnt->transform.position[0], bs);
        BS_DeSerializeFloat(&pOutEnt->transform.position[1], bs);
        BS_DeSerializeFloat(&pOutEnt->transform.scale[0], bs);
        BS_DeSerializeFloat(&pOutEnt->transform.scale[1], bs);
        BS_DeSerializeFloat(&pOutEnt->transform.rotation, bs);
        
        BS_DeSerializeU32(&version, bs);
        pOutEnt->bKeepInQuadtree = version != 0;
        pOutEnt->init = &Entity2DOnInit;
        pOutEnt->update = &Entity2DUpdate;
        pOutEnt->postPhys = &Entity2DUpdatePostPhysics;
        pOutEnt->draw = &Entity2DDraw;
        pOutEnt->input = &Entity2DInput;
        pOutEnt->onDestroy = &Entity2DOnDestroy;
        pOutEnt->getBB = &Entity2DGetBoundingBox;
        break;
    }
}


void Et2D_SerializeCommon(struct BinarySerializer* bs, struct Entity2D* pInEnt)
{
    u32 version = 1;
    BS_SerializeI32(version, bs);
    BS_SerializeFloat(pInEnt->transform.position[0], bs);
    BS_SerializeFloat(pInEnt->transform.position[1], bs);
    BS_SerializeFloat(pInEnt->transform.scale[0], bs);
    BS_SerializeFloat(pInEnt->transform.scale[1], bs);
    BS_SerializeFloat(pInEnt->transform.rotation, bs);
    version = (u32)pInEnt->bKeepInQuadtree;
    BS_SerializeU32(version, bs);
}

struct Entity2D* Et2D_GetEntity(HEntity2D hEnt)
{
    return &pEntityPool[hEnt];
}

void Et2D_IterateEntities(Entity2DIterator itr, void* pUser)
{
    HEntity2D hOnEnt = gEntityListHead;
    int i = 0;
    while(hOnEnt != NULL_HANDLE)
    {
        struct Entity2D* pEntity = Et2D_GetEntity(hOnEnt);
        if(!itr(pEntity, i++, pUser))
            break;
        hOnEnt = pEntity->nextSibling;
    }
}

