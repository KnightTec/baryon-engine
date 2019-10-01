#pragma once
#include "VirtualInputDevice.h"

#include "windows.h"

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
	class Axis
	{
		friend class Input;
	public:
		void setInputBinding(KEYBOARD_INPUT inputId, float scale);
		void setInputBinding(MOUSE_INPUT inputId, float scale);
		void setInputBinding(GAMEPAD_INPUT inputId, float scale);
		void removeInputBinding(KEYBOARD_INPUT inputId);
		void removeInputBinding(MOUSE_INPUT inputId);
		void removeInputBinding(GAMEPAD_INPUT inputId);
	private:
		void handleInput();

		std::vector<void(*)(float)> callbacks;
		std::vector<std::pair<KEYBOARD_INPUT, float>> keyboardBindings;
		std::vector<std::pair<MOUSE_INPUT, float>> mouseBindings;
		std::vector<std::pair<GAMEPAD_INPUT, float>> gamepadBindings;
	};

	class Action
	{
		friend class Input;
	public:
		enum TYPE
		{
			PRESSED,
			RELEASED
		};
		void setInputBinding(KEYBOARD_INPUT inputId);
		void setInputBinding(MOUSE_INPUT inputId);
		void setInputBinding(GAMEPAD_INPUT inputId);
	private:
		void handleInput();

		std::vector<std::pair<void(*)(), TYPE>> callbacks;
		std::vector<KEYBOARD_INPUT> keyboardBindings;
		std::vector<MOUSE_INPUT> mouseBindings;
		std::vector<GAMEPAD_INPUT> gamepadBindings;
	};

	static void initialize();
	static Axis& addAxis(const std::string& axisName);
	static Action& addAction(const std::string& actionName);

	/*
	 * returns false if axis/action does not exist
	 */
	static bool bindAxis(const std::string& axisName, void (*function)(float));
	static bool bindAction(const std::string& actionName, Action::TYPE type, void (*function)());

	static void passOSData(WPARAM wParam, LPARAM lParam);
	static void handleGameInput();
private:
	static void processKeyboard(RAWKEYBOARD* data);
	static void processMouse(RAWMOUSE* data);
	static void processXInput();
	static void processGenericGamepad(RAWINPUT* raw);

	static Keyboard keyboard;
	static Mouse mouse;
	static Gamepad gamepad;
	static std::unordered_map<std::string, Axis> inputAxes;
	static std::unordered_map<std::string, Action> inputActions;
};


inline Input::Axis& Input::addAxis(const std::string& axisName)
{
	return inputAxes[axisName];
}
inline Input::Action& Input::addAction(const std::string& actionName)
{
	return inputActions[actionName];
}
}
