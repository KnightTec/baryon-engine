#include "Editor.h"
#include <QResizeEvent>
#include <QtConcurrent/QtConcurrent>
#include "Renderer.h"


bool BaryonEditorApp::running = true;


Editor::Editor(Baryon::Renderer* renderer, QWidget* parent)
	: QWidget(parent), Window({ 2000, 2000 }), renderer(renderer)
{
	ui.setupUi(this);	
	hwnd = reinterpret_cast<HWND>(ui.frame->winId());
	setMinimumHeight(50);
	setAttribute(Qt::WA_NativeWindow);
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_MSWindowsUseDirect3D);
	setAutoFillBackground(false);
	ui.frame->setGeometry(0, 0, 1920, 1080);
	ui.frame->setAttribute(Qt::WA_NativeWindow);
	ui.frame->setAttribute(Qt::WA_PaintOnScreen);
	ui.frame->setAttribute(Qt::WA_NoSystemBackground);
	ui.frame->setAttribute(Qt::WA_MSWindowsUseDirect3D);
	ui.frame->setAttribute(Qt::WA_ForceUpdatesDisabled);
	ui.frame->setAutoFillBackground(false);
}

bool Editor::event(QEvent* event)
{
	switch (event->type())
	{
		case QEvent::WinIdChange:
			hwnd = reinterpret_cast<HWND>(ui.frame->winId());
			if (screen)
			{
				screen->terminate();
				screen->initialize(*this);
			}
			return true;
		default:
			return QWidget::event(event);
	}
}
void Editor::resizeEvent(QResizeEvent* event)
{
	const QSize& size = event->size();
	onResize(size);
}
void Editor::showEvent(QShowEvent* event)
{
	onResize(size());
}
void Editor::closeEvent(QCloseEvent* event)
{
	event->accept();
	BaryonEditorApp::running = false;
}
void Editor::onResize(const QSize& size)
{
	QSize frameSize = ui.frame->size();

	if (size.width() >= size.height())
	{
		int y = (size.height() - frameSize.height()) / 2;
		ui.frame->setGeometry(0, y, size.width(), size.width());
	}
	else
	{
		int x = (size.width() - frameSize.width()) / 2;
		ui.frame->setGeometry(x, 0, size.height(), size.height());
	}
}
