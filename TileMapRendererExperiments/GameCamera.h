#pragma once
#include "Camera2D.h"
class GameCamera : public Camera2D
{
public:
	GameCamera(u32 screenW, u32 screenH, u32 worldXSize, u32 worldYSize);
	virtual void SetWindowWidthAndHeight(u32 newW, u32 newH) override;
	void ClampPosition();
private:
	u32 m_worldXSize;
	u32 m_worldYSize;
};

