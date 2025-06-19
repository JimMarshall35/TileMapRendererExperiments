#pragma once
namespace flecs {
	struct world;
}
class NewRenderer;

void MetaspriteSystemSetup(flecs::world& ecs, NewRenderer* renderer);