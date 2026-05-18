#include "mainwindow.h"
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include <QCheckBox>
#include <QLineEdit>
#include <QIntValidator>
#include <QFrame>
#include <QStringEncoder>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("Serial Assistant");
    this->resize(1000, 600);
    Receive_Area();
    ShowMenu();
    ShowToolBar();
    Show_StatusBar();
    refreshSerialPorts();
    m_serial = new QSerialPort(this);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::onReadyRead);
    QTimer *portTimer = new QTimer(this);
    connect(portTimer, &QTimer::timeout, this, &MainWindow::refreshSerialPorts);
    portTimer->start(2000);
    autoSaveTimer = new QTimer(this);
    autoSaveTimer->setInterval(5 * 60 * 1000); // 5分钟
    connect(autoSaveTimer, &QTimer::timeout, this, &MainWindow::onAutoSaveFile);
    
    // 初始化解码器为UTF-8
    setDecoderForCodec(Codec::UTF8);
}

void MainWindow::ShowMenu()
{
    QMenuBar *menuBar = this->menuBar();
    QMenu *fileMenu = menuBar->addMenu("文件");
    QAction *saveAction = fileMenu->addAction("保存接收区");
    QAction *importAction = fileMenu->addAction("导入文件");
    QMenu *seeMenu = menuBar->addMenu("查看");
    QAction *DefautPlay = seeMenu->addAction("恢复默认布局");
    QAction *helpAction = menuBar->addAction("帮助");
    QAction *aboutAction = menuBar->addAction("关于");
    connect(saveAction, &QAction::triggered, this, &MainWindow::Save2File);
    connect(importAction, &QAction::triggered, this, &MainWindow::ImportFromFile);
    connect(DefautPlay, &QAction::triggered, this, &MainWindow::Display2Begin);
    connect(helpAction, &QAction::triggered, this, [this]()
            {
    QMessageBox::about(this, "帮助", "教程文档正在制作ing....."); });
    connect(aboutAction, &QAction::triggered, this, [this]()
            {
    QMessageBox::about(this, "关于", "串口助手 v1.0\n制作:Cassian"); });
}

