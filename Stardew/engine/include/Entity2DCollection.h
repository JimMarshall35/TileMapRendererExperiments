#ifndef ENTITY2DCOLLECTION_H
#define ENTITY2DCOLLECTION_H

#include "HandleDefs.h"
#include "ObjectPool.h"

struct Entity2DCollection
{
    HEntity2D gEntityListHead;
    HEntity2D gEntityListTail;
    int gNumEnts;
    OBJECT_POOL(struct Entity2D) pEntityPool;
};

#endif