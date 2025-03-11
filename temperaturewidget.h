#ifndef TEMPERATUREWIDGET_H
#define TEMPERATUREWIDGET_H

#include <QWidget>

class TemperatureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TemperatureWidget(QWidget *parent = nullptr);

    void setTemperature(float temp); // 设置温度值

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float temperature; // 当前温度值
    const float minTemp = -10.0f;  // 最低温度
    const float maxTemp = 50.0f;   // 最高温度
};

#endif // TEMPERATUREWIDGET_H
