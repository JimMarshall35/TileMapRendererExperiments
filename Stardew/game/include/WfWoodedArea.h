#ifndef WFWOODEDAREA_H
#define WFWOODEDAREA_H

struct BinarySerializer;
struct Entity2D;
struct GameLayer2DData;

void WfDeSerializeWoodedAreaEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData);

void WfSerializeWoodedAreaEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData);

void WfWoodedAreaInit();

#endif