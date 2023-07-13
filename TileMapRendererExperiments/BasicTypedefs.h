#pragma once
#include <stdint.h>
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

enum class Directions {
	NONE = 0,
	UP = 1,
	DOWN = 2,
	LEFT = 4,
	RIGHT = 8,

};
inline Directions operator|(Directions a, Directions b)
{
	return static_cast<Directions>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline void operator|=(Directions& a, Directions b)
{
	a = static_cast<Directions>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline unsigned int operator&(Directions a, Directions b)
{
	return static_cast<unsigned int>(a) & static_cast<unsigned int>(b);
}

#define NUMDIRECTIONS 4