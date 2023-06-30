#include "EntityInspectorTool.h"
#include "ECS.h"
#include "flecs.h"
#include <iostream>
#include "StaticPhysicsBody.h"
#include "MetaSpriteComponent.h"
#include "imgui/imgui.h"
#include "NewRenderer.h"



EntityInspectorTool::EntityInspectorTool(ECS* ecs, DynamicQuadTreeContainer<flecs::entity>* entityQuadTree, NewRenderer* renderer)
	:m_ecs(ecs),
	m_entityQuadTree(entityQuadTree),
	m_renderer(renderer)
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

bool EntityInspectorTool::WantsToDrawOverlay() const
{
	return true;
}

void EntityInspectorTool::DrawOverlay(const Camera2D& camera, const glm::vec2& mouseWorldSpacePos)
{
	using namespace glm;
	auto camTLBR = camera.GetTLBR();
	Rect r;
	r.pos.x = camTLBR.y;
	r.pos.y = camTLBR.x;

	r.dims.x = camTLBR.w - camTLBR.y;
	r.dims.y = camTLBR.z - camTLBR.x;
	auto vis = m_entityQuadTree->search(r);

	size_t visSize = vis.size();
	bool newVisibleSet = false;
	if (m_visibileItems.size() != visSize) {
		m_visibileItems.resize(visSize);
		newVisibleSet = true;
	}

	int i = 0;
	for (const auto& qItem : vis) {

		glm::vec4 colour = { 1.0, 1.0, 1.0, 1.0 };
		glm::vec4 colour2 = { 1.0, 1.0, 0.0, 1.0 };

		Rect entityRect = qItem->pItem.iterator->first;
		
		if (entityRect != m_visibileItems[i].second) {
			newVisibleSet = true;
		}
		if (newVisibleSet) {
			m_visibileItems[i] = std::pair<flecs::entity, Rect>(qItem->item, entityRect);
		}
		colour = qItem->item == m_selectedEntity ? colour2 : colour;

		glm::vec2 pt1 = vec2(entityRect.Left(), entityRect.Top());
		glm::vec2 pt2 = vec2(entityRect.Right(), entityRect.Top());
		m_renderer->DrawLine(pt1, pt2, colour, 3, camera);

		pt1 = vec2(entityRect.Right(), entityRect.Top());
		pt2 = vec2(entityRect.Right(), entityRect.Bottom());
		m_renderer->DrawLine(pt1, pt2, colour, 3, camera);

		pt1 = vec2(entityRect.Right(), entityRect.Bottom());
		pt2 = vec2(entityRect.Left(), entityRect.Bottom());
		m_renderer->DrawLine(pt1, pt2, colour, 3, camera);

		pt1 = vec2(entityRect.Left(), entityRect.Bottom());
		pt2 = vec2(entityRect.Left(), entityRect.Top());
		m_renderer->DrawLine(pt1, pt2, colour, 3, camera);

		i++;
	}
}

void EntityInspectorTool::RecieveWorldspaceClick(const glm::vec2& worldspace)
{
	for (const std::pair<flecs::entity, Rect>& pair : m_visibileItems)
	{
		if (pair.second.Contains(worldspace)) {
			m_selectedEntity = pair.first;
			return;
		}
	}
	m_selectedEntity = m_nullEntity;
}

void EntityInspectorTool::SaveEntitiesTest()
{
	std::string json = "{";
	
	m_ecs->GetWorld()->each([&](flecs::entity e, MetaSpriteComponent& meta) {
		const MetaSpriteComponent* ptr = &meta;
		json += e.to_json();//std::string(m_ecs->GetWorld()->to_json(ptr).c_str()) + ", \n";

	});
}
