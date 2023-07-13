#pragma once
#include "Forth2/Forth2.h"
#include <memory>

/*
	adapter between forth and the engine.
	functions:
		(definite) have a global forth vm, expose ecs to it (make entities, components)
		(possible) be able to output mini vms that can be attatched as a component to entities, these link back to global vm
		and only contain byte code / threaded code so can and must be small
*/

struct ForthEngineSystemInitArgs
{
	
	UCell memorySize;
	UCell intStackSize;
	UCell returnStackSize;
};

class ForthEngineSystem
{
public:
	ForthEngineSystem(const ForthEngineSystemInitArgs& args);
private:
	
	Cell* memoryForCompiledWordsAndVariables;
	Cell* intStack;
	Cell* returnStack;
};

