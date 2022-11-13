#include "GameCamera.h"

GameCamera::GameCamera()
:Camera2D("Game") {

}

void GameCamera::ClampPosition()
{
	auto tlbr = GetTLBR();
	
}
