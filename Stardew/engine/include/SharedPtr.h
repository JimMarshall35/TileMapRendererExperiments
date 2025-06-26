#ifndef  SHAREDPTR_H
#define SHAREDPTR_H

//called just before the memory is freed
#include <stddef.h>

typedef void(*SharedPtrDestuctorFn)(void* data);

void* Sptr_New(size_t size, SharedPtrDestuctorFn dtor);

void Sptr_AddRef(void* pointer);

void Sptr_RemoveRef(void* pointer);

#define SHARED_PTR(p) p*

#define SHARED_PTR_NEW(a, dtor) Sptr_New(sizeof(a), dtor)

#endif // ! SHAREDPTR_H
