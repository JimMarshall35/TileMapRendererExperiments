#ifndef ATLAS_H
#define ATLAS_H

#include "IntTypes.h"
#include "HandleDefs.h"
#include <cglm/cglm.h>
struct DrawContext;

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


#define MAX_FONT_NAME_SIZE 128
#define MAX_FONT_PATH_SIZE 256

#define MAX_NUM_FONT_SIZES 32

struct FontSize
{
	enum
	{
		FOS_Pixels,
		FOS_Pts
	}type;
	float val;
};

struct FontAtlasAdditionSpec
{
	enum
	{
		FS_Normal = 1,
		FS_Italic = 2,
		FS_Bold = 4,
		FS_Underline = 8
	} fontOptions;
	char name[MAX_FONT_NAME_SIZE];
	char path[MAX_FONT_PATH_SIZE];
	struct FontSize fontSizes[MAX_NUM_FONT_SIZES];
	size_t numFontSizes;
};

void At_Init();
void At_BeginAtlas();
hSprite At_AddSprite(const char* imgPath, int topLeftXPx, int topRightYPx, int widthPx, int heightPx, const char* name);
HFont At_AddFont(const struct FontAtlasAdditionSpec* pFontSpec);
hAtlas At_EndAtlas(struct DrawContext* pDC);
void At_DestroyAtlas(hAtlas atlas, struct DrawContext* pDC);
hSprite At_FindSprite(const char* name, hAtlas atlas);
AtlasSprite* At_GetSprite(hSprite sprite, hAtlas atlas);
hTexture At_GetAtlasTexture(hAtlas atlas);
float At_PixelsToPts(float val);

HFont Fo_FindFont(hAtlas hAtlas, const char* fontName, float sizePts);
float Fo_CharWidth(hAtlas hAtlas, HFont hFont, char c);
float Fo_CharHeight(hAtlas hAtlas, HFont hFont, char c);
float Fo_StringWidth(hAtlas hAtlas, HFont hFont, const char* stringVal);
float Fo_StringHeight(hAtlas hAtlas, HFont hFont, const char* stringVal);
AtlasSprite* Fo_GetCharSprite(hAtlas hAtlas, HFont hFont, char c);
float Fo_GetMaxYBearing(hAtlas hAtlas, HFont hFont, const char* str);

bool Fo_TryGetCharBearing(hAtlas hAtlas, HFont hFont, char c, vec2 outBearing);
bool Fo_TryGetCharAdvance(hAtlas hAtlas, HFont hFont, char c, float* outAdvance);

#endif