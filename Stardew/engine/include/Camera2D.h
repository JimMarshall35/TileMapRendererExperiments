#ifndef CAMERA2D_H
#define CAMERA2D_H

struct Transform2D;
#include <cglm/types.h>

void CenterCameraAt(float worldspaceX, float worldspaceY, struct Transform2D* pCam, int winWidth, int winHeight);
void GetViewportWorldspaceTLBR(vec2 outTL, vec2 outBR, struct Transform2D* pCam, int windowW, int windowH);
void GetCamWorldspaceCenter(struct Transform2D* pCam, int winWidth, int winHeight, vec2 outCenter);
void GetWorldspacePos(int screenPosX, int screenPosY, int screenW, int screenH, struct Transform2D* pCam, vec2 outWorldspace);
void ScreenSpaceToWorldSpaceTransVector(vec2 screenSpaceTranslateVector, int screenW, int screenH, struct Transform2D* pCam, vec2 outWorldspace);


#endif