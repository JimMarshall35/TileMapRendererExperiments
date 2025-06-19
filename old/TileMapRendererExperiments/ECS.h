#pragma once
#include "flecs.h"

class ECS
{
public:
	ECS();
	inline void Progress() { m_ecs.progress(); }
	inline flecs::world* GetWorld() { return &m_ecs; }
private:
	void Init();
private:
	flecs::world m_ecs;
};

