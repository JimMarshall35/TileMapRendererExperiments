#include "InputContext.h"
#include <string.h>
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <GLFW/glfw3.h>
#include "FileHelpers.h"
#include "AssertLib.h"

static int gJoystick = -1;

HMouseButtonBinding In_FindMouseBtnMapping(InputContext* context, const char* name)
{
	for (int i = 0; i < context->buttonMappings.MouseButtonMappings.size; i++)
	{
		if (strcmp(context->buttonMappings.MouseButtonMappings.arr[i].name, name) == 0)
		{
			return i;
		}
	}
	return NULL_HANDLE;
}

HMouseAxisBinding In_FindMouseAxisMapping(InputContext* context, const char* name)
{
	for (int i = 0; i < context->axisMappings.Mouse.size; i++)
	{
		if (strcmp(context->axisMappings.Mouse.arr[i].name, name) == 0)
		{
			return i;
		}
	}
	return NULL_HANDLE;
}

float In_GetMouseAxisValue(InputContext* context, HMouseAxisBinding hBinding)
{
	return (float)context->axisMappings.Mouse.arr[hBinding].data.axisMapping.fCurrent;
}

bool In_GetMouseButtonValue(InputContext* context, HMouseButtonBinding hBinding)
{
	return context->buttonMappings.MouseButtonMappings.arr[hBinding].data.ButtonMapping.bCurrent;
}

static bool TextEntryAllowedKeystroke(int key)
{
	return 
		(key >= GLFW_KEY_A && key <= GLFW_KEY_Z) ||
		(key >= GLFW_KEY_0 && key <= GLFW_KEY_9) ||
		key == GLFW_KEY_SPACE ||
		key == GLFW_KEY_APOSTROPHE ||
		key == GLFW_KEY_COMMA ||
		key == GLFW_KEY_MINUS ||
		key == GLFW_KEY_PERIOD ||
		key == GLFW_KEY_SLASH ||
		key == GLFW_KEY_SEMICOLON ||
		key == GLFW_KEY_EQUAL ||
		key == GLFW_KEY_RIGHT ||
		key == GLFW_KEY_LEFT ||
		key == GLFW_KEY_UP ||
		key == GLFW_KEY_DOWN ||
		key == GLFW_KEY_BACKSPACE;
}

static int Shifted(int c, bool capslock)
{
	// todo: do this properly for other keys besides 0-9
	if(c >= GLFW_KEY_0 && c <= GLFW_KEY_9)
	{
		int LUT[10] =
		{
			')',
			'!',
			'"',
			'$',//'£', // £ causes warning, multi character
			'$',
			'%',
			'^',
			'&',
			'*',
			'(',
		};
		return LUT[c - GLFW_KEY_0];
	}
	else if(c >= GLFW_KEY_A && c <= GLFW_KEY_Z)
	{
		if(capslock)
		{
			return tolower(c);	
		}
		else
		{
			return c;
		}
	}
	else
	{
		return c;
	}
	
}

static void DoTextInput(InputContext* context, int key, int scancode, int action, int mods)
{

	int ascii = 0;

	if(key == GLFW_KEY_CAPS_LOCK)
	{
		switch(action)
		{
		case GLFW_PRESS:
			EASSERT(!context->textInput.capslockModifier);
			context->textInput.capslockModifier = true;
			break;
		case GLFW_RELEASE:
			EASSERT(context->textInput.capslockModifier);
			context->textInput.capslockModifier = false;
			break;
		}
	}
	else if(key == GLFW_KEY_LEFT_SHIFT)
	{
		switch(action)
		{
		case GLFW_PRESS:
			EASSERT(!context->textInput.shiftModifier);
			context->textInput.shiftModifier = true;
			break;
		case GLFW_RELEASE:
			EASSERT(context->textInput.shiftModifier);
			context->textInput.shiftModifier = false;
			break;
		}
	}
	else if (action == GLFW_PRESS)
	{
		if(context->textInput.shiftModifier)
		{
			ascii = Shifted((char)key, context->textInput.capslockModifier);
		}
		else if(!context->textInput.capslockModifier && (key >= GLFW_KEY_A && key <= GLFW_KEY_Z))
		{
			ascii = tolower(key);
		}
		else
		{
			ascii = key;
		}
		context->textInput.keystrokes[context->textInput.nKeystrokesThisFrame++] = ascii;
	}
	
}

