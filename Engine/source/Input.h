#pragma once
#include "windows.h"
#include <utility>
#include <vector>

namespace Baryon
{
/*
 * Handles Input
 */
class Input
{
public:
	enum TYPE
	{
		KEYBOARD_ARROW_LEFT,
		KEYBOARD_ARROW_RIGHT,
		KEYBOARD_ARROW_UP,
		KEYBOARD_ARROW_DOWN,
		KEYBOARD_SPACE,
		KEYBOARD_ENTER,
		MOUSE_X,
		MOUSE_Y,
		MOUSE_LEFT_BUTTON,
		MOUSE_RIGHT_BUTTON
	};

	static void bindFunctionToInput(void(*function)(float), TYPE type);
	static void handleOSinput(int virtualKeyCode, bool pressed);
	static void handleGameInput();
private:
	static std::pair<bool, std::vector<void(*)(float)>> inputCallbacks[10];
};
}
