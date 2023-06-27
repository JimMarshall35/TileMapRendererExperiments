#include "EntityInspectorTool.h"

void EntityInspectorTool::DoUi()
{
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
