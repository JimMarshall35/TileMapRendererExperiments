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
	KeyboardButton
}ButtonSubType;

typedef enum
{
	Mouse
}AxisSubType6;

typedef struct
{
	InputMappingType type;
	const char* name;
	union
	{
		struct
		{
			struct
			{
				int keyboadCode;
			}keyboard;
			// in future, game pad for example
		}ButtonMapping;
		struct
		{
			struct
			{
				double x;
				double y;
			}mouse;
		}AxisMapping;
	}mapping;
}InputMapping;

typedef struct
{
	InputMapping* AxisMappings;
	InputMapping* ButtonMapping;
}InputContext;

void In_RecieveKeyboardKey(InputContext* context, int key, int scancode, int action, int mods);
void In_RecieveMouseMove(InputContext* context, double xposIn, double yposIn);
void In_RecieveMouseButton(InputContext* context, int button, int action, int mods);
void In_FramebufferResize(InputContext* context, int width, int height);
void In_RecieveScroll(InputContext* context, double xoffset, double yoffset);

InputContext In_InitInputContext();

#endif // !INPUTCONTEXT_H
