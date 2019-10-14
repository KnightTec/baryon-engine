#pragma once

#include <QtWidgets/QWidget>
#include "ui_Editor.h"

#include "Window.h"

class Editor : public QWidget
{
	Q_OBJECT

public:
	Editor(QWidget *parent = Q_NULLPTR);

private:
	Ui::EditorClass ui;
};
