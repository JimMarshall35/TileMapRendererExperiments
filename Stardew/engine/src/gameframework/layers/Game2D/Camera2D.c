#include "Camera2D.h"
#include "Game2DLayer.h"

void GetViewportWorldspaceTLBR(vec2 outTL, vec2 outBR, struct Transform2D* pCam, int windowW, int windowH)
{
	outTL[0] = pCam->position[0];
	outTL[1] = pCam->position[1];

	outTL[0] = -outTL[0];
	outTL[1] = -outTL[1];

	outBR[0] = outTL[0] + windowW / pCam->scale[0];
	outBR[1] = outTL[1] + windowH / pCam->scale[0];

}

void CenterCameraAt(float worldspaceX, float worldspaceY, struct Transform2D* pCam, int winWidth, int winHeight)
{
	vec2 tl, br;
	GetViewportWorldspaceTLBR(tl, br, pCam, winWidth, winHeight);
	pCam->position[0] = -(worldspaceX - (br[0] - tl[0]) / 2.0f);
	pCam->position[1] = -(worldspaceY - (br[1] - tl[1]) / 2.0f);
}

void GetCamWorldspaceCenter(struct Transform2D* pCam, int winWidth, int winHeight, vec2 outCenter)
{
	vec2 tl, br;
	vec2 add;
	GetViewportWorldspaceTLBR(tl, br, pCam, winWidth, winHeight);
	add[0] = (br[0] - tl[0]) / 2.0f;
	add[1] = (br[1] - tl[1]) / 2.0f;
	glm_vec2_add(tl, add, outCenter);
}

void GetWorldspacePos(int screenPosX, int screenPosY, int screenW, int screenH, struct Transform2D* pCam, vec2 outWorldspace)
{
	vec2 tl, br;
	GetViewportWorldspaceTLBR(tl, br, pCam, screenW, screenH);
	float fX = (float)screenPosX / (float)screenW;
	float fY = (float)screenPosY / (float)screenH;
	outWorldspace[0] = tl[0] + fX * (br[0] - tl[0]);
	outWorldspace[1] = tl[1] + fY * (br[1] - tl[1]);
}

void ScreenSpaceToWorldSpaceTransVector(vec2 screenSpaceTranslateVector, int screenW, int screenH, struct Transform2D* pCam, vec2 outWorldspace)
{
	vec2 tl, br;
	GetViewportWorldspaceTLBR(tl, br, pCam, screenW, screenH);
	vec2 norm = {
		screenSpaceTranslateVector[0] / screenW,
		screenSpaceTranslateVector[1] / screenH
	};
	outWorldspace[0] = norm[0] * (br[0] - tl[0]);
	outWorldspace[1] = norm[1] * (br[1] - tl[1]);
}
