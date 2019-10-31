#include "Editor.h"
#include <QResizeEvent>
#include <QTimer>
#include <QScreen>
#include <QBoxLayout>
#include "Renderer.h"


bool BaryonEditorApp::running = true;


Editor::Editor(Baryon::Renderer* renderer, QWidget* parent)
	: QWidget(parent), Window({(uint32_t)size().width(), (uint32_t)size().height()}), renderer(renderer), resizeTimerId(0)
{
	ui.setupUi(this);
	ui.frame->installEventFilter(this);
	hwnd = reinterpret_cast<HWND>(ui.frame->winId());
	setMinimumHeight(50);
	setAttribute(Qt::WA_NativeWindow);
	//setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(false);
	ui.frame->setAttribute(Qt::WA_NativeWindow);
	//ui.frame->setAttribute(Qt::WA_NoSystemBackground);
	ui.frame->setAutoFillBackground(false);
	ui.frame->setUpdatesEnabled(false);
}

bool Editor::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == ui.frame)
	{
		if (event->type() == QEvent::WinIdChange)
		{
			hwnd = reinterpret_cast<HWND>(ui.frame->winId());
			if (screen)
			{
				screen->terminate();
				screen->initialize(*this);
			}
			return true;
		}
	}
	return false;
}
void Editor::resizeEvent(QResizeEvent* event)
{
	if (resizeTimerId)
	{
		killTimer(resizeTimerId);
		resizeTimerId = 0;
	}
	else
	{
		QScreen* screen = BaryonEditorApp::primaryScreen();
		QSize screenSize = screen->size();
		if (screenSize.width() >= screenSize.height())
		{
			setResolution({(uint32_t)screenSize.width(), (uint32_t)screenSize.width()});
		}
		else
		{
			setResolution({ (uint32_t)screenSize.height(), (uint32_t)screenSize.height() });
		}

		renderer->render();
	}
	resizeTimerId = startTimer(20000);
	const QSize& size = event->size();
	onResize(size);
}
void Editor::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == resizeTimerId)
	{
		const QSize& s = ui.frame->size();
		setResolution({(uint32_t)s.width(), (uint32_t)s.height()});
		ui.frame->setGeometry(0, 0, s.width(), s.height());
		killTimer(resizeTimerId);
		resizeTimerId = 0;
	}
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
