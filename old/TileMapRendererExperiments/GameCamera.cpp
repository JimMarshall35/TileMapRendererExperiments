#include "GameCamera.h"

GameCamera::GameCamera(u32 screenW, u32 screenH, u32 worldXSize, u32 worldYSize)
	:Camera2D(screenW, screenH, "Game"),
	m_worldXSize(worldXSize),
	m_worldYSize(worldYSize)
{
	Zoom = 90.0f;
	ClampPosition();
}

void GameCamera::SetWindowWidthAndHeight(u32 newW, u32 newH)
{
	ClampPosition();
	Camera2D::SetWindowWidthAndHeight(newW, newH);
}

void GameCamera::ClampPosition()
{
	auto tlbr = GetTLBR();
	auto tl = glm::vec2(tlbr.y, tlbr.x);
	auto br = glm::vec2(tlbr.w, tlbr.z);

	auto mapTlCorner = glm::vec2(0, 0);
	
	auto tlToMapTlCorner = mapTlCorner - (tl + glm::vec2(0.5f,0.5f));
	
	if (tlToMapTlCorner.x > 0) {
		FocusPosition.x += tlToMapTlCorner.x;
	}
	if (tlToMapTlCorner.y > 0) {
		FocusPosition.y += tlToMapTlCorner.y;
	}
	
	auto realBrCorner = (mapTlCorner + glm::vec2(0.5f, 0.5f)) + glm::vec2(m_worldXSize,m_worldYSize) - glm::vec2(1,1);
	auto brToMapBrCorner = realBrCorner - br;
	if (brToMapBrCorner.x < 0) {
		FocusPosition.x += brToMapBrCorner.x;
	}
	if (brToMapBrCorner.y < 0) {
		FocusPosition.y += brToMapBrCorner.y;
	}

	//auto mapBrCorner = mapTlCorner + 
}
