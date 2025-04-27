#include "Atlas.h"
#include "DynArray.h"
#include "ImageFileRegstry.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "DrawContext.h"
#include "SharedPtr.h"
#include "FloatingPointLib.h"

FT_Library  gFTLib;
static int gSpriteId = 1;

struct AtlasSpriteData
{
	vec2 bearing;
	vec2 advance;
};

struct AtlasFont
{
	struct AtlasSpriteData spriteData[256];
	AtlasSprite sprites[256];
	char name[MAX_FONT_NAME_SIZE];
	SHARED_PTR(FT_Face) pFTFace;
	float fSizePts;
};

typedef struct
{
	bool bActive;
	u8* atlasBytes;
	int atlasWidth;
	int atlasHeight;
	hTexture texture;
	VECTOR(AtlasSprite) sprites;
	VECTOR(HImage) images;
	VECTOR(struct AtlasFont) fonts;
}Atlas;

static VECTOR(Atlas) gAtlases = NULL;
static int gCurrentAtlasIndex = -1;

Atlas* GetCurrentAtlas()
{
	return gCurrentAtlasIndex >= 0 && gCurrentAtlasIndex < VectorSize(gAtlases) ? &gAtlases[gCurrentAtlasIndex] : NULL;
}

Atlas* AqcuireAtlas()
{
	for (int i = 0; i < VectorSize(gAtlases); i++)
	{
		if (!gAtlases[i].bActive)
		{
			return &gAtlases[i];
		}
	}
	Atlas atlas;
	gAtlases = VectorPush(gAtlases, &atlas);
	return VectorTop(gAtlases);
}

void At_Init()
{
	FT_Error error = FT_Init_FreeType(&gFTLib);
	if (error)
	{
		//... an error occurred during library initialization ...
		printf("Error initialising freetype!!!\n");
		return;
	}

}

void At_BeginAtlas()
{
	if (gAtlases == NULL)
	{
		gAtlases = NEW_VECTOR(Atlas);
	}
	VectorData* pData = VectorData_DEBUG(gAtlases);
	Atlas* atlas = AqcuireAtlas();
	atlas->atlasBytes = NULL;
	atlas->atlasHeight = 0;
	atlas->atlasWidth = 0;
	atlas->sprites = NEW_VECTOR(AtlasSprite);
	atlas->images = NEW_VECTOR(HImage);
	atlas->fonts = NEW_VECTOR(struct AtlasFont);
	atlas->texture = NULL_HANDLE;
	//gAtlases = VectorPush(gAtlases, &atlas);
	gCurrentAtlasIndex = VectorSize(gAtlases) - 1;
}

hSprite At_AddSprite(const char* imgPath, int topLeftXPx, int topLeftYPx, int widthPx, int heightPx, const char* name)
{

	HImage img = IR_LookupHandleByPath(imgPath);
	Atlas* pAtlas = GetCurrentAtlas();
	if (!pAtlas || img == NULL_HIMAGE)
	{
		return NULL_HSPRITE;
	}
	AtlasSprite sprite;
	memset(&sprite, 0, sizeof(AtlasSprite));
	sprite.atlasTopLeftXPx = topLeftXPx;
	sprite.atlasTopLeftYPx = topLeftYPx;
	sprite.widthPx = widthPx;
	sprite.heightPx = heightPx;
	sprite.atlas = gCurrentAtlasIndex;
	sprite.srcImage = img;
	size_t sizeOfSpriteData = widthPx * heightPx * CHANNELS_PER_PIXEL;
	//sprite.individualTileBytes = malloc(sizeOfSpriteData);
	sprite.name = malloc(strlen(name) + 1);
	sprite.id = gSpriteId++;

	strcpy(sprite.name, name);
	
	u8* pData = IR_GetImageData(img);
	const struct ImageFile* pImg = IR_GetImageFile(img);
	sprite.individualTileBytes = pData;

	size_t rowWidth = pImg->width * CHANNELS_PER_PIXEL;
	size_t startIndex = (topLeftYPx * rowWidth) + (topLeftXPx * CHANNELS_PER_PIXEL);
	u8* writeStart = pData;
	for (int i = 0; i < heightPx; i++)
	{
		size_t startRow = startIndex + i * rowWidth;
		memcpy(writeStart, &pData[startRow], widthPx * CHANNELS_PER_PIXEL);
		writeStart += widthPx * CHANNELS_PER_PIXEL;
	}

	pAtlas->sprites = VectorPush(pAtlas->sprites, &sprite);
	return VectorSize(pAtlas->sprites) - 1;
}

