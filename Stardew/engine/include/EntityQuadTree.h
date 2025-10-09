#ifndef ENTITY2DQUADTREE_H
#define ENTITY2DQUADTREE_H
#include "HandleDefs.h"
#include "Entities.h"
#include <cglm/cglm.h>
#define VECTOR(a) a*

struct GameFrameworkLayer;

struct Entity2DQuadTreeInitArgs
{
    int x;
    int y;
    int w;
    int h;
};

void InitEntity2DQuadtreeSystem();

HEntity2DQuadtreeNode GetEntity2DQuadTree(struct Entity2DQuadTreeInitArgs* args);

void DestroyEntity2DQuadTree(HEntity2DQuadtreeNode quadTree);

HEntity2DQuadtreeEntityRef Entity2DQuadTree_Insert(HEntity2DQuadtreeNode quadTree, HEntity2D hEnt, struct GameFrameworkLayer* pLayer, int depth, int maxDepth);

void Entity2DQuadTree_Remove(HEntity2DQuadtreeNode quadTree, HEntity2DQuadtreeEntityRef ent);

VECTOR(HEntity2D) Entity2DQuadTree_Query(HEntity2DQuadtreeNode quadTree, vec2 regionTL, vec2 regionBR, VECTOR(HEntity2D) outEntities);

#endif

