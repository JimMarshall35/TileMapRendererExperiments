#include "ProceduralHelpers.h"


void GetRandomTileMap(u16* map, u32 size) {
	for (int i = 0; i < size; i++) {
		u16 r = GetRandomIntBetween(0, (28 * 37));
		map[i] = r;
	}
}

u32 SelectWeighted(const f32* weights, u32 numWeights) {
	f32 randOneToZero = GetRandomFloatZeroToOne();
	f32 accumulatedWeight = 0.0f;
	for (int i = 0; i < numWeights; i++) {
		accumulatedWeight += weights[i];
		if (accumulatedWeight >= randOneToZero) {
			return i;
		}
	}
	return 0;
}