static void* FTBitmapToNestableBitmap(FT_Bitmap* pBmp)
{
	int numPxls = pBmp->width* pBmp->rows;
	int allocSize = numPxls * 4;
	u8* pAlloc = malloc(allocSize);
	u8* readPtr = pBmp->buffer;
	for (int i = 0; i < allocSize; i+=4)
	{
		pAlloc[i + 0] = *readPtr;
			
		pAlloc[i + 1] = *readPtr;
			
		pAlloc[i + 2] = *readPtr;

		pAlloc[i + 3] = *readPtr;

		readPtr++;
	}
	return pAlloc;
}

static size_t CountAvailableChars(struct AtlasFont* pFont)
{
	size_t count = 0;
	for (int i = 0; i < 256; i++)
	{
		if (pFont->sprites[i].individualTileBytes)
		{
			++count;
		}
	}
	return count;
}

static void FaceDtor(void* pVal)
{
	FT_Face* pFace = pVal;
	FT_Done_Face(*pFace);
}

float At_PixelsToPts(float val)
{
	const float dpi = 92;
	int inches = val / dpi;
	val = inches * 72.0f;
	return val;
}

HFont At_AddFont(const struct FontAtlasAdditionSpec* pFontSpec)
{
	HFont hFont = NULL_HANDLE;
	const int dpi = 92;
	SHARED_PTR(FT_Face) face = SHARED_PTR_NEW(FT_Face, &FaceDtor);      /* handle to face object */
	Atlas* pAtlas = GetCurrentAtlas();
	FT_Error error =  FT_New_Face(gFTLib, pFontSpec->path, 0, face);
	if (error == FT_Err_Unknown_File_Format)
	{
		/*... the font file could be opened and read, but it appears
			... that its font format is unsupported*/
		printf("the font file '%s' could be opened and read, but it appears... that its font format is unsupported", pFontSpec->path);
	}
	else if (error)
	{
		/*... another error code means that the font file could not
			... be opened or read, or that it is broken...*/
		printf("font file '%s' another error code means that the font file could not... be opened or read, or that it is broken", pFontSpec->path);
	}
	for (int i = 0; i < pFontSpec->numFontSizes; i++)
	{
		struct FontSize fs = pFontSpec->fontSizes[i];
		switch (fs.type)
		{
		case FOS_Pixels:
			{
				fs.val = At_PixelsToPts(fs.val);
				fs.type = FOS_Pts;
				break;
			}
		}
		error = FT_Set_Char_Size(
			*face,    /* handle to face object         */
			fs.val * 64,       /* char_width in 1/64 of points  */
			fs.val * 64,   /* char_height in 1/64 of points */
			dpi,     /* horizontal device resolution  */
			dpi);   /* vertical device resolution    */
		if (error)
		{
			printf("FT_Set_Char_Size error");
			return NULL_HANDLE;
		}
		struct AtlasFont font;
		memset(&font, 0, sizeof(struct AtlasFont));
		font.fSizePts = fs.val;
		font.pFTFace = face;
		Sptr_AddRef(face);

		for (int j = 0; j < 256; j++)
		{
			FT_UInt index = FT_Get_Char_Index(*face, j);
			if (index == 0)
			{
				continue;
			}
			error = FT_Load_Glyph(
				*face,          /* handle to face object */
				index,   /* glyph index           */
				FT_LOAD_DEFAULT);  /* load flags, see below */
			if (error)
			{
				printf("FT_Load_Glyph error\n");
				continue;
			}
			error = FT_Render_Glyph((*face)->glyph,   /* glyph slot  */
				FT_RENDER_MODE_NORMAL); /* render mode */
			if (error)
			{
				printf("FT_Render_Glyph error\n");
				continue;
			}
			if ((*face)->glyph->bitmap.width == 0 || (*face)->glyph->bitmap.rows == 0)
			{
				continue;
			}
			AtlasSprite* pSprite = &font.sprites[j];
			pSprite->widthPx = (*face)->glyph->bitmap.width;
			pSprite->heightPx = (*face)->glyph->bitmap.rows;
			pSprite->individualTileBytes = FTBitmapToNestableBitmap(&(*face)->glyph->bitmap);
			pSprite->id = gSpriteId++;
			
			struct AtlasSpriteData* pSpriteData = &font.spriteData[j];
			pSpriteData->bearing[0] = (*face)->glyph->bitmap_left;
			pSpriteData->bearing[1] = (*face)->glyph->bitmap_top;
			pSpriteData->advance[0] = (float)((*face)->glyph->advance.x >> 6);
			pSpriteData->advance[1] = (float)((*face)->glyph->advance.y >> 6);
		}

		strcpy(font.name, pFontSpec->name);
		pAtlas->fonts = VectorPush(pAtlas->fonts, &font);
		hFont = VectorSize(pAtlas->fonts) - 1;
	}

	return hFont;
}

