#include "Undo.h"
#include "TiledWorld.h"
#define EDIT_STACK_SIZE 128
static UndoableEdit s_undoStack[EDIT_STACK_SIZE];
static UndoableEdit s_redoStack[EDIT_STACK_SIZE];

static i32 s_undoTopPointer = 0;
static u32 s_numEdits = 0;

static i32 s_redoTopPointer = 0;
static u32 s_numRedos;

static void PushRedoEdit(const UndoableEdit& edit) {
	s_redoStack[s_redoTopPointer++] = edit;
	if (s_redoTopPointer >= EDIT_STACK_SIZE) {
		s_redoTopPointer = 0;
	}
	if (s_numRedos < EDIT_STACK_SIZE) {
		s_numRedos++;
	}
}
void Redo(TiledWorld& world) {
	if (s_numRedos == 0) {
		return;
	}
	s_numRedos--;
	const UndoableEdit& edit = s_redoStack[--s_redoTopPointer];
	if (s_redoTopPointer < 0) {
		s_redoTopPointer = EDIT_STACK_SIZE - 1;
		s_redoStack[s_redoTopPointer];
	}

	switch (edit.type)
	{
	case EditType::SingleTile:
	{
		world.SetTile(
			edit.data.singleTile.x,
			edit.data.singleTile.y,
			edit.data.singleTile.z,
			edit.data.singleTile.newVal);
	}
	break;
	default:
		break;
	}
	PushEdit(edit, false);
}


void PushEdit(const UndoableEdit& edit, bool invalidateRedoStack)
{
	s_undoStack[s_undoTopPointer++] = edit;
	if (s_undoTopPointer >= EDIT_STACK_SIZE) {
		s_undoTopPointer = 0;
	}
	if (s_numEdits < EDIT_STACK_SIZE) {
		s_numEdits++;
	}
	if (invalidateRedoStack) {
		s_numRedos = 0;
		s_redoTopPointer = 0;
	}
}

void Undo(TiledWorld& world)
{
	if (s_numEdits == 0) {
		return;
	}
	s_numEdits--;
	const UndoableEdit& edit = s_undoStack[--s_undoTopPointer];
	if (s_undoTopPointer < 0) {
		s_undoTopPointer = EDIT_STACK_SIZE - 1;
		s_undoStack[s_undoTopPointer];
	}

	switch (edit.type)
	{
	case EditType::SingleTile:
		{
		world.SetTile(
			edit.data.singleTile.x,
			edit.data.singleTile.y,
			edit.data.singleTile.z,
			edit.data.singleTile.oldVal);
		}
		break;
	default:
		break;
	}
	PushRedoEdit(edit);
}

