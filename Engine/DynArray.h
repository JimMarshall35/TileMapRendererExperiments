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
	u32 itemSize;
	u32 capacity;
	u32 size;
	u32 _padding;
	// 16 byte aligned
} VectorData;

#define VectorSize(vector) ((((VectorData*)vector) - 1)->size)
#define VectorData_DEBUG(vector)(((VectorData*)vector) - 1)

#define NEW_VECTOR(a) VectorInit(sizeof(a));
#define VECTOR(a) a*

#endif