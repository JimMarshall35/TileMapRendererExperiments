#include "BinarySerializer.h"
#include "FileHelpers.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "DynArray.h"
#include "AssertLib.h"

void BS_CreateForLoad(const char* path, struct BinarySerializer* pOutSerializer)
{
	memset(pOutSerializer, 0, sizeof(struct BinarySerializer));
	pOutSerializer->bSaving = false;
	pOutSerializer->pData = LoadFile(path, &pOutSerializer->pDataSize);
	pOutSerializer->pReadPtr = pOutSerializer->pData;
	pOutSerializer->pPath = malloc(strlen(path) + 1);
	strcpy(pOutSerializer->pPath, path);
}

void BS_CreateForSave(const char* path, struct BinarySerializer* pOutSerializer)
{
	memset(pOutSerializer, 0, sizeof(struct BinarySerializer));
	pOutSerializer->bSaving = true;
	pOutSerializer->pData = NEW_VECTOR(char);
	pOutSerializer->pPath = malloc(strlen(path) + 1);
	strcpy(pOutSerializer->pPath, path);
}

void BS_Finish(struct BinarySerializer* pOutSerializer)
{
	if (pOutSerializer->bSaving)
	{
		FILE* pFile = fopen(pOutSerializer->pPath, "wb");
		fwrite(pOutSerializer->pData, 1, VectorSize(pOutSerializer->pData), pFile);
		fclose(pFile);
		DestoryVector(pOutSerializer->pData);
	}
	else
	{
		free(pOutSerializer->pData);
	}
	EASSERT(pOutSerializer->pPath);
	free(pOutSerializer->pPath);
}

void BS_SerializeI64(i64 val, struct BinarySerializer* pSerializer)
{
	char* pIn = (char*)&val;
	for (int i = 0; i < sizeof(i64); i++)
	{
		pSerializer->pData = VectorPush(pSerializer->pData, &pIn[i]);
	}
}

void BS_SerializeU64(u64 val, struct BinarySerializer* pSerializer)
{
	char* pIn = (char*)&val;
	for (int i = 0; i < sizeof(u64); i++)
	{
		pSerializer->pData = VectorPush(pSerializer->pData, &pIn[i]);
	}
}

void BS_SerializeI32(i32 val, struct BinarySerializer* pSerializer)
{
	char* pIn = (char*)&val;
	for (int i = 0; i < sizeof(i32); i++)
	{
		pSerializer->pData = VectorPush(pSerializer->pData, &pIn[i]);
	}
}

void BS_SerializeU32(u32 val, struct BinarySerializer* pSerializer)
{
	char* pIn = (char*)&val;
	for (int i = 0; i < sizeof(u32); i++)
	{
		pSerializer->pData = VectorPush(pSerializer->pData, &pIn[i]);
	}
}

void BS_SerializeI16(i16 val, struct BinarySerializer* pSerializer)
{
	char* pIn = (char*)&val;
	for (int i = 0; i < sizeof(i16); i++)
	{
		pSerializer->pData = VectorPush(pSerializer->pData, &pIn[i]);
	}
}

void BS_SerializeU16(u16 val, struct BinarySerializer* pSerializer)
{
	char* pIn = (char*)&val;
	for (int i = 0; i < sizeof(u16); i++)
	{
		pSerializer->pData = VectorPush(pSerializer->pData, &pIn[i]);
	}
}

void BS_SerializeI8(i8 val, struct BinarySerializer* pSerializer)
{
	pSerializer->pData = VectorPush(pSerializer->pData, &val);
}

void BS_SerializeU8(u8 val, struct BinarySerializer* pSerializer)
{
	pSerializer->pData = VectorPush(pSerializer->pData, &val);
}

void BS_SerializeBool(bool val, struct BinarySerializer* pSerializer)
{
	char* pIn = (char*)&val;
	for (int i = 0; i < sizeof(bool); i++)
	{
		pSerializer->pData = VectorPush(pSerializer->pData, &pIn[i]);
	}
}

