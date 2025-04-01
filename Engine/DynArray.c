#include "DynArray.h"
#include <stdint.h>
#include <string.h>

typedef struct
{
	uint64_t itemSize;
	uint64_t capacity;
	uint64_t size;
} VectorData;

void* VectorInit(unsigned int itemSize)
{
	VectorData* data = malloc(sizeof(VectorData) + itemSize);
	data->itemSize = itemSize;
	data->size = 1;
	data->capacity = 1;
	return data + 1;
}

void* VectorResize(void* vector, unsigned int size)
{
	VectorData* pData = ((VectorData*)vector) - 1;
	if (size <= pData->capacity)
	{
		pData->size = size;
		return vector;
	}
	else if (size > pData->capacity)
	{
		VectorData* pNewAlloc = malloc(sizeof(VectorData) + pData->itemSize * size);
		pData->size = size;
		pData->capacity = size;
		memcpy(pNewAlloc, pData, sizeof(VectorData) + pData->size * pData->itemSize);
		free(pData);
		return pNewAlloc + 1;
	}
	return vector;
}

void* VectorPush(void* vector, void* item)
{
	VectorData* pData = ((VectorData*)vector) - 1;
	if (++pData->size > pData->capacity)
	{
		pData->capacity *= 2;
		vector = VectorResize(vector, pData->capacity);
	}
	return vector;
}

void* VectorPop(void* vector)
{
	VectorData* pData = ((VectorData*)vector) - 1;
	pData->size--;
	return VectorTop(vector);
}

void* VectorTop(void* vector)
{
	VectorData* pData = ((VectorData*)vector) - 1;
	return (char*)vector + ((pData->size - 1) * pData->itemSize);
}

unsigned int VectorSize(void* vector)
{
	VectorData* pData = ((VectorData*)vector) - 1;
	return pData->size;
}

void DestoryVector(void* vector)
{
	VectorData* pData = ((VectorData*)vector) - 1;
	free(pData);
}