int SortFunc(const void* a, const void* b) 
{
	AtlasSprite* pSprite1 = (AtlasSprite*)a;
	AtlasSprite* pSprite2 = (AtlasSprite*)b;
	/*return
		pSprite1->heightPx > pSprite2->heightPx ? -1 : 0;*/
	int area1 = pSprite1->heightPx * pSprite1->widthPx;
	int area2 = pSprite2->heightPx * pSprite2->widthPx;
	return
		area1 < area2;
}

struct AtlasRect
{
	int w, h;
	int x, y;
	bool bTaken;
};

VECTOR(struct AtlasRect) AddNewFreeSpace(VECTOR(struct AtlasRect) outFreeSpace, int* currentWidth, int* currentHeight)
{
	if (*currentWidth > *currentHeight)
	{
		int oldHeight = *currentHeight;
		struct AtlasRect newR;
		newR.x = 0;
		newR.y = oldHeight;
		newR.bTaken = false;
		*currentHeight *= 2;
		newR.w = *currentWidth;
		newR.h = *currentHeight - oldHeight;
		return VectorPush(outFreeSpace, &newR);
	}
	else
	{
		int oldWidth = *currentWidth;
		struct AtlasRect newR;
		newR.bTaken = false;
		newR.y = 0;
		newR.x = oldWidth;
		*currentWidth *= 2;
		newR.w = *currentWidth - oldWidth;
		newR.h = *currentHeight;
		return VectorPush(outFreeSpace, &newR);

	}
}

bool FitsInRect(const AtlasSprite* sprite, const struct AtlasRect* rect)
{
	return sprite->widthPx <= rect->w && sprite->heightPx <= rect->h;
}

int FindFittingFreeSpace(const AtlasSprite* sprite, VECTOR(struct AtlasRect) freeSpace)
{
	for (int j = 0; j < VectorSize(freeSpace); j++)
	{
		struct AtlasRect* rct = &freeSpace[j];
		if (freeSpace[j].bTaken)
		{
			continue;
		}
		if (FitsInRect(sprite, rct))
		{
			return j;
		}
	}
	return -1;
}

static int Top(const struct AtlasRect* pRect)    { return pRect->y; }
static int Bottom(const struct AtlasRect* pRect) { return pRect->y + pRect->h; }
static int Left(const struct AtlasRect* pRect)   { return pRect->x; }
static int Right(const struct AtlasRect* pRect)  { return pRect->x + pRect->w; }


static VECTOR(struct AtlasRect) NestSingleSprite(int* outW, int* outH, AtlasSprite* pSprite, VECTOR(struct AtlasRect) freeSpace)
{
	size_t sizeofFreespace = VectorSize(freeSpace);
	int index = FindFittingFreeSpace(pSprite, freeSpace);
	if (index >= 0)
	{
		struct AtlasRect* rct = &freeSpace[index];
		rct->bTaken = true;

		pSprite->atlasTopLeftXPx = rct->x;
		pSprite->atlasTopLeftYPx = rct->y;
		int newRightX = rct->x + pSprite->widthPx;
		int newRightY = rct->y;
		int newW = rct->w - pSprite->widthPx;
		int newH = rct->h;
		struct AtlasRect newRectTR = { newW, newH, newRightX, newRightY, false };
		newRightX = rct->x;
		newRightY = rct->y + pSprite->heightPx;
		newW = pSprite->widthPx;
		newH = rct->h - pSprite->heightPx;
		struct AtlasRect newRectTR2 = { newW, newH, newRightX, newRightY, false };

		if (newRectTR.w > 0 && newRectTR.h > 0)
		{
			freeSpace = VectorPush(freeSpace, &newRectTR);
		}
		if (newRectTR2.w > 0 && newRectTR2.h > 0)
		{
			freeSpace = VectorPush(freeSpace, &newRectTR2);
		}
		return freeSpace;
	}
	else
	{
		freeSpace = AddNewFreeSpace(freeSpace, outW, outH);
		return NestSingleSprite(outW, outH, pSprite, freeSpace);
	}
}

