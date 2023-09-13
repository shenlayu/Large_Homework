#pragma once
#ifndef KMEANS_H
#define KMEANS_H

#include <QWidget>

class KMeans : public QWidget
{
    Q_OBJECT
public:
    explicit KMeans(const std :: vector<std :: vector<float>>& InptData,
            QWidget *parent = nullptr, std :: vector<float>* const group = nullptr,
            QMap<float, QString>* const map = nullptr, std :: vector<float>* const ConstGroup = nullptr);

signals:

};

#endif // KMEANS_H
