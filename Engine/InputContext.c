#include "InputContext.h"
#include <string.h>
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

static int gJoystick = -1;

static char* LoadFile(const char* path, int* outSize)
{
	FILE* fp = fopen(path, "r");
	if (!fp) return;
	fseek(fp, 0L, SEEK_END);
	*outSize = ftell(fp);
	int sz = *outSize;
	fseek(fp, 0L, SEEK_SET);
	void* pOut = malloc(sz);
	fread(pOut, 1, sz, fp);
	fclose(fp);
	return pOut;
}

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

void In_SetControllerPresent(int controllerNo)
{
	gJoystick = controllerNo;
}

void In_Poll()
{
}

InputContext In_InitInputContext()
{
	InputContext ctx;
	memset(&ctx, 0, sizeof(InputContext));

	int size = 0;
	char* data = LoadFile("Assets/Keymap.json", &size);
	cJSON* json = cJSON_ParseWithLength(data, size);
	return ctx;
}
