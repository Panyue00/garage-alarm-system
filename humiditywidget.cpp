#include "humiditywidget.h"
#include <QPainter>

HumidityWidget::HumidityWidget(QWidget *parent) : QWidget(parent), humidity(50.0f) // 默认50%
{
}

// 设置湿度值并触发重绘
void HumidityWidget::setHumidity(float hum)
{
    if (hum < minHumidity) hum = minHumidity;
    if (hum > maxHumidity) hum = maxHumidity;
    humidity = hum;
    update();
}

void HumidityWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    // **水滴形状（尖端朝上）**
    QPainterPath dropPath;
    dropPath.moveTo(w / 2, 10); // 顶部尖端
    dropPath.quadTo(w, h * 0.7, w / 2, h - 10); // 右侧弧线
    dropPath.quadTo(0, h * 0.7, w / 2, 10); // 左侧弧线

    painter.setBrush(Qt::white);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawPath(dropPath);

    // **计算水位高度**
    float humRatio = (humidity - minHumidity) / (maxHumidity - minHumidity);
    int waterLevel = static_cast<int>((h - 20) * (1.0f - humRatio)) + 10;

    // **创建水滴填充部分**
    QPainterPath waterFillPath;
    waterFillPath.addRect(0, waterLevel, w, h - waterLevel);  // 关键改动！

    // **让填充区域受水滴形状裁剪**
    QPainterPath clippedPath = dropPath.intersected(waterFillPath);

    painter.setClipPath(clippedPath);
    painter.setBrush(QColor(0, 150, 255, 180));  // 半透明蓝色
    painter.drawPath(clippedPath);  // 绘制填充


    // **绘制湿度文本**
    painter.setClipping(false);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    QString text = QString("%1% RH").arg(humidity);
    painter.drawText(rect(), Qt::AlignCenter, text);
}
