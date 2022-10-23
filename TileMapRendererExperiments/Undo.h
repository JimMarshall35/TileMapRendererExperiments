#pragma once
#include "BasicTypedefs.h"

class TiledWorld;
class LutDrawTool;

enum class EditType {
	SingleTile,
	TileToolLUTCasePush,
	TileToolLUTCasePop,
};

struct TileToolLUTCasePopData {
	u8 lutCase;
	u32 valuePopped;
};

struct TileToolLUTCasePushData {
	u8 lutCase;
	u32 valuePushed;
};

struct SingleTileEditData {
	u32 x;
	u32 y;
	u32 z;
	u16 oldVal;
	u16 newVal;
};

union UndoableEditDataUnion {
	SingleTileEditData singleTile; 
	TileToolLUTCasePopData tileToolLUTPop;
	TileToolLUTCasePushData tileToolLUTPush;
};
struct UndoableEdit {
	EditType type;
	UndoableEditDataUnion data;
};

void PushEdit(const UndoableEdit& edit, bool invalidateRedoStack = true);
void Undo(TiledWorld& world, LutDrawTool& editorUi);
void Redo(TiledWorld& world, LutDrawTool& editorUi);