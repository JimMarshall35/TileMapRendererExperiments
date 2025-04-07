#include "Atlas.h"
#include "DynArray.h"
#include "ImageFileRegstry.h"
#include <string.h>
#include <stdlib.h>

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
	u8* atlasBytes;
	size_t atlasBytesSize;
	VECTOR(AtlasSprite) sprites;
	VECTOR(HImage) images;
}Atlas;

static VECTOR(Atlas) gAtlases = NULL;
static int gCurrentAtlasIndex = -1;

Atlas* GetCurrentAtlas()
{
	return gCurrentAtlasIndex >= 0 && gCurrentAtlasIndex < VectorSize(gAtlases) ? &gAtlases[gCurrentAtlasIndex] : NULL;
}

void At_BeginAtlas()
{
	if (gAtlases == NULL)
	{
		gAtlases = NEW_VECTOR(Atlas);
	}
	VectorData* pData = VectorData_DEBUG(gAtlases);
	Atlas atlas;
	atlas.atlasBytes = NULL;
	atlas.atlasBytesSize = 0;
	atlas.sprites = NEW_VECTOR(AtlasSprite);
	atlas.images = NEW_VECTOR(HImage);
	gAtlases = VectorPush(gAtlases, &atlas);
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
	sprite.individualTileBytes = malloc(sizeOfSpriteData);
	sprite.name = malloc(strlen(name) + 1);
	sprite.id = id++;

	strcpy(sprite.name, name);
	
	u8* pData = IR_GetImageData(img);
	const struct ImageFile* pImg = IR_GetImageFile(img);

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

int Tallest(const void* a, const void* b) 
{
	AtlasSprite* pSprite1 = (AtlasSprite*)a;
	AtlasSprite* pSprite2 = (AtlasSprite*)b;
	return
		pSprite1->heightPx > pSprite2->heightPx ? -1 : 0;
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

static VECTOR(struct AtlasRect) NestSingleSprite(int* outW, int* outH, AtlasSprite* pSprite, VECTOR(struct AtlasRect) freeSpace)
{
	int index = FindFittingFreeSpace(pSprite, freeSpace);
	if (index >= 0)
	{
		struct AtlasRect* rct = &freeSpace[index];
		pSprite->atlasTopLeftXPx = rct->x;
		pSprite->atlasTopLeftYPx = rct->y;
		int newRightX = rct->x + pSprite->widthPx;
		int newRightY = rct->y;
		int newW = rct->w - pSprite->widthPx;
		int newH = rct->h;
		struct AtlasRect newRectTR = { newW, newH, newRightX, newRightY, false };
		freeSpace = VectorPush(freeSpace, &newRectTR);

		newRightX = rct->x;
		newRightY = rct->y + pSprite->heightPx;
		newW = rct->w - pSprite->widthPx;
		newH = rct->h - pSprite->heightPx;
		struct AtlasRect newRectTR2 = { newW, newH, newRightX, newRightY, false };
		freeSpace = VectorPush(freeSpace, &newRectTR2);

		rct->bTaken = true;
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

static void BuildAtlas(Atlas* outAtlas, AtlasSprite* sortedSpritesTallestToShortest, int numSprites)
{

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
		qsort(spritesCopy, numSprites, sizeof(AtlasSprite), &Tallest);
		int w, h;
		NestSprites(&w, &h, spritesCopy, numSprites);

		// copy nested positions to actual atlas sprites
		for (int i = 0; i < numSprites; i++)
		{
			int id = -1;
			AtlasSprite* pSp = &spritesCopy[i];
			for (int j = 0; j < numSprites; j++)
			{
				if (pAtlas->sprites[j].id == pSp->id)
				{
					id = j;
				}
			}
			pAtlas->sprites[id].atlasTopLeftXPx = pSp->atlasTopLeftXPx;
			pAtlas->sprites[id].atlasTopLeftYPx = pSp->atlasTopLeftYPx;
		}


		free(spritesCopy);
	}
	return gCurrentAtlasIndex;
}
