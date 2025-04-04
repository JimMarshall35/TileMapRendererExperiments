#ifndef INPUTCONTEXT_H
#define INPUTCONTEXT_H

#include "DynArray.h"

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
	GamePadAxis
}AxisSubType;

typedef enum
{
	gpAxis_LStick,
	gpAxis_RStick,
	gpAxis_LT,
	gpAxis_RT
}ControllerAxisType;

typedef struct
{
	InputMappingType type;
	const char* name;
	union
	{
		struct
		{
			ButtonSubType type;
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
			}data;
			
			// in future, game pad for example
		}ButtonMapping;
		struct
		{

			AxisSubType type;
			union
			{
				struct
				{
					double x;
					double y;
				}mouse;
				struct
				{
					ControllerAxisType type;
					double x;
					double y;
				}controller;
			}data;
			
		}AxisMapping;
	}mapping;
}InputMapping;

#define MAX_MAPPINGS 64

typedef struct
{
	InputMapping AxisMappings[MAX_MAPPINGS];
	InputMapping ButtonMapping[MAX_MAPPINGS];
	u64 ActiveButtons;
	u64 ActiveAxes;
	u32 NumButtonMappings;
	u32 NumAxisMappings;
}InputContext;

void In_RecieveKeyboardKey(InputContext* context, int key, int scancode, int action, int mods);
void In_RecieveMouseMove(InputContext* context, double xposIn, double yposIn);
void In_RecieveMouseButton(InputContext* context, int button, int action, int mods);
void In_FramebufferResize(InputContext* context, int width, int height);
void In_RecieveScroll(InputContext* context, double xoffset, double yoffset);
void In_SetControllerPresent(int controllerNo);
void In_Poll();
InputContext In_InitInputContext();

#endif // !INPUTCONTEXT_H