void MainWindow::ShowToolBar()
{
    SCtoolBar = addToolBar("Serial Config");
    addToolBar(Qt::TopToolBarArea, SCtoolBar);
    SHCtoolBar = addToolBar("Show Config");
    addToolBar(Qt::LeftToolBarArea, SHCtoolBar);

    QWidget *SCcontainer = new QWidget(this);
    QBoxLayout *SCLayout = new QBoxLayout(QBoxLayout::LeftToRight, SCcontainer);
    SCLayout->setContentsMargins(0, 0, 0, 0);
    SCLayout->setSpacing(25);
    SCtoolBar->addWidget(SCcontainer);

    portcombo = new LabeledComboBox("串口:", this);
    SCLayout->addWidget(portcombo);

    baudratecombo = new LabeledComboBox("波特率:", this);
    baudratecombo->addItems({"1200", "4800", "9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600", "自定义..."});
    SCLayout->addWidget(baudratecombo);
    baudratecombo->comboBox()->setCurrentIndex(2);

    stopbitscobo = new LabeledComboBox("停止位:", this);
    stopbitscobo->addItems({"1", "1.5", "2"});
    SCLayout->addWidget(stopbitscobo);

    databitscobo = new LabeledComboBox("数据位:", this);
    databitscobo->addItems({"5", "6", "7", "8"});
    databitscobo->comboBox()->setCurrentIndex(3);
    SCLayout->addWidget(databitscobo);

    paritycobo = new LabeledComboBox("校验位:", this);
    paritycobo->addItems({"None", "Even", "Odd"});
    SCLayout->addWidget(paritycobo);

    StartBtn = new QPushButton(this);
    StartBtn->setCheckable(true);
    StartBtn->setChecked(false);
    QIcon iconBlack(":/blackicon.png");
    StartBtn->setText("  打开串口");
    StartBtn->setIcon(iconBlack);
    StartBtn->setIconSize(QSize(35, 20));
    SCLayout->addWidget(StartBtn);

    QWidget *SHCcontainer = new QWidget(this);
    QBoxLayout *SHCLayout = new QBoxLayout(QBoxLayout::TopToBottom, SHCcontainer);
    SHCLayout->setContentsMargins(0, 0, 0, 0);
    SHCLayout->setSpacing(6);
    SHCtoolBar->addWidget(SHCcontainer);

    QLabel *showlabel = new QLabel("显示配置:", this);
    SHCLayout->addWidget(showlabel);
    QFont font = showlabel->font();
    font.setPointSize(9);
    font.setBold(true);
    showlabel->setFont(font);

    LabeledComboBox *showmcobo = new LabeledComboBox("显示形式:", this);
    showmcobo->addItems({"文本", "Hex"});
    SHCLayout->addWidget(showmcobo);
    showmcobo->comboBox()->setCurrentIndex(0);

    LabeledComboBox *showcodecobo = new LabeledComboBox("编码:", this);
    showcodecobo->addItems({"UTF-8", "GBK"});
    SHCLayout->addWidget(showcodecobo);
    showcodecobo->comboBox()->setCurrentIndex(0);

    QPushButton *saveBtn = new QPushButton("保存接收", this);
    SHCLayout->addWidget(saveBtn);

    QCheckBox *RTScheckBox = new QCheckBox("RTS", this);
    RTScheckBox->setChecked(false);
    SHCLayout->addWidget(RTScheckBox);

    QCheckBox *DTRcheckBox = new QCheckBox("DTR", this);
    DTRcheckBox->setChecked(false);
    SHCLayout->addWidget(DTRcheckBox);

    QCheckBox *savecheckBox = new QCheckBox("自动保存", this);
    savecheckBox->setChecked(false);
    SHCLayout->addWidget(savecheckBox);

    QCheckBox *timecheckBox = new QCheckBox("时间戳", this);
    timecheckBox->setChecked(false);
    SHCLayout->addWidget(timecheckBox);

    QPushButton *clearrecvBtn = new QPushButton("清空接收", this);
    SHCLayout->addWidget(clearrecvBtn);

    QFrame *hline = new QFrame;
    hline->setFrameShape(QFrame::HLine);
    hline->setFrameShadow(QFrame::Sunken);
    SHCLayout->addWidget(hline);

    QLabel *sendlabel = new QLabel("发送配置:", this);
    SHCLayout->addWidget(sendlabel);
    font = sendlabel->font();
    font.setPointSize(9);
    font.setBold(true);
    sendlabel->setFont(font);

    LabeledComboBox *sendmcobo = new LabeledComboBox("发送形式:", this);
    sendmcobo->addItems({"文本", "Hex"});
    SHCLayout->addWidget(sendmcobo);
    sendmcobo->comboBox()->setCurrentIndex(0);

    LabeledComboBox *sendcodecobo = new LabeledComboBox("编码:", this);
    sendcodecobo->addItems({"UTF-8", "GBK"});
    SHCLayout->addWidget(sendcodecobo);
    sendcodecobo->comboBox()->setCurrentIndex(0);

    LabeledComboBox *sendendcobo = new LabeledComboBox("结束符:", this);
    sendendcobo->addItems({"None", "\\n+\\r", "\\n", "\\r"});
    SHCLayout->addWidget(sendendcobo);
    sendendcobo->comboBox()->setCurrentIndex(0);

    QCheckBox *entercheckBox = new QCheckBox("回车发送", this);
    entercheckBox->setChecked(false);
    SHCLayout->addWidget(entercheckBox);

    QCheckBox *clearcheckBox = new QCheckBox("自动清空", this);
    clearcheckBox->setChecked(false);
    SHCLayout->addWidget(clearcheckBox);

    QPushButton *FileBtn = new QPushButton("从文件导入", this);
    SHCLayout->addWidget(FileBtn);

    QPushButton *advancesendBtn = new QPushButton("高级发送", this);
    SHCLayout->addWidget(advancesendBtn);

    // 连接部分
    connect(baudratecombo->comboBox(), &QComboBox::currentTextChanged,
            this, [this](const QString &text)
            {
        if (text == "自定义...") {
            bool ok;
            QString customBaudrate = QInputDialog::getText(this, 
                "自定义波特率", 
                "请输入波特率值:",
                QLineEdit::Normal,
                "115200",
                &ok);
            
            if (ok && !customBaudrate.isEmpty()) {
                bool isNumber;
                customBaudrate.toInt(&isNumber);
                
                if (isNumber) {
                    int index = baudratecombo->comboBox()->count() - 1;
                    baudratecombo->comboBox()->insertItem(index, customBaudrate);
                    baudratecombo->comboBox()->setCurrentText(customBaudrate);
                    qDebug() << "设置自定义波特率:" << customBaudrate;
                } else {
                    QMessageBox::warning(this, "错误", "请输入有效的数字！");
                    baudratecombo->comboBox()->setCurrentIndex(2);
                }
            } else {
                baudratecombo->comboBox()->setCurrentIndex(2);
            }
        } else {
            qDebug() << "波特率改变为:" << text;
        } });

    connect(StartBtn, &QPushButton::clicked, this, &MainWindow::StartSerialPort);
    connect(showmcobo->comboBox(), &QComboBox::currentTextChanged, this, [this](const QString &text)
            {
        if(text=="文本")
            m_displayMode=ShowMode::Text;
        if(text=="Hex")
            m_displayMode=ShowMode::Hex; });

    // 修正：UTF-8 判断，并同步切换解码器
    connect(showcodecobo->comboBox(), &QComboBox::currentTextChanged, this, [this](const QString &text)
            {
        if(text=="UTF-8") {
            m_receiveCodec=Codec::UTF8;
            setDecoderForCodec(Codec::UTF8);
        }
        if(text=="GBK") {
            m_receiveCodec=Codec::GBK;
            setDecoderForCodec(Codec::GBK);
        } });

    connect(sendmcobo->comboBox(), &QComboBox::currentTextChanged, this, [this](const QString &text)
            {
        if(text=="文本")
            m_sendMode=ShowMode::Text;
        if(text=="Hex")
            m_sendMode=ShowMode::Hex; });

    connect(sendcodecobo->comboBox(), &QComboBox::currentTextChanged, this, [this](const QString &text)
            {
        if(text=="UTF-8")
            m_sendCodec=Codec::UTF8;
        if(text=="GBK")
            m_sendCodec=Codec::GBK; });

    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::Save2File);
    connect(clearrecvBtn, &QPushButton::clicked, this, &MainWindow::ClearRecArea);
    connect(FileBtn, &QPushButton::clicked, this, &MainWindow::ImportFromFile);
    
    connect(RTScheckBox, &QCheckBox::toggled, this, [this](bool checked)
            {
    if (m_serial && m_serial->isOpen()) {
        m_serial->setRequestToSend(checked);
        qDebug() << "RTS 设置为:" << (checked ? "高电平" : "低电平");
    } });
    
    connect(DTRcheckBox, &QCheckBox::toggled, this, [this](bool checked)
            {
    if (m_serial && m_serial->isOpen()) {
        m_serial->setDataTerminalReady(checked);
        qDebug() << "DTR 设置为:" << (checked ? "高电平" : "低电平");
    } });
    
    connect(savecheckBox, &QCheckBox::toggled, this, [this](bool checked)
            {
        autosave = checked;
        qDebug() << "自动保存:" << (checked ? "启用" : "禁用"); 
            if (checked) {
        autoSaveTimer->start();
    } else {
        autoSaveTimer->stop();
    } });
    
    connect(timecheckBox, &QCheckBox::toggled, this, [this](bool checked)
            {
        timestamp=checked;
        qDebug()<<"时间戳:"<< (checked ? "启用" : "禁用"); });
    
    connect(entercheckBox, &QCheckBox::toggled, this, [this](bool checked)
            {
        entersend=checked;
        qDebug()<<"回车发送:"<< (checked ? "启用" : "禁用"); });
    
    connect(clearcheckBox, &QCheckBox::toggled, this, [this](bool checked)
            {
        autoclear=checked;
        qDebug()<<"自动清空:"<< (checked ? "启用" : "禁用"); });
    
    connect(sendendcobo->comboBox(), &QComboBox::currentTextChanged, this, [this](const QString &text)
            {
        if(text=="None")
            m_endstr=Endstr::None;
        if(text=="\\n+\\r")
            m_endstr=Endstr::CRNL;
        if(text=="\\n")
            m_endstr=Endstr::NL;
        if(text=="\\r")
            m_endstr=Endstr::CR; });
}

