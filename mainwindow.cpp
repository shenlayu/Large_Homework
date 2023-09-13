// MainWindow层
// 分4层：MainWindow层，包装层，可视化层，算法层
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "visualization.h"
#include "src/rowfeatureAlg.h"
#include "src/kmeansAlg.h"
#include "histogram.h"
#include "scatter.h"
#include "covariance.h"
#include "correlation.h"
#include "pca.h"
#include "kmeans.h"
#include "columnbar.h"
#include "src/dbscanALG.h"
#include "dbscan.h"
#include <QDebug>
#include <QFileDialog>
#include <QRegularExpression>
#define Table ui->DataTable
#define Bar1 ui -> ColumnBar1
#define Bar2 ui -> ColumnBar2

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), usage(0), Inputted(false)
{
    ui->setupUi(this);
    connect(ui -> actionInput, &QAction :: triggered, this, &MainWindow :: ImportFile);
    connect(ui -> actionHistogram, &QAction :: triggered, this, &MainWindow :: MakeHistogram);
    connect(ui -> CalculateAVG, &QPushButton :: clicked, this, &MainWindow :: AVG);
    connect(ui -> CalculateVAR, &QPushButton :: clicked, this, &MainWindow :: VAR);
    connect(ui -> actionScatterPlot, &QAction :: triggered, this, &MainWindow :: MakeScatter);
    connect(ui -> actionCovariance, &QAction :: triggered, this, &MainWindow :: MakeCovariance);
    connect(ui -> actionCorrelation, &QAction :: triggered, this, &MainWindow :: MakeCorrelation);
    connect(ui -> actionPCA, &QAction :: triggered, this, &MainWindow :: MakePCA);
    connect(ui -> actionKMeans, &QAction :: triggered, this, &MainWindow :: MakeKMeans);
    connect(ui -> SaveBar1, &QPushButton :: clicked, this, [this](){
         Bar1 -> SaveBar();
    });
    connect(ui -> SaveBar2, &QPushButton :: clicked, this, [this](){
         Bar2 -> SaveBar();
    });
    connect(ui -> AddBar1, &QPushButton :: clicked, this, &MainWindow :: AddBar1);
    connect(ui -> AddBar2, &QPushButton :: clicked, this, &MainWindow :: AddBar2);
    connect(ui -> UseBar1, &QPushButton :: clicked, this, &MainWindow :: UseBar1);
    connect(ui -> UseBar2, &QPushButton :: clicked, this, &MainWindow :: UseBar2);
    ui -> paint -> setVisible(false);
    connect(ui -> paint, &QPushButton :: clicked, this, &MainWindow :: Paint);
    connect(ui -> actionDBSCAN, &QAction :: triggered, this, &MainWindow :: MakeDBSCAN);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 导入文件
void MainWindow :: ImportFile()
{
    // ColumnBar
    usage = 0;
    ui -> ColumnBar1 -> Columns.clear();
    ui -> ColumnBar1 -> Saved = false;
    ui -> ColumnBar2 -> Columns.clear();
    ui -> ColumnBar2 -> Saved = false;
    QList<QStringList> list;

    // 选择框
    QString filePath = QFileDialog::getOpenFileName(this, "选择文件", QDir::homePath(), "所有文件 (*.*)");
    QFile InputFile(filePath);

    if(!InputFile.exists())
    {
        QMessageBox :: critical(this, "错误", "文件不存在");
        return;
    }
    if(!InputFile.open(QIODevice :: ReadOnly))
    {
        QMessageBox :: warning(this, "警告", "无法打开文件");
        return;
    }

    QTextStream stream(&InputFile);
    while(!stream.atEnd())
    {
        QString LineText = stream.readLine();
        QStringList TextList = LineText.split(',', Qt :: SkipEmptyParts);
        list.append(TextList);
    }

    // 修改表格内容
    Table -> setVisible(false);
    ui -> horizontalLayout_6 -> removeWidget(Table);
    Table = TableVis(list);
    Table -> setParent(this);
    ui -> horizontalLayout_6 -> addWidget(Table, 0, 0);
    ui -> horizontalLayout_6 -> removeItem(ui -> verticalLayout_5);
    ui -> horizontalLayout_6 -> addItem(ui -> verticalLayout_5);
    Table -> setMinimumSize(600, 600);
    Table -> show();
    Inputted = true;
    ui -> ColumnBar1 -> MaxRow = Table -> rowCount();
    ui -> ColumnBar1 -> MaxColumn = Table -> columnCount();
    ui -> ColumnBar2 -> MaxRow = Table -> rowCount();
    ui -> ColumnBar2 -> MaxColumn = Table -> columnCount();
}

// 检验列是否离散，返回一个映射后的数组（或原数组）
std :: vector<float> MainWindow :: CheckDiscrete(int column, int BeginRow, int EndRow)
{
    // 检验是否是数字
    QRegularExpression* checker = new QRegularExpression("^([0-9]+(.[0-9]*)?|[0-9]*)$");

    bool Discrete = false; // false代表连续
    QMap<QString, int> map;
    std :: vector<float> array;
    if(checker -> match(Table -> item(BeginRow, column) -> text()).hasMatch())
    {
        Discrete = false;
    }
    else
    {
        Discrete = true;
    }

    if(!Discrete) // 连续返回原数组
    {
        for(int row = BeginRow; row <= EndRow; row ++)
        {
            array.push_back(Table -> item(row, column) -> text().toFloat());
        }
    }
    else // 离散则建立映射后返回映射数组
    {
        QStringList Occupied;
        int index = 0;
        for(int row = BeginRow; row <= EndRow; row ++)
        {
            bool flag = false;
            for(QString str: Occupied)
            {
                if(str == Table -> item(row, column) -> text())
                {
                    flag = true;
                    break;
                }
            }
            if(flag)
            {
                array.push_back(map[Table -> item(row, column) -> text()]);
            }
            else
            {
                array.push_back(index);
                map.insert(Table -> item(row, column) -> text(), index ++);
                Occupied.push_back(Table -> item(row, column) -> text());
            }
        }
    }
    return array;
}

// 给离散的列建立生成值和原字符串的映射
QMap<float, QString> MainWindow :: BuildMap(const std :: vector<float>& index,int BeginRow, int EndRow)
{
    QMap<float, QString> map;
    QVector<QString> vector;
    int tmp = -1;
    for(int i = 0; i <= EndRow - BeginRow; i ++)
    {
        QString text = Table -> item(BeginRow + i, 1) -> text();
        if(index[i] > tmp)
        {
            tmp ++;
            vector.push_back(text);
        }
        map[i] = text;
    }
    return map;
}

// 生成一给dialog用于输入，可通过参数控制edit个数和提示内容，并控制有无缺省值，返回用户输入结果
QVector<int> MainWindow :: GetNumbers(const QStringList& Requests, const QVector<int>& Default)
{
    QDialog* window = new QDialog(this);
    int number = Requests.size();
    window -> resize(300, 27 + 50 * number);

    QVector<QLineEdit*> getter;
    for(int i = 0; i < number; i ++)
    {
        QLabel* tip = new QLabel(window);
        tip -> setText(Requests[i]);
        tip -> setGeometry(0, 50 * i, 200, 50);
        QLineEdit* edit = new QLineEdit(window);
        edit -> setGeometry(200, 50 * i, 100, 50);
        getter.push_back(edit);
    }
    QPushButton* OK = new QPushButton("确定", window);
    OK -> setGeometry(250, 50 * number, 50, 25);

    QVector<int> array;
    connect(OK, &QPushButton :: clicked, [=, &array](){
        for(int i = 0; i < number; i ++)
        {
            // 检查输入合法性
            if(getter[i] -> text() != NULL)
            {
                QString CheakerStr = "^[1-9][0-9]*$";
                QRegularExpression cheaker(CheakerStr);
                if(!cheaker.match(getter[i] -> text()).hasMatch())
                {
                    QMessageBox :: critical(window, "错误", "第" + QString :: number(i + 1) + "行输入不合法，请检查");
                    array.clear();
                    return;
                }
                array.push_back(getter[i] -> text().toFloat());
            }
            // 缺省值
            else if(Default[i] != -1)
            {
                array.push_back(Default[i]);
            }
            else
            {
                // 弹出报错
                QMessageBox :: critical(window, "错误", "第" + QString :: number(i + 1) + "行无默认值且未输入，请检查");
                array.clear();
                return;
            }
        }
        window -> close();
    });
    window -> exec();

    return array;
}

// 计算平均值
void MainWindow :: AVG()
{
    // 检查输入是否合法
    QString CheakerStr = "^[1-9][0-9]*$";
    QRegularExpression cheaker(CheakerStr);
    if(!(cheaker.match(ui -> AVGEdit1 -> text()).hasMatch())) // 列
    {
        QMessageBox :: critical(this, "错误", "输入不合法，请检查");
        return;
    }
    int column = ui -> AVGEdit1 -> text().toInt() - 1;
    int BeginRow, EndRow;
    if(ui -> AVGEdit2 -> text().size() != 0) // 输入非空 起始行
    {
        if(!(cheaker.match(ui -> AVGEdit2 -> text()).hasMatch())) // 检查
        {
            QMessageBox :: critical(this, "错误", "输入不合法，请检查");
            return;
        }
        BeginRow = ui -> AVGEdit2 -> text().toInt() - 1;
    }
    else // 缺省值
    {
        BeginRow = 1;
    }
    if(ui -> AVGEdit3 -> text().size() != 0) // 截止行
    {
        if(!(cheaker.match(ui -> AVGEdit3 -> text()).hasMatch()))
        {
            QMessageBox :: critical(this, "错误", "输入不合法，请检查");
            return;
        }
        EndRow = ui -> AVGEdit3 -> text().toInt() - 1;
    }
    else
    {
        EndRow = Table -> rowCount() - 1;
    }

    if(column > Table -> columnCount() || BeginRow > EndRow || BeginRow <= 0 || EndRow >= Table -> rowCount()) // 检查是否越界
    {
        QMessageBox :: critical(this, "错误", "行列输入不在合法范围");
        return;
    }
    auto data = CheckDiscrete(column, BeginRow, EndRow); // 检查是否离散
    auto result = std :: get<0>(getAvgVar(data));
    ui -> AVGLabel5 -> setText(QString :: number(result));
}

// 计算方差
void MainWindow :: VAR()
{
    // 检查输入是否合法
    QString CheakerStr = "^[1-9][0-9]*$";
    QRegularExpression cheaker(CheakerStr);
    if(!(cheaker.match(ui -> VAREdit1 -> text()).hasMatch()))
    {
        QMessageBox :: critical(this, "错误", "输入不合法，请检查");
        return;
    }
    int column = ui -> VAREdit1 -> text().toInt() - 1;
    int BeginRow, EndRow;
    if(ui -> VAREdit2 -> text().size() != 0)
    {
        if(!(cheaker.match(ui -> VAREdit2 -> text()).hasMatch()))
        {
            QMessageBox :: critical(this, "错误", "输入不合法，请检查");
            return;
        }
        BeginRow = ui -> VAREdit2 -> text().toInt() - 1;
    }
    else
    {
        BeginRow = 1;
    }
    if(ui -> VAREdit3 -> text().size() != 0)
    {
        if(!(cheaker.match(ui -> VAREdit3 -> text()).hasMatch()))
        {
            QMessageBox :: critical(this, "错误", "输入不合法，请检查");
            return;
        }
        EndRow = ui -> VAREdit3 -> text().toInt() - 1;
    }
    else
    {
        EndRow = Table -> rowCount() - 1;
    }

    if(column > Table -> columnCount() || BeginRow > EndRow || BeginRow <= 0 || EndRow >= Table -> rowCount())
    {
        QMessageBox :: critical(this, "错误", "行列输入不在合法范围");
        return;
    }
    auto data = CheckDiscrete(column, BeginRow, EndRow);
    auto result = std :: get<1>(getAvgVar(data));
    ui -> VARLabel5 -> setText(QString :: number(result));
}

// 直方图
void MainWindow :: MakeHistogram()
{
    // 检查是否导入数据
    if(!Inputted)
    {
        QMessageBox :: warning(this, "警告", "请先导入数据");
        return;
    }
    // 通过输入框获得行列信息
    auto PlaceData = GetNumbers({"输入列号:", "输入起始行号(默认为2):", "输入截止行号(默认为最后一列):"}, {-1, 2, Table -> rowCount()});
    if(PlaceData.size() == 0) // 用户直接关闭了窗口
    {
        return;
    }
    int column = PlaceData[0] - 1, BeginRow = PlaceData[1] - 1, EndRow = PlaceData[2] - 1;
    // 越界
    if(column > Table -> columnCount() || BeginRow > EndRow || BeginRow <= 0 || EndRow >= Table -> rowCount())
    {
        QMessageBox :: critical(this, "错误", "行列输入不在合法范围");
        return;
    }

    auto data = CheckDiscrete(column, BeginRow, EndRow);

    // 创建Histogram控件并承载
    QDialog* HistogramShower = new QDialog(this);
    Histogram* HistogramObject = new Histogram(data, HistogramShower);
    HistogramShower -> show();
}

// 散点图
void MainWindow :: MakeScatter()
{
    if(!Inputted)
    {
        QMessageBox :: warning(this, "警告", "请先导入数据");
        return;
    }
    auto PlaceData = GetNumbers({"输入横轴列号:", "输入纵轴列号:",
                                 "输入起始行号(默认为2):", "输入截止行号(默认为最后一列):"}, {-1, -1, 2, Table -> rowCount()});
    if(PlaceData.size() == 0)
    {
        return;
    }
    int ColumnX = PlaceData[0] - 1, ColumnY = PlaceData[1] - 1, BeginRow = PlaceData[2] - 1, EndRow = PlaceData[3] - 1;

    std :: vector<float> numbersX = CheckDiscrete(ColumnX, BeginRow, EndRow);
    std :: vector<float> numbersY = CheckDiscrete(ColumnY, BeginRow, EndRow);

    auto NameX = ui -> DataTable -> item(0, ColumnX) -> text();
    auto NameY = ui -> DataTable -> item(0, ColumnY) -> text();
    QDialog* ScatterShower = new QDialog(this);
    Scatter* ScatterObject = new Scatter(numbersX, numbersY, NameX, NameY, ScatterShower);
    ScatterShower -> show();
}

// 协方差
void  MainWindow :: MakeCovariance()
{
    if(!Inputted)
    {
        QMessageBox :: warning(this, "警告", "请先导入数据");
        return;
    }
    QStringList* Heads = new QStringList;
    QVector<int> Columns;
    int ColumnNum = 0, BeginRow = 0, EndRow = 0;

    // 手动输入/使用ColumnBar
    if(usage == 0)
    {
        auto PlaceData = GetNumbers({"输入列数:", "输入起始行号(默认为2):", "输入截止行号(默认为最后一列):"}, {-1, 2, Table -> rowCount()});
        if(PlaceData.size() == 0)
        {
            return;
        }
        ColumnNum = PlaceData[0];
        BeginRow = PlaceData[1] - 1;
        EndRow = PlaceData[2] - 1;

        for(int i = 0; i < ColumnNum; i ++)
        {
            auto Column = GetNumbers({"输入第" + QString :: number(i + 1) + "个列号:"}, {-1});
            if(Column.size() == 0)
            {
                return;
            }
            Columns.push_back(Column[0] - 1);
            Heads -> push_back(Table -> item(0, Columns[i]) -> text());
        }
    }
    else if(usage == 1)
    {
        if(!GetFromBar1(Columns, ColumnNum, BeginRow, EndRow))
        {
            return;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Heads -> push_back(Table -> item(0, Columns[i]) -> text());
        }
    }
    else if(usage == 2)
    {
        if(!GetFromBar2(Columns, ColumnNum, BeginRow, EndRow))
        {
            return;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Heads -> push_back(Table -> item(0, Columns[i]) -> text());
        }
    }

    std :: vector<std :: vector<float>> matrix;
    for(int i = 0; i < ColumnNum; i ++)
    {
        std :: vector<float> ColumnData;
        for(int row = BeginRow; row <= EndRow; row ++)
        {
            ColumnData.push_back(ui -> DataTable -> item(row, Columns[i]) -> text().toFloat());
        }
        matrix.push_back(ColumnData);
    }

    QDialog* CovarianceShower = new QDialog(this);
    Covariance* covarianceObject = new Covariance(matrix, Heads, CovarianceShower);
    CovarianceShower -> show();
}

// 相关性
void  MainWindow :: MakeCorrelation()
{
    if(!Inputted)
    {
        QMessageBox :: warning(this, "警告", "请先导入数据");
        return;
    }
    QStringList* Heads = new QStringList;
    QVector<int> Columns;
    int ColumnNum = 0, BeginRow = 0, EndRow = 0;

    if(usage == 0)
    {
        auto PlaceData = GetNumbers({"输入列数:", "输入起始行号(默认为2):", "输入截止行号(默认为最后一列):"}, {-1, 2, Table -> rowCount()});
        if(PlaceData.size() == 0)
        {
            return;
        }
        ColumnNum = PlaceData[0];
        BeginRow = PlaceData[1] - 1;
        EndRow = PlaceData[2] - 1;

        for(int i = 0; i < ColumnNum; i ++)
        {
            auto Column = GetNumbers({"输入第" + QString :: number(i + 1) + "个列号:"}, {-1});
            if(Column.size() == 0)
            {
                return;
            }
            Columns.push_back(Column[0] - 1);
            Heads -> push_back(Table -> item(0, Columns[i]) -> text());
        }
    }
    else if(usage == 1)
    {
        if(!GetFromBar1(Columns, ColumnNum, BeginRow, EndRow))
        {
            return;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Heads -> push_back(Table -> item(0, Columns[i]) -> text());
        }
    }
    else if(usage == 2)
    {
        if(!GetFromBar2(Columns, ColumnNum, BeginRow, EndRow))
        {
            return;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Heads -> push_back(Table -> item(0, Columns[i]) -> text());
        }
    }

    std :: vector<std :: vector<float>> matrix;
    for(int i = 0; i < ColumnNum; i ++)
    {
        std :: vector<float> ColumnData;
        for(int row = BeginRow; row <= EndRow; row ++)
        {
            ColumnData.push_back(ui -> DataTable -> item(row, Columns[i]) -> text().toFloat());
        }
        matrix.push_back(ColumnData);
    }

    QDialog* CorrelationShower = new QDialog(this);
    Correlation* correlationObjext = new Correlation(matrix, Heads, CorrelationShower);
    CorrelationShower -> show();
}

// 降维分析
void MainWindow :: MakePCA()
{
    if(!Inputted)
    {
        QMessageBox :: warning(this, "警告", "请先导入数据");
        return;
    }
    int ColumnNum = 0, BeginRow = 0, EndRow = 0, dimension = 0;
    QVector<int> Columns;

    if(usage == 0)
    {
        auto PlaceData = GetNumbers({"输入列数:",
                                     "输入起始行号(默认为2):", "输入截止行号(默认为最后一列):", "输入目标维数:"}, {-1, 2, Table -> rowCount(), -1});
        if(PlaceData.size() == 0)
        {
            return;
        }
        ColumnNum = PlaceData[0], BeginRow = PlaceData[1] - 1, EndRow = PlaceData[2] - 1, dimension = PlaceData[3];

        for(int i = 0; i < ColumnNum; i ++)
        {
            auto Column = GetNumbers({"输入第" + QString :: number(i + 1) + "个列号："}, {-1});
            if(Column.size() == 0)
            {
                return;
            }
            Columns.push_back(Column[0] - 1);
        }
    }
    else if(usage == 1)
    {
        if(!GetFromBar1(Columns, ColumnNum, BeginRow, EndRow))
        {
            return;
        }
        auto PlaceData = GetNumbers({"输入目标维数:"}, {-1});
        if(PlaceData.size() == 0)
        {
            return;
        }
        dimension = PlaceData[0];
    }
    else if(usage == 2)
    {
        if(!GetFromBar2(Columns, ColumnNum, BeginRow, EndRow))
        {
            return;
        }
        auto PlaceData = GetNumbers({"输入目标维数:"}, {-1});
        if(PlaceData.size() == 0)
        {
            return;
        }
        dimension = PlaceData[0];
    }
    if(dimension >= 4)
    {
        QMessageBox :: critical(this, "错误", "无法生成" + QString :: number(dimension) + "维图像");
        return;
    }

    if(ColumnNum < dimension)
    {
        QMessageBox :: critical(this, "错误", "维度不得低于列数");
        return;
    }
    std :: vector<std :: vector<float>> matrix;
    std :: vector<float>* group = new std :: vector<float>;
    *group = CheckDiscrete(1, BeginRow, EndRow);
    QMap<float, QString>* map = new QMap<float, QString>;
    *map = BuildMap(*group, BeginRow, EndRow);
    for(int row = BeginRow; row <= EndRow; row ++)
    {
        std :: vector<float> ColumnData;
        for(int i = 0; i < ColumnNum; i ++)
        {
            ColumnData.push_back(ui -> DataTable -> item(row, Columns[i]) -> text().toFloat());
        }
        matrix.push_back(ColumnData);
    }
    QDialog* PCAShower = new QDialog(this);
    PCA* PCAObject = new PCA(matrix, dimension, PCAShower, group, map);
    PCAShower -> show();
}

// 聚类分析
void MainWindow :: MakeKMeans()
{
    if(!Inputted)
    {
        QMessageBox :: warning(this, "警告", "请先导入数据");
        return;
    }
    int ColumnNum = 0, BeginRow = 0, EndRow = 0, k = 0;
    QVector<int> Columns;

    if(usage == 0)
    {
        auto PlaceData = GetNumbers({"输入列数:",
                                     "输入起始行号(默认为2):", "输入截止行号(默认为最后一列):", "输入聚合中心个数:"}, {-1, 2, Table -> rowCount(), -1});
        if(PlaceData.size() == 0)
        {
            return;
        }
        ColumnNum = PlaceData[0], BeginRow = PlaceData[1] - 1, EndRow = PlaceData[2] - 1, k = PlaceData[3];

        for(int i = 0; i < ColumnNum; i ++)
        {
            auto Column = GetNumbers({"输入第" + QString :: number(i + 1) + "个列号："}, {-1});
            if(Column.size() == 0)
            {
                return;
            }
            Columns.push_back(Column[0] - 1);
        }
    }
    else if(usage == 1)
    {
        if(!GetFromBar1(Columns, ColumnNum, BeginRow, EndRow))
        {
            return;
        }
        auto PlaceData = GetNumbers({"输入聚合中心个数:"}, {-1});
        if(PlaceData.size() == 0)
        {
            return;
        }
        k = PlaceData[0];
    }
    else if(usage == 2)
    {
        if(!GetFromBar2(Columns, ColumnNum, BeginRow, EndRow))
        {
            return;
        }
        auto PlaceData = GetNumbers({"输入聚合中心个数:"}, {-1});
        if(PlaceData.size() == 0)
        {
            return;
        }
        k = PlaceData[0];
    }

    if(ColumnNum < 2)
    {
        QMessageBox :: critical(this, "错误", "列数不得小于2");
        return;
    }
    if(k > 10)
    {
        QMessageBox :: warning(this, "警告", "不支持大于10个聚合中心");
        return;
    }
    std :: vector<std :: vector<float>> matrix;
    std :: vector<float>* group = new std :: vector<float>;
    *group = CheckDiscrete(1, BeginRow, EndRow);
    QMap<float, QString>* map = new QMap<float, QString>;
    *map = BuildMap(*group, BeginRow, EndRow);
    for(int row = BeginRow; row <= EndRow; row ++)
    {
        std :: vector<float> ColumnData;
        for(int i = 0; i < ColumnNum; i ++)
        {
            ColumnData.push_back(Table -> item(row, Columns[i]) -> text().toFloat());
        }
        matrix.push_back(ColumnData);
    }

    std :: vector<float>* VectorKMeans = new std :: vector<float>;
    auto VectorInt = std :: get<1>(clusterKMeans(matrix, k, 30));
    for(float number : VectorInt)
    {
        VectorKMeans -> push_back(number);
    }
    Table -> setColumnCount(Table -> columnCount() + 1);
    int column = Table -> columnCount() - 1;
    Table -> setItem(0, column, new QTableWidgetItem("KMeans"));
    for(int row = BeginRow, i = 0; row <= EndRow; row ++, i ++)
    {
        Table -> setItem(row, column, new QTableWidgetItem(QString :: number(VectorInt[i])));
    }
    for(int row = 1; row < BeginRow; row ++)
    {
        Table -> setItem(row, column, new QTableWidgetItem);
    }
    for(int row = Table -> rowCount() - 1; row > EndRow; row --)
    {
        Table -> setItem(row, column, new QTableWidgetItem);
    }

    QDialog* KMeansShower = new QDialog(this);
    KMeans* KMeansObjet = new KMeans(matrix, KMeansShower, VectorKMeans, map, group);
    KMeansShower -> show();
    ui -> paint -> setVisible(true);
}

// 为ColumnBar添加选中列
void MainWindow :: AddBar1()
{
    QList<QTableWidgetItem*> ItemList = Table -> selectedItems();
    if(ItemList.empty()){
        QMessageBox::warning(this,"警告","未选择列");
        return;
    }
    QList<int> NumberList;

    for(auto Item : ItemList)
    {
        NumberList.append(Item -> column() + 1);
    }
    ui -> ColumnBar1 -> AddBar(NumberList);
}
void MainWindow :: AddBar2()
{
    QList<QTableWidgetItem*> ItemList = Table -> selectedItems();
    if(ItemList.empty()){
        QMessageBox::warning(this,"警告","未选择列");
        return;
    }
    QList<int> NumberList;
    for(auto Item : ItemList)
    {
        NumberList.append(Item -> column() + 1);
    }
    ui -> ColumnBar2 -> AddBar(NumberList);
}

// 使用ColumnBar
void MainWindow :: UseBar1()
{
    if(ui -> UseBar1 -> text() == "保存并设为使用")
    {
        ui -> ColumnBar1 -> Saved = false;
        Bar1 -> SaveBar();
        if(ui -> ColumnBar1 -> Saved == true)
        {
            usage = 1;
            ui -> UseBar1 -> setText("取消使用");
            ui -> UseBar2 -> setText("保存并设为使用");
        }
    }
    else if(ui -> UseBar1 -> text() == "取消使用")
    {
        usage = 0;
        ui -> UseBar1 -> setText("保存并设为使用");
    }
}
void MainWindow :: UseBar2()
{
    if(ui -> UseBar2 -> text() == "保存并设为使用")
    {
        ui -> ColumnBar2 -> Saved = false;
        Bar2 -> SaveBar();
        if(ui -> ColumnBar2 -> Saved == true)
        {
            usage = 2;
            ui -> UseBar2 -> setText("取消使用");
            ui -> UseBar1 -> setText("保存并设为使用");
        }
    }
    else if(ui -> UseBar2 -> text() == "取消使用")
    {
        usage = 0;
        ui -> UseBar2 -> setText("保存并设为使用");
    }
}

// 从ColumnBar获取信息
bool MainWindow :: GetFromBar1(QVector<int>& Columns, int&ColumnNum, int&BeginRow, int&EndRow)
{
    Columns = Bar1 -> Columns;
    ColumnNum = Columns.size();
    if(ColumnNum == 0)
    {
        QMessageBox :: warning(this, "警告", "当前存储框中无元素");
        return false;
    }
    for(int i = 0; i < ColumnNum; i ++)
    {
        Columns[i] --;
    }
    BeginRow = Bar1 -> BeginRow - 1;
    EndRow = Bar1 -> EndRow - 1;
    return true;
}
bool MainWindow :: GetFromBar2(QVector<int>& Columns, int&ColumnNum, int&BeginRow, int&EndRow)
{
    Columns = Bar2 -> Columns;
    ColumnNum = Columns.size();
    if(ColumnNum == 0)
    {
        QMessageBox :: warning(this, "警告", "当前存储框中无元素");
        return false;
    }
    for(int i = 0; i < ColumnNum; i ++)
    {
        Columns[i] --;
    }
    BeginRow = Bar2 -> BeginRow - 1;
    EndRow = Bar2 -> EndRow - 1;
    return true;
}

// 聚类分析染色接口
void MainWindow :: Paint()
{
    if(ui -> paint -> text() == "根据聚类结果染色")
    {
        QVector<QColor> Colors = {Qt :: red, Qt :: blue, Qt :: green, Qt :: yellow, QColor(255, 192, 203), QColor(255, 165, 0),
                                     QColor(128, 128, 128), QColor(0, 128, 128), QColor(128, 0, 128), QColor(128, 128, 0)};
        int column = Table -> columnCount() - 1;
        for(int row = 1; row < Table -> rowCount(); row ++)
        {
            auto text = Table -> item(row, column) -> text();
            if(text.size() != 0)
            {
                auto color = Colors[text.toInt()];
                for(int i = 0; i <= column; i ++)
                {
                    Table -> item(row, i) -> setBackground(color);
                }
            }
        }
        ui -> paint -> setText("取消染色");
    }
    else
    {
        int row = Table -> rowCount(), column = Table -> columnCount();

        for(int i = 0; i < row; i ++)
        {
            for(int j = 0; j < column; j ++)
            {
                Table -> item(i, j) -> setBackground(Qt :: white);
            }
        }
        ui -> paint -> setText("根据聚类结果染色");
    }
}

void MainWindow :: MakeDBSCAN()
{
    if(!Inputted)
    {
        QMessageBox :: warning(this, "警告", "请先导入数据");
        return;
    }
    int ColumnNum = 0, BeginRow = 0, EndRow = 0, MinPts = 0;
    float eps = 0;
    QVector<int> Columns;

    if(usage == 0)
    {
        auto PlaceData = GetNumbers({"输入列数:", "输入起始行号(默认为2):",
                                     "输入截止行号(默认为最后一列):", "输入MinPts:"}, {-1, 2, Table -> rowCount(), -1});
        if(PlaceData.size() == 0)
        {
            return;
        }
        ColumnNum = PlaceData[0], BeginRow = PlaceData[1] - 1, EndRow = PlaceData[2] - 1, MinPts = PlaceData[3];
        float epstmp = GetFloat("请输入eps:");
        if(epstmp == -1)
        {
            return;
        }
        eps = epstmp;

        for(int i = 0; i < ColumnNum; i ++)
        {
            auto Column = GetNumbers({"输入第" + QString :: number(i + 1) + "个列号："}, {-1});
            if(Column.size() == 0)
            {
                return;
            }
            Columns.push_back(Column[0] - 1);
        }
    }
    else if(usage == 1)
    {
        if(!GetFromBar1(Columns, ColumnNum, BeginRow, EndRow))
        {
            return;
        }
        auto PlaceData = GetNumbers({"输入MinPts:"}, {-1});
        if(PlaceData.size() == 0)
        {
            return;
        }
        MinPts = PlaceData[0];
        float epstmp = GetFloat("请输入eps:");
        if(epstmp == -1)
        {
            return;
        }
        eps = epstmp;
    }
    else if(usage == 2)
    {
        if(!GetFromBar2(Columns, ColumnNum, BeginRow, EndRow))
        {
            return;
        }
        auto PlaceData = GetNumbers({"输入MinPts:"}, {-1});
        if(PlaceData.size() == 0)
        {
            return;
        }
        MinPts = PlaceData[0];
        int epstmp = GetFloat("请输入eps:");
        if(epstmp == -1)
        {
            return;
        }
        eps = epstmp;
    }

    if(ColumnNum < 2)
    {
        QMessageBox :: critical(this, "错误", "列数不得小于2");
        return;
    }

    std :: vector<std :: vector<float>> matrix;
    std :: vector<float>* group = new std :: vector<float>;
    *group = CheckDiscrete(1, BeginRow, EndRow);
    QMap<float, QString>* map = new QMap<float, QString>;
    *map = BuildMap(*group, BeginRow, EndRow);
    for(int row = BeginRow; row <= EndRow; row ++)
    {
        std :: vector<float> ColumnData;
        for(int i = 0; i < ColumnNum; i ++)
        {
            ColumnData.push_back(Table -> item(row, Columns[i]) -> text().toFloat());
        }
        matrix.push_back(ColumnData);
    }

    std :: vector<float>* VectorDBSCAN = new std :: vector<float>;
    auto VectorInt = clusterDBSCAN(matrix, eps, MinPts);
    for(float number : VectorInt)
    {
        VectorDBSCAN -> push_back(number);
    }
    Table -> setColumnCount(Table -> columnCount() + 1);
    int column = Table -> columnCount() - 1;
    Table -> setItem(0, column, new QTableWidgetItem("DBSCAN"));
    for(int row = BeginRow, i = 0; row <= EndRow; row ++, i ++)
    {
        Table -> setItem(row, column, new QTableWidgetItem(QString :: number(VectorInt[i])));
    }
    for(int row = 1; row < BeginRow; row ++)
    {
        Table -> setItem(row, column, new QTableWidgetItem);
    }
    for(int row = Table -> rowCount() - 1; row > EndRow; row --)
    {
        Table -> setItem(row, column, new QTableWidgetItem);
    }
    QDialog* DBSCANShower = new QDialog(this);
    DBSCAN* DBSCANObject = new DBSCAN(matrix, DBSCANShower, VectorDBSCAN, map, group);
    DBSCANShower -> show();
    ui -> paint -> setVisible(true);
}

float MainWindow :: GetFloat(QString tip)
{
    QDialog* window = new QDialog(this);
    window -> resize(300, 77);
    QLabel* label = new QLabel(tip, window);

    label -> setGeometry(0, 0, 200, 50);
    QLineEdit* edit = new QLineEdit(window);
    edit -> setGeometry(200, 0, 100, 50);

    QPushButton* OK = new QPushButton("确定", window);
    OK -> setGeometry(250, 50, 50, 25);

    float output = -1;
    connect(OK, &QPushButton :: clicked, [=, &output](){
        // 检查输入合法性
        QString text = edit -> text();
        if(text != 0)
        {
            QString CheakerStr = "^([0-9]\.[0-9]+|[1-9][0-9]*(\.[0-9]+)?)$";
            QRegularExpression cheaker(CheakerStr);
            if(!cheaker.match(text).hasMatch())
            {
                QMessageBox :: critical(window, "错误", "输入不合法，请检查");
                return;
            }
            output = text.toFloat();
        }
        else
        {
            // 弹出报错
            QMessageBox :: critical(window, "错误", "请输入");
            return;
        }
        window -> close();
    });
    window -> exec();
    return output;
}
