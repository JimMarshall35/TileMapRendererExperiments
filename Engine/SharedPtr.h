#ifndef  SHAREDPTR_H
#define SHAREDPTR_H



void* Sptr_New(size_t size);

void Sptr_AddRef(void* pointer);

void Sptr_RemoveRef(void* pointer);

#define SHARED_PTR(p) p*

#define SHARED_PTR_NEW(a) Sptr_New(sizeof(a))

#endif // ! SHAREDPTR_H
