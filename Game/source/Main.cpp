#include <windows.h>

#include "GameWindow.h"

#include "Camera.h"
#include "resource/Mesh.h"
#include "input/Input.h"
#include "Engine.h"
#include <string>


using namespace Baryon;

Camera camera{70, 1, 0.01f, 10000, {10, 10, 10}};
GameWindow* window;

float speed = 1;
void right(float f)
{
	camera.translate({f* 0.2f, 0, 0});
}
void forward(float f)
{
	camera.translate({0, 0, f * 0.2f * speed});
}

void turn(float f)
{
	camera.rotate(0, f * 0.005f, 0, SPACE::GLOBAL);
}
void lookUp(float f)
{
	camera.rotate(f * 0.005f, 0, 0);
}

void changeStyle()
{
	static int i = 1;
	window->setStyle(static_cast<GameWindow::STYLE>(i));
	i = (i + 1) % 3;
}

void setSpeed(float value)
{
	speed = 1 + 5 * value;
}

void changeRes()
{
	static int i = 5;
	window->setResolution(VirtualScreen::getSupportedResolutions()[i]);
	i = (i + 1) % VirtualScreen::getSupportedResolutions().size();
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	ShowCursor(0);
	WindowsApplication::initialize(hInstance);
	Engine engine;
	if (!engine.initialize())
	{
		return EXIT_FAILURE;
	}
	window = new GameWindow(L"Test Game", VirtualScreen::getSupportedResolutions()[6]);

	engine.createVirtualScreen(*window);

	window->setStyle(GameWindow::WINDOWED);
	window->setActiveCamera(&camera);
	window->show();

	// setup input
	Input::Axis& moveForwardAxis = Input::addAxis("MoveForward");
	Input::Axis& moveRightAxis = Input::addAxis("MoveRight");
	Input::Axis& turnAxis = Input::addAxis("Turn");
	Input::Axis& lookUpAxis = Input::addAxis("LookUp");
	Input::Action& switchResolutionAction = Input::addAction("SwitchRes");
	Input::Action& changeStyleAction = Input::addAction("ChangeStyle");
	Input::Axis& speedAxis = Input::addAxis("SpeedUp");

	moveForwardAxis.setInputBinding(KEYBOARD_INPUT::W, 1);
	moveForwardAxis.setInputBinding(KEYBOARD_INPUT::S, -1);
	moveForwardAxis.setInputBinding(GAMEPAD_INPUT::AXIS_LEFT_Y, -1);

	moveRightAxis.setInputBinding(KEYBOARD_INPUT::D, 1);
	moveRightAxis.setInputBinding(KEYBOARD_INPUT::A, -1);
	moveRightAxis.setInputBinding(GAMEPAD_INPUT::AXIS_LEFT_X, 1);

	turnAxis.setInputBinding(MOUSE_INPUT::AXIS_X, 1);
	turnAxis.setInputBinding(GAMEPAD_INPUT::AXIS_RIGHT_X, 1);

	lookUpAxis.setInputBinding(MOUSE_INPUT::AXIS_Y, 1);
	lookUpAxis.setInputBinding(GAMEPAD_INPUT::AXIS_RIGHT_Y, 1);

	switchResolutionAction.setInputBinding(KEYBOARD_INPUT::Q);
	switchResolutionAction.setInputBinding(GAMEPAD_INPUT::BUTTON_1);

	changeStyleAction.setInputBinding(KEYBOARD_INPUT::E);
	changeStyleAction.setInputBinding(GAMEPAD_INPUT::BUTTON_3);

	speedAxis.setInputBinding(GAMEPAD_INPUT::RIGHT_TRIGGER, 1);


	Input::bindAxis("MoveForward", forward);
	Input::bindAxis("MoveRight", right);
	Input::bindAxis("Turn", turn);
	Input::bindAxis("LookUp", lookUp);

	Input::bindAction("SwitchRes", Input::Action::PRESSED, changeRes);
	Input::bindAction("ChangeStyle", Input::Action::PRESSED, changeStyle);

	Input::bindAxis("SpeedUp", setSpeed);

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		engine.mainLoopIteration();
	}
	engine.terminate();

	delete window;
	return EXIT_SUCCESS;
}