static void NestSprites(int* outW, int* outH, AtlasSprite* sortedSpritesTallestToShortest, int numSprites)
{
	
	VECTOR(struct AtlasRect) freeSpace = NEW_VECTOR(struct AtlasRect);
	int tallestHeight = sortedSpritesTallestToShortest->heightPx;
	int currentW = sortedSpritesTallestToShortest[0].widthPx;
	int currentH = sortedSpritesTallestToShortest[0].heightPx;
	

	for (int i = 1; i < numSprites; i++)
	{
		bool bFitInEmptySpace = false;
		AtlasSprite* pSprt = &sortedSpritesTallestToShortest[i];
		freeSpace = NestSingleSprite(&currentW, &currentH, pSprt, freeSpace);
	}

	DestoryVector(freeSpace);
	*outW = currentW;
	*outH = currentH;
}

void BlitAtlasSprite(u8* dst, size_t dstWidthPx, AtlasSprite* pSprite)
{
	size_t startPx = pSprite->atlasTopLeftYPx * (dstWidthPx * CHANNELS_PER_PIXEL) + pSprite->atlasTopLeftXPx * CHANNELS_PER_PIXEL;
	const u8* readPtr = pSprite->individualTileBytes;
	u8* writePtr = &dst[startPx];
	size_t lineBytes = pSprite->widthPx * CHANNELS_PER_PIXEL;
	for (int i = 0; i < pSprite->heightPx; i++)
	{
		memcpy(writePtr, readPtr, lineBytes);
		writePtr += dstWidthPx * CHANNELS_PER_PIXEL;
		readPtr += lineBytes;
	}
}

void CopyNestedPositions(Atlas* pAtlasDest, AtlasSprite* spritesCopySrc, int spritesCopySrcSize)
{
	// copy nested positions to actual atlas sprites
	for (int i = 0; i < spritesCopySrcSize; i++)
	{
		int id = -1;
		AtlasSprite* pSp = &spritesCopySrc[i];
		bool bFound = false;
		for (int j = 0; j < VectorSize(pAtlasDest->sprites); j++)
		{
			if (pAtlasDest->sprites[j].id == pSp->id)
			{
				id = j;
				bFound = true;
				pAtlasDest->sprites[id].atlasTopLeftXPx = pSp->atlasTopLeftXPx;
				pAtlasDest->sprites[id].atlasTopLeftYPx = pSp->atlasTopLeftYPx;
				break;
			}
		}
		if (!bFound)
		{
			for (int j = 0; j < VectorSize(pAtlasDest->fonts); j++)
			{
				bool bShouldBreak = false;
				for (int k = 0; k < 256; k++)
				{
					if (pAtlasDest->fonts[j].sprites[k].id == pSp->id)
					{
						pAtlasDest->fonts[j].sprites[k].atlasTopLeftXPx = pSp->atlasTopLeftXPx;
						pAtlasDest->fonts[j].sprites[k].atlasTopLeftYPx = pSp->atlasTopLeftYPx;
						bShouldBreak = true;
						bFound = true;
						break;
					}
				}
				if (bShouldBreak)
				{
					break;
				}
			}
		}
		assert(bFound);
	}
}

static bool IsCharLoaded(struct AtlasFont* pFont, char c)
{
	return pFont->sprites[c].individualTileBytes != NULL;
}


static void CalculateSpriteUVs(AtlasSprite* pSprt, int atlasW, int atlasH)
{
	pSprt->topLeftUV_U = (float)pSprt->atlasTopLeftXPx / (float)atlasW;
	pSprt->topLeftUV_V = (float)pSprt->atlasTopLeftYPx / (float)atlasH;

	pSprt->bottomRightUV_U = (float)(pSprt->atlasTopLeftXPx + pSprt->widthPx) / (float)atlasW;
	pSprt->bottomRightUV_V = (float)(pSprt->atlasTopLeftYPx + pSprt->heightPx) / (float)atlasH;

}

