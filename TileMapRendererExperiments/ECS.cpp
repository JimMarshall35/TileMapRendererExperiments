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

// Reusable reflection support for std::vector https://github.com/SanderMertens/flecs/blob/master/examples/cpp/reflection/ser_std_vector/src/main.cpp
template <typename Elem, typename Vector = std::vector<Elem>>
flecs::opaque<Vector, Elem> std_vector_support(flecs::world& world) {
	return flecs::opaque<Vector, Elem>()
		.as_type(world.vector<Elem>())

		// Forward elements of std::vector value to serializer
		.serialize([](const flecs::serializer* s, const Vector* data) {
		for (const auto& el : *data) {
			s->value(el);
		}
		return 0;
			})

		// Return vector count
				.count([](const Vector* data) {
				return data->size();
					})

				// Resize contents of vector
						.resize([](Vector* data, size_t size) {
						data->resize(size);
							})

						// Ensure element exists, return pointer
								.ensure_element([](Vector* data, size_t elem) {
								if (data->size() <= elem) {
									data->resize(elem + 1);
								}

								return &data->data()[elem];
									});
}

void ECS::Init()
{
	m_ecs.import<flecs::monitor>();

	m_ecs.component<u16[MAX_METASPRITE_SIZE]>("MetaSpriteTilesArray");

	// Register reflection for std::string
	m_ecs.component<std::string>("stdString")
		.opaque(flecs::String) // Opaque type that maps to string
		.serialize([](const flecs::serializer* s, const std::string* data) {
		const char* str = data->c_str();
		return s->value(flecs::String, &str); // Forward to serializer
			})
		.assign_string([](std::string* data, const char* value) {
				*data = value; // Assign new value to std::string
			});

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

	m_ecs.component<std::vector<u16>>()
		.opaque(std_vector_support<u16>);

	auto metaSpriteDescription = m_ecs.component<MetaSpriteDescription>("MetaSpriteDescription")
		.member<std::string>("name")
		.member<u32>("spriteTilesWidth")
		.member<u32>("spriteTilesHeight")
		.member<std::vector<u16>>("tiles");


	//flecs::iter_to_json_desc_t desc;
	//auto metaSprite = m_ecs
}
