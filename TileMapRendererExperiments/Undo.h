#pragma once
#include "BasicTypedefs.h"
class TiledWorld;
enum class EditType {
	SingleTile
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
};
struct UndoableEdit {
	EditType type;
	UndoableEditDataUnion data;
};

void PushEdit(const UndoableEdit& edit, bool invalidateRedoStack = true);
void Undo(TiledWorld& world);
void Redo(TiledWorld& world);