static void CalculateAtlasUVs(Atlas* pAtlas)
{
	int atlasW = pAtlas->atlasWidth;
	int atlasH = pAtlas->atlasHeight;
	for (int i = 0; i < VectorSize(pAtlas->sprites); i++)
	{
		AtlasSprite* pSprt = &pAtlas->sprites[i];
		CalculateSpriteUVs(pSprt, atlasW, atlasH);
	}
	for (int i = 0; i < VectorSize(pAtlas->fonts); i++)
	{
		struct AtlasFont* pFont = &pAtlas->fonts[i];
		for(int j=0;j<256;j++)
		{
			if (IsCharLoaded(pFont, j))
			{
				AtlasSprite* pSprt = &pFont->sprites[j];
				CalculateSpriteUVs(pSprt, atlasW, atlasH);
			}
		}
	}
}

size_t CountTotalSpritesInFonts(Atlas* pAtlas)
{
	size_t total = 0;
	for (int i = 0; i < VectorSize(pAtlas->fonts); i++)
	{
		total += CountAvailableChars(&pAtlas->fonts[i]);
	}
	return total;
}

static void WriteFontSprites(Atlas* pAtlas, AtlasSprite* outFontSprites)
{
	for (int i = 0; i < VectorSize(pAtlas->fonts); i++)
	{
		for (int j = 0; j < 256; j++)
		{
			AtlasSprite* pSrc = &pAtlas->fonts[i].sprites[j];
			if (pSrc->individualTileBytes)
			{
				*outFontSprites = *pSrc;
				++outFontSprites;
			}
		}
	}
}

hAtlas At_EndAtlas(struct DrawContext* pDC)
{
	Atlas* pAtlas = GetCurrentAtlas();
	if (!pAtlas)
	{
		return NULL_ATLAS;
	}
	size_t numSprites = VectorSize(pAtlas->sprites);
	size_t numSpritesFromFonts = CountTotalSpritesInFonts(pAtlas);

	AtlasSprite* spritesCopy = malloc(sizeof(AtlasSprite) * (numSprites + numSpritesFromFonts));
	if (spritesCopy)
	{
		memcpy(spritesCopy, pAtlas->sprites, numSprites * sizeof(AtlasSprite));
		AtlasSprite* pOutFontSprites = spritesCopy + numSprites;
		WriteFontSprites(pAtlas, pOutFontSprites);

		qsort(spritesCopy, numSprites + numSpritesFromFonts, sizeof(AtlasSprite), &SortFunc);
		int w, h;
		NestSprites(&w, &h, spritesCopy, numSprites + numSpritesFromFonts);

		CopyNestedPositions(pAtlas, spritesCopy, numSprites + numSpritesFromFonts);
		free(spritesCopy);

		size_t atlasSizePxls = w * h;
		size_t atlasSizeBytes = atlasSizePxls * CHANNELS_PER_PIXEL;
		u8* pAtlasBytes = malloc(atlasSizeBytes);
		memset(pAtlasBytes, 0, atlasSizeBytes);
		for (int i = 0; i < VectorSize(pAtlas->sprites); i++)
		{
			AtlasSprite* pSprite = &pAtlas->sprites[i];
			BlitAtlasSprite(pAtlasBytes, w, pSprite);
		}

		for (int i = 0; i < VectorSize(pAtlas->fonts); i++)
		{
			for (int j = 0; j < 256; j++)
			{
				AtlasSprite* pSprite = &pAtlas->fonts[i].sprites[j];
				BlitAtlasSprite(pAtlasBytes, w, pSprite);
			}
		}
		pAtlas->atlasBytes = pAtlasBytes;
		pAtlas->atlasWidth = w;
		pAtlas->atlasHeight = h;

		CalculateAtlasUVs(pAtlas);

		stbi_write_bmp("testing123.bmp", w, h, CHANNELS_PER_PIXEL, pAtlasBytes);
		pAtlas->texture = pDC->UploadTexture(pAtlas->atlasBytes, CHANNELS_PER_PIXEL, w, h);
	}
	return gCurrentAtlasIndex;
}



#define ATLAS_HANDLE_BOUNDS_CHECK(atlas)\
{\
bool bAtlasHandleBoundsValid = atlas < VectorSize(gAtlases) && atlas >= 0;\
assert(bAtlasHandleBoundsValid);\
if(!bAtlasHandleBoundsValid){\
	printf("function '%s' invalid bounds handle %i", __FUNCTION__, atlas);\
	return;\
}\
}\

