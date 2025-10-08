#ifndef WFROCK_H
#define WFROCK_H


struct BinarySerializer;
struct Entity2D;
struct GameLayer2DData;

void WfDeSerializeRockEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData);

void WfSerializeRockEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData);


#endif