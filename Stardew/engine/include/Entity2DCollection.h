#ifndef ENTITY2DCOLLECTION_H
#define ENTITY2DCOLLECTION_H

#include "HandleDefs.h"
#include "ObjectPool.h"

/*
    Entities that are moving dynamically, we keep in a list so we can cull with brute force.
    The overhead of removing and inserting them from the quadtree every time they move is too much,
    while the quadtree remains a good option for static entities 
*/
struct DynamicEntityListItem
{
    HEntity2D hEnt;
    HDynamicEntityListItem hNext;
    HDynamicEntityListItem hPrev;
};

struct DynamicEnt2DList
{
    OBJECT_POOL(struct DynamicEntityListItem) pDynamicListItemPool;
    HDynamicEntityListItem hDynamicListHead;
    HDynamicEntityListItem hDynamicListTail;
    int nDynamicListSize;

};

struct Entity2DCollection
{
    HEntity2D gEntityListHead;
    HEntity2D gEntityListTail;
    int gNumEnts;
    OBJECT_POOL(struct Entity2D) pEntityPool;
    struct DynamicEnt2DList dynamicEntities;
};

HDynamicEntityListItem DynL_AddEntity(struct DynamicEnt2DList* pDynList, HEntity2D hEnt);
void DynL_RemoveItem(struct DynamicEnt2DList* pDynList, HDynamicEntityListItem hListItem);
int DynL_GetSize(struct DynamicEnt2DList* pDynList);



#endif