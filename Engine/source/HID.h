#pragma once
#include <cstdint>

enum class EMPTY_ENUM
{
};

template<int NUM_BUTTONS, int NUM_AXES, typename BUTTON, typename AXIS = EMPTY_ENUM>
class HID
{
	static_assert(std::is_enum<BUTTON>::value, "BUTTON must be an enum type!");
	static_assert(std::is_enum<BUTTON>::value, "AXIS must be an enum type!");
public:
	HID();

	void onButtonPressed(BUTTON buttonId);
	void onButtonReleased(BUTTON buttonId);
	bool isButtonPressed(BUTTON buttonId);
	bool isButtonReleased(BUTTON buttonId);
	bool isButtonDown(BUTTON buttonId);

	void onAxisInput(AXIS axisId, float value);
	float getAxis(AXIS axisId);

	/*
	 * called after input has been handled by the game
	 */
	void tick();
private:
	bool* buttonStates;
	bool* buttonStatesLast;
	bool buttonBuffer0[NUM_BUTTONS + 1] = {false};
	bool buttonBuffer1[NUM_BUTTONS + 1] = {false};

	float axisValues[NUM_AXES + 1] = {0};
	bool axisReveivedInput[NUM_AXES + 1] = {false};
};



enum class KEY : int
{
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		ARROW_LEFT, ARROW_RIGHT, ARROW_UP, ARROW_DOWN,
		SPACE, ENTER
};
typedef HID<128, 0, KEY> Keyboard;

inline int foo()
{
	Keyboard k;
	bool b = k.isButtonDown(KEY::A);
	return 0;
}


enum class MOUSE_BUTTON
{
	LEFT,
	RIGHT
};
enum class MOUSE_AXIS
{
	X,
	Y
};
typedef HID<4, 2, MOUSE_BUTTON, MOUSE_AXIS> Mouse;



template <int NUM_BUTTONS, int NUM_AXES, typename BUTTON, typename AXIS>
HID<NUM_BUTTONS, NUM_AXES, BUTTON, AXIS>::HID() : buttonStates(&buttonBuffer0[0]), buttonStatesLast(&buttonBuffer1[0])
{
}
template <int NUM_BUTTONS, int NUM_AXES, typename BUTTON, typename AXIS>
void HID<NUM_BUTTONS, NUM_AXES, BUTTON, AXIS>::onButtonPressed(BUTTON buttonId)
{
	buttonStates[static_cast<int>(buttonId)] = true;
}
template <int NUM_BUTTONS, int NUM_AXES, typename BUTTON, typename AXIS>
void HID<NUM_BUTTONS, NUM_AXES, BUTTON, AXIS>::onButtonReleased(BUTTON buttonId)
{
	buttonStates[static_cast<int>(buttonId)] = false;
}
template <int NUM_BUTTONS, int NUM_AXES, typename BUTTON, typename AXIS>
bool HID<NUM_BUTTONS, NUM_AXES, BUTTON, AXIS>::isButtonPressed(BUTTON buttonId)
{
	return buttonStates[static_cast<int>(buttonId)] && !buttonStatesLast[static_cast<int>(buttonId)];
}
template <int NUM_BUTTONS, int NUM_AXES, typename BUTTON, typename AXIS>
bool HID<NUM_BUTTONS, NUM_AXES, BUTTON, AXIS>::isButtonReleased(BUTTON buttonId)
{
	return !buttonStates[static_cast<int>(buttonId)] && buttonStatesLast[static_cast<int>(buttonId)];
}
template <int NUM_BUTTONS, int NUM_AXES, typename BUTTON, typename AXIS>
bool HID<NUM_BUTTONS, NUM_AXES, BUTTON, AXIS>::isButtonDown(BUTTON buttonId)
{
	return buttonStates[static_cast<int>(buttonId)];
}

template <int NUM_BUTTONS, int NUM_AXES, typename BUTTON, typename AXIS>
void HID<NUM_BUTTONS, NUM_AXES, BUTTON, AXIS>::onAxisInput(AXIS axisId, float value)
{
	axisValues[static_cast<int>(axisId)] = value;
	axisReveivedInput[static_cast<int>(axisId)] = true;
}
template <int NUM_BUTTONS, int NUM_AXES, typename BUTTON, typename AXIS>
float HID<NUM_BUTTONS, NUM_AXES, BUTTON, AXIS>::getAxis(AXIS axisId)
{
	if (axisReveivedInput[static_cast<int>(axisId)]) {
		return axisValues[static_cast<int>(axisId)];
	}
	return 0;
}
template <int NUM_BUTTONS, int NUM_AXES, typename BUTTON, typename AXIS>
void HID<NUM_BUTTONS, NUM_AXES, BUTTON, AXIS>::tick()
{
	bool* tmp = buttonStatesLast;
	buttonStatesLast = buttonStates;
	buttonStates = tmp;
	memset(axisReveivedInput, 0, sizeof(axisReveivedInput));
}