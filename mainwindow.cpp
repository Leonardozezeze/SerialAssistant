#include "mainwindow.h"
#include "LabeledComboBox.h"
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("Serial Assistant");
    this->resize(800, 600);
    ShowMenu();
    ShowToolBar();


    statusBar()->showMessage("Ready");
}
void MainWindow::ShowMenu()
{
    QMenuBar *menuBar = this->menuBar();
    QMenu *fileMenu = menuBar->addMenu("File");//以后留着设计 先不管考虑功能
}
void MainWindow::ShowToolBar()
{
    QToolBar *SCtoolBar = addToolBar("Serial Config");//串口配置工具栏
    QToolBar *SHCtoolBar = addToolBar("Show Config");//显示配置工具栏

    LabeledComboBox *baudratecombo = new LabeledComboBox("波特率:", this);
    baudratecombo->addItems({"9600", "19200", "115200"});
    SCtoolBar->addWidget(baudratecombo);

}