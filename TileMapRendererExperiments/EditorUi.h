#pragma once
#include "BasicTypedefs.h"
#include <string>

#define MAX_TILES_PER_LUT_CASE 20

class TiledWorld;
class AtlasLoader;
class Camera2D;

namespace flecs {
	class world;
}

class EditorUi
{
public:
	EditorUi(TiledWorld* tiledWorld, AtlasLoader* atlasLoader, flecs::world* ecsWorld);
	void DoUiWindow();
	void PushLookupTableCase(u8 caseIndex, u32 valueToPush);
	void PopLookupTableCase(u8 caseIndex);
	void MouseButtonCallback(float lastX, float lastY, u32 windowW, u32 windowH, const Camera2D& cam);
private:
	void LookupTableToolHandleMouse(i32 x, i32 y, i32 z);
	u8 GetLutCaseIndex(i32 x, i32 y, i32 z);
	bool IsTileIndexInLut(u16 tileIndex);
	void PushLookupTableCase();
	void PopLookupTableCase();
	u32 PeekLUTTableTop(u8 caseIndex) { return m_tileToolLookupTable[caseIndex][m_tileToolLookupTableEntriesPerCase[caseIndex] - 1]; }
	u8 GetCurrentLookupTableCaseIndex();
	void SaveToolLUT(std::string pathToSave);
	void LoadToolLUT(std::string pathToLoad);
private:
	TiledWorld* m_tiledWorld;
	AtlasLoader* m_atlasLoader;
	flecs::world* m_ecsWorld;
	bool m_tileToolBoxes[8] = { false,false,false,false,false,false,false,false, };
	int m_layerToSet = 0;
	int m_tileIndexToSet = 0;
	u16 m_tileToolLookupTable[256][MAX_TILES_PER_LUT_CASE];
	u32 m_tileToolLookupTableEntriesPerCase[256];
};