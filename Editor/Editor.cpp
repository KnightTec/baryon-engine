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
	BaryonEditorApp::instance()->installEventFilter(this);
	setAttribute(Qt::WA_NativeWindow);
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
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

static bool resizing = false;
static bool painting = false;


bool Editor::eventFilter(QObject* watched, QEvent* event)
{
	switch (event->type())
	{
		case QEvent::Resize:
			return resizing;
		case QEvent::Paint:
			return painting;
		default:
			return false;
	}
}
void Editor::resizeEvent(QResizeEvent* event)
{
	resizing = true;
	const QSize& size = event->size();
	QSize frameSize = ui.frame->size();
	int x = (size.width() - frameSize.width()) / 2;
	int y = (size.height() - frameSize.height()) / 2;
	ui.frame->setGeometry(x, y, frameSize.width(), frameSize.height());
	resizeAndRerender(size);
}
void Editor::paintEvent(QPaintEvent* event)
{
	painting = true;
	renderer->render();
	resizing = false;
	painting = false;
}
void Editor::closeEvent(QCloseEvent* event)
{
	event->accept();
	BaryonEditorApp::running = false;
}
void Editor::showEvent(QShowEvent* event)
{
	setResolution({1920, 1080});
}

void Editor::resizeAndRerender(const QSize& size)
{
	//setResolution({ (uint32_t)size.width(), (uint32_t)size.height() });
	//setResolution({ 1000,  1000 });

	int x = size.width();
	int y = size.height();
	//screen->setViewportSize(x, y);
	//screen->getActiveCamera()->setAspectRatio(float(x) / y);
	//renderer->render();
}
