#include "EntityInspectorTool.h"
#include "ECS.h"
#include "flecs.h"
#include <iostream>
#include "StaticPhysicsBody.h"
#include "MetaSpriteComponent.h"
#include "imgui/imgui.h"




EntityInspectorTool::EntityInspectorTool(ECS* ecs)
	:m_ecs(ecs)
{
}

void EntityInspectorTool::DoUi()
{
	if (ImGui::Button("save entities as json")) {
		SaveEntitiesTest();
	}
}

void EntityInspectorTool::RecieveTileClick(i32 x, i32 y, i32 z)
{
}

void EntityInspectorTool::TileSelectionChanged(u32 newTile)
{
}

const std::string& EntityInspectorTool::GetName()
{
	static std::string name = "Entity Inspector Tool";
	return name;
}

void EntityInspectorTool::SaveEntitiesTest()
{
	std::string json = "{";
	
	m_ecs->GetWorld()->each([&](flecs::entity e, MetaSpriteComponent& meta) {
		const MetaSpriteComponent* ptr = &meta;
		json += e.to_json();//std::string(m_ecs->GetWorld()->to_json(ptr).c_str()) + ", \n";

	});
}
