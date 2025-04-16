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

typedef struct 
{
	AtlasSprite sprite;
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph

}AtlasFontCharacterSprite;

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
HFont At_FindFont(hAtlas hAtlas, const char* fontName);
float At_StringLength(hAtlas hAtlas, HFont hFont, const char* stringVal);
float At_StringHeight(hAtlas hAtlas, HFont hFont, const char* stringVal);

#endif