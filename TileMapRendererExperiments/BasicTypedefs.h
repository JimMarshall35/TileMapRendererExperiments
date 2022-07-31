#pragma once
#include <stdint.h>
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int8_t i8;
typedef int16_t i16;
typedef int16_t i32;

typedef float f32;
typedef double f64;

enum class Directions : u32 {
	UP,
	DOWN,
	LEFT,
	RIGHT,

	NUMDIRECTIONS
};