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

	Baryon::Mesh mesh;
	int i = 30;
	std::vector<Baryon::Entity> entities(i * i, Baryon::Entity{ mesh });

	for (int y = 0; y < i; y++)
	{
		for (int x = 0; x < i; x++)
		{
			entities[x + y * i].transform.translate(5 * x - 10, 5, 5 * y - 10);
			entities[x + y * i].transform.scale(1, (rand() % 12 + 1), 1);
			engine.getRenderer().submitEntity(&entities[x + y * i]);
		}
	}

	while (BaryonEditorApp::running)
	{
		QApplication::processEvents();
		mainWindow.tick();
		engine.mainLoopIteration();
	}
	engine.terminate();
	return EXIT_SUCCESS;
}
