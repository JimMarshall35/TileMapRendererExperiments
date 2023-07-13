#ifndef FORTH2GAMELIB
#define FORTH2GAMELIB
#ifdef __cplusplus
extern "C" {
#endif
#include "ForthCommonTypedefs.h"
#include "Forth2.h"


ExecutionToken RegisterCallableFunction(ForthVm* vm, ForthCFunc function, const char* name);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif
#endif