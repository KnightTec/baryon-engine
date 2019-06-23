#include "Input.h"
#include <string>
#include <iomanip>
#include <sstream>
#include "HID.h"


namespace
{
/*
* Usage tables source: https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
*/
struct HID_USAGE_PAGE_GENERIC
{
	static const USHORT ID = 0x01;

	enum USAGE : USHORT
	{
		MOUSE = 0x02,
		GAMEPAD = 0x04,
		KEYBOARD = 0x06
	};
};
}

using namespace Baryon;


std::pair<float, std::vector<void(*)(float)>> Input::inputCallbacks[10];
Keyboard Input::keyboard;
Mouse Input::mouse;
Controller Input::controller;

struct ControllerInputReport
{
	unsigned int : 8;
	unsigned int leftX : 8;
	unsigned int leftY : 8;
	unsigned int rightX : 8;
	unsigned int rightY: 8;

	unsigned int dPad : 4;
	unsigned int button1 : 1;
	unsigned int button2 : 1;
	unsigned int button3 : 1;
	unsigned int button4 : 1;

	unsigned int button5 : 1;
	unsigned int button6 : 1;
	unsigned int button7 : 1;
	unsigned int button8 : 1;
	unsigned int button9 : 1;
	unsigned int button10 : 1;
	unsigned int : 10;
};

void Input::initialize()
{
	const UINT numDevices = 3;
	RAWINPUTDEVICE rid[numDevices];

	// keyboard
	rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC::ID;
	rid[0].usUsage = HID_USAGE_PAGE_GENERIC::KEYBOARD;
	rid[0].dwFlags = 0;
	rid[0].hwndTarget = nullptr;

	// mouse
	rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC::ID;
	rid[1].usUsage = HID_USAGE_PAGE_GENERIC::MOUSE;
	rid[1].dwFlags = 0;
	rid[1].hwndTarget = nullptr;

	// gamepad
	rid[2].usUsagePage = HID_USAGE_PAGE_GENERIC::ID;
	rid[2].usUsage = HID_USAGE_PAGE_GENERIC::GAMEPAD;
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


void Input::processOSInput(WPARAM wParam, LPARAM lParam)
{
	unsigned size = sizeof(RAWINPUT);
	RAWINPUT raw;
	GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

	switch (raw.header.dwType)
	{
	case RIM_TYPEMOUSE:
		{
			// handle mouse input
			inputCallbacks[MOUSE_X].first += raw.data.mouse.lLastX;
			inputCallbacks[MOUSE_Y].first += raw.data.mouse.lLastY;
		}
		break;
	case RIM_TYPEKEYBOARD:
		// handle keyboard input
		if (raw.data.keyboard.Flags & RI_KEY_BREAK)
		{
			handleKeyboard(raw.data.keyboard.VKey, 0);
		}
		else
		{
			handleKeyboard(raw.data.keyboard.VKey, 1);
		}
		break;
	case RIM_TYPEHID:
		{
			UINT bufSize = raw.data.hid.dwCount * raw.data.hid.dwSizeHid;
			BYTE* buffer = raw.data.hid.bRawData;


			ControllerInputReport report = {};
			memcpy_s(&report, sizeof report, buffer, sizeof report);

			//OutputDebugStringA("HID input detected: \n");
			std::stringstream ss;
			for (int i = 0; i < bufSize; i++)
			{
				ss << std::setfill('0') << std::setw(2) << std::hex << int(buffer[i]) << ' ';
			}

			OutputDebugStringA(("D-Pad: " + std::to_string(report.dPad) + "\n").c_str());
			OutputDebugStringA(("Button1: " + std::to_string(report.button1) + "\n").c_str());
			OutputDebugStringA(("Button2: " + std::to_string(report.button2) + "\n").c_str());
			OutputDebugStringA(("Button3: " + std::to_string(report.button3) + "\n").c_str());
			OutputDebugStringA(("Button4: " + std::to_string(report.button4) + "\n").c_str());
			OutputDebugStringA(("Button5: " + std::to_string(report.button5) + "\n").c_str());
			OutputDebugStringA(("Button6: " + std::to_string(report.button6) + "\n").c_str());
			OutputDebugStringA(("Button7: " + std::to_string(report.button7) + "\n").c_str());
			OutputDebugStringA(("Button8: " + std::to_string(report.button8) + "\n").c_str());
			OutputDebugStringA(("Button9: " + std::to_string(report.button9) + "\n").c_str());
			OutputDebugStringA(("Button10: " + std::to_string(report.button10) + "\n").c_str());

			//TODO: clean this mess up
			float xRate = (report.rightX - 127.5) / 16.0f;
			float deadZone = 0.5;
			if (abs(xRate) < deadZone)
			{
				xRate = 0;
			}
			float yRate = (report.rightY - 127.5) / 16.0f;
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


void Input::handleKeyboard(int virtualKeyCode, float value)
{
	// handle character keys
	if (virtualKeyCode >= 0x41 && virtualKeyCode <= 0x5A)
	{
		keyboard.onInput(static_cast<KEYBOARD_INPUT>(virtualKeyCode - 0x41), value);
		return;
	}
	switch (virtualKeyCode)
	{
	case VK_LEFT:
		keyboard.onInput(KEYBOARD_INPUT::ARROW_LEFT, value);
		break;
	case VK_RIGHT:
		keyboard.onInput(KEYBOARD_INPUT::ARROW_RIGHT, value);
		break;
	case VK_UP:
		keyboard.onInput(KEYBOARD_INPUT::ARROW_UP, value);
		break;
	case VK_DOWN:
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
	inputCallbacks[MOUSE_X].first = 0;
	inputCallbacks[MOUSE_Y].first = 0;

	keyboard.tick();
	mouse.tick();
	controller.tick();
}