void In_RecieveKeyboardKey(InputContext* context, int key, int scancode, int action, int mods)
{
	DoTextInput(context, key, scancode, action, mods);
	for (int i = 0; i < context->buttonMappings.KeyboardButtonMappings.size; i++)
	{
		InputMapping* pMapping = &context->buttonMappings.KeyboardButtonMappings.arr[i];
		if (context->buttonMappings.KeyboardButtonMappings.ActiveMask & (1 << i))
		{
			if (pMapping->data.ButtonMapping.data.keyboard.keyboadCode == key)
			{
				if (action == GLFW_PRESS)
				{
					pMapping->data.ButtonMapping.bCurrent = true;
				}
				else if (action == GLFW_RELEASE)
				{
					pMapping->data.ButtonMapping.bCurrent = false;
				}
			}
		}
	}
}

void In_RecieveMouseMove(InputContext* context, double xposIn, double yposIn)
{
	for (int i = 0; i < context->axisMappings.Mouse.size; i++)
	{
		InputMapping* pMapping = &context->axisMappings.Mouse.arr[i];
		if (context->axisMappings.Mouse.ActiveMask & (1 << i))
		{
			if (pMapping->data.axisMapping.data.mouse.axis == Axis_X)
			{
				pMapping->data.axisMapping.fCurrent = xposIn;
			}
			else if (pMapping->data.axisMapping.data.mouse.axis == Axis_Y)
			{
				pMapping->data.axisMapping.fCurrent = yposIn;
			}
		}
	}
}

void In_RecieveMouseButton(InputContext* context, int button, int action, int mods)
{
	for (int i = 0; i < context->buttonMappings.MouseButtonMappings.size; i++)
	{
		InputMapping* pMapping = &context->buttonMappings.MouseButtonMappings.arr[i];
		if (context->buttonMappings.MouseButtonMappings.ActiveMask & (1 << i))
		{
			if (pMapping->data.ButtonMapping.data.mouseBtn.button == button)
			{
				if (action == GLFW_PRESS)
				{
					static int dCounter = 0;
					printf("glfw mouse press %i \n", dCounter++);
					pMapping->data.ButtonMapping.bCurrent = true;
				}
				else if (action == GLFW_RELEASE)
				{
					pMapping->data.ButtonMapping.bCurrent = false;
				}
			}
		}
	}
}

void In_FramebufferResize(InputContext* context, int width, int height)
{
	context->screenH = height;
	context->screenW = width;
}

void In_RecieveScroll(InputContext* context, double xoffset, double yoffset)
{
	for (int i = 0; i < context->axisMappings.MouseScroll.size; i++)
	{
		InputMapping* pMapping = &context->axisMappings.MouseScroll.arr[i];
		if (context->axisMappings.MouseScroll.ActiveMask & (1 << i))
		{
			if (pMapping->data.axisMapping.data.mouseScroll.axis == Axis_X)
			{
				pMapping->data.axisMapping.fCurrent = xoffset;
			}
			else if (pMapping->data.axisMapping.data.mouseScroll.axis == Axis_Y)
			{
				pMapping->data.axisMapping.fCurrent = yoffset;
			}
		}
	}
}

void In_SetControllerPresent(int controllerNo)
{
	gJoystick = controllerNo;
}

void In_EndFrame(InputContext* context)
{
	context->textInput.nKeystrokesThisFrame = 0;
}

typedef void(*SetButtonCodeCallback)(InputMapping*, int);

