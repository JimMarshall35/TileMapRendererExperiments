#ifndef ATLAS_H
#define ATLAS_H

#include "IntTypes.h"
typedef i32 hAtlas;
typedef i32 hSprite;
#define NULL_ATLAS -1
#define NULL_HSPRITE -1

void At_BeginAtlas();
hSprite At_AddSprite(const char* imgPath, int topLeftXPx, int topRightYPx, int widthPx, int heightPx, const char* name);
hAtlas At_EndAtlas();
void At_DestroyAtlas(hAtlas atlas);

#endif