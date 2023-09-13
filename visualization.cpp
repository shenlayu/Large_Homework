#include "visualization.h"
#include "src/rowfeatureAlg.h"
#include "src/leastsquareAlg.h"
#include <QTableWidget>
#include <QtCharts>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/QScatterDataProxy>
#include <QtDataVisualization/QScatterDataArray>

// 制表接口，返回表格
QTableWidget* TableVis(const QList<QStringList>& InputData, const QStringList* const HeadsX, QStringList* HeadsY)
{
    QTableWidget* Table = new QTableWidget;

    int RowAmount = InputData.size(), ColumnAmount = InputData[0].size();
    Table -> setRowCount(RowAmount);
    Table -> setColumnCount(ColumnAmount);

    // 如果需要，更改表头内容
    if(HeadsX != nullptr)
    {
        Table -> setHorizontalHeaderLabels(*HeadsX);
    }

    if(HeadsY != nullptr)
    {
        Table -> setVerticalHeaderLabels(*HeadsY);
    }

    // 填充
    for(int row = 0; row < RowAmount; row ++)
    {
        for(int column = 0; column < ColumnAmount; column ++)
        {
            Table -> setItem(row, column, new QTableWidgetItem(InputData[row][column]));
        }
    }

    return Table;
}

// 直方图可视化
QChart* HistogramVis(const std :: vector<float>& InputData, float space, QChart* ExistedChart, bool ClearExistedChart)
{
    // 如果有必要，重新画图
    if(ClearExistedChart)
    {
        ExistedChart -> removeAllSeries();
        QList<QAbstractAxis*> axesList = ExistedChart->axes();
        for (QAbstractAxis *axis : axesList) {
            ExistedChart -> removeAxis(axis);
            delete axis;
        }
    }
    float MinVal = InputData[0], MaxVal = InputData[0];
    std :: vector<int> ValAmount; // 每个区间高度

    // 计算极值，准备定范围
    for(float number: InputData)
    {
        MinVal = MinVal < number ? MinVal: number;
        MaxVal = MaxVal > number ? MaxVal: number;
    }

    // 给每个点决定对应区间
    for(int i = 1; i <= int((MaxVal - MinVal) / space) + 1; i ++)
    {
        ValAmount.push_back(0);
    }
    for(float number: InputData)
    {
        ValAmount[(number - MinVal) / space] ++;
    }

    QBarSet* BarSetHistogram = new QBarSet("ok");

    int CategoriesNumber = ValAmount.size();
    int MaxAmount = ValAmount[0];
    for(int i = 0; i < CategoriesNumber; i ++)
    {
        *BarSetHistogram << ValAmount[i];
        BarSetHistogram -> setLabel(QString :: number(ValAmount[i]));
        BarSetHistogram -> setLabelColor(Qt :: black);

        MaxAmount = MaxAmount > ValAmount[i] ? MaxAmount : ValAmount[i];
    }

    QBarSeries* BarSeriesHistogram = new QBarSeries();
    BarSeriesHistogram -> append(BarSetHistogram);
    BarSeriesHistogram -> setLabelsVisible(true);
    BarSeriesHistogram -> setName("直方图");

    // 若不需要在已经绘制的图添加，则新建图
    if(ExistedChart == nullptr)
    {
        ExistedChart = new QChart();
    }
    ExistedChart -> addSeries(BarSeriesHistogram);
    ExistedChart -> legend() -> setVisible(false);

    // 加轴
    QValueAxis* axisX = new QValueAxis();
    axisX -> setRange(MinVal, MinVal + CategoriesNumber * space);
    axisX -> setTickCount(CategoriesNumber + 1);
    ExistedChart -> addAxis(axisX, Qt::AlignBottom);
    // 不能AttachSeries

    QValueAxis* axisY = new QValueAxis();
    axisY -> setRange(0, 4 * (int(MaxAmount / 4.0) + 1));
    axisY -> setTitleText("直方图");
    ExistedChart -> addAxis(axisY, Qt::AlignLeft);
    BarSeriesHistogram -> attachAxis(axisY);

    return ExistedChart;
}

