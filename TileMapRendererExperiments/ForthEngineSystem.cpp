#include "ForthEngineSystem.h"
#include "ECS.h"
#include <conio.h>

ECS* ForthEngineSystem::s_ecs;

ForthEngineSystem::ForthEngineSystem(ForthEngineSystemInitArgs* args, ECS* ecs)
	:m_intStackSizeCells(args->intStackSizeCells),
	m_returnStackSizeCells(args->returnStackSizeCells),
	m_memorySizeCells(args->memorySizeCells)

{
	s_ecs = ecs;
	StartGlobalVm(args);
	


}

void ForthEngineSystem::StartGlobalVm(const ForthEngineSystemInitArgs* args)
{
	u32 globalVmSizeBytes = (args->intStackSizeCells + args->returnStackSizeCells + args->memorySizeCells) * sizeof(Cell);
	m_globalVmMemory = std::make_unique<Cell[]>(globalVmSizeBytes);
	Cell* memoryBase = m_globalVmMemory.get();
	m_intStack = memoryBase;
	m_returnStack = memoryBase + m_intStackSizeCells;
	m_memoryForCompiledWordsAndVariables = memoryBase + m_intStackSizeCells + m_returnStackSizeCells;
	m_globalVm = Forth_Initialise(
		m_memoryForCompiledWordsAndVariables,
		m_memorySizeCells,
		m_intStack,
		m_intStackSizeCells,
		m_returnStack,
		m_returnStackSizeCells,
		&putchar, _getch
	);
}

Bool ForthEngineSystem::ForthFun_CreateEntity(ForthVm* vm)
{
	PushIntStack(vm, s_ecs->GetWorld()->entity());
	return True;
}

Bool ForthEngineSystem::ForthFun_AddB2DDynamicCircleComponent(ForthVm* vm)
{
	return Bool();
}
