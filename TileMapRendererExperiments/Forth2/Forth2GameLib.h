#ifndef FORTH2GAMELIB
#define FORTH2GAMELIB
#ifdef __cplusplus
extern "C" {
#endif
#include "ForthCommonTypedefs.h"
#include "Forth2.h"
#include <stdio.h>

	void PreProcessForthSourceFileContentsIntoBuffer(FILE* fp, char* buf);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif
#endif