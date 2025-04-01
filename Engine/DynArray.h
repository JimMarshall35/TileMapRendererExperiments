#ifndef DYNARRAY_H
#define DYNARRAY_H

void* VectorInit(unsigned int itemSize);
void* VectorResize(void* vector, unsigned int size);
void* VectorPush(void* vector, void* item);
void* VectorPop(void* vector);
void* VectorTop(void* vector);

unsigned int VectorSize(void* vector);
void DestoryVector(void* vector);

#define VECTOR(a) VectorInit(sizeof(a));

#endif