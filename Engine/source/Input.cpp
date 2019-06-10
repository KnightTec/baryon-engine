#include "Input.h"
#include <string>
#include <iomanip>
#include <sstream>

/*
 * Usage tables source: https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
 */

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif
#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD     ((USHORT) 0x06)
#endif
#ifndef HID_USAGE_GENERIC_GAMEPAD
#define HID_USAGE_GENERIC_GAMEPAD      ((USHORT) 0x04)
#endif

#ifndef HID_USAGE_PAGE_GAME
#define HID_USAGE_PAGE_GAME            ((USHORT) 0x05)
#endif
#ifndef HID_USAGE_GAME_GAMEPAD_TRIGGER
#define HID_USAGE_GAME_GAMEPAD_TRIGGER ((USHORT) 0x37)
#endif

#ifndef VK_W
#define VK_W	0x57
#endif
#ifndef VK_A
#define VK_A	0x41
#endif
#ifndef VK_S
#define VK_S	0x53
#endif
#ifndef VK_D
#define VK_D	0x44
#endif

using namespace Baryon;

class HIDKeyboard
{
	
};

std::pair<float, std::vector<void(*)(float)>> Input::inputCallbacks[10];


void Input::initialize()
{
	const UINT numDevices = 3;
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

	// gamepad
	rid[2].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[2].usUsage = HID_USAGE_GENERIC_GAMEPAD;
	rid[2].dwFlags = 0;
	rid[2].hwndTarget = nullptr;

	if (!RegisterRawInputDevices(rid, numDevices, sizeof(RAWINPUTDEVICE)))
	{
		OutputDebugStringA("Input device registration failed!\n");
	}
}

void Input::bindFunctionToInput(void (* function)(float), TYPE type)
{
	inputCallbacks[type].second.push_back(function);
}


void Input::handleOSInput(WPARAM wParam, LPARAM lParam)
{
	unsigned size = sizeof(RAWINPUT);
	static RAWINPUT raw;
	GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

	switch (raw.header.dwType)
	{
	case RIM_TYPEMOUSE:
		{
			// handle mouse input
			LONG xPosRelative = raw.data.mouse.lLastX;
			LONG yPosRelative = raw.data.mouse.lLastY;

			if (!inputCallbacks[MOUSE_X].second.empty())
			{
				inputCallbacks[MOUSE_X].second.at(0)(xPosRelative);
			}
			if (!inputCallbacks[MOUSE_Y].second.empty())
			{
				inputCallbacks[MOUSE_Y].second.at(0)(yPosRelative);
			}
		}
		break;
	case RIM_TYPEKEYBOARD:
		// handle keyboard input
		if (raw.data.keyboard.Flags & RI_KEY_BREAK)
		{
			Input::handleOSInputOld(raw.data.keyboard.VKey, 0);
		}
		else
		{
			Input::handleOSInputOld(raw.data.keyboard.VKey, 1);
		}
		break;
	case RIM_TYPEHID:
		{
			UINT bufSize = raw.data.hid.dwCount * raw.data.hid.dwSizeHid;
			BYTE* buffer = raw.data.hid.bRawData;

			//OutputDebugStringA("HID input detected: \n");
			std::stringstream ss;
			for (int i = 0; i < bufSize; i++)
			{
				ss << std::setfill('0') << std::setw(2) << std::hex << int(buffer[i]) << ' ';
			}

			//TODO: clean this mess up
			float xRate = (float(buffer[3]) - 127.5) / 32.0f;
			float deadZone = 0.5;
			if (abs(xRate) < deadZone)
			{
				xRate = 0;
			}
			float yRate = (float(buffer[4]) - 127.5) / 32.0f;
			if (abs(yRate) < deadZone)
			{
				yRate = 0;
			}
			inputCallbacks[MOUSE_X].first = xRate;
			inputCallbacks[MOUSE_Y].first = yRate;

			OutputDebugStringA((ss.str() + "\n").c_str());
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
	case VK_A:
		inputCallbacks[KEYBOARD_ARROW_LEFT].first = value;
		break;
	case VK_RIGHT:
	case VK_D:
		inputCallbacks[KEYBOARD_ARROW_RIGHT].first = value;
		break;
	case VK_UP:
	case VK_W:
		inputCallbacks[KEYBOARD_ARROW_UP].first = value;
		break;
	case VK_DOWN:
	case VK_S:
		inputCallbacks[KEYBOARD_ARROW_DOWN].first = value;
		break;
	case VK_SPACE:
		//TODO: uncomment this line
		//inputCallbacks[KEYBOARD_SPACE].first = value;
		if (value == 1)
		{
			inputCallbacks[KEYBOARD_SPACE].second.at(0)(0);
		}
		break;
	case VK_RETURN:
		//inputCallbacks[KEYBOARD_ENTER].first = value;
		if (value == 1)
		{
			inputCallbacks[KEYBOARD_ENTER].second.at(0)(0);
		}
		break;
	default:
		break;
	}
}

void Input::handleGameInput()
{
	for (auto& inputCallback : inputCallbacks)
	{
		if (inputCallback.first)
		{
			for (void (*callback)(float) : inputCallback.second)
			{
				callback(inputCallback.first);
			}
		}
	}
}
