#ifndef STRING_KEY_HASH_MAP_H
#define STRING_KEY_HASH_MAP_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <stdbool.h>

struct HashMap
{
	int capacity;
	int size;
	int valueSize;
	void* pData;
	struct KVP* pHead;
	struct KVP* pEnd;
	float fLoadFactor;
};

void HashmapInit(struct HashMap* pMap, int capacity, int valSize);
void HashmapInitWithLoadFactor(struct HashMap* pMap, int capacity, int valSize, float loadFactor);
void* HashmapSearch(struct HashMap* pMap, char* key);

/// <summary>
/// 
/// </summary>
/// <param name="pMap"></param>
/// <param name="key"></param>
/// <param name="pVal"></param>
/// <returns> true if a new key and inserted, false if an existing key and value overwritten </returns>
void* HashmapInsert(struct HashMap* pMap, char* key, void* pVal);
bool HashmapDeleteItem(struct HashMap* pMap, char* key);
void HashmapDeInit(struct HashMap* pMap);
void HashmapPrintEntries(struct HashMap* pMap, const char* hashMapName);

struct HashmapKeyIterator
{
	struct HashMap* pHashMap;
	struct KVP* pOnKVP;
};

struct HashmapKeyIterator GetKeyIterator(struct HashMap* pHashMap);
char* NextHashmapKey(struct HashmapKeyIterator*);

#ifdef __cplusplus
}
#endif

#endif