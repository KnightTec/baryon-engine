#include <VirtualInputDevice.h>


using namespace Baryon;

void Mouse::onInput(MOUSE_INPUT inputId, float value)
{
	float sensitity = 0.5;
	if (inputId == MOUSE_INPUT::AXIS_X || inputId == MOUSE_INPUT::AXIS_Y)
	{
		inputs[static_cast<int>(inputId)] += value * sensitity;
	}
	else
	{
		super::onInput(inputId, value);
	}
}
void Mouse::tick()
{
	super::tick();
	inputs[static_cast<int>(MOUSE_INPUT::AXIS_X)] = 0;
	inputs[static_cast<int>(MOUSE_INPUT::AXIS_Y)] = 0;
}

void Gamepad::clearButtons()
{
	memset(inputs, 0, sizeof(GAMEPAD_INPUT) * numberOfButtons);
}
void Gamepad::onInput(GAMEPAD_INPUT inputId, float value)
{
	super::onInput(inputId, value);
}
void Gamepad::onInputStick(STICK stick, float x, float y)
{
	float deadZone = 0.2;
	float sensitivity = 1;

	float magnitude = sqrt(x * x + y * y);
	float normX = x / magnitude;
	float normY = y / magnitude;

	if (magnitude < deadZone)
	{
		x = y = 0;
	}
	else
	{
		if (magnitude > 1)
		{
			magnitude = 1;
		}
		x = normX * ((magnitude - deadZone) / (1 - deadZone));
		y = normY * ((magnitude - deadZone) / (1 - deadZone));
	}

	switch (stick)
	{
	case STICK::LEFT:
		super::onInput(GAMEPAD_INPUT::AXIS_LEFT_X, x);
		super::onInput(GAMEPAD_INPUT::AXIS_LEFT_Y, y);
		break;
	case STICK::RIGHT:
		sensitivity = 10;
		super::onInput(GAMEPAD_INPUT::AXIS_RIGHT_X, sensitivity * x);
		super::onInput(GAMEPAD_INPUT::AXIS_RIGHT_Y, sensitivity * y);
		break;
	}
}
