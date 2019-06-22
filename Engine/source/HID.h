#pragma once
#include <cstdint>

enum class EMPTY_ENUM
{
};

template<int NUM_BUTTONS, int NUM_AXES/*, typename BUTTON_ENUM, typename AXIS_ENUM*/>
class HID
{
	//static_assert(std::is_enum<BUTTON_ENUM>::value, "BUTTON_ENUM must be an enum type!");
	//static_assert(std::is_enum<BUTTON_ENUM>::value, "AXIS_ENUM must be an enum type!");
public:
	HID();
	virtual ~HID() = 0;
	/*
	 * called after input has been handled by the game
	 */
	void tick();
protected:
	void onButtonPressed(int buttonId);
	void onButtonReleased(int buttonId);
	bool isButtonPressed(int buttonId);
	bool isButtonReleased(int buttonId);
	bool isButtonDown(int buttonId);

	void onAxisInput(int axisId, float value);
	float getAxis(int axisId);
private:
	bool* buttonStates;
	bool* buttonStatesLast;
	bool buttonBuffer0[NUM_BUTTONS + 1] = {false};
	bool buttonBuffer1[NUM_BUTTONS + 1] = {false};

	float axisValues[NUM_AXES + 1] = {0};
	bool axisReveivedInput[NUM_AXES + 1] = {false};
};



class Keyboard : public HID<128, 0>
{
	typedef HID<128, 0> super;
public:
	enum KEY : int
{
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		ARROW_LEFT, ARROW_RIGHT, ARROW_UP, ARROW_DOWN,
		SPACE, ENTER
	};

	void onButtonPressed(KEY key);
	void onButtonReleased(KEY key);

	bool isButtonPressed(KEY key);
	bool isButtonReleased(KEY key);
	bool isButtonDown(KEY key);
};


class Mouse : public HID<2, 2>
{
	typedef HID<2, 2> super;
public:
	enum BUTTON
	{
		LEFT,
		RIGHT
	};
	enum AXIS
	{
		X,
		Y
	};

	void onButtonPressed(BUTTON key);
	void onButtonReleased(BUTTON key);

	bool isButtonPressed(BUTTON key);
	bool isButtonReleased(BUTTON key);
	bool isButtonDown(BUTTON key);

	void onAxisInput(AXIS axis, float value);
	float getAxis(AXIS axis);
};
#include "HID.inl"