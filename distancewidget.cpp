#include "distancewidget.h"
#include <QPainter>
#include <QLinearGradient>

DistanceWidget::DistanceWidget(QWidget *parent) : QWidget(parent), distance(50) // 默认50
{
    setMinimumSize(200,100);
}

// 设置距离值，并触发重绘
void DistanceWidget::setDistance(int dist)
{
    if (dist < 0) dist = 0;
    if (dist > 200) dist = 200; // 限制最大值
    distance = dist;
    update();
}

void DistanceWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    // **背景**
    painter.setBrush(QColor(220, 220, 220));
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, w, h);

    // **进度条**
    int barWidth = static_cast<int>(w * (static_cast<float>(distance) / 200.0f));

    // **颜色渐变**
    QLinearGradient gradient(0, 0, w, 0);
    gradient.setColorAt(0.0, QColor(255, 0, 0));   // 近（红色）
    gradient.setColorAt(1.0, QColor(0, 0, 255));   // 远（蓝色）

    painter.setBrush(gradient);
    painter.drawRect(0, 0, barWidth, h);

    // **绘制数值文本**
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    QString text = QString("%1 cm").arg(distance);
    painter.drawText(rect(), Qt::AlignCenter, text);
}
