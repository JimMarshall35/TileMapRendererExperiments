#pragma once
#include "BasicTypedefs.h"
#include <glm/glm.hpp>
#include <string>
class Camera2D;
class EditorToolBase
{
public:
	virtual void DoUi() = 0;
	virtual void RecieveTileClick(i32 x, i32 y, i32 z) = 0;
	virtual void TileSelectionChanged(u32 newTile) = 0;
	virtual const std::string& GetName() = 0;
	virtual void RecieveWorldspaceClick(const glm::vec2& worldspace);
	virtual bool WantsToDrawOverlay() const;
	virtual void DrawOverlay(const Camera2D& camera, const glm::vec2& mouseWorldSpacePos) const;
};

