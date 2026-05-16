#include "mainwindow.h"

#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Qt6 Template");

    m_centralLabel = new QLabel("Hello from Qt6!", this);
    m_centralLabel->setAlignment(Qt::AlignCenter);
    setCentralWidget(m_centralLabel);

    auto *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("E&xit", this, &QWidget::close);

    statusBar()->showMessage("Ready");
}
