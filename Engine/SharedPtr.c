#include "SharedPtr.h"
#include <stdlib.h>
#include "IntTypes.h"

struct SharedPtrHeader
{
	i32 reference;
	i32 padding[3];
};

void* Sptr_New(size_t size)
{
	struct SharedPtrHeader* ptr = malloc(size + sizeof(struct SharedPtrHeader));

	ptr->reference = 1;
	return ptr + 1;;
}

void Sptr_AddRef(void* pointer)
{
	struct SharedPtrHeader* ptr = (struct SharedPtrHeader*)pointer - 1;
	ptr->reference++;
}

void Sptr_RemoveRef(void* pointer)
{

	struct SharedPtrHeader* ptr = (struct SharedPtrHeader*)pointer - 1;
	ptr->reference--;
	if (ptr->reference <= 0)
	{
		free(ptr);
	}
}