void MainWindow::Receive_Area()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    QLabel *recvLabel = new QLabel("接收区:", this);
    recvEdit = new QTextEdit(this);
    recvEdit->setReadOnly(true);
    recvEdit->setPlaceholderText("接收到的数据会显示在这里...");
    QLabel *sendLabel = new QLabel("发送区:", this);
    sendEdit = new QTextEdit(this);
    sendEdit->setPlaceholderText("输入要发送的数据...");
    sendEdit->setMaximumHeight(120);

    QPushButton *sendBtn = new QPushButton("发送", this);
    QPushButton *clearBtn = new QPushButton("清空发送", this);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(clearBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(sendBtn);

    layout->addWidget(recvLabel);
    layout->addWidget(recvEdit);
    layout->addWidget(sendLabel);
    layout->addWidget(sendEdit);
    layout->addLayout(btnLayout);

    connect(sendBtn, &QPushButton::clicked, this, &MainWindow::Senddata);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::Clearsendedit);
}

void MainWindow::Show_StatusBar()
{
    QStatusBar *statusBar = this->statusBar();
    QWidget *container = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    QLabel *readyLabel = new QLabel("Ready", this);
    statusLabel = new QLabel(this);
    UpdateSR();
    QLabel *timeLabel = new QLabel(this);

    auto createVLine = []() -> QFrame *
    {
        QFrame *line = new QFrame;
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setStyleSheet("QFrame { background: gray; }");
        line->setFixedWidth(2);
        return line;
    };
    QFrame *vline1 = createVLine();
    QFrame *vline2 = createVLine();

    readyLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    timeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    vline1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    vline2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    layout->addStretch();
    layout->addWidget(readyLabel);
    layout->addStretch();
    layout->addWidget(vline1);
    layout->addStretch();
    layout->addWidget(statusLabel);
    layout->addStretch();
    layout->addWidget(vline2);
    layout->addStretch();
    layout->addWidget(timeLabel);
    layout->addStretch();

    statusBar->addWidget(container, 1);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]()
            {
        QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        timeLabel->setText("当前时间:" + currentTime); });
    timer->start(1000);
    timeLabel->setText("当前时间:" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

void MainWindow::refreshSerialPorts()
{
    QComboBox *combo = portcombo->comboBox();
    if (!combo)
        return;

    const auto infos = QSerialPortInfo::availablePorts();
    QSet<QString> availablePorts;
    for (const auto &info : infos)
        availablePorts.insert(info.portName());

    bool oldBlocked = combo->blockSignals(true);

    for (int i = combo->count() - 1; i >= 0; --i)
    {
        if (!availablePorts.contains(combo->itemData(i).toString()))
        {
            if (m_serial->isOpen() && m_serial->portName() == combo->itemData(i).toString())
            {
                m_serial->close();
                QIcon iconBlack(":/blackicon.png");
                StartBtn->setChecked(false);
                StartBtn->setText("  打开串口");
                StartBtn->setIcon(iconBlack);
                StartBtn->setIconSize(QSize(35, 20));
                // ✅ 恢复串口配置控件
                portcombo->comboBox()->setEnabled(true);
                baudratecombo->comboBox()->setEnabled(true);
                stopbitscobo->comboBox()->setEnabled(true);
                databitscobo->comboBox()->setEnabled(true);
                paritycobo->comboBox()->setEnabled(true);
                UpdateSR();
            }
            combo->removeItem(i);
        }
    }
    for (const auto &info : infos)
    {
        if (combo->findData(info.portName()) == -1)
        {
            combo->addItem(info.portName() + " " + info.description(), info.portName());
        }
    }

    if (combo->currentIndex() == -1 && combo->count() > 0)
    {
        combo->setCurrentIndex(0);
    }
    combo->blockSignals(oldBlocked);
}

void MainWindow::StartSerialPort(bool checked)
{
    if (checked)
    {
        QIcon iconRed(":/redicon.png");

        QString portName = portcombo->comboBox()->currentData().toString();
        qint32 baud = baudratecombo->comboBox()->currentText().toInt();

        QString stopbitstr = stopbitscobo->comboBox()->currentText();
        QSerialPort::StopBits stopBits;
        if (stopbitstr == "1")
            stopBits = QSerialPort::OneStop;
        else if (stopbitstr == "1.5")
            stopBits = QSerialPort::OneAndHalfStop;
        else if (stopbitstr == "2")
            stopBits = QSerialPort::TwoStop;

        QString databitstr = databitscobo->comboBox()->currentText();
        QSerialPort::DataBits dataBits;
        if (databitstr == "5")
            dataBits = QSerialPort::Data5;
        else if (databitstr == "6")
            dataBits = QSerialPort::Data6;
        else if (databitstr == "7")
            dataBits = QSerialPort::Data7;
        else if (databitstr == "8")
            dataBits = QSerialPort::Data8;

        QString paritystr = paritycobo->comboBox()->currentText();
        QSerialPort::Parity parity;
        if (paritystr == "None")
            parity = QSerialPort::NoParity;
        else if (paritystr == "Even")
            parity = QSerialPort::EvenParity;
        else if (paritystr == "Odd")
            parity = QSerialPort::OddParity;

        // 先配置所有参数，再打开串口
        m_serial->setPortName(portName);
        m_serial->setBaudRate(baud);
        m_serial->setStopBits(stopBits);
        m_serial->setDataBits(dataBits);
        m_serial->setParity(parity);

        if (m_serial->open(QIODevice::ReadWrite))
        {
            StartBtn->setText("  关闭串口");
            StartBtn->setIcon(iconRed);
            StartBtn->setIconSize(QSize(35, 20));
            UpdateSR();
            // ✅ 禁用串口配置控件
            portcombo->comboBox()->setEnabled(false);
            baudratecombo->comboBox()->setEnabled(false);
            stopbitscobo->comboBox()->setEnabled(false);
            databitscobo->comboBox()->setEnabled(false);
            paritycobo->comboBox()->setEnabled(false);
        }
        else
        {
            QMessageBox::critical(this, "错误", "无法打开串口 " + portName);
            StartBtn->setChecked(false);
        }
    }
    else
    {
        m_serial->close();
        QIcon iconBlack(":/blackicon.png");
        StartBtn->setText("  打开串口");
        StartBtn->setIcon(iconBlack);
        StartBtn->setIconSize(QSize(35, 20));
        UpdateSR();
         // ✅ 恢复串口配置控件
        portcombo->comboBox()->setEnabled(true);
        baudratecombo->comboBox()->setEnabled(true);
        stopbitscobo->comboBox()->setEnabled(true);
        databitscobo->comboBox()->setEnabled(true);
        paritycobo->comboBox()->setEnabled(true);
    }
}

void MainWindow::Save2File()
{
    if (!recvEdit || recvEdit->toPlainText().isEmpty())
    {
        QMessageBox::information(this, "提示", "接收区没有数据可保存");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, "保存接收数据", QDir::homePath(), "文本文件 (*.txt);;所有文件 (*)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        // 明确设定编码为 UTF-8，并写入 BOM（可选）
        stream.setEncoding(QStringConverter::Utf8);
        stream.setGenerateByteOrderMark(true);  // 写入 UTF-8 BOM
        stream << recvEdit->toPlainText();
        file.close();
        QMessageBox::information(this, "成功", "文件已保存（UTF-8 编码）");
    }
    else
    {
        QMessageBox::critical(this, "错误", "无法创建文件：" + file.errorString());
    }
}

void MainWindow::ImportFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "选择文件",
                                                    QDir::homePath(),
                                                    "文本文件 (*.txt);;所有文件 (*)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QString content = stream.readAll();
        file.close();
        sendEdit->setPlainText(content);
    }
    else
    {
        QMessageBox::critical(this, "错误", "无法打开文件：" + file.errorString());
    }
}

