#pragma once
#ifndef SCATTER_H
#define SCATTER_H

#include <QWidget>

class Scatter : public QWidget
{
    Q_OBJECT
public:
    explicit Scatter(const std ::vector<float>& InputDataX, const std :: vector<float>& InputDataY, const QString& NameX,
    const QString& NameY, QWidget *parent = nullptr, std :: vector<float>* const group = nullptr, QMap<float, QString>* map = nullptr);

public slots:
    int GetDimension();

signals:

};

#endif // SCATTER_H
