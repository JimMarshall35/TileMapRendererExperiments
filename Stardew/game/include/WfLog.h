#ifndef WFLOG_H
#define WFLOG_H

struct BinarySerializer;
struct Entity2D;
struct GameLayer2DData;

void WfDeSerializeLogEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData);
void WfSerializeLogEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData);

#endif
