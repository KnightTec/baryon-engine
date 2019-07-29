#include "Input.h"

#include "Xinput.h"
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

void Input::Axis::setInputBinding(GAMEPAD_INPUT inputId, float scale)
{
	for (auto& input : gamepadBindings)
	{
		if (inputId == input.first)
		{
			input.second = scale;
			return;
		}
	}
	gamepadBindings.emplace_back(inputId, scale);
}

void Input::Axis::handleInput()
{
	float axisValue = 0;
	for (const auto& input : keyboardBindings)
	{
		axisValue += keyboard.getInputValue(input.first) * input.second;
	}
	for (const auto& input : mouseBindings)
	{
		axisValue += mouse.getInputValue(input.first) * input.second;
	}
	for (const auto& input : gamepadBindings)
	{
		axisValue += gamepad.getInputValue(input.first) * input.second;
	}
	for (auto function : callbacks)
	{
		function(axisValue);
	}
}
void Input::Action::setInputBinding(KEYBOARD_INPUT inputId)
{
	for (auto& input : keyboardBindings)
	{
		if (inputId == input)
		{
			return;
		}
	}
	keyboardBindings.emplace_back(inputId);
}
void Input::Action::setInputBinding(MOUSE_INPUT inputId)
{
	for (auto& input : mouseBindings)
	{
		if (inputId == input)
		{
			return;
		}
	}
	mouseBindings.emplace_back(inputId);
}
void Input::Action::setInputBinding(GAMEPAD_INPUT inputId)
{
	for (auto& input : gamepadBindings)
	{
		if (inputId == input)
		{
			return;
		}
	}
	gamepadBindings.emplace_back(inputId);
}
void Input::Action::handleInput()
{
	std::vector<TYPE> actionBuffer;
	for (const auto& inputId : keyboardBindings)
	{
		if (keyboard.isInputStart(inputId))
		{
			actionBuffer.push_back(PRESSED);
		}
		else if (keyboard.isInputEnd(inputId))
		{
			actionBuffer.push_back(RELEASED);
		}
	}
	for (const auto& inputId : mouseBindings)
	{
		if (mouse.isInputStart(inputId))
		{
			actionBuffer.push_back(PRESSED);
		}
		else if (mouse.isInputEnd(inputId))
		{
			actionBuffer.push_back(RELEASED);
		}
	}
	for (const auto& inputId : gamepadBindings)
	{
		if (gamepad.isInputStart(inputId))
		{
			actionBuffer.push_back(PRESSED);
		}
		else if (gamepad.isInputEnd(inputId))
		{
			actionBuffer.push_back(RELEASED);
		}
	}
	for (const auto& callback : callbacks)
	{
		for (TYPE type : actionBuffer)
		{
			if (type == callback.second)
			{
				callback.first();
			}
		}
	}
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
}


Keyboard Input::keyboard;
Mouse Input::mouse;
Gamepad Input::gamepad;
std::unordered_map<std::string, Input::Axis> Input::inputAxes;
std::unordered_map<std::string, Input::Action> Input::inputActions;

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
		OutputDebugStringW(L"[Warning] Input device registration failed!\n");
	}
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

bool Input::bindAction(const std::string& actionName, Action::TYPE type, void (* function)())
{
	auto action = inputActions.find(actionName);
	if (action == inputActions.end())
	{
		return false;
	}
	action->second.callbacks.emplace_back(std::pair(function, type));
	return true;
}

