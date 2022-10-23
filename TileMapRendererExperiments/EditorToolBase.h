#pragma once
#include "BasicTypedefs.h"
#include <string>
class EditorToolBase
{
public:
	virtual void DoUi() = 0;
	virtual void RecieveTileClick(i32 x, i32 y, i32 z) = 0;
	virtual void TileSelectionChanged(u32 newTile) = 0;
	virtual const std::string& GetName() = 0;
};

