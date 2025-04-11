#ifndef ATLAS_H
#define ATLAS_H

#include "IntTypes.h"
#include "HandleDefs.h"

typedef struct
{
	hAtlas atlas;
	HImage srcImage;
	const char* name;
	int srcImageTopLeftXPx;
	int srcImageTopLeftYPx;
	int widthPx;
	int heightPx;

	int atlasTopLeftXPx;
	int atlasTopLeftYPx;

	float topLeftUV_U;
	float topLeftUV_V;
	float bottomRightUV_U;
	float bottomRightUV_V;

	u8* individualTileBytes;

	int id;
}AtlasSprite;

void At_BeginAtlas();
hSprite At_AddSprite(const char* imgPath, int topLeftXPx, int topRightYPx, int widthPx, int heightPx, const char* name);
hAtlas At_EndAtlas();
void At_DestroyAtlas(hAtlas atlas);
hSprite At_FindSprite(const char* name, hAtlas atlas);

AtlasSprite* At_GetSprite(hSprite sprite, hAtlas atlas);

#endif