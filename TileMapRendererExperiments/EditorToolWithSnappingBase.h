#pragma once
#include "EditorToolBase.h"

#include <glm/glm.hpp>
class EditorToolWithSnappingBase : public EditorToolBase
{
public:
	// Inherited via EditorToolBase
	virtual void DoUi() override;
protected:
	glm::vec2 GetSnappedPosition(const glm::vec2& worldSpace);
protected:
	u32 m_snappingIncrementTileDivider = 0; // 0 = off

};