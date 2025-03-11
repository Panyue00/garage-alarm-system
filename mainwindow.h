#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QSerialPort>
#include <QSerialPortInfo>
#include<sunlightwidget.h>
#include"distancewidget.h"
#include"temperaturewidget.h"
#include "humiditywidget.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readSerialData();

private:
    QLabel *labelLight;
    QLabel *labelDistance;
    QLabel *labelTemperature;
    QLabel *labelHumidity;
    QSerialPort *serial;
     SunlightWidget *sunlightWidget;  // 声明太阳组件指针
     DistanceWidget *distancewidget;
     TemperatureWidget *tempwidget;
     HumidityWidget *humwidget;
     QString receivedData;
};

#endif // MAINWINDOW_H