#define ATLAS_HANDLE_BOUNDS_CHECK(atlas, rVal)\
{\
bool bAtlasHandleBoundsValid = atlas < VectorSize(gAtlases) && atlas >= 0;\
assert(bAtlasHandleBoundsValid);\
if(!bAtlasHandleBoundsValid){\
	printf("function '%s' invalid bounds handle %i", __FUNCTION__, atlas);\
	return rVal;\
}\
}

#define FONT_HANDLE_BOUNDS_CHECK(hAtlas, hFont, rVal)\
{\
Atlas* pAtlas = &gAtlases[hAtlas];\
bool bAtlasHandleBoundsValid = hFont < VectorSize(pAtlas->fonts) && hFont >= 0;\
assert(bAtlasHandleBoundsValid);\
if(!bAtlasHandleBoundsValid){\
	printf("function '%s' invalid bounds font handle %i", __FUNCTION__, hFont);\
	return rVal;\
}\
}


void At_DestroyAtlas(hAtlas atlas, struct DrawContext* pDC)
{
	ATLAS_HANDLE_BOUNDS_CHECK(atlas)
	pDC->DestroyTexture(atlas);
	gAtlases[atlas].bActive = false;
	for (int i = 0; i < VectorSize(gAtlases[atlas].sprites); i++)
	{
		AtlasSprite* pSprite = &gAtlases[atlas].sprites[i];
		
		if (pSprite->name)
		{
			free(pSprite->name);
		}
		if (pSprite->individualTileBytes)
		{
			free(pSprite->individualTileBytes);
		}
	}

	for (int i = 0; i < VectorSize(gAtlases[atlas].fonts); i++)
	{
		struct AtlasFont* pFont = &gAtlases[atlas].fonts[i];
		Sptr_RemoveRef(pFont->pFTFace);
		for (int j = 0; j < 256; j++)
		{
			AtlasSprite* pSprite = &gAtlases[atlas].fonts[i].sprites[j];

			if (pSprite->name)
			{
				free(pSprite->name);
			}
			if (pSprite->individualTileBytes)
			{
				free(pSprite->individualTileBytes);
			}
		}
	}


	DestoryVector(gAtlases[atlas].sprites);
	DestoryVector(gAtlases[atlas].images);
	DestoryVector(gAtlases[atlas].fonts);
	free(gAtlases[atlas].atlasBytes);
}

hSprite At_FindSprite(const char* name, hAtlas atlas)
{
	ATLAS_HANDLE_BOUNDS_CHECK(atlas)
	Atlas* pAtlas = &gAtlases[atlas];
	for (int i = 0; i < VectorSize(pAtlas->sprites); i++)
	{
		if (strcmp(pAtlas->sprites[i].name, name) == 0)
		{
			return i;
		}
	}
	return NULL_HSPRITE;
}

AtlasSprite* At_GetSprite(hSprite sprite, hAtlas atlas)
{
	ATLAS_HANDLE_BOUNDS_CHECK(atlas, NULL);
	Atlas* pAtlas = &gAtlases[atlas];
	if (sprite < 0 || sprite >= VectorSize(pAtlas->sprites))
	{
		printf("function '%s' invalid sprite bounds handle %i", __FUNCTION__, sprite);
		return NULL;
	}
	return &pAtlas->sprites[sprite];
}

hTexture At_GetAtlasTexture(hAtlas atlas)
{
	ATLAS_HANDLE_BOUNDS_CHECK(atlas, NULL_HANDLE);
	Atlas* pAtlas = &gAtlases[atlas];
	return pAtlas->texture;
}

HFont Fo_FindFont(hAtlas hAtlas, const char* fontName, float sizePts)
{
	ATLAS_HANDLE_BOUNDS_CHECK(hAtlas, NULL);
	Atlas* pAtlas = &gAtlases[hAtlas];
	HFont matchingName = NULL_HANDLE; // if one with a matching name but not matching size is found then return that
	
	for (int i = 0; i < VectorSize(pAtlas->fonts); i++)
	{
		struct AtlasFont* pFont = &pAtlas->fonts[i];
		if (strcmp(pFont->name, fontName) == 0)
		{
			matchingName = i;
			if (CompareFloat(pFont->fSizePts, sizePts))
			{
				return i;
			}
		}
	}

	return matchingName;
}

float Fo_CharWidth(hAtlas hAtlas, HFont hFont, char c)
{
	return gAtlases[hAtlas].fonts[hFont].sprites[(u8)c].widthPx;
}

float Fo_CharHeight(hAtlas hAtlas, HFont hFont, char c)
{
	return gAtlases[hAtlas].fonts[hFont].sprites[(u8)c].heightPx;
}


