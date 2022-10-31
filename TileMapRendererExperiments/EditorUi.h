#pragma once
#include "BasicTypedefs.h"
#include <string>

#define MAX_TILES_PER_LUT_CASE 20

class TiledWorld;
class AtlasLoader;
class Camera2D;
class IFileSystem;
class EditorToolBase;

namespace flecs {
	class world;
}

class EditorUi
{
public:
	EditorUi(TiledWorld* tiledWorld, AtlasLoader* atlasLoader, flecs::world* ecsWorld, const IFileSystem* fileSystem, EditorToolBase** tools, u32 numTools);
	void DoUiWindow();
	void MouseButtonCallback(float lastX, float lastY, const Camera2D& cam);
private:

private:
	TiledWorld* m_tiledWorld;
	AtlasLoader* m_atlasLoader;
	flecs::world* m_ecsWorld;

	EditorToolBase** m_tools;
	u32 m_numTools;
	u32 m_selectedTool = 0;
	int m_layerToSet;
};