void Input::passOSData(WPARAM wParam, LPARAM lParam)
{
	unsigned sizeHeader = sizeof(RAWINPUTHEADER);
	unsigned size = sizeof(RAWINPUT);
	RAWINPUT raw;

	// get input device information
	GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_HEADER, &raw.header, &sizeHeader, sizeof(RAWINPUTHEADER));
	wchar_t deviceName[256];
	UINT nameSize = 256;
	GetRawInputDeviceInfoW(raw.header.hDevice, RIDI_DEVICENAME, deviceName, &nameSize);
	RID_DEVICE_INFO deviceInfo;
	UINT deviceInfoSize = sizeof(deviceInfo);
	GetRawInputDeviceInfoW(raw.header.hDevice, RIDI_DEVICEINFO, &deviceInfo, &deviceInfoSize);

	if (raw.header.dwType == RIM_TYPEHID && wcsstr(deviceName, L"IG_"))
	{
		processXInput();
		return;
	}

	// check if the device is a Dualshock 4 gamepad
	if (deviceInfo.hid.dwVendorId == 0x54c && (deviceInfo.hid.dwProductId == 0x5c4 || deviceInfo.hid.dwProductId == 0x9cc))
	{
		// with a Dualshock 4 GetRawInputData only returns garbage, but GetRawInputBuffer works
		UINT cbSize;
		GetRawInputBuffer(nullptr, &cbSize, sizeof(RAWINPUTHEADER));
		if (cbSize == 0)
		{
			return;
		}
		auto pRawInput = static_cast<PRAWINPUT>(malloc(cbSize));
		GetRawInputBuffer(pRawInput, &cbSize, sizeof(RAWINPUTHEADER));
		raw = *pRawInput;
		free(pRawInput);
	}
	else
	{
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));
	}

	switch (raw.header.dwType)
	{
	case RIM_TYPEMOUSE:
		processMouse(&raw.data.mouse);
		break;
	case RIM_TYPEKEYBOARD:
		processKeyboard(&raw.data.keyboard);
		break;
	case RIM_TYPEHID:
		{
			processGenericGamepad(&raw);
			break;
		}
	default:
		break;
	}
}


void Input::processKeyboard(RAWKEYBOARD* data)
{
	float value = float(1 - data->Flags & RI_KEY_BREAK);

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
		keyboard.onInput(KEYBOARD_INPUT::ARROW_DOWN, value);
		break;
	case VK_SPACE:
		keyboard.onInput(KEYBOARD_INPUT::SPACE, value);
		break;
	case VK_RETURN:
		keyboard.onInput(KEYBOARD_INPUT::ENTER, value);
		break;
	default:
		break;
	}
}

void Input::processMouse(RAWMOUSE* data)
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
	}
}

void Input::processXInput()
{
	XINPUT_STATE state;
	if (XInputGetState(0, &state) != ERROR_SUCCESS)
	{
		return;
	}
	static DWORD packetNumber = 0;
	// check if the state of the gamepad has changed
	if (state.dwPacketNumber == packetNumber)
	{
		return;
	}
	packetNumber = state.dwPacketNumber;

	gamepad.onInputStick(Gamepad::STICK::LEFT, float(state.Gamepad.sThumbLX) / 32767, -float(state.Gamepad.sThumbLY) / 32767);
	gamepad.onInputStick(Gamepad::STICK::RIGHT, float(state.Gamepad.sThumbRX) / 32767, -float(state.Gamepad.sThumbRY) / 32767);

	gamepad.onInput(GAMEPAD_INPUT::LEFT_SHOULDER, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0));
	gamepad.onInput(GAMEPAD_INPUT::RIGHT_SHOULDER, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0));

	gamepad.onInput(GAMEPAD_INPUT::LEFT_TRIGGER, float(state.Gamepad.bLeftTrigger) / 255);
	gamepad.onInput(GAMEPAD_INPUT::LEFT_TRIGGER, float(state.Gamepad.bRightTrigger) / 255);

	gamepad.onInput(GAMEPAD_INPUT::BUTTON_A, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0));
	gamepad.onInput(GAMEPAD_INPUT::BUTTON_B, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0));
	gamepad.onInput(GAMEPAD_INPUT::BUTTON_X, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0));
	gamepad.onInput(GAMEPAD_INPUT::BUTTON_Y, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0));

	gamepad.onInput(GAMEPAD_INPUT::BUTTON_BACK, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0));
	gamepad.onInput(GAMEPAD_INPUT::BUTTON_START, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0));

	gamepad.onInput(GAMEPAD_INPUT::DPAD_LEFT, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0));
	gamepad.onInput(GAMEPAD_INPUT::DPAD_RIGHT, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0));
	gamepad.onInput(GAMEPAD_INPUT::DPAD_UP, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0));
	gamepad.onInput(GAMEPAD_INPUT::DPAD_DOWN, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0));

	gamepad.onInput(GAMEPAD_INPUT::STICK_LEFT, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0));
	gamepad.onInput(GAMEPAD_INPUT::STICK_RIGHT, static_cast<float>((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0));
}

