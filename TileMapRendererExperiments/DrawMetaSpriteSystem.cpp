#include "DrawMetaSpriteSystem.h"
#include "flecs.h"
#include "MetaSpriteComponent.h"
#include "Position.h"
#include "Scale.h"
#include "NewRenderer.h"


static NewRenderer* s_renderer;

void MetaspriteSystemSetup(flecs::world& ecs, NewRenderer* renderer)
{
    s_renderer = renderer;
    //ecs.system<const Position, const Scale, const MetaSpriteComponent>()
    //    .each([](const Position& p, const Scale& s, const MetaSpriteComponent& m) {
    //    
    //    });
}
