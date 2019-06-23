#pragma once
#include <utility>

namespace Baryon
{

enum class KEYBOARD_INPUT : int
{
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	ARROW_LEFT, ARROW_RIGHT, ARROW_UP, ARROW_DOWN,
	SPACE, ENTER,
	ENUM_SIZE
};

enum class MOUSE_INPUT : int
{
	BUTTON_LEFT, BUTTON_RIGHT,
	AXIS_X, AXIS_Y,
	ENUM_SIZE
};

enum class CONTROLLER_INPUT : int
{
	BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8, BUTTON_9, BUTTON_10,
	DPAD_LEFT, DPAD_RIGHT, DPAD_UP, DPAD_DOWN,
	AXIS_LEFT_X, AXIS_LEFT_Y, AXIS_RIGHT_X, AXIS_RIGHT_Y,
	ENUM_SIZE
};

template <typename INPUT, int NUM_INPUTS>
class HID
{
	static_assert(std::is_enum<INPUT>::value, "INPUT must be an enum type!");
public:
	HID();
	virtual ~HID() = default;

	virtual void onInput(INPUT inputId, float value);

	bool isInputStart(INPUT inputId);
	bool isInputEnd(INPUT inputId);
	float getInputValue(INPUT inputId);

	/*
	 * called after input has been handled by the game
	 */
	virtual void tick();
protected:
	float inputs[NUM_INPUTS + 1] = {0};
	float lastInputs[NUM_INPUTS + 1] = {0};
};


typedef HID<KEYBOARD_INPUT, static_cast<int>(KEYBOARD_INPUT::ENUM_SIZE)> Keyboard;

class Mouse : public HID<MOUSE_INPUT, static_cast<int>(MOUSE_INPUT::ENUM_SIZE)>
{
	typedef HID<MOUSE_INPUT, static_cast<int>(MOUSE_INPUT::ENUM_SIZE)> super;
public:
	void onInput(MOUSE_INPUT inputId, float value) override;
	void tick() override;
};

typedef HID<CONTROLLER_INPUT, static_cast<int>(CONTROLLER_INPUT::ENUM_SIZE)> Controller;


template <typename INPUT, int NUM_INPUTS>
HID<INPUT, NUM_INPUTS>::HID()
{
}
template <typename INPUT, int NUM_INPUTS>
void HID<INPUT, NUM_INPUTS>::onInput(INPUT inputId, float value)
{
	inputs[static_cast<int>(inputId)] = value;
}
template <typename INPUT, int NUM_INPUTS>
bool HID<INPUT, NUM_INPUTS>::isInputStart(INPUT inputId)
{
	return inputs[static_cast<int>(inputId)] && !lastInputs[static_cast<int>(inputId)];
}
template <typename INPUT, int NUM_INPUTS>
bool HID<INPUT, NUM_INPUTS>::isInputEnd(INPUT inputId)
{
	return !inputs[static_cast<int>(inputId)] && lastInputs[static_cast<int>(inputId)];
}
template <typename INPUT, int NUM_INPUTS>
float HID<INPUT, NUM_INPUTS>::getInputValue(INPUT inputId)
{
	return inputs[static_cast<int>(inputId)];
}
template <typename INPUT, int NUM_INPUTS>
void HID<INPUT, NUM_INPUTS>::tick()
{
	memcpy_s(lastInputs, sizeof(lastInputs), inputs, sizeof(inputs));
}

inline void Mouse::onInput(MOUSE_INPUT inputId, float value)
{
	if (inputId == MOUSE_INPUT::AXIS_X || inputId == MOUSE_INPUT::AXIS_Y)
	{
		inputs[static_cast<int>(inputId)] += value;
	} 
	else
	{
		super::onInput(inputId, value);
	}
}
inline void Mouse::tick()
{
	super::tick();
	inputs[static_cast<int>(MOUSE_INPUT::AXIS_X)] = 0;
	inputs[static_cast<int>(MOUSE_INPUT::AXIS_Y)] = 0;
}
}
