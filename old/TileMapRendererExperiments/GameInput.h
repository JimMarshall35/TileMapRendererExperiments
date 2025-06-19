#pragma once
#include "BasicTypedefs.h"

enum class GameInputType {
	None,
	DirectionInput,
	MouseButton,
	MouseMove,
	MouseScrollWheel,
	CtrlHeldKeyPress
};

struct DirectionInput {
	Directions directions;
};


enum class MouseButton {
	None,
	Left,
	Right
};

enum class MouseButtonAction {
	None,
	Press,
	Release
};

struct MouseButtonInput {
	MouseButton button;
	MouseButtonAction action;
};

struct MouseMoveInput {
	double xposIn;
	double yposIn;
};

struct MouseScrollWheelInput {
	double offset;
};

struct CtrlHeldKeyPressInput {
	char keyPressed;
};

struct GameInput {
	GameInputType type;
	union {
		DirectionInput direction;
		MouseButtonInput mouseButton;
		MouseMoveInput mouseMove;
		MouseScrollWheelInput mouseScrollWheel;
		CtrlHeldKeyPressInput ctrlKeyCombo;
	}data;
};