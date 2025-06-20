#include "Scripting.h"
#include "lua.h"
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "AssertLib.h"
#include "XMLUIGameLayer.h"
#include "RootWidget.h"

#define GAME_LUA_MINOR_VERSION 1

static lua_State* gL = NULL;

static void OnPropertyChangedInternal(XMLUIData* pUIData, HWidget hWidget, const char* pChangedPropName)
{
	while (hWidget != NULL_HWIDGET)
	{
		struct UIWidget* pWidget = UI_GetWidget(hWidget);
		for (int i = 0; i < pWidget->numBindings; i++)
		{
			struct WidgetPropertyBinding* pBinding = &pWidget->bindings[i];
			if (strcmp(pBinding->name, pChangedPropName) == 0)
			{
				if (pWidget->fnOnBoundPropertyChanged)
				{
					pWidget->fnOnBoundPropertyChanged(pWidget, pBinding);
				}
			}
		}
		OnPropertyChangedInternal(pUIData, pWidget->hFirstChild, pChangedPropName);

		hWidget = pWidget->hNext;
	}
}

static int L_OnPropertyChanged(lua_State* L)
{
	// args: (table) viewmodel, (string) propertyName
	int top = lua_gettop(L);
	EASSERT(top == 2);
	bool bIsTable = lua_istable(L, 1);
	EASSERT(bIsTable);
	const char* pStr = luaL_checkstring(L, 2);
	const char* pNameCpy = malloc(strlen(pStr) + 1);
	strcpy(pNameCpy, pStr);
	lua_pop(L, 1);
	lua_getfield(L, -1, "XMLUIDataPtr");
	XMLUIData* pUIData = lua_touserdata(L, -1);
	HWidget hWidget = pUIData->rootWidget;
	OnPropertyChangedInternal(pUIData, hWidget, pNameCpy);
	free(pNameCpy);
	SetRootWidgetIsDirty(pUIData->rootWidget, true);
}


void Sc_InitScripting()
{
	gL = luaL_newstate();
	luaL_openlibs(gL); /* Load Lua libraries */
	lua_pushcfunction(gL, &L_OnPropertyChanged);
	lua_setglobal(gL, "OnPropertyChanged");

}

void Sc_DeInitScripting()
{
	lua_close(gL);
}

bool Sc_OpenFile(const char* path)
{
	int status = luaL_loadfile(gL, path);
	if (status)
	{
		/* If something went wrong, error message is at the top of */
		/* the stack */
		fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(gL, -1));
		return false;
	}
	lua_pcall(gL, 0, LUA_MULTRET, 0);
	return status == 0;
#if GAME_LUA_VERSION > 1
	return status == LUA_OK;
#endif
}

static const char* GetTypeOnTopOfStack()
{
	if (lua_isnil(gL, -1))
	{
		return "nil";
	}
	else if (lua_isnumber(gL, -1))
	{
		return "number";
	}
	else if (lua_isstring(gL, -1))
	{
		return "string";
	}
	else if (lua_isboolean(gL, -1))
	{
		return "boolean";
	}
	else if (lua_istable(gL, -1))
	{
		return "table";
	}
	else if (lua_isuserdata(gL, -1))
	{
		return "userdata";
	}
	return "unknown";
}

static void PushFunctionCallArgsOntoStack(struct ScriptCallArgument* pArgs, int numArgs)
{
	for (int i = 0; i < numArgs; i++)
	{
		struct ScriptCallArgument* pArg = &pArgs[i];
		switch (pArg->type)
		{
		case SCA_nil:
			lua_pushnil(gL);
			break;
		case SCA_boolean:
			lua_pushboolean(gL, pArg->val.boolean);
			break;
		case SCA_number:
			lua_pushnumber(gL, pArg->val.number);
			break;
		case SCA_string:
			lua_pushstring(gL, pArg->val.string);
			break;
		case SCA_table:
			lua_rawgeti(gL, LUA_REGISTRYINDEX, pArg->val.table);
			EASSERT(lua_istable(gL, -1));
			break;
		case SCA_userdata:
			lua_pushlightuserdata(gL, pArg->val.userData);
			break;
		case SCA_int:
			lua_pushinteger(gL, pArg->val.i);
			break;
		}
	}
}

