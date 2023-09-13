#ifndef COVIARIATION_H
#define COVIARIATION_H

#include <QWidget>

class Covariance : public QWidget
{
    Q_OBJECT
public:
    explicit Covariance(const std :: vector<std :: vector<float>>& InputData, QStringList* const Heads, QWidget *parent = nullptr);

signals:

};

#endif // COVIARIATION_H
