// 包装层直方图模块 构造函数传入数据
#pragma once
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>

class Histogram : public QWidget
{
    Q_OBJECT
public:
    explicit Histogram(const std ::vector<float>& InputData, QWidget *parent = nullptr);

signals:

};

#endif // HISTOGRAM_H
