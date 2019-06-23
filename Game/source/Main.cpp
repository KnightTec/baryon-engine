#include <windows.h>

#include "GameWindow.h"

#include "Engine.h"
#include "Mesh.h"
#include "input\\Input.h"
#include <string>


using namespace Baryon;

Camera camera{70, 1, 0.01, 1000, {2, 1.5, -5}};
GameWindow* window;


void right(float f)
{
	camera.translate({f* 0.1f, 0, 0});
}
void forward(float f)
{
	camera.translate({0, 0, f * 0.1f});
}

void turn(float f)
{
	camera.rotate(0, f * 0.005, 0, SPACE::GLOBAL);
}
void lookUp(float f)
{
	camera.rotate(f * 0.005, 0, 0);
}

void changeStyle(float)
{
	static int i = 1;
	window->setStyle(static_cast<GameWindow::STYLE>(i));
	i = (i + 1) % 3;
}

void changeRes(float)
{
	static int i = 1;
	window->setResolution(VirtualScreen::getSupportedResolutions()[i]);
	i = (i + 1) % VirtualScreen::getSupportedResolutions().size();
}


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	WindowsApplication::initialize(hInstance);
	Engine engine;
	if (!engine.initialize())
	{
		return EXIT_FAILURE;
	}
	window = new GameWindow(L"Test Game", VirtualScreen::getSupportedResolutions()[0], GameWindow::WINDOWED);

	if (!engine.getRenderer().createVirtualScreen(*window))
	{
		MessageBoxW(nullptr, L"Error: Failed to create VirtualScreen.", L"Baryon Engine", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}

	// setup input
	Input::Axis& moveForwardAxis = Input::addAxis("MoveForward");
	Input::Axis& moveRightAxis = Input::addAxis("MoveRight");
	Input::Axis& turnAxis = Input::addAxis("Turn");
	Input::Axis& lookUpAxis = Input::addAxis("LookUp");

	moveForwardAxis.setInputBinding(KEYBOARD_INPUT::W, 1);
	moveForwardAxis.setInputBinding(KEYBOARD_INPUT::S, -1);
	moveForwardAxis.setInputBinding(CONTROLLER_INPUT::AXIS_LEFT_Y, 1);

	moveRightAxis.setInputBinding(KEYBOARD_INPUT::D, 1);
	moveRightAxis.setInputBinding(KEYBOARD_INPUT::A, -1);
	moveRightAxis.setInputBinding(CONTROLLER_INPUT::AXIS_LEFT_X, 1);

	turnAxis.setInputBinding(MOUSE_INPUT::AXIS_X, 1);
	turnAxis.setInputBinding(CONTROLLER_INPUT::AXIS_RIGHT_X, 1);

	lookUpAxis.setInputBinding(MOUSE_INPUT::AXIS_Y, 1);
	lookUpAxis.setInputBinding(CONTROLLER_INPUT::AXIS_RIGHT_Y, 1);

	Input::bindAxis("MoveForward", forward);
	Input::bindAxis("MoveRight", right);
	Input::bindAxis("Turn", turn);
	Input::bindAxis("LookUp", lookUp);

	Input::bindFunctionToInput(changeStyle, Input::KEYBOARD_SPACE);
	Input::bindFunctionToInput(changeRes, Input::KEYBOARD_ENTER);


	window->setActiveCamera(&camera);
	Mesh cube;
	engine.getRenderer().submitMesh(cube);

	engine.run();

	delete window;
	return EXIT_SUCCESS;
}