float Fo_StringWidth(hAtlas hAtlas, HFont hFont, const char* stringVal)
{
	ATLAS_HANDLE_BOUNDS_CHECK(hAtlas, 0.0f);
	FONT_HANDLE_BOUNDS_CHECK(hAtlas, hFont, false);

	float width = 0.0f;
	size_t stringLen = strlen(stringVal);
	struct AtlasFont* pFont = &gAtlases[hAtlas].fonts[hFont];
	for (int i = 0; i < stringLen; i++)
	{
		u8 c = (u8)stringVal[i];
		if (!IsCharLoaded(pFont, c))
		{
			continue;
		}
		width += pFont->spriteData[c].advance[0]; // x advance
	}
	return width;
}

float Fo_StringHeight(hAtlas hAtlas, HFont hFont, const char* stringVal)
{
	ATLAS_HANDLE_BOUNDS_CHECK(hAtlas, 0.0f);
	FONT_HANDLE_BOUNDS_CHECK(hAtlas, hFont, false);

	float minBelowBaseline = 0.0f;
	float maxAboveBaseline = 0.0f;
	size_t stringLen = strlen(stringVal);
	struct AtlasFont* pFont = &gAtlases[hAtlas].fonts[hFont];
	for (int i = 0; i < stringLen; i++)
	{
		u8 c = (u8)stringVal[i];
		if (!IsCharLoaded(pFont, c))
		{
			continue;
		}
		struct AtlasSpriteData* pData = &pFont->spriteData[c];
		AtlasSprite* pSprite = &pFont->sprites[c];

		if (pData->bearing[1] > maxAboveBaseline)
		{
			maxAboveBaseline = pData->bearing[1];
		}

		
		float height = (float)pSprite->heightPx;
		float belowBaseline = pData->bearing[1] - height;
		if (belowBaseline < minBelowBaseline)
		{
			minBelowBaseline = belowBaseline;
		}
	}

	float r = maxAboveBaseline - minBelowBaseline;
	return r;
}

AtlasSprite* Fo_GetCharSprite(hAtlas hAtlas, HFont hFont, char c)
{
	ATLAS_HANDLE_BOUNDS_CHECK(hAtlas, NULL);
	FONT_HANDLE_BOUNDS_CHECK(hAtlas, hFont, false);

	return &gAtlases[hAtlas].fonts[hFont].sprites[(u8)c];
}

float Fo_GetMaxYBearing(hAtlas hAtlas, HFont hFont, const char* str)
{
	ATLAS_HANDLE_BOUNDS_CHECK(hAtlas, false);
	FONT_HANDLE_BOUNDS_CHECK(hAtlas, hFont, false);

	float maxBearing = 0.0f;
	struct AtlasFont* pFont = &gAtlases[hAtlas].fonts[hFont];
	
	size_t len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		char c = str[i];
		if (IsCharLoaded(pFont, c))
		{
			if (pFont->spriteData[c].bearing[1] > maxBearing)
			{
				maxBearing = pFont->spriteData[c].bearing[1];
			}
		}
	}
	return maxBearing;
}

bool Fo_TryGetCharBearing(hAtlas hAtlas, HFont hFont, char c, vec2 outBearing)
{
	ATLAS_HANDLE_BOUNDS_CHECK(hAtlas, false);
	FONT_HANDLE_BOUNDS_CHECK(hAtlas, hFont, false);

	if (!gAtlases[hAtlas].fonts[hFont].sprites[(u8)c].individualTileBytes)
	{
		return false;
	}
	outBearing[0] = gAtlases[hAtlas].fonts[hFont].spriteData[(u8)c].bearing[0];
	outBearing[1] = gAtlases[hAtlas].fonts[hFont].spriteData[(u8)c].bearing[1]; 
	return true;
}

bool Fo_TryGetCharAdvance(hAtlas hAtlas, HFont hFont, char c, float* outAdvance)
{
	ATLAS_HANDLE_BOUNDS_CHECK(hAtlas, false);
	FONT_HANDLE_BOUNDS_CHECK(hAtlas, hFont, false);

	if (!gAtlases[hAtlas].fonts[hFont].sprites[(u8)c].individualTileBytes)
	{
		return false;
	}
	*outAdvance = gAtlases[hAtlas].fonts[hFont].spriteData[(u8)c].advance[0];
	return true;
}
