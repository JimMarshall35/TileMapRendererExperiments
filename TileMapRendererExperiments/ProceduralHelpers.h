#pragma once
#include "BasicTypedefs.h"
#include <stdlib.h>

inline static u16 GetRandomIntBetween(u16 min, u16 max) {
	return (u16)rand() % (max - min + 1) + min;
}

inline static u32 GetRandomU32Between(u32 min, u32 max) {
	return (u32)rand() % (max - min + 1) + min;
}

inline static f32 GetRandomFloatZeroToOne() {
	return rand() / (RAND_MAX + 1.);
}


void GetRandomTileMap(u16* map, u32 size);

u32 SelectWeighted(const f32* weights, u32 numWeights);
