#include "ForthEngineSystem.h"
#include "ECS.h"
#include "MetaSpriteComponent.h"
#include "Position.h"
#include "Rect.h"
#include "Rotation.h"
#include "Scale.h"
#include "Forth2/Forth2GameLib.h"

#include <memory>
#include <iostream>
#include <conio.h>
#include <stdio.h>

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
	u32 globalVmSizeBytes = (args->intStackSizeCells + args->returnStackSizeCells + args->memorySizeCells + args->floatStackSize) * sizeof(Cell);
	m_globalVmMemory = std::make_unique<Cell[]>(globalVmSizeBytes);
	Cell* memoryBase = m_globalVmMemory.get();
	m_intStack = memoryBase;
	m_returnStack = memoryBase + m_intStackSizeCells;
	m_floatStack = memoryBase + m_intStackSizeCells + m_returnStackSizeCells;
	m_memoryForCompiledWordsAndVariables = memoryBase + m_intStackSizeCells + m_returnStackSizeCells + m_floatStackSize;
	m_globalVm = Forth_Initialise(
		m_memoryForCompiledWordsAndVariables,
		m_memorySizeCells,
		m_intStack,
		m_intStackSizeCells,
		m_returnStack,
		m_returnStackSizeCells,
		m_floatStack,
		m_floatStackSize,
		&putchar, _getch
	);
	RegisterEngineWords();
}

bool ForthEngineSystem::LoadStartupScript(const std::string& folder, const std::string& file)
{
	std::string path = (folder + "/" + file);
	FILE* fp = fopen(path.c_str(), "r");
	if (fp == NULL) {
		return false;
	}
	// get size
	fseek(fp, 0L, SEEK_END);
	long unsigned int fileSize = ftell(fp);
	rewind(fp);

	std::unique_ptr<char[]> buf = std::make_unique<char[]>(fileSize);

	// pre process forth source file so you can format it nicely in the source file.
	// Outside of comments and string literals turns newlines into spaces, remove duplicate spaces and remove all tabs entirely.
	PreProcessForthSourceFileContentsIntoBuffer(fp, buf.get());
	Forth_DoString(&m_globalVm, buf.get());
	m_startupScript = Forth_SearchForExecutionToken(&m_globalVm, "startup");

	if (m_startupScript == NULL) {
		printf("ForthEngineSystem::LoadStartupScript - script %s does not contain a word called startup", path.c_str());
	}

	fclose(fp);
	return true;
}

void ForthEngineSystem::RunStartupScriptIfLoaded()
{
	if (m_startupScript) {
		Forth_DoExecutionToken(&m_globalVm, m_startupScript);
	}
	else {
		std::cout << "ForthEngineSystem::RunStartupScriptIfLoaded no startup script loaded\n";
	}
}

void ForthEngineSystem::RegisterEngineWords()
{
	ExecutionToken addEntity = Forth_RegisterCFunc(&m_globalVm, &ForthEngineSystem::ForthFun_CreateEntity, "entity", False);
	ExecutionToken endEntity = Forth_RegisterCFunc(&m_globalVm, &ForthEngineSystem::ForthFun_EndEntity, "endentity", False);

	ExecutionToken addMetaSpriteComponent = MetaSpriteComponent::RegisterForthAdder(&m_globalVm);
	ExecutionToken addPosition = Position::RegisterForthAdder(&m_globalVm);
	ExecutionToken addRect = Rect::RegisterForthAdder(&m_globalVm);
	ExecutionToken addRotation = Rotation::RegisterForthAdder(&m_globalVm);
	ExecutionToken addScale = Scale::RegisterForthAdder(&m_globalVm);

}

Bool ForthEngineSystem::ForthFun_CreateEntity(ForthVm* vm)
{
	u32 esize =  sizeof(flecs::entity);
	flecs::entity entity = s_ecs->GetWorld()->entity();
	PushIntStack(vm, entity.id());
	PushIntStack(vm, (UCell)s_ecs->GetWorld());
	return True;
}

Bool ForthEngineSystem::ForthFun_EndEntity(ForthVm* vm)
{
	PopIntStack(vm);
	PopIntStack(vm);
	return True;
}

Bool ForthEngineSystem::ForthFun_AddB2DDynamicCircleComponent(ForthVm* vm)
{
	return Bool();
}
