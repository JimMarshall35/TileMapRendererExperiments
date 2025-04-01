#include "MetaSpriteComponent.h"
#include "ECS.h"
#include <vector>
#include <string>
#include "BasicTypedefs.h"

COMPONENT_STATIC_FUNCTIONS_AUTOIMPL(MetaSpriteDescription)

COMPONENT_REGISTER_DEFINITION(MetaSpriteDescription)
{
	return ecs->GetWorld()->component<MetaSpriteDescription>("MetaSpriteDescription")
		.member<std::string>("name")
		.member<u32>("spriteTilesWidth")
		.member<u32>("spriteTilesHeight")
		.member<std::vector<u16>>("tiles");
}

COMPONENT_FORTH_ADDER_DEFINITION(MetaSpriteDescription)
{
	return True;
}

COMPONENT_STATIC_FUNCTIONS_AUTOIMPL(MetaSpriteComponent)

COMPONENT_REGISTER_DEFINITION(MetaSpriteComponent)
{
	return ecs->GetWorld()->component<MetaSpriteComponent>("MetaSpriteComponent")
		.member<i32>("handle")
		.member<u32>("arrayTexture")
		.member<glm::vec2>("pos");
}

COMPONENT_FORTH_ADDER_DEFINITION(MetaSpriteComponent)
{
	i32 handle = PopIntStack(vm);
	FCell y = PopFloatStack(vm);
	FCell x = PopFloatStack(vm);
	flecs::entity entity = flecs::entity(PeekIntStack(vm));
	entity.set([x, y, handle](MetaSpriteComponent& msc) {msc.handle = handle; msc.pos.y = y; msc.pos.x = x; });
	return True;
}

