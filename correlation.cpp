#include "correlation.h"
#include "src/covarianceAlg.h"
#include "visualization.h"
#include <QTableWidget>

Correlation::Correlation(const std :: vector<std :: vector<float>>& InputData, QStringList* const Heads, QWidget *parent) : QWidget(parent)
{
    auto Covariance = getCovariance(InputData); // 协方差矩阵
    std::vector<float> vars;
    for (auto vec : InputData)
    {
        auto avgVar = getAvgVar(vec);
        vars.push_back(std::get<1>(avgVar));
    }
    auto Correlation = getPearsonCorr(Covariance, vars); // 相关性矩阵

    // CovariationTable
    QList<QStringList> StringCovariance;
    int size = InputData.size();
    float MaxValCovarianceP = 0, MaxValCovarianceN = 0;

    // 记录正负极大值
    for(int row = 0; row < size; row ++)
    {
        QStringList RowData;
        for(int column = 0; column < size; column ++)
        {
            float value = Covariance(row, column);
            RowData.push_back(QString :: number(value, 'f', 3));
            if(value > 0)
            {
                MaxValCovarianceP = MaxValCovarianceP > value ? MaxValCovarianceP : value;
            }
            else
            {
                MaxValCovarianceN = MaxValCovarianceN < value ? MaxValCovarianceN : value;
            }
        }
        StringCovariance.push_back(RowData);
    }

    // 制表
    auto TableCovariance = TableVis(StringCovariance, Heads, Heads);
    TableCovariance -> horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    TableCovariance -> verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 取三次根后根据相对大小进行染色
    float MaxPsqrt = std :: pow(MaxValCovarianceP, 0.33f), MaxNsqrt = 0 - std :: pow(0 - MaxValCovarianceN, 0.33f);
    for(int row = 0; row < size; row ++)
    {
        for(int column = 0; column < size; column ++)
        {
            QColor color = Qt :: white;
            float value = Covariance(row, column);
            float Valuesqrt;
            if(value >= 0)
            {
                Valuesqrt = std :: pow(value, 0.33f);
            }
            else
            {
                Valuesqrt = 0 - std :: pow(0 - value, 0.33f);
            }
            if(value > 0)
            {
                color.setRgb(255,
                             255 * (1 - (Valuesqrt / MaxPsqrt)),
                             255 * (1 - (Valuesqrt / MaxPsqrt)));
            }
            else if(value < 0)
            {
                color.setRgb(255 * (1 - (Valuesqrt / MaxNsqrt)),
                             255 * (1 - (Valuesqrt / MaxNsqrt)),
                             255);
            }
            TableCovariance -> item(row, column) -> setBackground(color);
        }
    }

    // CorrelationTable
    QList<QStringList> StringCorrelation;
    for(int row = 0; row < size; row ++)
    {
        QStringList RowData;
        for(int column = 0; column < size; column ++)
        {
            float value = Correlation(row, column);
            RowData.push_back(QString :: number(value, 'f', 3));
        }
        StringCorrelation.push_back(RowData);
    }
    // 制表
    auto TableCorrelation = TableVis(StringCorrelation, Heads, Heads);
    TableCorrelation -> horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    TableCorrelation -> verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for(int row = 0; row < size; row ++)
    {
        for(int column = 0; column < size; column ++)
        {
            QColor color;
            float value = Correlation(row, column);
            if(value > 0)
            {
                color.setRgb(255, (1 - value) * 255, (1 - value) * 255);
            }
            else
            {
                color.setRgb((1 + value) * 255, (1 + value) * 255, 255);
            }
            TableCorrelation -> item(row, column) -> setBackground(color);
        }
    }

    setFixedSize(900, 660);

    // 控制显示表格
    TableCovariance -> setParent(this);
    TableCovariance -> setGeometry(0, 60, 600, 600);
    TableCorrelation -> setParent(this);
    TableCorrelation -> setGeometry(0, 60, 600, 600);
    QPushButton* changer = new QPushButton(this);
    changer -> setGeometry(0, 0, 150, 30);

    // 热图刻度表
    QLabel* LabelChart = new QLabel(this);
    LabelChart -> setGeometry(630, 167, 50, 392);
    QString SheetStyle = "background-color:qlineargradient(spread:pad, x1:0, x2:0, y1:0, y2:1,";
            SheetStyle.append("stop: 0 rgba(255,0,0,255),stop: 0.495 rgba(255,255,255,255),stop: 0.505 ");
            SheetStyle.append("rgba(255,255,255,255), stop: 1 rgba(0,0,255,255));");
    LabelChart -> setStyleSheet(SheetStyle);
    QVector<QLabel*> ScaleCovariance(9);
    for(int i = 0; i <= 3; i ++)
    {
        ScaleCovariance[i] = new QLabel("-" + QString :: number(MaxValCovarianceP * (1 - i / 4.0), 'f', 2), this);
        ScaleCovariance[i] -> setGeometry(680, 150 + 49 * i, 100, 30);
    }
    ScaleCovariance[4] = new QLabel("-" + QString :: number(0), this);
    ScaleCovariance[4] -> setGeometry(680, 150 + 49 * 4, 100, 30);
    for(int i = 5; i <= 8; i ++)
    {
        ScaleCovariance[i] = new QLabel("-" + QString :: number(MaxValCovarianceN * (1 - (8 - i) / 4.0), 'f', 2), this);
        ScaleCovariance[i] -> setGeometry(680, 150 + 49 * i, 100, 30);
    }

    // 刻度
    QVector<QLabel*> ScaleCorrelation(9);
    for(int i = 0; i < 9; i ++)
    {
        ScaleCorrelation[i] = new QLabel("-" + QString :: number(1.0 - 0.25 * i), this);
        ScaleCorrelation[i] -> setGeometry(680, 150 + 49 * i, 100, 30);
    }

    TableCovariance -> setVisible(false);
    for(auto label : ScaleCovariance)
    {
        label -> setVisible(false);
    }
    changer -> setText("切换为相关性矩阵");

    connect(changer, &QPushButton :: clicked, [=](){
        if(changer -> text() == "切换为相关性矩阵")
        {
            TableCorrelation -> setVisible(false);
            TableCovariance -> setVisible(true);
            for(auto label : ScaleCorrelation)
            {
                label -> setVisible(false);
            }
            for(auto label : ScaleCovariance)
            {
                label -> setVisible(true);
            }
            changer -> setText("切换为协方差矩阵");
        }
        else
        {
            TableCorrelation -> setVisible(true);
            TableCovariance -> setVisible(false);
            for(auto label : ScaleCorrelation)
            {
                label -> setVisible(true);
            }
            for(auto label : ScaleCovariance)
            {
                label -> setVisible(false);
            }
            changer -> setText("切换为相关性矩阵");
        }
    });
    this -> show();
}
