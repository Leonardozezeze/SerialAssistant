#pragma once

#include <QMainWindow>

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;
private:
    void ShowMenu();
    void ShowToolBar();
    void Receive_Area();
    void Show_StatusBar();
protected:

};
