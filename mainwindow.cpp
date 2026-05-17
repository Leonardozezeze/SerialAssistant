#include "mainwindow.h"
#include "LabeledComboBox.h"
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QIntValidator>
#include <QFrame>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("Serial Assistant");
    this->resize(800, 600);
    Receive_Area();
    ShowMenu();
    ShowToolBar();
    Show_StatusBar();
}
void MainWindow::ShowMenu()
{
    QMenuBar *menuBar = this->menuBar();
    QMenu *fileMenu = menuBar->addMenu("文件"); // 以后留着设计 先不管考虑功能
    QMenu *seeMenu = menuBar->addMenu("查看");
    QMenu *helpMenu = menuBar->addMenu("帮助");
    QMenu *aboutMenu = menuBar->addMenu("关于");
}
void MainWindow::ShowToolBar()
{
    QToolBar *SCtoolBar = addToolBar("Serial Config"); // 串口配置工具栏
    addToolBar(Qt::TopToolBarArea, SCtoolBar);
    QToolBar *SHCtoolBar = addToolBar("Show Config"); // 显示配置工具栏
    addToolBar(Qt::LeftToolBarArea, SHCtoolBar);

    QWidget *SCcontainer = new QWidget(this);
    QBoxLayout *SCLayout = new QBoxLayout(QBoxLayout::LeftToRight, SCcontainer);
    SCLayout->setContentsMargins(0, 0, 0, 0);
    SCLayout->setSpacing(25);
    SCtoolBar->addWidget(SCcontainer);

    LabeledComboBox *portcombo = new LabeledComboBox("串口:", this);
    portcombo->addItems({"COM1", "COM2", "COM3"}); // 留着获得实际的串口调用 注意以后修改
    SCLayout->addWidget(portcombo);

    LabeledComboBox *baudratecombo = new LabeledComboBox("波特率:", this);
    baudratecombo->addItems({"1200", "4800", "9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600", "自定义..."});
    SCLayout->addWidget(baudratecombo);
    baudratecombo->comboBox()->setCurrentIndex(2);

    LabeledComboBox *stopbitscobo = new LabeledComboBox("停止位:", this);
    stopbitscobo->addItems({"1", "1.5", "2"});
    SCLayout->addWidget(stopbitscobo);

    LabeledComboBox *databitscobo = new LabeledComboBox("数据位:", this);
    databitscobo->addItems({"5", "6", "7", "8"});
    databitscobo->comboBox()->setCurrentIndex(3);
    SCLayout->addWidget(databitscobo);

    LabeledComboBox *paritycobo = new LabeledComboBox("校验位:", this);
    paritycobo->addItems({"None", "Even", "Odd"});
    SCLayout->addWidget(paritycobo);

    QPushButton *StartBtn = new QPushButton(this);
    StartBtn->setCheckable(true); // 启用开关状态
    StartBtn->setChecked(false);  // 初始状态: 关闭(false) 或 开启(true)
    QIcon iconBlack(":/blackicon.png");
    QIcon iconRed(":/redicon.png");
    // 设置初始图标和文本
    StartBtn->setText("  打开串口"); // 文本与图标之间的空格可自行调整
    StartBtn->setIcon(iconBlack);
    StartBtn->setIconSize(QSize(20, 24)); // 根据图标实际尺寸调整
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

    // 处理选择
    connect(baudratecombo->comboBox(), &QComboBox::currentTextChanged,
            this, [this, baudratecombo](const QString &text)
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
            // 你的串口配置代码
        } });
}
void MainWindow::Receive_Area()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    // 主布局
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    // 接收区
    QLabel *recvLabel = new QLabel("接收区:", this);
    QTextEdit *recvEdit = new QTextEdit(this);
    recvEdit->setReadOnly(true);
    recvEdit->setPlaceholderText("接收到的数据会显示在这里...");
    // 发送区
    QLabel *sendLabel = new QLabel("发送区:", this);
    QTextEdit *sendEdit = new QTextEdit(this);
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