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
	
	static void initialize();
	static void bindFunctionToInput(void(*function)(float), TYPE type);

	static void handleOSInput(WPARAM wParam, LPARAM lParam);
	static void handleOSInputOld(int virtualKeyCode, float value);

	static void handleGameInput();
private:
	static std::pair<float, std::vector<void(*)(float)>> inputCallbacks[10];

	// TODO: add following callback types: InputAction, InputAxis1D, InputAxis2D
};
}
