#ifndef WFTREE_H
#define WFTREE_H

struct BinarySerializer;
struct Entity2D;
struct GameLayer2DData;

void WfDeSerializeTreeEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData);

void WfSerializeTreeEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData);

#endif