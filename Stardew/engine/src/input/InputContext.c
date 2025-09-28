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

int In_FindMouseButtonMapping(InputContext* context, const char* name)
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

int In_FindKeyboardButtonMapping(InputContext* context, const char* name)
{
	for (int i = 0; i < context->buttonMappings.KeyboardButtonMappings.size; i++)
	{
		if (strcmp(context->buttonMappings.KeyboardButtonMappings.arr[i].name, name) == 0)
		{
			return i;
		}
	}
	return NULL_HANDLE;
}

int In_FindGamepadButtonMapping(InputContext* context, const char* name)
{
	for (int i = 0; i < context->buttonMappings.GamepadMappings.size; i++)
	{
		if (strcmp(context->buttonMappings.GamepadMappings.arr[i].name, name) == 0)
		{
			return i;
		}
	}
	return NULL_HANDLE;
}

int In_FindMouseScrollButtonMapping(InputContext* context, const char* name)
{
	for (int i = 0; i < context->buttonMappings.MouseScrollButtonMappings.size; i++)
	{
		if (strcmp(context->buttonMappings.MouseScrollButtonMappings.arr[i].name, name) == 0)
		{
			return i;
		}
	}
	return NULL_HANDLE;
}

int In_FindMouseAxisMapping(InputContext* context, const char* name)
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

int In_FindControllerAxisMapping(InputContext* context, const char* name)
{
	for (int i = 0; i < context->axisMappings.Controller.size; i++)
	{
		if (strcmp(context->axisMappings.Controller.arr[i].name, name) == 0)
		{
			return i;
		}
	}
	return NULL_HANDLE;
}

int In_FindMouseScrollAxisMapping(InputContext* context, const char* name)
{
	for (int i = 0; i < context->axisMappings.MouseScroll.size; i++)
	{
		if (strcmp(context->axisMappings.MouseScroll.arr[i].name, name) == 0)
		{
			return i;
		}
	}
	return NULL_HANDLE;
}


struct AxisBinding In_FindAxisMapping(InputContext* context, const char* name)
{
	int handle = In_FindMouseAxisMapping(context, name);
	if (handle != NULL_HANDLE)
	{
		struct AxisBinding b = { MouseAxis, handle };
		return b;
	}
	handle = In_FindControllerAxisMapping(context, name);
	if (handle != NULL_HANDLE)
	{
		struct AxisBinding b = { GamePadAxis, handle };
		return b;
	}
	handle = In_FindMouseScrollAxisMapping(context, name);
	if (handle != NULL_HANDLE)
	{
		struct AxisBinding b = { MouseScrollAxis, handle };
		return b;
	}
	struct AxisBinding b = { UnknownAxis, NULL_HANDLE };
	return b;
}

struct ButtonBinding In_FindButtonMapping(InputContext* context, const char* name)
{
	int handle = In_FindMouseButtonMapping(context, name);
	if (handle != NULL_HANDLE)
	{
		struct ButtonBinding b = { MouseButton, handle };
		return b;
	}
	handle = In_FindGamepadButtonMapping(context, name);
	if (handle != NULL_HANDLE)
	{
		struct ButtonBinding b = { GamepadButton, handle };
		return b;
	}
	handle = In_FindKeyboardButtonMapping(context, name);
	if (handle != NULL_HANDLE)
	{
		struct ButtonBinding b = { KeyboardButton, handle };
		return b;
	}
	handle = In_FindMouseScrollButtonMapping(context, name);
	if (handle != NULL_HANDLE)
	{
		struct ButtonBinding b = { MouseScrollButton, handle };
		return b;
	}
	struct ButtonBinding b = { UnknownButton, NULL_HANDLE };
	return b;
}

float In_GetAxisValue(InputContext* context, struct AxisBinding binding)
{
	EASSERT(binding.index > -1);
	EASSERT(binding.type != UnknownAxis);
	switch (binding.type)
	{
	case MouseAxis:
		return context->axisMappings.Mouse.arr[binding.index].data.axisMapping.fCurrent;
		break;
	case GamePadAxis:
		return context->axisMappings.Controller.arr[binding.index].data.axisMapping.fCurrent;
	case MouseScrollAxis:
		return context->axisMappings.MouseScroll.arr[binding.index].data.axisMapping.fCurrent;
	}
	return 0.0f;
}

