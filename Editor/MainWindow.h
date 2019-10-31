#pragma once
#include <QMainWindow>
#include <QDockWidget>
#include "Editor.h"

class Engine;

class MainWindow : public QMainWindow
{
public:
	MainWindow(Engine* engine);
	void tick();
	~MainWindow()
	{
		delete editor;
	}

private:
	Engine* engine;

	Editor* editor;
};

//MainWindow()
//{
//	QDockWidget *dock = new QDockWidget(tr("World ViewX"), this);
//	dock->setAttribute(Qt::WA_PaintOnScreen);
//	//editor = new Editor(this);
//	//dock->setWidget(editor);
//	addDockWidget(Qt::RightDockWidgetArea, dock, Qt::Horizontal);
//}