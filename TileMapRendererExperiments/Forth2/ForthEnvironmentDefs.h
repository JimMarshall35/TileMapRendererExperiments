#pragma once
#include <stdint.h>
#define CAN_USE_FLOATS // comment out to disable

#ifdef _WIN64 
typedef int64_t Cell;
typedef uint64_t UCell;
typedef double FCell;
#elif _WIN32
typedef int32_t Cell;
typedef uint32_t UCell;
typedef float FCell;
#endif
