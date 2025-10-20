#include "EntityQuadTree.h"
#include "ObjectPool.h"
#include "Entities.h"
#include "AssertLib.h"
#include "Geometry.h"
#include "Game2DLayer.h"
#include "GameFramework.h"

enum Entity2DQuadtreeQuadrant
{
    Quadtree_TL,
    Quadtree_TR,
    Quadtree_BL,
    Quadtree_BR,
};

struct Entity2DQuadTreeEntityRef
{
    HEntity2DQuadtreeEntityRef hNextSibling;
    HEntity2DQuadtreeEntityRef hPrevSibling;
    HEntity2DQuadtreeNode hParentNode;
    HEntity2D hEntity;
};

struct Entity2DQuadtreeNode
{
    vec2 tl;
    float w, h;

    HEntity2DQuadtreeEntityRef entityListHead;
    HEntity2DQuadtreeEntityRef entityListTail;
    int numEntities;
    HEntity2DQuadtreeNode children[4];
};

static OBJECT_POOL(struct Entity2DQuadtreeNode) gNodePool = NULL;

static OBJECT_POOL(struct Entity2DQuadTreeEntityRef) gEntityRefPool = NULL;

static void GetQuadtreeNodeQuadrant(struct Entity2DQuadtreeNode* pNode, enum Entity2DQuadtreeQuadrant quadrant, vec2 tl, vec2 br)
{
    switch (quadrant)
    {
    case Quadtree_TL:
        tl[0] = pNode->tl[0];
        tl[1] = pNode->tl[1];
        br[0] = pNode->tl[0] + pNode->w / 2.0f;
        br[1] = pNode->tl[1] + pNode->h / 2.0f;
        break;
    case Quadtree_TR:
        tl[0] = pNode->tl[0] + pNode->w / 2.0f;
        tl[1] = pNode->tl[1];
        br[0] = pNode->tl[0] + pNode->w;
        br[1] = pNode->tl[1] + pNode->h / 2.0f;
        break;
    case Quadtree_BL:
        tl[0] = pNode->tl[0];
        tl[1] = pNode->tl[1] + pNode->h / 2.0f;
        br[0] = pNode->tl[0] + pNode->w / 2.0f;
        br[1] = pNode->tl[1] + pNode->h;
        break;
    case Quadtree_BR:
        tl[0] = pNode->tl[0] + pNode->w / 2.0f;
        tl[1] = pNode->tl[1] + pNode->h / 2.0f;
        br[0] = pNode->tl[0] + pNode->w;
        br[1] = pNode->tl[1] + pNode->h;
        break;
    default:
        EASSERT(false);
        break;
    }
}

static bool IsContainedWithin(vec2 quadrantTL, vec2 quadrantBR, vec2 rectTL, vec2 rectBR)
{
    if(rectTL[0] >= quadrantTL[0])
    {
        if(rectBR[0] <= quadrantBR[0])
        {
            if(rectTL[1] >= quadrantTL[1])
            {
                if(rectBR[1] <= quadrantBR[1])
                {
                    return true;
                }
            }
        }
    }
    return false;
}

static void NewQuadtreeNode(int x, int y, int w, int h, struct Entity2DQuadtreeNode* pNode)
{
    pNode->tl[0] = x;
    pNode->tl[1] = y;
    pNode->w = w;
    pNode->h = h;
    pNode->entityListHead = NULL_HANDLE;
    pNode->entityListTail = NULL_HANDLE;

    pNode->numEntities = 0;
    for(int i=0; i<4; i++)
    {
        pNode->children[i] = NULL_HANDLE;
    }
}

static void NewQuadTreeEntityRef(HEntity2D hEnt, struct Entity2DQuadTreeEntityRef* pRef)
{
    pRef->hNextSibling = NULL_HANDLE;
    pRef->hPrevSibling = NULL_HANDLE;
    pRef->hEntity = hEnt;
}

void InitEntity2DQuadtreeSystem()
{
    gEntityRefPool = NEW_OBJECT_POOL(struct Entity2DQuadTreeEntityRef, 1024);
    gNodePool = NEW_OBJECT_POOL(struct Entity2DQuadtreeNode, 512);
}

HEntity2DQuadtreeNode GetEntity2DQuadTree(struct Entity2DQuadTreeInitArgs* args)
{
    HEntity2DQuadtreeNode hOutNode;
    gNodePool = GetObjectPoolIndex(gNodePool, &hOutNode);

    NewQuadtreeNode(args->x, args->y, args->w, args->h, &gNodePool[hOutNode]);
    return hOutNode;
}

void DestroyEntity2DQuadTree(HEntity2DQuadtreeNode quadTree)
{
    for(int i=0; i<4; i++)
    {
        HEntity2DQuadtreeNode child = gNodePool[quadTree].children[i];
        if(child != NULL_HANDLE)
        {
            DestroyEntity2DQuadTree(child);
        }
    }
    FreeObjectPoolIndex(gNodePool, quadTree);
}

