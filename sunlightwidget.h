#ifndef SUNLIGHTWIDGET_H
#define SUNLIGHTWIDGET_H

#include <QWidget>

class SunlightWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SunlightWidget(QWidget *parent = nullptr);
    void setValue(float value);  // 设置光照强度
    float getValue() const;      // 获取当前光照强度

protected:
    void paintEvent(QPaintEvent *event) override;  // 绘制太阳组件

private:
    float value;  // 光照强度
};

#endif // SUNLIGHTWIDGET_H
