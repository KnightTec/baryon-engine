#include "Editor.h"
#include <QtWidgets/QApplication>
#include <QThread>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>

#include "MainWindow.h"
#include "Engine.h"

int main(int argc, char *argv[])
{
	Baryon::Engine engine;
	if (!engine.initialize())
	{
		return EXIT_FAILURE;
	}

	BaryonEditorApp app(argc, argv);
	app.initialize();

	MainWindow mainWindow(&engine);
	mainWindow.showMaximized();

	while (BaryonEditorApp::running)
	{
		QApplication::processEvents();
		mainWindow.tick();
		engine.mainLoopIteration();
	}
	engine.terminate();
	return EXIT_SUCCESS;
}
