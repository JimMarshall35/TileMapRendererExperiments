#pragma once
#include <glm/glm.hpp>
#include <box2d/b2_world.h>
#include <box2d/b2_api.h>
#include "BasicTypedefs.h"

class ECS;
namespace flecs {
	struct entity;
};

template<typename T>
class DynamicQuadTreeContainer;

class PhysicsWorld
{
public:
	PhysicsWorld(b2Vec2 gravity, ECS* m_ecs, DynamicQuadTreeContainer<flecs::entity>* entityQuadTree);
	void Step();
	bool AddStaticPolygon(const glm::vec2* points, u32 numPoints, flecs::entity& entity);

private:
	b2World m_b2World;
	ECS* m_ecs;

	f32 m_timeStep;
	i32 m_velocityIterations;
	i32 m_positionIterations;
	DynamicQuadTreeContainer<flecs::entity>* m_entityQuadTree;
};