// 正态分布曲线可视化
QChart* NormalDistributionVis(const std :: vector<float>& InputData, QChart* ExistedChart)
{
    QLineSeries* NormalDistributionSeries = new QLineSeries();
    NormalDistributionSeries -> setName("正态分布曲线");
    auto AVGAndVAR = getAvgVar(InputData);
    float AVG = std :: get<0>(AVGAndVAR), VAR = std :: get<1>(AVGAndVAR), stdDev = sqrt(VAR);

    int numPoints = 100;

    for (int i = 0; i < numPoints; ++i)
    {
        float x = AVG - 3 * stdDev + 6 * stdDev * i / (numPoints - 1);
        float y = std::exp(-0.5 * std::pow((x - AVG) / stdDev, 2)) / (stdDev * std::sqrt(2 * M_PI));
        NormalDistributionSeries->append(x, y);
    }
    QValueAxis* NormalAxisY = new QValueAxis();
    NormalAxisY -> setRange(0, 1.2 * (1.0 / (stdDev * sqrt(2 * M_PI))));
    NormalAxisY -> setTitleText("正态曲线");

    // 因为本项目中不会出现，省略Existed == nullptr情况，但可以轻松扩展，其他位置同理
    if(ExistedChart != nullptr)
    {
        ExistedChart -> addAxis(NormalAxisY, Qt :: AlignRight);
    }
    ExistedChart -> addSeries(NormalDistributionSeries);

    // 这里有复制粘贴很小一段代码，目的是为正态曲线创建一个和原来一样的坐标轴，原因如下
    // 第一，当希望在已有的图上添加内容，没有找到很好的获得已有坐标轴的安全方式，如果为了坐标轴修改所有可视化函数的返回值感觉得不偿失
    // 第二，在已有图上新加内容时，新加内容创建并依附自己的新坐标轴很符合直觉，而一味依附已有坐标轴可能导致耦合度增加
    float MinVal = InputData[0], MaxVal = InputData[0];
    std :: vector<int> ValAmount; // 每个区间高度
    for(float number: InputData)
    {
        MinVal = MinVal < number ? MinVal: number;
        MaxVal = MaxVal > number ? MaxVal: number;
    }
    QValueAxis* NormalAxisX = new QValueAxis();
    NormalAxisX -> setRange(MinVal, MinVal + int(MaxVal - MinVal));
    ExistedChart -> addAxis(NormalAxisX, Qt::AlignBottom);
    NormalAxisX -> setVisible(false);

    NormalDistributionSeries -> attachAxis(NormalAxisX);
    NormalDistributionSeries -> attachAxis(NormalAxisY);

    return ExistedChart;
}

