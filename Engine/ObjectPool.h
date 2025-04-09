#ifndef  OBJECT_POOL_H
#define OBJECT_POOL_H

void* InitObjectPool(int objectSize, int poolInitialSize);

void* GetObjectPoolIndex(void* pObjectPool, int* pOutIndex);

void FreeObjectPoolIndex(void* pObjectPool, int indexToFree);


struct ObjectPoolData
{
	i64 objectSize;
	i64 capacity;
	i64 freeObjectsArraySize;
	u64* freeObjectIndicessArray;
};

#define OBJECT_POOL(a) a*
#define NEW_OBJECT_POOL(a, size) InitObjectPool(sizeof(a),size);

#define ObjectPoolCapacity(pObjectPool) ((((struct ObjectPoolData*)pObjectPool) - 1)->capacity)

#endif // ! OBJECT_POOL_H