int Sc_CallGlobalFuncReturningTableAndStoreResultInReg(const char* funcName, struct ScriptCallArgument* pArgs, int numArgs)
{
	int rVal = 0;
	lua_getglobal(gL, funcName);
	if (lua_isfunction(gL, -1))
	{
		PushFunctionCallArgsOntoStack(pArgs, numArgs);
		const int returnvalues_count = 1; // function returns 0 values

		lua_pcall(gL, numArgs, returnvalues_count, 0); // now call the function
		EASSERT(lua_istable(gL, -1));
		rVal = luaL_ref(gL, LUA_REGISTRYINDEX);
	}
	else
	{
		printf("Sc_CallGlobalFuncReturningTableAndStoreResultInReg funcName '%s' was not a function, it was type '%s'\n", funcName, GetTypeOnTopOfStack());
	}
	lua_settop(gL, 0);
	return rVal;
}

void Sc_CallFuncInRegTableEntryTable(int regIndex, const char* funcName, struct ScriptCallArgument* pArgs, int numArgs, int numReturnVals)
{
	lua_rawgeti(gL, LUA_REGISTRYINDEX, regIndex);
	bool bIstable = lua_istable(gL, -1);
	if (!bIstable)
	{
		printf("Sc_CallFuncInRegTableEntryTable. Reg table entry %i is not a table, but %s\n", regIndex, GetTypeOnTopOfStack());
		lua_settop(gL, 0);
		return;
	}
	lua_pushstring(gL, funcName);
	lua_gettable(gL, -2);
	if (lua_isfunction(gL, -1))
	{
		lua_rawgeti(gL, LUA_REGISTRYINDEX, regIndex);
		PushFunctionCallArgsOntoStack(pArgs, numArgs);
		lua_pcall(gL, numArgs + 1, numReturnVals, 0);
	}
	else
	{
		printf("object at key '%s' not a function but type %s \n", funcName, GetTypeOnTopOfStack());
	}
	//lua_settop(gL, 0);
}

void Sc_AddLightUserDataValueToTable(int regIndex, const char* userDataKey, void* userDataValue)
{
	lua_rawgeti(gL, LUA_REGISTRYINDEX, regIndex);
	bool bIstable = lua_istable(gL, -1);
	if (!bIstable)
	{
		printf("Sc_CallFuncInRegTableEntryTable. Reg table entry %i is not a table, but %s\n", regIndex, GetTypeOnTopOfStack());
		lua_settop(gL, 0);
		return;
	}
	lua_pushlightuserdata(gL, userDataValue);
	lua_setfield(gL, -2, userDataKey);
	lua_settop(gL, 0);
	printf("Sc_AddLightUserDataValueToTable done\n");
}

int Sc_Int()
{
#if GAME_LUA_MINOR_VERSION >= 3
	EASSERT(lua_isinteger(gL, -1));
#endif
	return lua_tointeger(gL, -1);
}

float Sc_Float()
{
	EASSERT(lua_isnumber(gL, -1));
	return lua_tonumber(gL, -1);
}

size_t Sc_StackTopStringLen()
{
	EASSERT(lua_isstring(gL, -1));
	const char* str = lua_tostring(gL, -1);
	return strlen(str);
}

void Sc_StackTopStrCopy(char* pOutString)
{
	EASSERT(lua_isstring(gL, -1));
	const char* str = lua_tostring(gL, -1);
	strcpy(pOutString, str);
}

void Sc_ResetStack()
{
	lua_settop(gL, 0);
}

void Sc_DeleteTableInReg(int index)
{
	luaL_unref(gL, LUA_REGISTRYINDEX, index);
}
