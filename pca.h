#pragma once
#ifndef PCA_H
#define PCA_H

#include <QWidget>

class PCA : public QWidget
{
    Q_OBJECT
public:
    explicit PCA(const std :: vector<std :: vector<float>>& InputData,
                 int dimension, QWidget *parent = nullptr, std :: vector<float>* const group = nullptr, QMap<float, QString>* map = nullptr);

signals:

};

#endif // PCA_H
