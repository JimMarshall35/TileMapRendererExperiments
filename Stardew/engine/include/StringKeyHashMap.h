#ifndef STRING_KEY_HASH_MAP_H
#define STRING_KEY_HASH_MAP_H

#ifdef __cplusplus
extern "C" {
#endif 

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
void HashmapInsert(struct HashMap* pMap, char* key, void* pVal);
void HashmapDeleteItem(struct HashMap* pMap, char* key);
void HashMapDeInit(struct HashMap* pMap);

struct KeyIterator;

struct KeyIterator GetKeyIterator(struct HashMap* pHashMap);
char* NextHashmapKey(struct KeyIterator*);

#ifdef __cplusplus
}
#endif

#endif