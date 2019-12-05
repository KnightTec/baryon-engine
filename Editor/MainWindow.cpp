#include "MainWindow.h"
#include <QPalette>
#include <QStyleFactory>

bool BaryonEditorApp::running = true;

void BaryonEditorApp::initialize() {
	setStyle(QStyleFactory::create("Fusion"));

	QPalette darkPalette;
	darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::WindowText, Qt::white);
	darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
	darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
	darkPalette.setColor(QPalette::ToolTipText, Qt::white);
	darkPalette.setColor(QPalette::Text, Qt::white);
	darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::ButtonText, Qt::white);
	darkPalette.setColor(QPalette::BrightText, Qt::red);
	darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::HighlightedText, Qt::black);

	setPalette(darkPalette);
	setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
}


MainWindow::MainWindow(Baryon::Engine* engine)
	: engine(engine), camera(80, 1, 0.01, 1000, { 0, 20, 0 })
{
	dockEditor = new QDockWidget(tr("World View"), this);
	editor = new Editor(engine, dockEditor);
	

	editor->setActiveCamera(&camera);
	dockEditor->setWidget(editor);
	addDockWidget(Qt::RightDockWidgetArea, dockEditor, Qt::Horizontal);
}
void MainWindow::tick()
{
	editor->onResize(editor->size());
}
void MainWindow::closeEvent(QCloseEvent* event)
{
	BaryonEditorApp::running = false;
}
MainWindow::~MainWindow()
{
	delete editor;
	delete dockEditor;
}
