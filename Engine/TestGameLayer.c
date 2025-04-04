#include "TestGameLayer.h"
#include "DrawContext.h"
#include "InputContext.h"
#include "GameFramework.h"
#include <stdio.h>

static void Update(float deltaT)
{

}

static void Draw(DrawContext* dc)
{

}

static void Input(InputContext* ctx)
{

}

static void OnPush()
{
	printf("on push");
}

static void OnPop()
{
	printf("on pop");
}

void TestLayer_Get(struct GameFrameworkLayer* pLayer)
{
	pLayer->draw = &Draw;
	pLayer->update = &Update;
	pLayer->input = &Input;
	pLayer->onPop = &OnPop;
	pLayer->onPush = &OnPush;
	pLayer->flags = 0;
	pLayer->flags |= EnableDrawFn | EnableInputFn | EnableUpdateFn | EnableOnPop | EnableOnPush;
}
