#include "InputContext.h"

void In_RecieveKeyboardKey(InputContext* context, int key, int scancode, int action, int mods)
{

}

void In_RecieveMouseMove(InputContext* context, double xposIn, double yposIn)
{
}

void In_RecieveMouseButton(InputContext* context, int button, int action, int mods)
{
}

void In_FramebufferResize(InputContext* context, int width, int height)
{
}

void In_RecieveScroll(InputContext* context, double xoffset, double yoffset)
{
}

InputContext In_InitInputContext()
{
	InputContext ctx;
	ctx.AxisMappings = VECTOR(InputMapping);
	ctx.ButtonMapping = VECTOR(InputMapping)
	return ctx;
}
