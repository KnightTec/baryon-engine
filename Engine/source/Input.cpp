#include "Input.h"

using namespace Baryon;

std::pair<float, std::vector<void(*)(float)>> Input::inputCallbacks[10];

void Input::bindFunctionToInput(void(* function)(float), TYPE type)
{
	inputCallbacks[type].second.push_back(function);
}

void Input::handleOSinput(int virtualKeyCode, float value)
{
	//TODO: handle WM_INPUT

	switch (virtualKeyCode)
	{
	case VK_LEFT:
		inputCallbacks[KEYBOARD_ARROW_LEFT].first = value;
		break;
	case VK_RIGHT:
		inputCallbacks[KEYBOARD_ARROW_RIGHT].first = value;
		break;
	case VK_UP:
		inputCallbacks[KEYBOARD_ARROW_UP].first = value;
		break;
	case VK_DOWN:
		inputCallbacks[KEYBOARD_ARROW_DOWN].first = value;
		break;
	case VK_SPACE:
		//TODO: uncomment this line
		//inputCallbacks[KEYBOARD_SPACE].first = value;
		if (value == 1) {
			inputCallbacks[KEYBOARD_SPACE].second.at(0)(0);
		}
		break;
	case VK_RETURN:
		//inputCallbacks[KEYBOARD_ENTER].first = value;
		if (value == 1) {
			inputCallbacks[KEYBOARD_ENTER].second.at(0)(0);
		}
		break;
	}
}

void Input::handleGameInput()
{
	for (auto& inputCallback : inputCallbacks)
	{
		if (inputCallback.first)
		{
			for (void(*callback)(float) : inputCallback.second)
			{
				callback(inputCallback.first);
			}
		}
	}
}
