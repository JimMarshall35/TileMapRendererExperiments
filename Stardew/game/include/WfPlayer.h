#ifndef WFPLAYER_H
#define WFPLAYER_H

#include <cglm/cglm.h>
struct BinarySerializer;
struct Entity2D;
struct GameLayer2DData;

void WfInitPlayer();

void WfDeSerializePlayerEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData);

void WfSerializePlayerEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData);

void WfMakeIntoPlayerEntity(struct Entity2D* pInEnt, struct GameLayer2DData* pData, vec2 spawnAtGroundPos);

#endif