void MainWindow::ClearRecArea()
{
    recvEdit->clear();
}

void MainWindow::Senddata()
{
    if (!m_serial || !m_serial->isOpen())
    {
        qDebug() << "串口未打开，不能发送";
        return;
    }
    QString text = sendEdit->toPlainText();
    if (text.isEmpty())
        return;

    QByteArray data;

    if (m_sendMode == ShowMode::Text)
    {
        if (m_sendCodec == Codec::UTF8)
        {
            data = text.toUtf8();
        }
        else
        {
            data = text.toLocal8Bit();
        }
    }
    else
    {
        QString hexStr = text.simplified();
        hexStr.remove(' ');
        data = QByteArray::fromHex(hexStr.toLatin1());
        if (data.isEmpty() && !hexStr.isEmpty())
        {
            QMessageBox::warning(this, "错误", "无效的十六进制输入");
            return;
        }
    }

    switch (m_endstr)
    {
    case Endstr::CR:
        data.append('\r');
        break;
    case Endstr::NL:
        data.append('\n');
        break;
    case Endstr::CRNL:
        data.append("\r\n");
        break;
    default:
        break;
    }

    qint64 written = m_serial->write(data);
    if (written == -1)
    {
        QMessageBox::critical(this, "错误", "发送失败：" + m_serial->errorString());
        return;
    }

    QString displayText;
    if (m_sendMode == ShowMode::Text)
    {
        displayText = text;
    }
    else
    {
        displayText = data.toHex(' ').toUpper();
    }

    if (timestamp)
    {
        QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        displayText = "[" + timeStr + "] " + displayText;
    }

    recvEdit->append("[TX] " + displayText);
    recvEdit->moveCursor(QTextCursor::End);

    qDebug() << "发送了" << written << "字节";
    m_sendtimes++;
    UpdateSR();

    if (autoclear)
    {
        sendEdit->clear();
    }
}

