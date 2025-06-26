#ifndef  MAIN_H

#include "DrawContext.h"
#include "InputContext.h"

int Mn_GetScreenWidth();
int Mn_GetScreenHeight();

typedef void(*GameInitFn)(InputContext*,DrawContext*);

int EngineStart(int argc, char** argv, GameInitFn init);

#endif // ! MAIN_H
