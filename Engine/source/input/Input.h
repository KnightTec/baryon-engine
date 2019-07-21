#pragma once
#include "windows.h"

#include "HID.h"

#include <unordered_map>
#include <vector>


namespace Baryon
{
/*
 * Handles Input
 */
class Input
{
public:
	//TODO: Input Action

	class Axis
	{
		friend class Input;
	public:
		void setInputBinding(KEYBOARD_INPUT inputId, float scale);
		void setInputBinding(MOUSE_INPUT inputId, float scale);
		void setInputBinding(CONTROLLER_INPUT inputId, float scale);
		void removeInputBinding(KEYBOARD_INPUT inputId);
		void removeInputBinding(MOUSE_INPUT inputId);
		void removeInputBinding(CONTROLLER_INPUT inputId);
	private:
		void handleInput();

		float axisValue = 0;
		std::vector<void(*)(float)> callbacks;
		std::vector<std::pair<KEYBOARD_INPUT, float>> keyboardBindings;
		std::vector<std::pair<MOUSE_INPUT, float>> mouseBindings;
		std::vector<std::pair<CONTROLLER_INPUT, float>> controllerBindings;
	};

	enum TYPE
	{
		KEYBOARD_SPACE,
		KEYBOARD_ENTER,
	};

	static void initialize();
	static void bindFunctionToInput(void (*function)(float), TYPE type);

	static Axis& addAxis(const std::string& axisName);

	/*
	 * returns false if axis does not exist
	 */
	static bool bindAxis(const std::string& axisName, void (*function)(float));
	static void processOSInput(WPARAM wParam, LPARAM lParam);
	static void handleGameInput();

private:
	static void handleKeyboard(RAWKEYBOARD* data);
	static void handleMouse(RAWMOUSE* data);
	static void handleController(RAWHID* data);
	
	static std::pair<float, std::vector<void(*)(float)>> inputCallbacks[10];

	static Keyboard keyboard;
	static Mouse mouse;
	static Controller controller;
	static std::unordered_map<std::string, Axis> inputAxes;
};

inline Input::Axis& Input::addAxis(const std::string& axisName)
{
	return inputAxes[axisName];
}
}
