#include "Input.h"
#include <string>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif
#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD     ((USHORT) 0x06)
#endif

using namespace Baryon;

std::pair<float, std::vector<void(*)(float)>> Input::inputCallbacks[10];


void Input::initialize()
{
	const UINT numDevices = 2;
	RAWINPUTDEVICE rid[numDevices];
	// keyboard
	rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[0].usUsage = HID_USAGE_GENERIC_KEYBOARD;
	rid[0].dwFlags = 0;
	rid[0].hwndTarget = nullptr;

	// mouse
	rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[1].usUsage = HID_USAGE_GENERIC_MOUSE;
	rid[1].dwFlags = 0;
	rid[1].hwndTarget = nullptr;

	if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE)))
	{
		OutputDebugStringA("Input device registration failed!\n");
	}
}

void Input::bindFunctionToInput(void(* function)(float), TYPE type)
{
	inputCallbacks[type].second.push_back(function);
}

void Input::handleOSInput(WPARAM wParam, LPARAM lParam)
{
	unsigned size = sizeof(RAWINPUT);
	static RAWINPUT raw;
	GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

	LONG xPosRelative;
	LONG yPosRelative;
	
	USHORT flags;
	switch (raw.header.dwType)
	{
	case RIM_TYPEMOUSE:

		//TODO: handle mouse input
		xPosRelative = raw.data.mouse.lLastX;
		yPosRelative = raw.data.mouse.lLastY;


		if (xPosRelative) {
			OutputDebugStringA(("Mouse x: " + std::to_string(xPosRelative) + "\n").c_str());
		}
		if (yPosRelative) {
			OutputDebugStringA(("Mouse y: " + std::to_string(yPosRelative) + "\n").c_str());
		}

		break;
	case RIM_TYPEKEYBOARD:
		if (raw.data.keyboard.Flags & RI_KEY_BREAK)
		{
			Input::handleOSInputOld(raw.data.keyboard.VKey, 0);
		}
		else
		{
			Input::handleOSInputOld(raw.data.keyboard.VKey, 1);
		}
		break;
	default:
		break;
	}
}


void Input::handleOSInputOld(int virtualKeyCode, float value)
{
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