void BS_SerializeFloat(float val, struct BinarySerializer* pSerializer)
{
	char* pIn = (char*)&val;
	for (int i = 0; i < sizeof(float); i++)
	{
		pSerializer->pData = VectorPush(pSerializer->pData, &pIn[i]);
	}
}

void BS_SerializeDouble(double val, struct BinarySerializer* pSerializer)
{
	char* pIn = (char*)&val;
	for (int i = 0; i < sizeof(double); i++)
	{
		pSerializer->pData = VectorPush(pSerializer->pData, &pIn[i]);
	}
}

void BS_SerializeString(char* val, struct BinarySerializer* pSerializer)
{
	if (!val)
	{
		BS_SerializeU32(0, pSerializer);
		return;
	}
	BS_SerializeU32(strlen(val), pSerializer);
	while (*val)
	{
		pSerializer->pData = VectorPush(pSerializer->pData, val++);
	}
}

void BS_SerializeBytes(char* val, u32 len, struct BinarySerializer* pSerializer)
{
	BS_SerializeU32(len, pSerializer);
	for (int i = 0; i < len; i++)
	{
		BS_SerializeU8((u8)val[i], pSerializer);
	}
}

void BS_DeSerializeI64(i64* val, struct BinarySerializer* pSerializer)
{
	*val = *((i64*)pSerializer->pReadPtr);
	pSerializer->pReadPtr += sizeof(i64);
}

void BS_DeSerializeU64(u64* val, struct BinarySerializer* pSerializer)
{
	*val = *((u64*)pSerializer->pReadPtr);
	pSerializer->pReadPtr += sizeof(u64);
}

void BS_DeSerializeI32(i32* val, struct BinarySerializer* pSerializer)
{
	*val = *((i32*)pSerializer->pReadPtr);
	pSerializer->pReadPtr += sizeof(i32);
}

void BS_DeSerializeU32(u32* val, struct BinarySerializer* pSerializer)
{
	*val = *((u32*)pSerializer->pReadPtr);
	pSerializer->pReadPtr += sizeof(u32);
}

void BS_DeSerializeI16(i16* val, struct BinarySerializer* pSerializer)
{
	*val = *((i16*)pSerializer->pReadPtr);
	pSerializer->pReadPtr += sizeof(i16);
}

void BS_DeSerializeU16(u16* val, struct BinarySerializer* pSerializer)
{
	*val = *((u16*)pSerializer->pReadPtr);
	pSerializer->pReadPtr += sizeof(i16);
}

void BS_DeSerializeI8(i8* val, struct BinarySerializer* pSerializer)
{
	*val = *((i8*)pSerializer->pReadPtr);
	pSerializer->pReadPtr += sizeof(i8);
}

void BS_DeSerializeU8(u8* val, struct BinarySerializer* pSerializer)
{
	*val = *((u8*)pSerializer->pReadPtr);
	pSerializer->pReadPtr += sizeof(u8);
}

void BS_DeSerializeBool(bool* val, struct BinarySerializer* pSerializer)
{
	*val = *((bool*)pSerializer->pReadPtr);
	pSerializer->pReadPtr += sizeof(bool);
}

void BS_DeSerializeFloat(float* val, struct BinarySerializer* pSerializer)
{
	*val = *((float*)pSerializer->pReadPtr);
	pSerializer->pReadPtr += sizeof(float);
}

void BS_DeSerializeDouble(double* val, struct BinarySerializer* pSerializer)
{
	*val = *((float*)pSerializer->pReadPtr);
	pSerializer->pReadPtr += sizeof(float);
}

void BS_DeSerializeString(char** val, struct BinarySerializer* pSerializer)
{
	u32 len = 0;
	BS_DeSerializeU32(&len, pSerializer);
	*val = malloc(len + 1);
	memcpy(*val, pSerializer->pReadPtr, len);
	(*val)[len] = '\0';
	pSerializer->pReadPtr += len;
}

void BS_BytesRead(struct BinarySerializer* pSerializer, u32 numBytes, char* pDst)
{
	memcpy(pDst, pSerializer->pReadPtr, numBytes);
	pSerializer->pReadPtr += numBytes;
}