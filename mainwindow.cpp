#include "mainwindow.h"
#include <QVBoxLayout>
#include <QDebug>
#include<QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // **左侧垂直布局**
    QVBoxLayout *leftLayout = new QVBoxLayout();
    sunlightWidget = new SunlightWidget(this);
    distancewidget = new DistanceWidget(this);
    humwidget = new HumidityWidget(this);

    // **右侧温度组件**
    tempwidget = new TemperatureWidget(this);

    // **设置组件的大小策略**
    sunlightWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    distancewidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    humwidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tempwidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // **添加左侧组件**
    leftLayout->addWidget(sunlightWidget, 1);  // 光照
    leftLayout->addWidget(distancewidget, 1);  // 距离
    leftLayout->addWidget(humwidget, 1);       // 湿度

    // **添加到主布局**
    mainLayout->addLayout(leftLayout, 2);  // 左侧占比2
    mainLayout->addWidget(tempwidget, 1);  // 右侧温度组件占比1

    // **设置主窗口的中央部件**
    setCentralWidget(centralWidget);

    // 初始化串口通信
    serial = new QSerialPort(this);
    serial->setPortName("COM7");  // Linux/macOS: "/dev/ttyUSB0"
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadOnly)) {
        connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
    } else {
        qDebug() << "无法打开串口";
    }
}

// 读取串口数据并更新太阳组件
void MainWindow::readSerialData()
{
    while (serial->canReadLine()) {
           QByteArray line = serial->readLine().trimmed();
           receivedData = QString::fromUtf8(line);
           qDebug() << "收到数据：" << receivedData;
       }
    QStringList values = receivedData.split(",");

    if (values.size() >= 1) {  // 读取光照值
        float lightValue = values[0].toFloat();
        float disValue = values[1].toFloat();
        float temValue = values[2].toFloat();
        float humValue = values[3].toFloat();
        sunlightWidget->setValue(lightValue);
        distancewidget->setDistance(disValue);
        tempwidget->setTemperature(temValue);
        humwidget->setHumidity(humValue);
    }
}

MainWindow::~MainWindow()
{
    if (serial->isOpen()) {
        serial->close();
    }
}
