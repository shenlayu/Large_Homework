#include "columnbar.h"
#include "ui_columnbar.h"
#include <QRegularExpression>
#include <QMessageBox>
#include <QDebug>

ColumnBar::ColumnBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColumnBar), Saved(false), BeginRow(-1), EndRow(-1)
{
    ui->setupUi(this);
}

ColumnBar::~ColumnBar()
{
    delete ui;
}

// 添加选中列
void ColumnBar :: AddBar(QList<int> Numbers)
{
    // 保存
    Saved = false;
    SaveBar();
    if(!Saved)
    {
        return;
    }
    for(auto number : Numbers)
    {
        Columns.push_back(number);
    }

    // 排序并去重
    std::sort(Columns.begin(),Columns.end());
    auto it= std::unique(Columns.begin(),Columns.end());
    Columns.erase(it,Columns.end());

    // 重新填充
    QString str;
    QVector<int> VectorNumbers;
    for(auto number : Columns)
    {
        str += QString :: number(number);
        str += ",";
    }
    str.chop(1);
    ui -> BarEdit -> setText(str);
}

// 保存
void ColumnBar :: SaveBar()
{
    // 检查输入合法性
    QRegularExpression* cheaker = new QRegularExpression("^([1-9][0-9]*(,[1-9][0-9]*)*)?$");

    QString text = ui -> BarEdit -> toPlainText();
    if(!cheaker -> match(text).hasMatch())
    {
        QMessageBox :: critical(this, "错误", "输入不合法,请检查");
        return;
    }
    Columns.clear();
    int size = text.size();
    QString TmpStr;
    for(int i = 0; i < size; i ++)
    {
        if(text[i] != ',')
        {
            TmpStr.append(text[i]);
        }
        else
        {
            int number = TmpStr.toInt();
            if(number > MaxColumn)
            {
                QMessageBox :: warning(this, "警告", "保存失败，存在大于最大列数的列");
                return;
            }
            Columns.push_back(number);
            TmpStr.clear();
        }
    }
    if(TmpStr.size() != 0) // 列
    {
        int number = TmpStr.toInt();
        if(number > MaxColumn)
        {
            QMessageBox :: warning(this, "警告", "保存失败，存在大于最大列数的列");
            return;
        }
        Columns.push_back(number);
    }

    std::sort(Columns.begin(),Columns.end());
    auto it= std::unique(Columns.begin(),Columns.end());
    Columns.erase(it,Columns.end());

    QString str;
    QVector<int> VectorNumbers;
    for(auto number : Columns)
    {
        str += QString :: number(number);
        str += ",";
    }
    str.chop(1);
    ui -> BarEdit -> setText(str);
    int tmpBeginRow, tmpEndRow;

    if(ui -> BarBeginRowEdit -> text().size() != 0) // 起始行
    {
        QString text = ui -> BarBeginRowEdit -> text();
        QString CheakerStr = "^[1-9][0-9]*$";
        QRegularExpression cheaker(CheakerStr);
        if(!cheaker.match(text).hasMatch())
        {
            QMessageBox :: critical(this, "错误", "起始行输入不合法，请检查");
            return;
        }
        tmpBeginRow = text.toInt();
        if(tmpBeginRow < 2)
        {
            QMessageBox :: warning(this, "警告", "保存失败，起始行不得小于2");
            return;
        }
    }
    else // 缺省值
    {
        tmpBeginRow = 2;
    }
    if(ui -> BarEndRowEdit -> text().size() != 0) // 截止行
    {
        QString text = ui -> BarEndRowEdit -> text();
        QString CheakerStr = "^[1-9][0-9]*$";
        QRegularExpression cheaker(CheakerStr);
        if(!cheaker.match(text).hasMatch())
        {
            QMessageBox :: critical(this, "错误", "截止行输入不合法，请检查");
            return;
        }
        tmpEndRow = text.toInt();
        if(tmpEndRow > MaxRow)
        {
            QMessageBox :: warning(this, "警告", "保存失败，截止行不得大于最大行");
            return;
        }
    }
    else
    {
        tmpEndRow = MaxRow;
    }

    if(tmpBeginRow > tmpEndRow)
    {
        QMessageBox :: warning(this, "警告", "保存失败，起始行不得大于截止行");
        return;
    }

    BeginRow = tmpBeginRow;
    EndRow = tmpEndRow;
    Saved = true;
}
