#pragma once
#include "EditorToolBase.h"

#define MAX_TILES_PER_LUT_CASE 20


class IFileSystem;
class TiledWorld;
class AtlasLoader;

class LutDrawTool
	:public EditorToolBase
{
public:
	LutDrawTool(IFileSystem* fs, TiledWorld* tw, AtlasLoader* atlasLoader);
	// Inherited via EditorToolBase
	virtual void DoUi() override;
	virtual void RecieveTileClick(i32 x, i32 y, i32 z) override;
	virtual void TileSelectionChanged(u32 newTile) override;
	virtual const std::string& GetName() override;
	void PushLookupTableCase(u8 caseIndex, u32 valueToPush);
	void PopLookupTableCase(u8 caseIndex);
private:
	u8 GetLutCaseIndex(i32 x, i32 y, i32 z);
	bool IsTileIndexInLut(u16 tileIndex);
	void PushLookupTableCase();
	void PopLookupTableCase();
	u32 PeekLUTTableTop(u8 caseIndex) { return m_tileToolLookupTable[caseIndex][m_tileToolLookupTableEntriesPerCase[caseIndex] - 1]; }
	u8 GetCurrentLookupTableCaseIndex();
	void SaveToolLUT(std::string pathToSave);
	void LoadToolLUT(std::string pathToLoad);
private:
	const IFileSystem* m_fileSystem;
	TiledWorld* m_tiledWorld;
	bool m_tileToolBoxes[8] = { false,false,false,false,false,false,false,false, };
	u16 m_tileToolLookupTable[256][MAX_TILES_PER_LUT_CASE];
	u32 m_tileToolLookupTableEntriesPerCase[256];
	u32 m_tileIndexToSet = 0;
	AtlasLoader* m_atlasLoader;
};

