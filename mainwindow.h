#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void ImportFile();
    void AVG();
    void VAR();
    void MakeHistogram();
    void MakeScatter();
    void MakeCovariance();
    void MakeCorrelation();
    void MakePCA();
    void MakeKMeans();
    void AddBar1();
    void AddBar2();
    void UseBar1();
    void UseBar2();
    bool GetFromBar1(QVector<int>&, int&, int&, int&);
    bool GetFromBar2(QVector<int>&, int&, int&, int&);
    void Paint();
    void MakeDBSCAN();
    float GetFloat(QString tip);

private:
    Ui::MainWindow *ui;
    std :: vector<float> CheckDiscrete(int column, int BeginRow, int EndRow);
    QVector<int> GetNumbers(const QStringList& Requests, const QVector<int>& Default);
    QMap<float, QString> BuildMap(const std :: vector<float>& index, int BeginRow, int EndRow);
    int usage;
    bool Inputted;

};
#endif // MAINWINDOW_H
