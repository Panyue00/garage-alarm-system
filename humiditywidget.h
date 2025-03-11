#ifndef HUMIDITYWIDGET_H
#define HUMIDITYWIDGET_H

#include <QWidget>

class HumidityWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HumidityWidget(QWidget *parent = nullptr);

    void setHumidity(float humidity); // 设置湿度值

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float humidity;  // 当前湿度值
    const float minHumidity = 0.0f;  // 最低湿度
    const float maxHumidity = 100.0f; // 最高湿度
};

#endif // HUMIDITYWIDGET_H
