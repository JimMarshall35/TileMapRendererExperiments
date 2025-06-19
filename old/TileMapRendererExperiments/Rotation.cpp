#include "Rotation.h"
COMPONENT_STATIC_FUNCTIONS_AUTOIMPL(Rotation)

COMPONENT_REGISTER_DEFINITION(Rotation)
{
	return ecs->GetWorld()->component<Rotation>("Rotation")
		.member<float>("radians");
}

COMPONENT_FORTH_ADDER_DEFINITION(Rotation)
{
	FCell rot = PopFloatStack(vm);
	flecs::entity entity = (flecs::entity)PeekIntStack(vm);
	entity.set([rot](Rotation& r) { r.Value = rot; });
	return True;
}