// 散点图可视化
std :: pair<QChart*, QScatterSeries*>ScatterVis(const std :: vector<float>& InputDataX, const std :: vector<float>& InputDataY,
            const QString& NameX, const QString& NameY, std :: vector<int>* usage, QMap<float, QString>* const SeriesName, QChart* ExistedChart)
{
    bool newChart = false;
    float MinValX = InputDataX[0], MaxValX = InputDataX[0];
    float MinValY = InputDataY[0], MaxValY = InputDataY[0];

    // 极值
    for(auto number: InputDataX)
    {
        MinValX = MinValX < number ? MinValX : number;
        MaxValX = MaxValX > number ? MaxValX : number;
    }
    for(auto number: InputDataY)
    {
        MinValY = MinValY < number ? MinValY : number;
        MaxValY = MaxValY > number ? MaxValY : number;
    }
    float spaceX = MaxValX - MinValX, spaceY = MaxValY - MinValY;

    // 如果有必要，通过usage确定将哪些点加入图
    QScatterSeries* ScatterSeries = new QScatterSeries();
    ScatterSeries -> setName("散点图");
    if(usage == nullptr)
    {
        int size = InputDataX.size();
        for(int i = 0; i < size; i ++)
        {
            ScatterSeries -> append(InputDataX[i], InputDataY[i]);
        }
    }
    else
    {
        int size = usage -> size();
        for(int i = 0; i < size; i ++)
        {
            ScatterSeries -> append(InputDataX[(*usage)[i]], InputDataY[(*usage)[i]]);
        }
        if(SeriesName != nullptr)
        {
            ScatterSeries -> setName((*SeriesName)[(*usage)[0]]);
        }
    }
    ScatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    ScatterSeries->setMarkerSize(5);

    if(ExistedChart == nullptr)
    {
        ExistedChart = new QChart();
        newChart = true;
    }
    ExistedChart -> addSeries(ScatterSeries);
    ExistedChart -> setTitle("散点图");
    ExistedChart -> setAnimationOptions(QChart::SeriesAnimations);

    // 加轴
    QValueAxis* axisX = new QValueAxis();
    axisX -> setRange(MinValX - 0.2 * spaceX, MaxValX + 0.2 * spaceX);
    axisX -> setTitleText(NameX);
    ExistedChart -> addAxis(axisX, Qt::AlignBottom);
    ScatterSeries -> attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY -> setRange(MinValY - 0.2 * spaceY, MaxValY + 0.2 * spaceY);
    axisY -> setTitleText(NameY);
    ExistedChart -> addAxis(axisY, Qt::AlignLeft);
    ScatterSeries -> attachAxis(axisY);
    if(!newChart)
    {
        axisX -> setVisible(false);
        axisY -> setVisible(false);
    }

    return {ExistedChart, ScatterSeries};
}

// 多项式曲线可视化
QChart* PolynomialLineVis(const std :: vector<float>& InputDataX, const std :: vector<float>& InputDataY, int dimension, QChart* ExistedChart)
{
    QLineSeries* CurveSeries = new QLineSeries();
    CurveSeries -> setName(QString :: number(dimension) + "次多项式曲线");
    auto result = fitLeastSquareAndPR(InputDataX, InputDataY, dimension);

    // 算极值
    float MinValX = InputDataX[0], MaxValX = InputDataX[0];
    float MinValY = InputDataY[0], MaxValY = InputDataY[0];
    for(auto number: InputDataX)
    {
        MinValX = MinValX < number ? MinValX : number;
        MaxValX = MaxValX > number ? MaxValX : number;
    }
    for(auto number: InputDataY)
    {
        MinValY = MinValY < number ? MinValY : number;
        MaxValY = MaxValY > number ? MaxValY : number;
    }
    float spaceX = MaxValX - MinValX, spaceY = MaxValY - MinValY;

    // 加轴
    QValueAxis* axisX = new QValueAxis();
    axisX -> setRange(MinValX - 0.2 * spaceX, MaxValX + 0.2 * spaceX);
    QValueAxis* axisY = new QValueAxis();
    axisY -> setRange(MinValY - 0.2 * spaceY, MaxValY + 0.2 * spaceY);

    // 获得每阶项数
    QVector<float> coefficient;
    for(int i = 0; i < std::get<0>(result).size(); i ++)
    {
        coefficient.push_back(std::get<0>(result)[i]);
    }

    // 拟合曲线
    int numPoints = 40;

    for (int i = 0; i < numPoints; ++i)
    {
        float x = MinValX - 0.1 * spaceX + i * (1.2 * MaxValX - 1.2 * MinValX) / numPoints;
        float y = 0;
        for(int j = 0; j < coefficient.size(); j ++)
        {
            y += coefficient[j] * qPow(x, j);
        }
        CurveSeries -> append(x, y);
    }
    if(ExistedChart != nullptr)
    {
        ExistedChart -> addSeries(CurveSeries);
        ExistedChart -> addAxis(axisX, Qt::AlignBottom);
        axisX -> setVisible(false);
        ExistedChart -> addAxis(axisY, Qt::AlignLeft);
        axisY -> setVisible(false);
    }

    CurveSeries -> attachAxis(axisX);
    CurveSeries -> attachAxis(axisY);
    return ExistedChart;
}

