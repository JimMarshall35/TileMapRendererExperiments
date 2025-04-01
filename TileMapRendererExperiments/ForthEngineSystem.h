#pragma once
#include "Forth2/Forth2.h"
#include "ECS.h"
#include "BasicTypedefs.h"
#include <memory>
#include <string>

/*
	adapter between forth and the engine.
	functions:
		(definite) have a global forth vm, expose ecs to it (make entities, components)
		(possible) be able to output mini vms that can be attatched as a component to entities, these link back to global vm
		and only contain byte code / threaded code so can and must be small
*/

struct ForthEngineSystemInitArgs
{
	UCell memorySizeCells;
	UCell intStackSizeCells;
	UCell returnStackSizeCells;
	UCell floatStackSize;
};
class ECS;

class ForthEngineSystem
{
public:
	ForthEngineSystem(ForthEngineSystemInitArgs* args, ECS* ecs);
	bool LoadStartupScript(const std::string& folder, const std::string& file);
	void RunStartupScriptIfLoaded();
private:
	void StartGlobalVm(const ForthEngineSystemInitArgs* args);
	
	void RegisterEngineWords();
private:
	std::unique_ptr<Cell[]> m_globalVmMemory;
	Cell* m_memoryForCompiledWordsAndVariables;
	Cell* m_intStack;
	Cell* m_returnStack;
	ForthVm m_globalVm;
	UCell m_memorySizeCells;
	UCell m_intStackSizeCells;
	UCell m_returnStackSizeCells;
	Cell* m_floatStack;
	UCell m_floatStackSize;
	static ECS* s_ecs;
private:
	static Bool ForthFun_CreateEntity(ForthVm* vm);
	static Bool ForthFun_EndEntity(ForthVm* vm);

	static Bool ForthFun_AddB2DDynamicCircleComponent(ForthVm* vm);
	ExecutionToken m_startupScript = nullptr;
};

