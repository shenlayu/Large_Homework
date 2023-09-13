#ifndef CORRELATION_H
#define CORRELATION_H

#include <QWidget>

class Correlation : public QWidget
{
    Q_OBJECT
public:
    explicit Correlation(const std :: vector<std :: vector<float>>& InputData, QStringList* const Heads, QWidget *parent = nullptr);

signals:

};

#endif // CORRELATION_H
