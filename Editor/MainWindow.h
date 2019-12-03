#pragma once
#include <QMainWindow>
#include <QDockWidget>

#include "Editor.h"
#include "Engine.h"
#include "Camera.h"


struct BaryonEditorApp : QApplication
{
	using QApplication::QApplication;
	static bool running;
	void initialize();
};

class MainWindow : public QMainWindow
{
public:
	MainWindow(Baryon::Engine* engine);
	~MainWindow();

	void tick();
	void closeEvent(QCloseEvent* event) override;
private:
	Baryon::Engine* engine;
	Baryon::Camera camera;

	Editor* editor;
	QDockWidget* dockEditor;
};