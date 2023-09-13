#ifndef DBSCAN_H
#define DBSCAN_H

#include <QWidget>

class DBSCAN : public QWidget
{
    Q_OBJECT
public:
    explicit DBSCAN(const std :: vector<std :: vector<float>>& InptData,
                    QWidget *parent = nullptr, std :: vector<float>* const group = nullptr,
                    QMap<float, QString>* const map = nullptr, std :: vector<float>* const ConstGroup = nullptr);

signals:

};

#endif // DBSCAN_H
