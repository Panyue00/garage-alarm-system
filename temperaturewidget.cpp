#include "temperaturewidget.h"
#include <QPainter>

TemperatureWidget::TemperatureWidget(QWidget *parent) : QWidget(parent), temperature(25.0f) // 默认25℃
{
    setMinimumSize(100,300);
}

// 设置温度值并触发重绘
void TemperatureWidget::setTemperature(float temp)
{
    if (temp < minTemp) temp = minTemp;
    if (temp > maxTemp) temp = maxTemp;
    temperature = temp;
    update();
}

void TemperatureWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    // **温度计外框**
    int bulbRadius = w / 4;  // 底部圆的半径
    int tubeWidth = w / 5;   // 玻璃管宽度
    int tubeHeight = h - 2 * bulbRadius; // 玻璃管高度
    int tubeX = (w - tubeWidth) / 2;
    int tubeY = bulbRadius;

    painter.setBrush(Qt::white);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(tubeX, tubeY, tubeWidth, tubeHeight); // 画温度计管

    painter.setBrush(Qt::white);
    painter.drawEllipse(QPoint(w / 2, h - bulbRadius), bulbRadius, bulbRadius); // 画底部圆

    // **计算液柱高度**
    float tempRatio = (temperature - minTemp) / (maxTemp - minTemp);
    int liquidHeight = static_cast<int>(tubeHeight * tempRatio);
    int liquidY = tubeY + tubeHeight - liquidHeight;

    // **绘制液柱**
    painter.setBrush(Qt::red);
    painter.drawRect(tubeX, liquidY, tubeWidth, liquidHeight);
    painter.drawEllipse(QPoint(w / 2, h - bulbRadius), bulbRadius - 2, bulbRadius - 2);

    // **绘制温度文本**
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    QString text = QString("%1 °C").arg(temperature);
    painter.drawText(rect(), Qt::AlignRight, text);
}
