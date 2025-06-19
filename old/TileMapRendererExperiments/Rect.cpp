#include "Rect.h"
#include <glm/glm.hpp>

COMPONENT_STATIC_FUNCTIONS_AUTOIMPL(Rect)

COMPONENT_REGISTER_DEFINITION(Rect)
{
	return ecs->GetWorld()->component<Rect>("Rect")
		.member<glm::vec2>("pos")
		.member<glm::vec2>("dims");
}

COMPONENT_FORTH_ADDER_DEFINITION(Rect)
{
	FCell dimy = PopFloatStack(vm);
	FCell dimx = PopFloatStack(vm);
	FCell y = PopFloatStack(vm);
	FCell x = PopFloatStack(vm);
	flecs::entity entity = (flecs::entity)PeekIntStack(vm);
	entity.set([x, y, dimx, dimy](Rect& r) { r.pos.x = x; r.pos.y = y; r.dims.x = dimx; r.dims.y = dimy; });
	return True;
}