HEntity2DQuadtreeEntityRef Entity2DQuadTree_Insert(struct Entity2DCollection* pCollection, HEntity2DQuadtreeNode quadTree, HEntity2D hEnt, struct GameFrameworkLayer* pLayer, int depth, int maxDepth)
{
    struct GameLayer2DData* pData = pLayer->userData;

    struct Entity2D* pEnt = Et2D_GetEntity(pCollection, hEnt);
    struct Entity2DQuadtreeNode* pNode = &gNodePool[quadTree];
    vec2 bbtl, bbbr;
    pEnt->getBB(pEnt, pLayer, bbtl, bbbr);
    for(int i=0; i<4; i++)
    {
        vec2 quadrantTL, quadrantBR;
        GetQuadtreeNodeQuadrant(pNode, (enum Entity2DQuadtreeQuadrant)i, bbtl, bbbr);
        if(IsContainedWithin(quadrantTL, quadrantBR, bbtl, bbbr) && ++depth < maxDepth)
        {
            if(pNode->children[i] == NULL_HANDLE)
            {
                HEntity2DQuadtreeNode hNode;
                gNodePool = GetObjectPoolIndex(gNodePool, &hNode);
                struct Entity2DQuadtreeNode* pChildNode = &gNodePool[hNode];
                NewQuadtreeNode(quadrantTL[0], quadrantTL[1], quadrantBR[0] - quadrantTL[0], quadrantBR[1] - quadrantTL[1], pChildNode);
                pNode->children[i] = hNode;
            }
            Entity2DQuadTree_Insert(pCollection, pNode->children[i], hEnt, pLayer, depth, maxDepth);
        }
        else
        {
            HEntity2DQuadtreeEntityRef hRef;
            gEntityRefPool = GetObjectPoolIndex(gEntityRefPool, &hRef);
            struct Entity2DQuadTreeEntityRef* pRef = &gEntityRefPool[hRef];
            NewQuadTreeEntityRef(hEnt, pRef);
            if(pNode->numEntities == 0)
            {
                pNode->entityListHead = hRef;
                pNode->entityListTail = hRef;
            }
            else
            {
                struct Entity2DQuadTreeEntityRef* pTail = &gEntityRefPool[pNode->entityListTail];
                pTail->hNextSibling = hRef;
                pRef->hPrevSibling = pNode->entityListTail;
                pNode->entityListTail = hRef;
            }
            pNode->numEntities++;
        }
    }
}

void Entity2DQuadTree_Remove(HEntity2DQuadtreeNode quadTree, HEntity2DQuadtreeEntityRef ent)
{
    struct Entity2DQuadtreeNode* pNode = &gNodePool[gEntityRefPool[ent].hParentNode];

    pNode->numEntities--;

    if(pNode->entityListHead == ent)
    {
        pNode->entityListHead = NULL_HANDLE;
    }

    if(pNode->entityListTail == ent)
    {
        pNode->entityListTail = gEntityRefPool[ent].hPrevSibling;
    }

    if(gEntityRefPool[ent].hPrevSibling != NULL_HANDLE)
    {
        struct Entity2DQuadTreeEntityRef* pPrev = &gEntityRefPool[gEntityRefPool[ent].hPrevSibling];
        pPrev->hNextSibling = gEntityRefPool[ent].hNextSibling;
    }

    if(gEntityRefPool[ent].hNextSibling)
    {
        struct Entity2DQuadTreeEntityRef* pNext = &gEntityRefPool[gEntityRefPool[ent].hNextSibling];
        pNext->hPrevSibling = gEntityRefPool[ent].hPrevSibling;
    }

    FreeObjectPoolIndex(gEntityRefPool, ent);
}

VECTOR(HEntity2D) Entity2DQuadTree_Query(HEntity2DQuadtreeNode quadTree, vec2 regionTL, vec2 regionBR, VECTOR(HEntity2D) outEntities)
{
    /* Implement me next! */
    struct Entity2DQuadtreeNode* pNode = &gNodePool[quadTree];
    vec2 nodeBr = {
        pNode->tl[0] + pNode->w,
        pNode->tl[1] + pNode->h
    };
    if(Ge_AABBIntersect(pNode->tl, nodeBr, regionTL, regionBR))
    {
        HEntity2DQuadtreeEntityRef ref = pNode->entityListHead;
        while(ref != NULL_HANDLE)
        {
            struct Entity2DQuadTreeEntityRef* pRef = &gEntityRefPool[ref];
            outEntities = VectorPush(outEntities, &pRef->hEntity);
            ref = pRef->hNextSibling;
        }
        for(int i=0; i<4; i++)
        {
            HEntity2DQuadtreeNode child = pNode->children[i];
            if(child != NULL_HANDLE)
            {
                outEntities = Entity2DQuadTree_Query(child, regionTL, regionBR, outEntities);
            }
        }
    }
    
    return outEntities;
}

