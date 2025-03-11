#include "sunlightwidget.h"
#include <QPainter>
#include <QPen>
#include <QtMath>

SunlightWidget::SunlightWidget(QWidget *parent)
    : QWidget(parent), value(50.0f)  // 默认光照值 50
{
    setMinimumSize(100, 100);  // 太阳组件最小尺寸
}

// 设置光照值并更新界面
void SunlightWidget::setValue(float newValue)
{
    value = qBound(600.0f, newValue, 1000.0f);  // 限制范围 0-100
    update();  // 触发重绘
}

// 获取当前光照强度
float SunlightWidget::getValue() const
{
    return value;
}

// 处理绘制事件
void SunlightWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
       painter.setRenderHint(QPainter::Antialiasing);

       int w = width();
       int h = height();
       int size = qMin(w, h);

       // **反向归一化光照值**
       float normalized = (1000 -value) / 400.0f;

       // **颜色变换：从亮黄到深黄**
       QColor sunColor = QColor::fromHsv(50, 255, static_cast<int>(normalized * 255));

       // 画太阳主体
       painter.setBrush(QBrush(sunColor));
       painter.setPen(Qt::NoPen);
       painter.drawEllipse(QPoint(w / 2, h / 2), size / 4, size / 4);

       // 画光晕
       painter.setPen(QPen(sunColor, 2));
       int rayLength = static_cast<int>((size / 3) + (normalized * (size / 6)));
       for (int i = 0; i < 12; ++i) {
           float angle = i * 30 * 3.14159 / 180;
           int x1 = w / 2 + cos(angle) * (size / 4);
           int y1 = h / 2 + sin(angle) * (size / 4);
           int x2 = w / 2 + cos(angle) * rayLength;
           int y2 = h / 2 + sin(angle) * rayLength;
           painter.drawLine(x1, y1, x2, y2);
       }
}
