#ifndef DISTANCEWIDGET_H
#define DISTANCEWIDGET_H

#include <QWidget>

class DistanceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DistanceWidget(QWidget *parent = nullptr);

    void setDistance(int dist); // 设置距离值

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int distance; // 存储当前距离值
};

#endif // DISTANCEBARWIDGET_H
