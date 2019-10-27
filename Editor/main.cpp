#include "Editor.h"
#include <QtWidgets/QApplication>
#include <QThread>
#include <QPalette>
#include <QStyleFactory>

#include "Engine.h"

int main(int argc, char *argv[])
{
	Baryon::Engine engine;
	if (!engine.initialize())
	{
		return EXIT_FAILURE;
	}

	BaryonEditorApp app(argc, argv);

	BaryonEditorApp::setStyle(QStyleFactory::create("Fusion"));

	QPalette darkPalette;
	darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::WindowText, Qt::white);
	darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
	darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
	darkPalette.setColor(QPalette::ToolTipText, Qt::white);
	darkPalette.setColor(QPalette::Text, Qt::white);
	darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::ButtonText, Qt::white);
	darkPalette.setColor(QPalette::BrightText, Qt::red);
	darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::HighlightedText, Qt::black);

	BaryonEditorApp::setPalette(darkPalette);
	app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");


	Editor w(&engine.getRenderer());
	Baryon::Camera camera(80, 1, 0.01, 1000, { 0, 20, 0 });

	if (!engine.getRenderer().createVirtualScreen(w))
	{
		MessageBoxW(nullptr, L"Error: Failed to create VirtualScreen.", L"Baryon Engine", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
	w.setActiveCamera(&camera);
	w.show();

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
		engine.mainLoopIteration();
	}
	engine.terminate();
	return EXIT_SUCCESS;
}
