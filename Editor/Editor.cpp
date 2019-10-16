#include "Editor.h"
#include <QResizeEvent>
#include <QtConcurrent/QtConcurrent>
#include "Renderer.h"


bool BaryonEditorApp::running = true;


Editor::Editor(Baryon::Renderer* renderer, QWidget* parent)
	: QWidget(parent), Window({(uint32_t)size().width(), (uint32_t)size().height()}), renderer(renderer)
{
	ui.setupUi(this);
	hwnd = reinterpret_cast<HWND>(ui.frame->winId());
}

bool Editor::event(QEvent* event)
{
	switch (event->type())
	{
	case QEvent::WinIdChange:
		hwnd = reinterpret_cast<HWND>(ui.frame->winId());
		screen->terminate();
		screen->initialize(*this);
		return true;
	default:
		return QWidget::event(event);
	}
}


void Editor::resizeEvent(QResizeEvent* event)
{
	const QSize& size = event->size();
	ui.frame->resize(size);
	event->accept();
	if (!resizing) {
		resizing = true;
		QtConcurrent::run(this, &Editor::resizeAndRerender, size);
	}

}
void Editor::closeEvent(QCloseEvent* event)
{
	event->accept();
	BaryonEditorApp::running = false;
}

void Editor::resizeAndRerender(const QSize& size)
{
	setResolution({ (uint32_t)size.width(), (uint32_t)size.height() });
	renderer->render();
	resizing = false;
}