void MainWindow::Clearsendedit()
{
    sendEdit->clear();
}

void MainWindow::onReadyRead()
{
    if (!m_serial || !m_serial->isOpen())
        return;
    QByteArray data = m_serial->readAll();
    if (data.isEmpty())
        return;

    QString displayText;
    if (m_displayMode == ShowMode::Text)
    {
        // 使用QStringDecoder处理可能的分片数据，避免乱码
        if (m_decoder)
            displayText = m_decoder->decode(data);   // Qt6标准方法
        else
            displayText = QString::fromUtf8(data);   // fallback
    }
    else
    {
        displayText = data.toHex(' ').toUpper();
    }

    if (timestamp)
    {
        QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        displayText = "[" + timeStr + "] " + displayText;
    }

    recvEdit->append("[RX] " + displayText);
    m_receivedtimes++;
    UpdateSR();
    recvEdit->moveCursor(QTextCursor::End);
}

void MainWindow::onAutoSaveFile()
{
    if (!autosave)
        return;
    if (!recvEdit || recvEdit->toPlainText().isEmpty())
        return;

    QString saveDir = QApplication::applicationDirPath() + "/AutoSaveFiles/";
    QDir dir;
    if (!dir.mkpath(saveDir))
    {
        qWarning() << "无法创建自动保存目录:" << saveDir;
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString fileName = saveDir + QString("autosave_received_%1.txt").arg(timestamp);

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        // 显式设置为 UTF-8 编码，并写入 BOM（字节顺序标记）
        stream.setEncoding(QStringConverter::Utf8);
        stream.setGenerateByteOrderMark(true);
        stream << recvEdit->toPlainText();
        file.close();
        qDebug() << "自动保存成功（UTF-8）:" << fileName;
    }
    else
    {
        qWarning() << "自动保存失败:" << file.errorString();
    }
}

