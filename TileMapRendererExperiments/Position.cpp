#include "Position.h"
#include <glm/glm.hpp>
#include "ECS.h"

COMPONENT_STATIC_FUNCTIONS_AUTOIMPL(Position)

COMPONENT_REGISTER_DEFINITION(Position)
{
	return ecs->GetWorld()->component<Position>("Position")
		.member<glm::vec2>("val");
}

COMPONENT_FORTH_ADDER_DEFINITION(Position)
{
	FCell y = PopFloatStack(vm);
	FCell x = PopFloatStack(vm);
	Cell entityId = PopIntStack(vm);
	flecs::world* world = (flecs::world*)PopIntStack(vm);

	//world->id(entityId)->set([x, y](Position& pos) {pos.val.x = x; pos.val.y = y; });
	return True;
}

