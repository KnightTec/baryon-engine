#pragma once

#include <QtWidgets/QWidget>
#include "ui_Editor.h"

#include "Window.h"
#include "Engine.h"


class Editor : public QWidget, public Baryon::Window
{
	Q_OBJECT

public:
	Editor(Baryon::Engine* engine, QWidget *parent = Q_NULLPTR);
	bool eventFilter(QObject* watched, QEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void timerEvent(QTimerEvent* event) override;
	void showEvent(QShowEvent* event) override;
	void closeEvent(QCloseEvent* event) override;

	void onResize(const QSize& size);
private:
	Ui::EditorClass ui;
	Baryon::Engine* engine;
	int resizeTimerId;
};







