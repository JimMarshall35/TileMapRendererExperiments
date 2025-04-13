#ifndef ROOTWIDGET_H
#define ROOTWIDGET_H
#include "Widget.h"


struct RootWidgetData
{
	int windowW;
	int windowH;
};


HWidget NewRootWidget();

void RootWidget_OnWindowSizeChanged(HWidget rootWidget, int newW, int newH);

#endif