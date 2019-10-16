#pragma once

#include <QtWidgets/QWidget>
#include "ui_Editor.h"

#include "Window.h"

struct BaryonEditorApp : QApplication
{
	using QApplication::QApplication;
	static bool running;
};

class Editor : public QWidget, public Baryon::Window
{
	Q_OBJECT

public:
	Editor(Baryon::Renderer* renderer, QWidget *parent = Q_NULLPTR);

	bool event(QEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void closeEvent(QCloseEvent* event) override;


private:
	void resizeAndRerender(const QSize& size);

	Ui::EditorClass ui;

	Baryon::Renderer* renderer;

	bool resizing = false;
};





