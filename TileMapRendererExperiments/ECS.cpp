#include "ECS.h"
#include "Position.h"
#include "Rect.h"
#include "Scale.h"
#include "TestMoveComponent.h"
#include "MetaSpriteComponent.h"
#include "Rotation.h"
#include <string>
#include "MetaAtlas.h"

ECS::ECS()
{
	m_ecs.set_target_fps(60.0f);
	Init();
}

void ECS::Init()
{
	auto v2 = m_ecs.component<glm::vec2>("vec2")
		.member<float>("x")
		.member<float>("y");
	auto p = m_ecs.component<Position>("Position")
		.member<glm::vec2>("val");

	auto rect = m_ecs.component<Rect>("Rect")
		.member<glm::vec2>("pos")
		.member<glm::vec2>("dims");

	auto rotation = m_ecs.component<Rotation>("Rotation")
		.member<float>("radians");

	auto scale = m_ecs.component<Scale>("Scale")
		.member<glm::vec2>("val");

	auto metaSprite = m_ecs.component<MetaSpriteComponent>("MetaSpriteComponent")
		.member<i32>("handle")
		.member<glm::vec2>("pos");

	//flecs::iter_to_json_desc_t desc;
	//auto metaSprite = m_ecs
}
