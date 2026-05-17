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
    this->resize(800, 600);
    Receive_Area();
    ShowMenu();
    ShowToolBar();
    Show_StatusBar();
    refreshSerialPorts();
    m_serial = new QSerialPort(this);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::onReadyRead);
    QTimer *portTimer = new QTimer(this);
    connect(portTimer, &QTimer::timeout, this, &MainWindow::refreshSerialPorts);
    portTimer->start(2000); // 每2秒刷新一次
    autoSaveTimer = new QTimer(this);
    autoSaveTimer->setInterval(60); // 每5分钟触发一次，可按需调整
    connect(autoSaveTimer, &QTimer::timeout, this, &MainWindow::onAutoSaveFile);
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
    // 显示关于对话框
    QMessageBox::about(this, "帮助", "教程文档正在制作ing....."); });
    connect(aboutAction, &QAction::triggered, this, [this]()
            {
    // 显示关于对话框
    QMessageBox::about(this, "关于", "串口助手 v1.0\n制作:Cassian"); });
}
void MainWindow::ShowToolBar()
{
    SCtoolBar = addToolBar("Serial Config"); // 串口配置工具栏
    addToolBar(Qt::TopToolBarArea, SCtoolBar);
    SHCtoolBar = addToolBar("Show Config"); // 显示配置工具栏
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
    StartBtn->setCheckable(true); // 启用开关状态
    StartBtn->setChecked(false);  // 初始状态: 关闭(false) 或 开启(true)
    QIcon iconBlack(":/blackicon.png");
    // 设置初始图标和文本
    StartBtn->setText("  打开串口"); // 文本与图标之间的空格可自行调整
    StartBtn->setIcon(iconBlack);
    StartBtn->setIconSize(QSize(35, 20)); // 根据图标实际尺寸调整
    SCLayout->addWidget(StartBtn);

    QWidget *SHCcontainer = new QWidget(this);
    QBoxLayout *SHCLayout = new QBoxLayout(QBoxLayout::TopToBottom, SHCcontainer);
    SHCLayout->setContentsMargins(0, 0, 0, 0);
    SHCLayout->setSpacing(6);
    SHCtoolBar->addWidget(SHCcontainer);

    QLabel *showlabel = new QLabel("显示配置:", this);
    SHCLayout->addWidget(showlabel);
    QFont font = showlabel->font(); // 获取当前字体
    font.setPointSize(9);           // 设置字号（单位：磅）
    font.setBold(true);             // 设置加粗
    showlabel->setFont(font);       // 应用字体设置

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
    RTScheckBox->setChecked(false); // 初始未选中
    SHCLayout->addWidget(RTScheckBox);

    QCheckBox *DTRcheckBox = new QCheckBox("DTR", this);
    DTRcheckBox->setChecked(false); // 初始未选中
    SHCLayout->addWidget(DTRcheckBox);

    QCheckBox *savecheckBox = new QCheckBox("自动保存", this);
    savecheckBox->setChecked(false); // 初始未选中
    SHCLayout->addWidget(savecheckBox);

    QCheckBox *timecheckBox = new QCheckBox("时间戳", this);
    timecheckBox->setChecked(false); // 初始未选中
    SHCLayout->addWidget(timecheckBox);

    QPushButton *clearrecvBtn = new QPushButton("清空接收", this);
    SHCLayout->addWidget(clearrecvBtn);

    QFrame *hline = new QFrame;
    hline->setFrameShape(QFrame::HLine);
    hline->setFrameShadow(QFrame::Sunken); // 可选阴影效果
    SHCLayout->addWidget(hline);

    QLabel *sendlabel = new QLabel("发送配置:", this);
    SHCLayout->addWidget(sendlabel);
    font = sendlabel->font(); // 获取当前字体
    font.setPointSize(9);     // 设置字号（单位：磅）
    font.setBold(true);       // 设置加粗
    sendlabel->setFont(font); // 应用字体设置

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
    entercheckBox->setChecked(false); // 初始未选中
    SHCLayout->addWidget(entercheckBox);

    QCheckBox *clearcheckBox = new QCheckBox("自动清空", this);
    clearcheckBox->setChecked(false); // 初始未选中
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
            // 弹出输入对话框
            bool ok;
            QString customBaudrate = QInputDialog::getText(this, 
                "自定义波特率", 
                "请输入波特率值:",
                QLineEdit::Normal,
                "115200",
                &ok);
            
            if (ok && !customBaudrate.isEmpty()) {
                // 验证是否为数字
                bool isNumber;
                customBaudrate.toInt(&isNumber);
                
                if (isNumber) {
                    // 添加到下拉框（在"自定义..."前面插入）
                    int index = baudratecombo->comboBox()->count() - 1;
                    baudratecombo->comboBox()->insertItem(index, customBaudrate);
                    baudratecombo->comboBox()->setCurrentText(customBaudrate);
                    qDebug() << "设置自定义波特率:" << customBaudrate;
                } else {
                    QMessageBox::warning(this, "错误", "请输入有效的数字！");
                    // 恢复上次的选择
                    baudratecombo->comboBox()->setCurrentIndex(2);
                }
            } else {
                // 用户取消，恢复默认选择
                baudratecombo->comboBox()->setCurrentIndex(2);
            }
        } else {
            // 正常波特率处理
            qDebug() << "波特率改变为:" << text;
        } });
    connect(StartBtn, &QPushButton::clicked, this, &MainWindow::StartSerialPort);
    connect(showmcobo->comboBox(), &QComboBox::currentTextChanged, this, [this](const QString &text)
            {
        if(text=="文本")
            m_displayMode=ShowMode::Text;
        if(text=="Hex")
            m_displayMode=ShowMode::Hex; });
    connect(showcodecobo->comboBox(), &QComboBox::currentTextChanged, this, [this](const QString &text)
            {
        if(text=="UTF8")
            m_receiveCodec=Codec::UTF8;
        if(text=="GBK")
            m_receiveCodec=Codec::GBK; });
    connect(sendmcobo->comboBox(), &QComboBox::currentTextChanged, this, [this](const QString &text)
            {
        if(text=="文本")
            m_sendMode=ShowMode::Text;
        if(text=="Hex")
            m_sendMode=ShowMode::Hex; });
    connect(sendcodecobo->comboBox(), &QComboBox::currentTextChanged, this, [this](const QString &text)
            {
        if(text=="UTF8")
            m_sendCodec=Codec::UTF8;
        if(text=="GBK")
            m_sendCodec=Codec::GBK; });
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::Save2File);
    connect(clearrecvBtn, &QPushButton::clicked, this, &MainWindow::ClearRecArea);
    connect(FileBtn, &QPushButton::clicked, this, &MainWindow::ImportFromFile);
    // RTS 复选框
    connect(RTScheckBox, &QCheckBox::toggled, this, [this](bool checked)
            {
    if (m_serial && m_serial->isOpen()) {
        m_serial->setRequestToSend(checked);
        qDebug() << "RTS 设置为:" << (checked ? "高电平" : "低电平");
    } });
    // DTR 复选框
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
        autoSaveTimer->start();   // 启动定时器
    } else {
        autoSaveTimer->stop();    // 停止定时器
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
    // 主布局
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    // 接收区
    QLabel *recvLabel = new QLabel("接收区:", this);
    recvEdit = new QTextEdit(this);
    recvEdit->setReadOnly(true);
    recvEdit->setPlaceholderText("接收到的数据会显示在这里...");
    // 发送区
    QLabel *sendLabel = new QLabel("发送区:", this);
    sendEdit = new QTextEdit(this);
    sendEdit->setPlaceholderText("输入要发送的数据...");
    sendEdit->setMaximumHeight(120);

    QPushButton *sendBtn = new QPushButton("发送", this);
    QPushButton *clearBtn = new QPushButton("清空发送", this);

    // 按钮布局
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(clearBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(sendBtn);

    // 添加到主布局
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
    layout->setContentsMargins(0, 0, 0, 0); // 移除内边距，使布局紧凑
    layout->setSpacing(0);                  // 先将间距设0，用stretch控制距离
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // 创建三个标签
    QLabel *readyLabel = new QLabel("Ready", this);
    QLabel *statusLabel = new QLabel("R:    S:    ", this);
    QLabel *timeLabel = new QLabel(this);

    // 创建两条竖线（灰色阴影效果）
    auto createVLine = []() -> QFrame *
    {
        QFrame *line = new QFrame;
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);                // 阴影效果（凹陷）
        line->setStyleSheet("QFrame { background: gray; }"); // 确保灰色
        line->setFixedWidth(2);                              // 宽度加一点更明显
        return line;
    };
    QFrame *vline1 = createVLine();
    QFrame *vline2 = createVLine();

    // 为所有需要拉伸的控件设置大小策略（水平方向可拉伸）
    readyLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    timeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // 竖线固定大小，不拉伸
    vline1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    vline2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    // 按顺序添加到布局，并在每一项周围添加弹性空间，实现等距分布
    layout->addStretch(); // 左端弹性空间
    layout->addWidget(readyLabel);
    layout->addStretch(); // 标签1与竖线1之间的弹性空间
    layout->addWidget(vline1);
    layout->addStretch(); // 竖线1与标签2之间的弹性空间
    layout->addWidget(statusLabel);
    layout->addStretch(); // 标签2与竖线2之间的弹性空间
    layout->addWidget(vline2);
    layout->addStretch(); // 竖线2与标签3之间的弹性空间
    layout->addWidget(timeLabel);
    layout->addStretch(); // 右端弹性空间

    statusBar->addWidget(container, 1);

    // 设置时间更新定时器
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
    // 获取可用串口列表
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    QStringList portList;
    // 遍历列表，提取端口名称（如COM1, ttyUSB0）
    for (const QSerialPortInfo &info : serialPortInfos)
    {
        portList.append(info.portName() + " " + info.description());
        // 存储纯端口名作为用户数据
        portcombo->comboBox()->setItemData(portcombo->comboBox()->count() - 1, info.portName());
    }
    portcombo->addItems(portList);
}
void MainWindow::StartSerialPort(bool checked)
{
    if (checked)
    {
        QIcon iconRed(":/redicon.png");

        // 获取串口名（从 itemData 中取纯端口名）
        QString portName = portcombo->comboBox()->currentData().toString();

        // 波特率
        qint32 baud = baudratecombo->comboBox()->currentText().toInt();

        // 停止位
        QString stopbitstr = stopbitscobo->comboBox()->currentText();
        QSerialPort::StopBits stopBits;
        if (stopbitstr == "1")
            stopBits = QSerialPort::OneStop;
        else if (stopbitstr == "1.5")
            stopBits = QSerialPort::OneAndHalfStop;
        else if (stopbitstr == "2")
            stopBits = QSerialPort::TwoStop;

        // 数据位
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

        // 校验位
        QString paritystr = paritycobo->comboBox()->currentText();
        QSerialPort::Parity parity;
        if (paritystr == "None")
            parity = QSerialPort::NoParity;
        else if (paritystr == "Even")
            parity = QSerialPort::EvenParity;
        else if (paritystr == "Odd")
            parity = QSerialPort::OddParity;
        m_serial->setPortName(portName);
        if (m_serial->open(QIODevice::ReadWrite))
        {
            m_serial->setBaudRate(baud);
            m_serial->setStopBits(stopBits);
            m_serial->setDataBits(dataBits);
            m_serial->setParity(parity);
            // 流控等可以后面再加
            // 设置初始图标和文本
            StartBtn->setText("  关闭串口"); // 文本与图标之间的空格可自行调整
            StartBtn->setIcon(iconRed);
            StartBtn->setIconSize(QSize(35, 20)); // 根据图标实际尺寸调整
        }
        else
        {
            // 5. 如果打开失败，弹窗警告，并让按钮弹回"打开"状态
            QMessageBox::critical(this, "错误", "无法打开串口 " + portName);
            StartBtn->setChecked(false);
        }
    }
    else
    {
        QIcon iconBlack(":/blackicon.png");
        // 设置初始图标和文本
        StartBtn->setText("  打开串口"); // 文本与图标之间的空格可自行调整
        StartBtn->setIcon(iconBlack);
        StartBtn->setIconSize(QSize(35, 20)); // 根据图标实际尺寸调整
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
        return; // 用户取消
    // 将接收区纯文本写入文件
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << recvEdit->toPlainText();
        file.close();
        QMessageBox::information(this, "成功", "文件已保存");
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

    // 1. 根据发送模式转换
    if (m_sendMode == ShowMode::Text)
    {
        if (m_sendCodec == Codec::UTF8)
        {
            data = text.toUtf8();
        }
        else
        {                              // GBK
            data = text.toLocal8Bit(); // Windows下为GBK
        }
    }
    else
    { // Hex 模式
        QString hexStr = text.simplified();
        hexStr.remove(' ');
        data = QByteArray::fromHex(hexStr.toLatin1());
        if (data.isEmpty() && !hexStr.isEmpty())
        {
            QMessageBox::warning(this, "错误", "无效的十六进制输入");
            return;
        }
    }

    // 2. 添加结束符
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

    // 3. 发送
    qint64 written = m_serial->write(data);
    if (written == -1)
    {
        QMessageBox::critical(this, "错误", "发送失败：" + m_serial->errorString());
        return; // 发送失败，不进行回显和清空
    }

    // 4. 发送成功后的回显（只有成功才显示）
    QString displayText;
    if (m_sendMode == ShowMode::Text)
    {
        displayText = text; // 原始文本
    }
    else
    {
        displayText = data.toHex(' ').toUpper(); // 实际发送的十六进制
    }

    // 添加时间戳（如果启用）
    if (timestamp)
    {
        QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        displayText = "[" + timeStr + "] " + displayText;
    }

    // 在接收区显示发送标记（统一用 [TX] 标识发送，[RX] 标识接收）
    recvEdit->append("[TX] " + displayText);

    // 自动滚动到底部（可选）
    recvEdit->moveCursor(QTextCursor::End);

    qDebug() << "发送了" << written << "字节";

    // 5. 自动清空发送区
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
        // 文本模式：按指定编码解码
        if (m_receiveCodec == Codec::UTF8)
        {
            displayText = QString::fromUtf8(data);
        }
        else
        { // GBK
            displayText = QString::fromLocal8Bit(data);
        }
    }
    else
    { // Hex 模式
        displayText = data.toHex(' ').toUpper();
        // 可以让每个字节用大写字母，用空格分隔（toHex(' ') 已经加了空格）
    }
    // 2. 添加时间戳（如果需要）
    if (timestamp)
    {
        QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        displayText = "[" + timeStr + "] " + displayText;
    }
    // 3. 显示到接收区
    recvEdit->append("[RX]" + displayText);
    // 4. 自动滚动到底部（可选，用户通常希望这样）
    recvEdit->moveCursor(QTextCursor::End);
}
void MainWindow::onAutoSaveFile()
{
    if (!autosave)
        return;
    if (!recvEdit || recvEdit->toPlainText().isEmpty())
        return;

    // 1. 定义保存目录
    QString saveDir = QApplication::applicationDirPath() + "/AutoSaveFiles/";

    // 2. 确保目录存在（不存在则创建）
    QDir dir;
    if (!dir.mkpath(saveDir)) {
        qWarning() << "无法创建自动保存目录:" << saveDir;
        return;
    }

    // 3. 生成带时间戳的文件名
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString fileName = saveDir + QString("autosave_received_%1.txt").arg(timestamp);

    // 4. 静默写入
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << recvEdit->toPlainText();
        file.close();
        qDebug() << "自动保存成功:" << fileName;
    } else {
        qWarning() << "自动保存失败:" << file.errorString();
    }
}
void MainWindow::Display2Begin()
{
    addToolBar(Qt::TopToolBarArea, SCtoolBar);
    addToolBar(Qt::LeftToolBarArea, SHCtoolBar);
}