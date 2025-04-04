#ifndef DYNARRAY_H
#define DYNARRAY_H
#include "IntTypes.h"

void* VectorInit(unsigned int itemSize);
void* VectorResize(void* vector, unsigned int size);
void* VectorPush(void* vector, void* item);
void* VectorPop(void* vector);
void* VectorTop(void* vector);

//unsigned int VectorSize(void* vector);
void DestoryVector(void* vector);

typedef struct
{
	u64 itemSize;
	u64 capacity;
	u64 size;
} VectorData;

#define VectorSize(vector) ((((VectorData*)vector) - 1)->size)
#define NEW_VECTOR(a) VectorInit(sizeof(a));
#define VECTOR(a) a*

#endif