bool In_GetButtonValue(InputContext* context, struct ButtonBinding binding)
{
	EASSERT(binding.index > -1);
	EASSERT(binding.type != UnknownButton);
	switch (binding.type)
	{
	case MouseButton:
		return context->buttonMappings.MouseButtonMappings.arr[binding.index].data.ButtonMapping.bCurrent;
	case KeyboardButton:
		return context->buttonMappings.KeyboardButtonMappings.arr[binding.index].data.ButtonMapping.bCurrent;
	case GamepadButton:
		return context->buttonMappings.GamepadMappings.arr[binding.index].data.ButtonMapping.bCurrent;
	case MouseScrollButton:
		return context->buttonMappings.MouseScrollButtonMappings.arr[binding.index].data.ButtonMapping.bCurrent;
	}
	return false;
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
	for (int i = 0; i < context->buttonMappings.MouseScrollButtonMappings.size; i++)
	{
		InputMapping* pMapping = &context->buttonMappings.MouseScrollButtonMappings.arr[i];
		if (context->buttonMappings.MouseScrollButtonMappings.ActiveMask & (1 << i))
		{
			switch (pMapping->data.ButtonMapping.data.mouseScrollButton.axis)
			{
			case Axis_X:
				if (pMapping->data.ButtonMapping.data.mouseScrollButton.dir == Axis_Pos && xoffset > 0)
				{
					pMapping->data.ButtonMapping.bCurrent = true;
				}
				if (xoffset < 0)
				{
					pMapping->data.ButtonMapping.bCurrent = true;
				}
				break;
			case Axis_Y:
				if (pMapping->data.ButtonMapping.data.mouseScrollButton.dir == Axis_Pos && yoffset > 0)
				{
					pMapping->data.ButtonMapping.bCurrent = true;
				}
				if (yoffset < 0)
				{
					pMapping->data.ButtonMapping.bCurrent = true;
				}
				break;
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

	/*
		Reset these - mappings where the mouse scroll wheel is behaving as a button.
		Unlike other buttons this one isn't pressed and released, its an instantaneous event that should be handled once
	*/
	for (int i = 0; i < context->buttonMappings.MouseScrollButtonMappings.size; i++)
	{
		InputMapping* pMapping = &context->buttonMappings.MouseScrollButtonMappings.arr[i];
		pMapping->data.ButtonMapping.bCurrent = false;
	}
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

static void SetMouseScrollBtnCode(InputMapping* mapping, int code) 
{
	/*
		X Axis + : 3
		X Axis - : 1
		Y Axis + : 2
		Y Axis - : 0
	*/
	if (code & 1)
	{
		mapping->data.ButtonMapping.data.mouseScrollButton.axis = Axis_X;
	}
	else
	{
		mapping->data.ButtonMapping.data.mouseScrollButton.axis = Axis_Y;
	}

	if (code & 2)
	{
		mapping->data.ButtonMapping.data.mouseScrollButton.dir = Axis_Pos;
	}
	else
	{
		mapping->data.ButtonMapping.data.mouseScrollButton.dir = Axis_Neg;
	}
}

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

	cJSON* mouseScrollBtn = cJSON_GetObjectItem(buttons, "MouseScroll");
	ERROR(mouseScrollBtn, "MouseScroll");
	cJSON* mouseScrollBtnMap = mouseScrollBtn->child;
	AddChildButtonStructs(mouseScrollBtnMap, &ctx.buttonMappings.MouseScrollButtonMappings, &SetMouseScrollBtnCode, MouseScrollButton);
	
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


void In_GetMask(struct ActiveInputBindingsMask* pOutMask, InputContext* pCtx)
{
	pOutMask->MouseButtonMappings = pCtx->buttonMappings.MouseButtonMappings.ActiveMask;
	pOutMask->KeyboardButtonMappings = pCtx->buttonMappings.KeyboardButtonMappings.ActiveMask;
	pOutMask->GamepadButtonMappings = pCtx->buttonMappings.GamepadMappings.ActiveMask;
	pOutMask->MouseScrollButtonMappings = pCtx->buttonMappings.MouseScrollButtonMappings.ActiveMask;

	pOutMask->MouseAxisMappings = pCtx->axisMappings.Mouse.ActiveMask;
	pOutMask->ControllerAxisMappings = pCtx->axisMappings.Controller.ActiveMask;
	pOutMask->MouseScrollAxisMappings = pCtx->axisMappings.MouseScroll.ActiveMask;
}

void In_SetMask(struct ActiveInputBindingsMask* mask, InputContext* pCtx)
{
	pCtx->buttonMappings.MouseButtonMappings.ActiveMask = mask->MouseButtonMappings;
	pCtx->buttonMappings.KeyboardButtonMappings.ActiveMask = mask->KeyboardButtonMappings;
	pCtx->buttonMappings.GamepadMappings.ActiveMask = mask->GamepadButtonMappings;
	pCtx->buttonMappings.MouseScrollButtonMappings.ActiveMask = mask->MouseScrollButtonMappings;

	pCtx->axisMappings.Mouse.ActiveMask = mask->MouseAxisMappings;
	pCtx->axisMappings.Controller.ActiveMask = mask->ControllerAxisMappings;
	pCtx->axisMappings.MouseScroll.ActiveMask = mask->MouseScrollAxisMappings;
}

void In_ActivateButtonBinding(struct ButtonBinding binding, struct ActiveInputBindingsMask* pMask)
{
	EASSERT(binding.index <= 63);
	switch (binding.type)
	{
	case MouseButton:
		pMask->MouseButtonMappings |= (1 << binding.index);
		break;
	case KeyboardButton:
		pMask->KeyboardButtonMappings |= (1 << binding.index);
		break;
	case GamepadButton:
		pMask->GamepadButtonMappings |= (1 << binding.index);
		break;
	case MouseScrollButton:
		pMask->MouseScrollButtonMappings |= (1 << binding.index);
		break;
	}
}

void In_ActivateAxisBinding(struct AxisBinding binding, struct ActiveInputBindingsMask* pMask)
{
	EASSERT(binding.index <= 63);
	switch (binding.type)
	{
	case MouseAxis:
		pMask->MouseAxisMappings |= (1 << binding.index);
		break;
	case GamePadAxis:
		pMask->ControllerAxisMappings |= (1 << binding.index);
		break;
	case MouseScrollAxis:
		pMask->MouseScrollAxisMappings |= (1 << binding.index);
		break;
	}
}