static void AddChildButtonStructs(cJSON* parent, InputMappingArray* outMappings, SetButtonCodeCallback callback, ButtonSubType btnSubType)
{
	while (parent)
	{
		const char* name = parent->child->string;
		int val = parent->child->valueint;
		InputMapping mapping;
		size_t  allocSize = strlen(name) + 1;
		mapping.name = malloc(allocSize);
		memset(mapping.name, 0, allocSize);
		strcpy(mapping.name, name);
		mapping.type = Button;
		mapping.data.ButtonMapping.type = btnSubType;
		mapping.data.ButtonMapping.bCurrent = false;
		callback(&mapping, val);
		outMappings->arr[outMappings->size++] = mapping;
		parent = parent->next;
	}
}

static void SetKeyboardCode(InputMapping* mapping, int code) { mapping->data.ButtonMapping.data.keyboard.keyboadCode = code; }
static void SetMouseBtnCode(InputMapping* mapping, int code) { mapping->data.ButtonMapping.data.mouseBtn.button = code; }
static void SetGamepadBtnCode(InputMapping* mapping, int code) { mapping->data.ButtonMapping.data.gamepadBtn.button = code; }

static void AddMouseAxisMappingsStructs(cJSON* parent, InputContext* ctx)
{
	while (parent)
	{
		InputMapping mapping;
		memset(&mapping, 0, sizeof(InputMapping));

		cJSON* child = parent->child;
		const char* name = child->string;
		mapping.name = malloc(strlen(name) + 1);
		strcpy(mapping.name, name);
		mapping.type = Axis;
		//strcpy(mapping.name, name);
		mapping.data.axisMapping.type = MouseAxis;
		cJSON* props = child->child;
		while (props)
		{
			if (strcmp("axis", props->string) == 0)
			{
				if (strcmp("x", props->valuestring) == 0)
				{
					mapping.data.axisMapping.data.mouse.axis = Axis_X;
				}
				else if (strcmp("y", props->valuestring) == 0)
				{
					mapping.data.axisMapping.data.mouse.axis = Axis_Y;
				}
			}
			props = props->next;
		}
		parent = parent->next;
		ctx->axisMappings.Mouse.arr[ctx->axisMappings.Mouse.size++] = mapping;
	}
}

static void AddScrollAxisMappingsStructs(cJSON* parent, InputContext* ctx)
{
	while (parent)
	{
		InputMapping mapping;
		memset(&mapping, 0, sizeof(InputMapping));

		cJSON* child = parent->child;
		const char* name = child->string;
		mapping.name = malloc(strlen(name) + 1);
		strcpy(mapping.name, name);
		mapping.type = Axis;
		//strcpy(mapping.name, name);
		mapping.data.axisMapping.type = MouseScrollAxis;
		cJSON* props = child->child;
		while (props)
		{
			if (strcmp("axis", props->string) == 0)
			{
				if (strcmp("x", props->valuestring) == 0)
				{
					mapping.data.axisMapping.data.mouseScroll.axis = Axis_X;
				}
				else if (strcmp("y", props->valuestring) == 0)
				{
					mapping.data.axisMapping.data.mouseScroll.axis = Axis_Y;
				}
			}
			props = props->next;
		}
		parent = parent->next;
		ctx->axisMappings.MouseScroll.arr[ctx->axisMappings.MouseScroll.size++] = mapping;
	}
}

static void AddGamepadAxisMappingsStructs(cJSON* parent, InputContext* ctx)
{
	while (parent)
	{
		InputMapping mapping;
		memset(&mapping, 0, sizeof(InputMapping));

		cJSON* child = parent->child;
		const char* name = child->string;
		mapping.name = malloc(strlen(name) + 1);
		strcpy(mapping.name, name);
		mapping.type = Axis;
		//strcpy(mapping.name, name);
		mapping.data.axisMapping.type = GamePadAxis;
		cJSON* props = child->child;
		while (props)
		{
			if (strcmp("axis", props->string) == 0)
			{
				if (strcmp("x", props->valuestring) == 0)
				{
					mapping.data.axisMapping.data.controller.axis = Axis_X;
				}
				else if (strcmp("y", props->valuestring) == 0)
				{
					mapping.data.axisMapping.data.controller.axis = Axis_Y;
				}
			}
			else if (strcmp("stick", props->string) == 0)
			{
				if (strcmp("lstick", props->valuestring) == 0)
				{
					mapping.data.axisMapping.data.controller.type = gpAxis_LStick;
				}
				else if (strcmp("rstick", props->valuestring) == 0)
				{
					mapping.data.axisMapping.data.controller.type = gpAxis_RStick;
				}
				else if (strcmp("lTrigger", props->valuestring) == 0)
				{
					mapping.data.axisMapping.data.controller.type = gpAxis_LT;
				}
				else if (strcmp("rTrigger", props->valuestring) == 0)
				{
					mapping.data.axisMapping.data.controller.type = gpAxis_RT;
				}

			}
			props = props->next;
		}
		parent = parent->next;
		ctx->axisMappings.Controller.arr[ctx->axisMappings.Controller.size++] = mapping;
	}
}

