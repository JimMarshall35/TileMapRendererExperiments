#include "StringKeyHashMap.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "AssertLib.h"

#define STARDEW_HASHMAP_DEFAULT_LOAD_FACTOR 0.75f

struct KVP
{
	struct KVP* pNext;
	struct KVP* pPrev;
	char* pKey;
	unsigned int keyHash;
};

unsigned int djb2(char* str)
{
	unsigned int hash = 5381;
	int c;

	while (c = *((unsigned char*)str)++)
	{
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}



static void HashmapInitBase(struct HashMap* pMap, int capacity, int valSize)
{
	memset(pMap, 0, sizeof(struct HashMap));
	pMap->capacity = capacity;
	pMap->size = 0;
	pMap->valueSize = valSize;
	pMap->pData = malloc(capacity * (sizeof(struct KVP) + valSize));
	memset(pMap->pData, 0, capacity * (sizeof(struct KVP) + valSize));
	
}

void HashmapInit(struct HashMap* pMap, int capacity, int valSize)
{
	HashmapInitBase(pMap, capacity, valSize);
	pMap->fLoadFactor = STARDEW_HASHMAP_DEFAULT_LOAD_FACTOR;
}

void HashmapInitWithLoadFactor(struct HashMap* pMap, int capacity, int valSize, float loadFactor)
{
	HashmapInitBase(pMap, capacity, valSize);
	pMap->fLoadFactor = loadFactor;
}

static void* BucketAtIndex(struct HashMap* pMap, int i)
{
	return (char*)pMap->pData + i * (sizeof(struct KVP) + pMap->valueSize);
}

static int NextIndex(struct HashMap* pMap, int i)
{
	if (++i >= pMap->capacity)
	{
		return 0;
	}
	return i;
}

void* HashmapSearch(struct HashMap* pMap, char* key)
{
	unsigned int hash = djb2(key);
	int index = hash % pMap->capacity;
	/* linear probe */
	void* pAt = BucketAtIndex(pMap, index);
	struct KVP* pKVP = (struct KVP*)pAt;

	while (true)
	{
		if (pKVP->pKey && pKVP->keyHash == hash)
		{
			return pKVP + 1;
		}
		else if (pKVP->pKey) // pKey set == "there's something in this bucket"
		{
			index = NextIndex(pMap, index);
			pKVP = BucketAtIndex(pMap, index);
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}

/// <summary>
/// insert from the old allocation to the new
/// </summary>
/// <param name="pMap">
/// </param>
/// <param name="pKVPsrc">
/// kvp from old allocation
/// </param>
/// <param name="pNewAlloc">
/// New allocation
/// </param>
/// <returns>
/// The KVP inserted into pNewAlloc
/// </returns>
struct KVP* InsertKVPIntoNewAlloc(struct HashMap* pMap, struct KVP* pKVPsrc, char* pNewAlloc)
{
	unsigned int hash = djb2(pKVPsrc->pKey);
	int index = hash % pMap->capacity;
	void* pAt = pNewAlloc + index * (sizeof(struct KVP) + pMap->valueSize);
	struct KVP* pKVP = (struct KVP*)pAt;
	while (true)
	{
		if (pKVP->pKey && pKVP->keyHash != hash)
		{
			// occupied bucket
			index = NextIndex(pMap, index);
			pKVP = BucketAtIndex(pMap, index);
		}
		else
		{
			if (pKVP->pKey)
			{
				free(pKVP->pKey);
			}
			memset(pKVP, 0, sizeof(struct KVP));
			pKVP->keyHash = hash;
			pKVP->pKey = malloc(strlen(pKVPsrc->pKey) + 1);
			strcpy(pKVP->pKey, pKVPsrc->pKey);
			memcpy(pKVP + 1, pKVPsrc + 1, pMap->valueSize);
			return pKVP;
		}
	}
	return pKVP;
}

void HashmapResize(struct HashMap* pMap)
{
	char* pNewAlloc = malloc(pMap->capacity * 2);
	pMap->capacity *= 2;
	struct KVP* pKVP = pMap->pHead;
	struct KVP* pNewHead = NULL;
	struct KVP* pNewTail = NULL;
	while (pKVP)
	{
		struct KVP* pKVPDst = InsertKVPIntoNewAlloc(pMap, pKVP, pNewAlloc);
		if (pNewHead == NULL)
		{
			pNewHead = pKVPDst;
			pNewTail = pNewHead;
		}
		else
		{
			pNewTail->pNext = pKVPDst;
			pKVPDst->pPrev = pNewTail;
			pNewTail = pKVPDst;
		}
		EASSERT(pKVP->pKey);
		free(pKVP->pKey);
	}
	if (pNewHead && !pNewTail)
	{
		pNewTail = pNewHead;
	}
	pMap->pHead = pNewHead;
	pMap->pEnd = pNewTail;
	free(pMap->pData);
	pMap->pData = pNewAlloc;
}

void HashmapInsert(struct HashMap* pMap, char* key, void* pVal)
{
	float newLoad = (float)(pMap->size + 1) / (float)pMap->capacity;
	if(newLoad > pMap->fLoadFactor)
	{
		HashmapResize(pMap);
	}
	// todo: refactor InsertKVPIntoNewAlloc and from here downwards to use a common helper
	unsigned int hash = djb2(key);
	int index = hash % pMap->capacity;
	void* pAt = (char*)pMap->pData + index * (sizeof(struct KVP) + pMap->valueSize);
	struct KVP* pKVP = (struct KVP*)pAt;
	struct KVP* pNewKVP = NULL;
	while (true)
	{
		if (pKVP->pKey && pKVP->keyHash != hash)
		{
			// occupied bucket
			index = NextIndex(pMap, index);
			pKVP = BucketAtIndex(pMap, index);
		}
		else
		{
			if (pKVP->pKey)
			{
				free(pKVP->pKey);
			}
			memset(pKVP, 0, sizeof(struct KVP));
			pKVP->keyHash = hash;
			pKVP->pKey = malloc(strlen(key) + 1);
			strcpy(pKVP->pKey, key);
			memcpy(pKVP + 1, pVal, pMap->valueSize);
			pNewKVP = pKVP;
			break;
		}
	}
	EASSERT(pNewKVP);
	if (!pMap->pHead)
	{
		pMap->pHead = pNewKVP;
		pMap->pEnd = pMap->pHead;
	}
	else
	{
		pMap->pEnd->pNext = pNewKVP;
		pNewKVP->pPrev = pMap->pEnd;
		pMap->pEnd = pNewKVP;
	}
	pMap->size++;
}

void HashmapDeleteItem(struct HashMap* pMap, char* key)
{
	struct KVP* pFound = HashmapSearch(pMap, key);
	if (pFound)
	{
		free(pFound->pKey);
		pFound->pNext->pPrev = pFound->pPrev;
		pFound->pPrev->pNext = pFound->pNext;
	}
	pMap->size--;
}

struct KeyIterator
{
	struct HashMap* pHashMap;
	struct KVP* pOnKVP;
};

struct KeyIterator GetKeyIterator(struct HashMap* pHashMap)
{
	struct KeyIterator itr = {
		pHashMap,
		pHashMap->pHead
	};
	return itr;
}

char* NextHashmapKey(struct KeyIterator* itr)
{
	EASSERT(itr->pHashMap);
	if (!itr->pOnKVP)
	{
		return NULL;
	}
	char* r = itr->pOnKVP->pKey;
	itr->pOnKVP = itr->pOnKVP->pNext;
	return r;
}

void HashMapDeInit(struct HashMap* pMap)
{
	struct KVP* pKVP = pMap->pHead;
	while (pKVP)
	{
		EASSERT(pKVP->pKey);
		free(pKVP->pKey);
		pKVP = pKVP->pNext;
	}
	free(pMap->pData);
}
