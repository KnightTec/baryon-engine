

template <int NUM_BUTTONS, int NUM_AXES>
HID<NUM_BUTTONS, NUM_AXES>::HID() : buttonStates(&buttonBuffer0), buttonStatesLast(&buttonBuffer1)
{
}
template <int NUM_BUTTONS, int NUM_AXES>
HID<NUM_BUTTONS, NUM_AXES>::~HID()
{
}
template <int NUM_BUTTONS, int NUM_AXES>
void HID<NUM_BUTTONS, NUM_AXES>::onButtonPressed(int buttonId)
{
	buttonStates[buttonId] = true;
}
template <int NUM_BUTTONS, int NUM_AXES>
void HID<NUM_BUTTONS, NUM_AXES>::onButtonReleased(int buttonId)
{
	buttonStates[buttonId] = false;
}
template <int NUM_BUTTONS, int NUM_AXES>
bool HID<NUM_BUTTONS, NUM_AXES>::isButtonPressed(int buttonId)
{
	return buttonStates[buttonId] && !buttonStatesLast[buttonId];
}
template <int NUM_BUTTONS, int NUM_AXES>
bool HID<NUM_BUTTONS, NUM_AXES>::isButtonReleased(int buttonId)
{
	return !buttonStates[buttonId] && buttonStatesLast[buttonId];
}
template <int NUM_BUTTONS, int NUM_AXES>
bool HID<NUM_BUTTONS, NUM_AXES>::isButtonDown(int buttonId)
{
	return buttonStates[buttonId];
}
template <int NUM_BUTTONS, int NUM_AXES>
void HID<NUM_BUTTONS, NUM_AXES>::tick()
{
	bool* tmp = buttonStatesLast;
	buttonStatesLast = buttonStates;
	buttonStates = tmp;
	memset(axisReveivedInput, 0, sizeof(axisReveivedInput));
}
template <int NUM_BUTTONS, int NUM_AXES>
void HID<NUM_BUTTONS, NUM_AXES>::onAxisInput(int axisId, float value)
{
	axisValues[axisId] = value;
	axisReveivedInput[axisId] = true;
}
template <int NUM_BUTTONS, int NUM_AXES>
float HID<NUM_BUTTONS, NUM_AXES>::getAxis(int axisId)
{
	if (axisReveivedInput[axisId]) {
		return axisValues[axisId];
	}
	return 0;
}



inline void Keyboard::onButtonPressed(KEY key)
{
	super::onButtonPressed(key);
}
inline void Keyboard::onButtonReleased(KEY key)
{
	super::onButtonReleased(key);
}
inline bool Keyboard::isButtonPressed(KEY key)
{
	return super::isButtonPressed(key);
}
inline bool Keyboard::isButtonReleased(KEY key)
{
	return super::isButtonPressed(key);
}
inline bool Keyboard::isButtonDown(KEY key)
{
	return super::isButtonDown(key);
}


inline void Mouse::onButtonPressed(BUTTON key)
{
	super::onButtonPressed(key);
}
inline void Mouse::onButtonReleased(BUTTON key)
{
	super::onButtonReleased(key);
}
inline bool Mouse::isButtonPressed(BUTTON key)
{
	return super::isButtonPressed(key);
}
inline bool Mouse::isButtonReleased(BUTTON key)
{
	return super::isButtonPressed(key);
}
inline bool Mouse::isButtonDown(BUTTON key)
{
	return super::isButtonDown(key);
}
inline void Mouse::onAxisInput(AXIS axis, float value)
{
	super::onAxisInput(axis, value);
}
inline float Mouse::getAxis(AXIS axis)
{
	return super::getAxis(axis);
}