void Input::processGenericGamepad(RAWINPUT* raw)
{
	UINT bufferSize;
	GetRawInputDeviceInfoW(raw->header.hDevice, RIDI_PREPARSEDDATA, nullptr, &bufferSize);
	auto pPreparsedData = static_cast<PHIDP_PREPARSED_DATA>(malloc(bufferSize));
	GetRawInputDeviceInfoW(raw->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &bufferSize);
	HIDP_CAPS caps;
	if (HidP_GetCaps(pPreparsedData, &caps) != HIDP_STATUS_SUCCESS)
	{
		free(pPreparsedData);
		return;
	}
	auto buttonCaps = new HIDP_BUTTON_CAPS[caps.NumberInputButtonCaps];
	if (HidP_GetButtonCaps(HidP_Input, buttonCaps, &caps.NumberInputButtonCaps, pPreparsedData) != HIDP_STATUS_SUCCESS)
	{
		delete[] buttonCaps;
		free(pPreparsedData);
		return;
	}
	auto valueCaps = new HIDP_VALUE_CAPS[caps.NumberInputValueCaps];
	if (HidP_GetValueCaps(HidP_Input, valueCaps, &caps.NumberInputValueCaps, pPreparsedData) != HIDP_STATUS_SUCCESS)
	{
		delete[] valueCaps;
		delete[] buttonCaps;
		free(pPreparsedData);
		return;
	}
	ULONG usageLength = HidP_MaxUsageListLength(HidP_Input, buttonCaps->UsagePage, pPreparsedData);
	auto usage = new USAGE[usageLength];


	if (HidP_GetUsages(HidP_Input, buttonCaps->UsagePage, 0, usage, &usageLength, pPreparsedData,
	                   reinterpret_cast<PCHAR>(raw->data.hid.bRawData), raw->data.hid.dwSizeHid) != HIDP_STATUS_SUCCESS)
	{
		delete[] usage;
		delete[] valueCaps;
		delete[] buttonCaps;
		free(pPreparsedData);
		return;
	}
	if (usageLength > 0 && usage[0] != 0)
	{
		int x = 0;
	}

	gamepad.clearButtons();
	for (ULONG i = 0; i < usageLength; i++)
	{
		int buttonId = usage[i] - buttonCaps->Range.UsageMin;
		if (buttonId < Gamepad::numberOfButtons)
		{
			gamepad.onInput(static_cast<GAMEPAD_INPUT>(buttonId), 1);
		}
	}
	float axisLeftX = 0;
	float axisLeftY = 0;
	float axisRightX = 0;
	float axisRightY = 0;
	for (int i = 0; i < caps.NumberInputValueCaps; i++)
	{
		ULONG tmp;
		HidP_GetUsageValue(HidP_Input, valueCaps[i].UsagePage, 0,
		                   valueCaps[i].Range.UsageMin, &tmp, pPreparsedData,
		                   reinterpret_cast<PCHAR>(raw->data.hid.bRawData), raw->data.hid.dwSizeHid);
		float value = tmp << 1;
		value /= float(valueCaps[i].LogicalMax - valueCaps[i].LogicalMin);
		value -= 1;
		switch (valueCaps[i].Range.UsageMin)
		{
		case 0x30:
			axisLeftX = value;
			break;
		case 0x31:
			axisLeftY = value;
			break;
		case 0x32:
			axisRightX = value;
			break;
		case 0x35:
			axisRightY = value;
			break;
		case 0x39:
			// TODO: D-Pad
			break;
		case 0x33:
			// DS4 left trigger
			break;
		case 0x34:
			{
				// DS4 right trigger in range [-1, 1]
				std::string s = std::to_string(value) + "\n";
				OutputDebugStringA(s.c_str());
				break;
			}
		default:
			break;
		}
	}
	gamepad.onInputStick(Gamepad::STICK::LEFT, axisLeftX, axisLeftY);
	gamepad.onInputStick(Gamepad::STICK::RIGHT, axisRightX, axisRightY);

	// free all memory
	delete[] usage;
	delete[] valueCaps;
	delete[] buttonCaps;
	free(pPreparsedData);
}

void Input::handleGameInput()
{
	for (auto& action : inputActions)
	{
		action.second.handleInput();
	}
	for (auto& axis : inputAxes)
	{
		axis.second.handleInput();
	}
	keyboard.tick();
	mouse.tick();
	gamepad.tick();
}