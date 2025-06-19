#pragma once
#include <map>
#include "Forth2/Forth2.h"
//class ComponentType {
//
//};
//
//struct ComponentBase {
//	ComponentBase();
//private:
//	static std::map<type_info, ComponentType> s_ComponentType;
//};

#define COMPONENT(name) \
struct name {\
static flecs::untyped_component RegisterWithECS(ECS* ecs);\
static Bool AddComponentForth_Implementation(ForthVm* vm);\
static Bool AddComponentForth(ForthVm* vm);\
static ExecutionToken RegisterForthAdder(ForthVm* vm);


#define ENDCOMPONENT };
 
#define COMPONENT_ADDCOMPONENT_FORTH_DEFINITION(componentName)\
	Bool componentName##::AddComponentForth(ForthVm* vm){\
		return AddComponentForth_Implementation(vm);\
	}
#define COMPONENT_REGISTER_FORTH_DEFINITION(componentName)\
	ExecutionToken componentName##::RegisterForthAdder(ForthVm* vm){\
		return Forth_RegisterCFunc(vm, &##componentName##::AddComponentForth, #componentName, False);\
	}

#define COMPONENT_STATIC_FUNCTIONS_AUTOIMPL(componentName) COMPONENT_ADDCOMPONENT_FORTH_DEFINITION(componentName) COMPONENT_REGISTER_FORTH_DEFINITION(componentName)

#define COMPONENT_REGISTER_DEFINITION(componentName) flecs::untyped_component componentName##::RegisterWithECS(ECS* ecs)
#define COMPONENT_FORTH_ADDER_DEFINITION(componentName) Bool componentName##::AddComponentForth_Implementation(ForthVm* vm)