InputContext In_InitInputContext()
{
#define ERROR(test, str) if(!test) {printf("In_InitInputContext %s", str); InputContext v; memset(&v, 0, sizeof(InputContext)); return v;} 
	InputContext ctx;
	memset(&ctx, 0, sizeof(InputContext));

	int size = 0;
	char* data = LoadFile("./Assets/Keymap.json", &size);
	ERROR(data, "can't load data");
	cJSON* json = cJSON_ParseWithLength(data, size);
	ERROR(json, "can't parse json");
	cJSON* buttons = cJSON_GetObjectItem(json, "Buttons");
	ERROR(buttons, "json child 'Buttons' not found.");


	cJSON* keyboard = cJSON_GetObjectItem(buttons, "Keyboard");
	ERROR(keyboard, "json child 'Buttons.Keyboard' not found.");
	cJSON* keyboardKeyMap = keyboard->child;
	AddChildButtonStructs(keyboardKeyMap, &ctx.buttonMappings.KeyboardButtonMappings, &SetKeyboardCode, KeyboardButton);

	cJSON* mouseBtn = cJSON_GetObjectItem(buttons, "Mouse");
	ERROR(mouseBtn, "mouseBtn");
	cJSON* mouseBtnMap = mouseBtn->child;
	AddChildButtonStructs(mouseBtnMap, &ctx.buttonMappings.MouseButtonMappings, &SetMouseBtnCode, MouseButton);
	
	cJSON* gamepadBtn = cJSON_GetObjectItem(buttons, "GamePad");
	ERROR(gamepadBtn, "GamePad");
	cJSON* gamepadBtnMap = gamepadBtn->child;
	AddChildButtonStructs(gamepadBtnMap, &ctx.buttonMappings.GamepadMappings, &SetGamepadBtnCode, GamepadButton);

	cJSON* axes = cJSON_GetObjectItem(json, "Axes");
	ERROR(axes, "json child 'Axes' not found.");

	cJSON* mouseAxes = cJSON_GetObjectItem(axes, "Mouse");
	ERROR(mouseAxes, "json child 'Axes.Mouse' not found.");
	cJSON* parent = mouseAxes->child;
	AddMouseAxisMappingsStructs(parent, &ctx);

	cJSON* gamepadAxis = cJSON_GetObjectItem(axes, "GamePad");
	ERROR(gamepadAxis, "json child 'Axes.GamePad' not found.");
	parent = gamepadAxis->child;
	AddGamepadAxisMappingsStructs(parent, &ctx);

	cJSON* mouseScroll = cJSON_GetObjectItem(axes, "MouseScroll");
	ERROR(mouseScroll, "json child 'Axes.MouseScroll' not found.");
	parent = mouseScroll->child;
	AddScrollAxisMappingsStructs(parent, &ctx);

	// HACK BEGIN - to be done some other better way in future
	ctx.axisMappings.Mouse.ActiveMask |= 1;
	ctx.axisMappings.Mouse.ActiveMask |= 2;
	ctx.buttonMappings.MouseButtonMappings.ActiveMask |= 3;
	// HACK END

	cJSON_Delete(json);
	free(data);
	return ctx;

#undef ERROR
}
