#pragma once
#include "windows.h"

#include "HID.h"

#include <unordered_map>
#include <set>
#include <vector>


namespace Baryon
{
/*
 * Handles Input
 */
class Input
{
public:
	class InputAxis
	{
	public:
		void setInputBinding(KEYBOARD_INPUT inputId, float scale)
		{
			for (auto& input : keyboardBindings)
			{
				if (inputId == input.first)
				{
					input.second = scale;
					return;
				}
			}
			keyboardBindings.emplace_back(inputId, scale);
		}
		void setInputBinding(MOUSE_INPUT inputId, float scale);
		void setInputBinding(CONTROLLER_INPUT inputId, float scale);

		void handleInput()
		{
			for (const auto& input : keyboardBindings)
			{
				axisValue += keyboard.getInputValue(input.first) * input.second;
			}
			for (const auto& input : mouseBindings)
			{
				axisValue += mouse.getInputValue(input.first) * input.second;
			}
			for (const auto& input : controllerBindings)
			{
				axisValue += controller.getInputValue(input.first) * input.second;
			}
			for (auto function : callbacks)
			{
				function(axisValue);
			}
			axisValue = 0;
		}
	private:
		float axisValue = 0;
		std::vector<void(*)(float)> callbacks;
		std::vector<std::pair<KEYBOARD_INPUT, float>> keyboardBindings;
		std::vector<std::pair<MOUSE_INPUT, float>> mouseBindings;
		std::vector<std::pair<CONTROLLER_INPUT, float>> controllerBindings;
	};


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
	static void bindFunctionToInput(void (*function)(float), TYPE type);

	static bool bindAxis(std::string axisName, void (*function)(float));
	static InputAxis& addAxis(std::string axisName);
	static InputAxis& getAxis(std::string axisName);

	static void processOSInput(WPARAM wParam, LPARAM lParam);
	static void handleGameInput();
private:
	
	static void handleKeyboard(int virtualKeyCode, float value);

	static std::pair<float, std::vector<void(*)(float)>> inputCallbacks[10];

	static Keyboard keyboard;
	static Mouse mouse;
	static Controller controller;
	//static std::unordered_map<std::string, InputAxis> inputAxes;
};
}
