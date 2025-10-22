#ifndef WFPLAYER_START_H
#define WFPLAYER_START_H

struct BinarySerializer;
struct Entity2D;
struct GameLayer2DData;

void WfDeSerializePlayerStartEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData);

void WfSerializePlayerStartEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData);

void WfInitPlayerStart();

#endif