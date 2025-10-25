#include "WfInit.h"
#include "WfWoodedArea.h"
#include "WfPlayerStart.h"
#include "WfPlayer.h"
void WfInit()
{
    WfWoodedAreaInit();
    WfInitPlayerStart();
    WfInitPlayer();
}

