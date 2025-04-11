#include "Atlas.h"
#include "DynArray.h"
#include "ImageFileRegstry.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

typedef struct
{
	bool bActive;
	u8* atlasBytes;
	int atlasWidth;
	int atlasHeight;
	VECTOR(AtlasSprite) sprites;
	VECTOR(HImage) images;
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
	//gAtlases = VectorPush(gAtlases, &atlas);
	gCurrentAtlasIndex = VectorSize(gAtlases) - 1;
}

hSprite At_AddSprite(const char* imgPath, int topLeftXPx, int topLeftYPx, int widthPx, int heightPx, const char* name)
{
	static int id = 1;

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
	sprite.id = id++;

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
		for (int j = 0; j < spritesCopySrcSize; j++)
		{
			if (pAtlasDest->sprites[j].id == pSp->id)
			{
				id = j;
			}
		}
		pAtlasDest->sprites[id].atlasTopLeftXPx = pSp->atlasTopLeftXPx;
		pAtlasDest->sprites[id].atlasTopLeftYPx = pSp->atlasTopLeftYPx;
	}
}

static void CalculateAtlasUVs(Atlas* pAtlas)
{
	int atlasW = pAtlas->atlasWidth;
	int atlasH = pAtlas->atlasHeight;
	for (int i = 0; i < VectorSize(pAtlas->sprites); i++)
	{
		AtlasSprite* pSprt = &pAtlas->sprites[i];
		pSprt->topLeftUV_U = (float)atlasW / (float)pSprt->atlasTopLeftXPx;
		pSprt->topLeftUV_V = (float)atlasH / (float)pSprt->atlasTopLeftYPx;

		pSprt->bottomRightUV_U = (float)atlasW / (float)(pSprt->atlasTopLeftXPx + pSprt->widthPx);
		pSprt->bottomRightUV_V = (float)atlasH / (float)(pSprt->atlasTopLeftYPx + pSprt->heightPx);
	}
}

hAtlas At_EndAtlas()
{
	Atlas* pAtlas = GetCurrentAtlas();
	if (!pAtlas)
	{
		return NULL_ATLAS;
	}
	size_t numSprites = VectorSize(pAtlas->sprites);
	AtlasSprite* spritesCopy = malloc(sizeof(AtlasSprite) * numSprites);
	if (spritesCopy)
	{
		memcpy(spritesCopy, pAtlas->sprites, numSprites * sizeof(AtlasSprite));
		qsort(spritesCopy, numSprites, sizeof(AtlasSprite), &SortFunc);
		int w, h;
		NestSprites(&w, &h, spritesCopy, numSprites);

		CopyNestedPositions(pAtlas, spritesCopy, numSprites);
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
		pAtlas->atlasBytes = pAtlasBytes;
		pAtlas->atlasWidth = w;
		pAtlas->atlasHeight = h;

		CalculateAtlasUVs(pAtlas);

		stbi_write_bmp("testing123.bmp", w, h, CHANNELS_PER_PIXEL, pAtlasBytes);

	}
	return gCurrentAtlasIndex;
}



#define ATLAS_HANDLE_BOUNDS_CHECK(atlas)\
bool bAtlasHandleBoundsValid = atlas < VectorSize(gAtlases) && atlas >= 0;\
assert(bAtlasHandleBoundsValid);\
if(!bAtlasHandleBoundsValid){\
	printf("function '%s' invalid bounds handle %i", __FUNCTION__, atlas);\
	return;\
}

#define ATLAS_HANDLE_BOUNDS_CHECK(atlas, rVal)\
bool bAtlasHandleBoundsValid = atlas < VectorSize(gAtlases) && atlas >= 0;\
assert(bAtlasHandleBoundsValid);\
if(!bAtlasHandleBoundsValid){\
	printf("function '%s' invalid bounds handle %i", __FUNCTION__, atlas);\
	return rVal;\
}

void At_DestroyAtlas(hAtlas atlas)
{
	ATLAS_HANDLE_BOUNDS_CHECK(atlas)

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
	DestoryVector(gAtlases[atlas].sprites);
	DestoryVector(gAtlases[atlas].images);
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
