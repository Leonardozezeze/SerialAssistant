#pragma once

#include <QMainWindow>
#include "LabeledComboBox.h"
#include <QPushButton>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QStringList>
#include <QStringDecoder>
enum class ShowMode { Text, Hex };
enum class Codec { UTF8, GBK };
enum class Endstr {None,CR,NL,CRNL};
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
    void setDecoderForCodec(Codec codec);
    QStringDecoder *m_decoder = nullptr;
    QTimer *autoSaveTimer=nullptr;
    QToolBar *SCtoolBar =nullptr;
    QToolBar *SHCtoolBar =nullptr;
    LabeledComboBox *portcombo = nullptr;
    QPushButton *StartBtn = nullptr;
    LabeledComboBox *baudratecombo = nullptr;
    LabeledComboBox *stopbitscobo = nullptr;
    LabeledComboBox *databitscobo = nullptr;
    LabeledComboBox *paritycobo = nullptr;
    QSerialPort *m_serial = nullptr;
    QTextEdit *recvEdit = nullptr;
    QTextEdit *sendEdit =nullptr;
    QLabel *statusLabel=nullptr;
    ShowMode m_displayMode = ShowMode::Text;
    ShowMode m_sendMode = ShowMode::Text;
    Codec m_receiveCodec = Codec::UTF8;
    Codec m_sendCodec = Codec::UTF8;
    Endstr m_endstr = Endstr::None;
    bool autosave=false;
    bool timestamp=false;
    bool entersend=false;
    bool autoclear=false;
    qint32 m_receivedtimes = 0;
    qint32 m_sendtimes = 0;
private slots:
    void refreshSerialPorts();
    void StartSerialPort(bool checked);
    void Save2File();
    void ImportFromFile();
    void ClearRecArea();
    void Senddata();
    void Clearsendedit();
    void onReadyRead();
    void onAutoSaveFile();
    void Display2Begin();
    void UpdateSR();
    void closeEvent(QCloseEvent *event);
protected:
bool eventFilter(QObject *obj, QEvent *event) override;  // 新增
};
