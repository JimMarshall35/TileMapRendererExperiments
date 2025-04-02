#pragma once
typedef struct
{
	int screenWidth;
	int screenHeight;

}DrawContext;

DrawContext Dr_InitDrawContext();
void Dr_OnScreenDimsChange(int newW, int newH);
