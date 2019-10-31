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

	bool eventFilter(QObject* watched, QEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void timerEvent(QTimerEvent* event) override;
	void showEvent(QShowEvent* event) override;
	void closeEvent(QCloseEvent* event) override;

	void onResize(const QSize& size);
private:
	Ui::EditorClass ui;
	Baryon::Renderer* renderer;
	int resizeTimerId;
};







