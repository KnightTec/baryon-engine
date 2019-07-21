#include "Input.h"
#include "HID.h"

//#include "Xinput.h"
#include "hidsdi.h"

#include <string>
#include <iomanip>
#include <sstream>


using namespace Baryon;

void Input::Axis::setInputBinding(KEYBOARD_INPUT inputId, float scale)
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

void Input::Axis::setInputBinding(MOUSE_INPUT inputId, float scale)
{
	for (auto& input : mouseBindings)
	{
		if (inputId == input.first)
		{
			input.second = scale;
			return;
		}
	}
	mouseBindings.emplace_back(inputId, scale);
}

void Input::Axis::setInputBinding(CONTROLLER_INPUT inputId, float scale)
{
	for (auto& input : controllerBindings)
	{
		if (inputId == input.first)
		{
			input.second = scale;
			return;
		}
	}
	controllerBindings.emplace_back(inputId, scale);
}

void Input::Axis::handleInput()
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


namespace
{
/*
* Usage tables source: https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
*/
struct STRUCT_HID_USAGE_PAGE_GENERIC
{
	static const USHORT ID = 0x01;

	enum USAGE : USHORT
	{
		MOUSE = 0x02,
		JOYSTICK = 0x04,
		GAMEPAD = 0x05,
		KEYBOARD = 0x06
	};
};

struct GamepadInputReport
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
	unsigned int leftStick : 1;
	unsigned int rightStick: 1;
	unsigned int : 8;
};
}


std::pair<float, std::vector<void(*)(float)>> Input::inputCallbacks[10];
Keyboard Input::keyboard;
Mouse Input::mouse;
Controller Input::controller;
std::unordered_map<std::string, Input::Axis> Input::inputAxes;

void Input::initialize()
{
	const UINT numDevices = 4;
	RAWINPUTDEVICE rid[numDevices];
	rid[0].usUsagePage = STRUCT_HID_USAGE_PAGE_GENERIC::ID;
	rid[0].usUsage = STRUCT_HID_USAGE_PAGE_GENERIC::KEYBOARD;
	rid[0].dwFlags = 0;
	rid[0].hwndTarget = nullptr;

	rid[1].usUsagePage = STRUCT_HID_USAGE_PAGE_GENERIC::ID;
	rid[1].usUsage = STRUCT_HID_USAGE_PAGE_GENERIC::MOUSE;
	rid[1].dwFlags = 0;
	rid[1].hwndTarget = nullptr;

	rid[2].usUsagePage = STRUCT_HID_USAGE_PAGE_GENERIC::ID;
	rid[2].usUsage = STRUCT_HID_USAGE_PAGE_GENERIC::JOYSTICK;
	rid[2].dwFlags = 0;
	rid[2].hwndTarget = nullptr;

	rid[3].usUsagePage = STRUCT_HID_USAGE_PAGE_GENERIC::ID;
	rid[3].usUsage = STRUCT_HID_USAGE_PAGE_GENERIC::GAMEPAD;
	rid[3].dwFlags = 0;
	rid[3].hwndTarget = nullptr;


	if (!RegisterRawInputDevices(rid, numDevices, sizeof(RAWINPUTDEVICE)))
	{
		OutputDebugStringA("[Warning] Input device registration failed!\n");
	}
}

void Input::bindFunctionToInput(void (* function)(float), TYPE type)
{
	inputCallbacks[type].second.push_back(function);
}

bool Input::bindAxis(const std::string& axisName, void (* function)(float))
{
	auto axis = inputAxes.find(axisName);
	if (axis == inputAxes.end())
	{
		return false;
	}
	axis->second.callbacks.push_back(function);
	return true;
}

void Input::processOSInput(WPARAM wParam, LPARAM lParam)
{
	unsigned sizeHeader = sizeof(RAWINPUTHEADER);
	unsigned size = sizeof(RAWINPUT);
	RAWINPUTHEADER rawHeader;
	RAWINPUT raw;
	GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_HEADER, &rawHeader, &sizeHeader, sizeof(RAWINPUTHEADER));
	GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

	switch (rawHeader.dwType)
	{
	case RIM_TYPEMOUSE:
		handleMouse(&raw.data.mouse);
		break;
	case RIM_TYPEKEYBOARD:
		handleKeyboard(&raw.data.keyboard);
		break;
	case RIM_TYPEHID:
		//TODO: XInput
		//HidP_GetButtonCaps
		/*{
			HidP_GetButtonCaps()
			//TODO: dualshock 4

			BYTE* buffer = new BYTE[128];

			unsigned s = 256;
			char str[256];
			GetRawInputDeviceInfoA(rawHeader.hDevice, RIDI_DEVICENAME, &str, &s);


			OutputDebugStringA(str);
			HANDLE hid_device = CreateFileA(str, GENERIC_READ | GENERIC_WRITE,
			                                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			                                OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

			unsigned long sn;
			//OVERLAPPED o = {};
			//bool b = ReadFile(hid_device, buffer, 64, &sn, &o);
			OVERLAPPED ov =  {};
			std::stringstream ss;
			for (int i = 0; i < 64; i++)
			{
				ss << std::setfill('0') << std::setw(2) << std::hex << int(buffer[i]) << ' ';
			}
			OutputDebugStringA((ss.str() + "\n").c_str());

			uint8_t buf[32];
			memset(buf, 0, sizeof(buf));
			buf[0] = 0x05;
			buf[1] = 0xFF;
			buf[4] = 0; // 0-255
			buf[5] = 0; // 0-255
			buf[6] = 0; // 0-255
			buf[7] = 255; // 0-255
			buf[8] = 255; // 0-255
			DWORD bytes_written;
			WriteFile(hid_device, buf, sizeof(buf), &bytes_written, &ov);
			//	
		}*/

		//OutputDebugStringA(("Hid input " + std::to_string(c++) + "\n").c_str());
		handleController(&raw.data.hid);
		break;

	default:
		//OutputDebugStringA((std::to_string(rawHeader.dwType) + "\n").c_str());
		break;
	}
}