// 3D散点图
std :: pair<QtDataVisualization :: Q3DScatter*, QtDataVisualization::QScatter3DSeries*>
Scatter3DVis(const Eigen :: MatrixXf& InputData, std :: vector<int>* usage,
             QMap<float, QString>* map, QtDataVisualization :: Q3DScatter* ExistedChart)
{
    bool newChart = false;
    QtDataVisualization::QScatterDataArray data;

    // 如果有必要，通过usage决定将哪些点加入图中
    if(usage == nullptr)
    {
        int size = 1;InputData.rows();
        for(int i = 0; i < size; i ++)
        {
            data << QVector3D(InputData(i, 0), InputData(i, 1), InputData(i, 2));
        }
    }
    else
    {
        int size = usage -> size();
        for(int i = 0; i < size; i ++)
        {
            int row = (*usage)[i];
            data << QVector3D(InputData(row, 0), InputData(row, 1), InputData(row, 2));
        }
    }

    if(ExistedChart == nullptr)
    {
        ExistedChart = new QtDataVisualization :: Q3DScatter();
        newChart = true;
    }

    QtDataVisualization::QScatterDataProxy *PCAProxy = new QtDataVisualization::QScatterDataProxy();
    PCAProxy -> addItems(data);

    QtDataVisualization::QScatter3DSeries *Series3DScatter = new QtDataVisualization::QScatter3DSeries(PCAProxy);

    Series3DScatter -> setItemSize(0.1f);
    if(map != nullptr)
    {
        Series3DScatter -> setName((*map)[(*usage)[0]]);
    }

    ExistedChart -> addSeries(Series3DScatter);

    if(newChart)
    {
        float MinValX = InputData(0, 0), MaxValX = InputData(0, 0),
              MinValY = InputData(0, 1), MaxValY = InputData(0, 1),
              MinValZ = InputData(0, 2), MaxValZ = InputData(0, 2);
        int size = InputData.rows();
        for(int i = 0; i < size; i ++)
        {
            float x = InputData(i, 0), y = InputData(i, 1), z = InputData(i, 2);
            MinValX = MinValX < x ? MinValX : x;
            MaxValX = MaxValX > x ? MaxValX : x;
            MinValY = MinValY < y ? MinValY : y;
            MaxValY = MaxValY > y ? MaxValY : y;
            MinValZ = MinValZ < z ? MinValZ : z;
            MaxValZ = MaxValZ > z ? MaxValZ : z;
        }
        float spaceX = MaxValX - MinValX, spaceY = MaxValY - MinValY, spaceZ = MaxValZ - MinValZ;
        ExistedChart -> scene() -> activeCamera() -> setCameraPosition(45, 30, 100);
        ExistedChart -> axisX()->setRange(MinValX - 0.1 * spaceX, MaxValX + 0.1 * spaceX);
        ExistedChart -> axisY()->setRange(MinValY - 0.1 * spaceY, MaxValY + 0.1 * spaceY);
        ExistedChart -> axisZ()->setRange(MinValZ - 0.1 * spaceZ, MaxValZ + 0.1 * spaceZ);
        ExistedChart->axisX()->setTitle("X");
        ExistedChart->axisY()->setTitle("Y");
        ExistedChart->axisZ()->setTitle("Z");
        ExistedChart -> setAspectRatio(1);
        ExistedChart -> setHorizontalAspectRatio(1);
        ExistedChart->axisX()->setSegmentCount(5);
        ExistedChart->axisY()->setSegmentCount(5);
        ExistedChart->axisZ()->setSegmentCount(5);
    }

    return {ExistedChart, Series3DScatter};
}