void MainWindow::Display2Begin()
{
    addToolBar(Qt::TopToolBarArea, SCtoolBar);
    addToolBar(Qt::LeftToolBarArea, SHCtoolBar);
}

void MainWindow::UpdateSR()
{
    statusLabel->setText(QString("串口状态: %1       ").arg(m_serial && m_serial->isOpen() ? "已连接" : "未连接") + "R:" + QString::number(m_receivedtimes) + "       S:" + QString::number(m_sendtimes));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_serial && m_serial->isOpen())
    {
        m_serial->close();
    }
    delete m_decoder;
    m_decoder = nullptr;
    event->accept();
}

void MainWindow::setDecoderForCodec(Codec codec)
{
    if (m_decoder) {
        delete m_decoder;
        m_decoder = nullptr;
    }
    if (codec == Codec::UTF8) {
        m_decoder = new QStringDecoder(QStringDecoder::Utf8);
    } else { // GBK
        // 尝试通过名称创建 GBK 解码器
        m_decoder = new QStringDecoder("GBK");
        if (!m_decoder->isValid()) {
            // GBK 不可用，尝试 GB18030
            delete m_decoder;
            m_decoder = new QStringDecoder("GB18030");
            if (!m_decoder->isValid()) {
                // 如果也不可用，回退到系统编码（Windows 下通常为 GBK）
                delete m_decoder;
                m_decoder = new QStringDecoder(QStringDecoder::System);
            }
        }
    }
}