void Input::handleKeyboard(RAWKEYBOARD* data)
{
	float value = 1 - data->Flags & RI_KEY_BREAK;

	// handle character keys
	if (data->VKey >= 0x41 && data->VKey <= 0x5A)
	{
		keyboard.onInput(static_cast<KEYBOARD_INPUT>(data->VKey - 0x41), value);
		return;
	}
	switch (data->VKey)
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
		keyboard.onInput(KEYBOARD_INPUT::ARROW_DOWN, value);;
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

void Input::handleMouse(RAWMOUSE* data)
{
	if (data->usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
	{
		mouse.onInput(MOUSE_INPUT::BUTTON_LEFT, 1);
	}
	if (data->usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
	{
		mouse.onInput(MOUSE_INPUT::BUTTON_LEFT, 0);
	}
	if (data->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
	{
		mouse.onInput(MOUSE_INPUT::BUTTON_RIGHT, 1);
	}
	if (data->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
	{
		mouse.onInput(MOUSE_INPUT::BUTTON_RIGHT, 0);
	}
	if (!data->usFlags)
	{
		// handle relative mouse movement
		mouse.onInput(MOUSE_INPUT::AXIS_X, data->lLastX);
		mouse.onInput(MOUSE_INPUT::AXIS_Y, data->lLastY);
		OutputDebugStringA((std::to_string(data->lLastX) + "\n").c_str());
	}
}

void Input::handleController(RAWHID* data)
{
	// TODO: get device info
	UINT bufSize = data->dwCount * data->dwSizeHid;
	BYTE* buffer = data->bRawData;

	auto* report = reinterpret_cast<GamepadInputReport*>(buffer);

	std::stringstream ss;
	for (int i = 0; i < bufSize; i++)
	{
		ss << std::setfill('0') << std::setw(2) << std::hex << int(buffer[i]) << ' ';
	}
	OutputDebugStringA((ss.str() + "\n").c_str());
	//OutputDebugStringA(("D-Pad: " + std::to_string(report->dPad) + "\n").c_str());
	//OutputDebugStringA(("Button1: " + std::to_string(report->button1) + "\n").c_str());
	//OutputDebugStringA(("Button2: " + std::to_string(report->button2) + "\n").c_str());
	//OutputDebugStringA(("Button3: " + std::to_string(report->button3) + "\n").c_str());
	//OutputDebugStringA(("Button4: " + std::to_string(report->button4) + "\n").c_str());
	//OutputDebugStringA(("Button5: " + std::to_string(report->button5) + "\n").c_str());
	//OutputDebugStringA(("Button6: " + std::to_string(report->button6) + "\n").c_str());
	//OutputDebugStringA(("Button7: " + std::to_string(report->button7) + "\n").c_str());
	//OutputDebugStringA(("Button8: " + std::to_string(report->button8) + "\n").c_str());
	//OutputDebugStringA(("Button9: " + std::to_string(report->button9) + "\n").c_str());
	//OutputDebugStringA(("Button10: " + std::to_string(report->button10) + "\n").c_str());
	//OutputDebugStringA(("Left stick: " + std::to_string(report->leftStick) + "\n").c_str());
	//OutputDebugStringA(("Right stick: " + std::to_string(report->rightStick) + "\n").c_str());

	controller.onInput(CONTROLLER_INPUT::BUTTON_1, report->button1);
	controller.onInput(CONTROLLER_INPUT::BUTTON_2, report->button2);
	controller.onInput(CONTROLLER_INPUT::BUTTON_3, report->button3);
	controller.onInput(CONTROLLER_INPUT::BUTTON_4, report->button4);
	controller.onInput(CONTROLLER_INPUT::BUTTON_5, report->button5);
	controller.onInput(CONTROLLER_INPUT::BUTTON_6, report->button6);
	controller.onInput(CONTROLLER_INPUT::BUTTON_7, report->button7);
	controller.onInput(CONTROLLER_INPUT::BUTTON_8, report->button8);
	controller.onInput(CONTROLLER_INPUT::BUTTON_9, report->button9);
	controller.onInput(CONTROLLER_INPUT::BUTTON_10, report->button10);

	controller.onInput(CONTROLLER_INPUT::AXIS_LEFT_X, (report->leftX - 127.5f) / 127.5f);
	controller.onInput(CONTROLLER_INPUT::AXIS_LEFT_Y, (report->leftY - 127.5f) / 127.5f);
	controller.onInput(CONTROLLER_INPUT::AXIS_RIGHT_X, (report->rightX - 127.5f) / 127.5f);
	controller.onInput(CONTROLLER_INPUT::AXIS_RIGHT_Y, (report->rightY - 127.5f) / 127.5f);

	//TODO: handle dpad
}

void Input::handleGameInput()
{
	//TODO: remove this loop
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

	for (auto& axis : inputAxes)
	{
		axis.second.handleInput();
	}
	keyboard.tick();
	mouse.tick();
	controller.tick();
}
