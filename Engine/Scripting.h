#ifndef SCRIPTING_H
#define SCRIPTING_H


#include <stdbool.h>
#include "IntTypes.h"

#define MAX_SCRIPT_FUNCTION_NAME_SIZE 32

enum ScriptCallArgumentDataType
{
	SCA_nil, SCA_boolean, SCA_number, SCA_string, SCA_userdata, SCA_table

};

struct ScriptCallArgument
{
	enum ScriptCallArgumentDataType type;
	union
	{
		bool boolean;
		double number;
		char* string;
		void* userData;
		int table; // in our case it will be a table stored int he regsitry with this value as a key
	}val;
};

void Sc_InitScripting();
void Sc_DeInitScripting();
bool Sc_OpenFile(const char* path);

/// <summary>
/// 
/// </summary>
/// <param name="funcName"></param>
/// <param name="pArgs"></param>
/// <param name="numArgs"></param>
/// <param name="pAllocationFor">
/// 
/// What C memory allocation is the tables lifetime tied to. this value will be used for the reg table key.
/// This is global and shared among other lua libraries, to get a unique key to store things in we use the pointer for a C memory 
/// allocation. When the memory is deallocated the table should be deleted by calling Sc_DeleteTableInReg
/// 
/// </param>
/// <returns> key to the table returned, stored in the registry table. 0 if failed </returns>
int Sc_CallGlobalFuncReturningTableAndStoreResultInReg(const char* funcName, struct ScriptCallArgument* pArgs, int numArgs);

void Sc_CallFuncInRegTableEntryTable(int regIndex, const char* funcName, struct ScriptCallArgument* pArgs, int numArgs, int numReturnVals);

void Sc_AddLightUserDataValueToTable(int regIndex, const char* userDataKey, void* userDataValue);

size_t Sc_StackTopStringLen();
void Sc_StackTopStrCopy(char* pOutString);
void Sc_ResetStack();

/// <summary>
/// 
/// </summary>
/// <param name="index"> delete a table stored in the lua registry </param>
void Sc_DeleteTableInReg(int index);

int Sc_Int();

#endif