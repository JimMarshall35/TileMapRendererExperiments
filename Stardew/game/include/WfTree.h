#ifndef WFTREE_H
#define WFTREE_H

#include "HandleDefs.h"
#include "WfEnums.h"
struct BinarySerializer;
struct Entity2D;
struct GameLayer2DData;
struct Entity2DCollection;

struct WfTreeSprites
{
    hSprite coniferousTop1;
    hSprite coniferousTop2;
    hSprite deciduousTop1;
    hSprite deciduousTop2;
    hSprite trunk1;
    hSprite trunk2;
};
enum WfTreeType
{
    Coniferous,
    Deciduous
};

struct WfTreeDef
{
    enum WfSeason season;
    enum WfTreeType type;
    int subtype;
};

void WfGetTreeSprites(struct WfTreeSprites* spritesPerSeason, hAtlas atlas);

void WfDeSerializeTreeEntity(struct BinarySerializer* bs, struct Entity2D* pOutEnt, struct GameLayer2DData* pData);

void WfSerializeTreeEntity(struct BinarySerializer* bs, struct Entity2D* pInEnt, struct GameLayer2DData* pData);

void WfAddTreeBasedAt(float x, float y, struct WfTreeDef* def, struct WfTreeSprites* spritesPerSeason, struct Entity2DCollection* pEntityCollection);

void WfTreeInit();

#endif