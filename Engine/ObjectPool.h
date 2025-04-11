#ifndef  OBJECT_POOL_H
#define OBJECT_POOL_H

#include "IntTypes.h"

void* InitObjectPool(int objectSize, int poolInitialSize);

void* GetObjectPoolIndex(void* pObjectPool, int* pOutIndex);

void FreeObjectPoolIndex(void* pObjectPool, int indexToFree);


struct ObjectPoolData
{
	i64 objectSize;
	i64 capacity;
	i64 freeObjectsArraySize;
	u64* freeObjectIndicessArray;
	// 16 byte aligned
};

#define OBJECT_POOL(a) a*
#define NEW_OBJECT_POOL(a, size) InitObjectPool(sizeof(a),size);

#define ObjectPoolCapacity(pObjectPool) ((((struct ObjectPoolData*)pObjectPool) - 1)->capacity)

#define OBJ_POOL_BOUNDS_CHECK(handle, rVal, pool)\
{\
	bool bBoundsValid = handle < ObjectPoolCapacity(pool) && handle >= 0;\
	if(!bBoundsValid){\
		printf("function '%s' invalid bounds handle %i", __FUNCTION__, handle);\
		return rVal;\
	}\
}

#define OBJ_POOL_BOUNDS_CHECK(handle, pool)\
{\
	bool bBoundsValid = handle < ObjectPoolCapacity(pool) && handle >= 0;\
	if(!bBoundsValid){\
		printf("function '%s' invalid bounds handle %i", __FUNCTION__, handle);\
		return;\
	}\
}


#endif // ! OBJECT_POOL_H
