#ifndef INPUTCONTEXT_H
#define INPUTCONTEXT_H

#include "DynArray.h"
#include <stdbool.h>
#include "HandleDefs.h"

#define KEYSTROKE_LEFT      263
#define KEYSTROKE_RIGHT     262
#define KEYSTROKE_UP        265
#define KEYSTROKE_DOWN      264
#define KEYSTROKE_BACKSPACE 259


typedef enum
{
	Axis,
	Button
}InputMappingType;

typedef enum
{
	MouseButton,
	KeyboardButton,
	GamepadButton
}ButtonSubType;

typedef enum
{
	MouseAxis,
	GamePadAxis,
	MouseScrollAxis
}AxisSubType;

typedef enum
{
	gpAxis_LStick,
	gpAxis_RStick,
	gpAxis_LT,
	gpAxis_RT
}ControllerAxisType;

typedef enum
{
	Axis_X,
	Axis_Y
} WhichAxis;
typedef struct
{
	InputMappingType type;
	char* name;
	union
	{
		struct
		{
			ButtonSubType type;
			bool bCurrent;
			union
			{
				struct
				{
					int keyboadCode;
				}keyboard;
				struct
				{
					int button;
				}mouseBtn;
				struct
				{
					int button;
				}gamepadBtn;
			}data;
			// in future, game pad for example
		}ButtonMapping;
		struct
		{

			AxisSubType type;
			double fCurrent;
			union
			{
				struct
				{
					WhichAxis axis;
				}mouse;
				struct
				{
					ControllerAxisType type;
					WhichAxis axis;
				}controller;
				struct
				{
					WhichAxis axis;
				}mouseScroll;
			}data;
			
		}axisMapping;
	}data;
}InputMapping;

#define MAX_MAPPINGS 64

typedef struct
{
	InputMapping arr[MAX_MAPPINGS];
	int size;
	u64 ActiveMask;
}InputMappingArray;

#define MAX_KEYS_DOWN_PER_FRAME 256
struct TextInputState
{
	int keystrokes[MAX_KEYS_DOWN_PER_FRAME];
	int nKeystrokesThisFrame;
	bool capslockModifier;
	bool shiftModifier;
};

typedef struct
{
	/*InputMapping AxisMappings[MAX_MAPPINGS];
	InputMapping ButtonMapping[MAX_MAPPINGS];*/
	struct
	{
		InputMappingArray MouseButtonMappings;
		InputMappingArray KeyboardButtonMappings;
		InputMappingArray GamepadMappings;
	}buttonMappings;

	struct
	{

		InputMappingArray Mouse;
		InputMappingArray Controller;
		InputMappingArray MouseScroll;
	}axisMappings;

	struct TextInputState textInput;
	int screenW, screenH;
}InputContext;

void In_RecieveKeyboardKey(InputContext* context, int key, int scancode, int action, int mods);
void In_RecieveMouseMove(InputContext* context, double xposIn, double yposIn);
void In_RecieveMouseButton(InputContext* context, int button, int action, int mods);
void In_FramebufferResize(InputContext* context, int width, int height);
void In_RecieveScroll(InputContext* context, double xoffset, double yoffset);
void In_SetControllerPresent(int controllerNo);
void In_EndFrame(InputContext* context);
InputContext In_InitInputContext();

HMouseAxisBinding In_FindMouseAxisMapping(InputContext* context, const char* name);
HMouseButtonBinding In_FindMouseBtnMapping(InputContext* context, const char* name);
float In_GetMouseAxisValue(InputContext* context, HMouseAxisBinding hBinding);
bool In_GetMouseButtonValue(InputContext* context, HMouseButtonBinding hBinding);

#endif // !INPUTCONTEXT_H
