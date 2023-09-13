#ifndef COLUMNBAR_H
#define COLUMNBAR_H

#include <QWidget>
#include <QVector>

namespace Ui {
class ColumnBar;
}

class ColumnBar : public QWidget
{
    Q_OBJECT

public:
    explicit ColumnBar(QWidget *parent = nullptr);
    ~ColumnBar();
    QVector<int> Columns;
    int BeginRow, EndRow;
    void SetMax();
    int MaxRow, MaxColumn;
    bool Saved;
public slots:
    void SaveBar();
    void AddBar(QList<int>);

private:
    Ui::ColumnBar *ui;
};

#endif // COLUMNBAR_H
