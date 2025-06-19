#include "Scale.h"
#include <glm/glm.hpp>

COMPONENT_STATIC_FUNCTIONS_AUTOIMPL(Scale)

COMPONENT_REGISTER_DEFINITION(Scale)
{
	return ecs->GetWorld()->component<Scale>("Scale")
		.member<glm::vec2>("val");
}

COMPONENT_FORTH_ADDER_DEFINITION(Scale)
{
	FCell y = PopFloatStack(vm);
	FCell x = PopFloatStack(vm);
	flecs::entity entity = (flecs::entity)PeekIntStack(vm);
	entity.set([x, y](Scale& pos) {pos.val.x = x; pos.val.y = y; });
	return True;
}