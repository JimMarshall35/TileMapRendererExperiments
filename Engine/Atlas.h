#ifndef ATLAS_H
#define ATLAS_H

#include "IntTypes.h"
#include "HandleDefs.h"

void At_BeginAtlas();
hSprite At_AddSprite(const char* imgPath, int topLeftXPx, int topRightYPx, int widthPx, int heightPx, const char* name);
hAtlas At_EndAtlas();
void At_DestroyAtlas(hAtlas atlas);
hSprite At_FindSprite(const char* name, hAtlas atlas);

#endif