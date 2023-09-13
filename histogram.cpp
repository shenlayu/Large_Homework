#include "histogram.h"
#include "visualization.h"
#include <QRegularExpression>

Histogram :: Histogram(const std :: vector<float>& InputData, QWidget *parent) : QWidget(parent)
{
    // 制图
    QChart* ChartHistogram = new QChart;
    QChartView* ChartViewHistogram = new QChartView(ChartHistogram);
    setFixedSize(900, 630);

    // 显示/输入控件
    QLabel* DimensionTip = new QLabel("输入组距：", this);
    DimensionTip -> setGeometry(0, 0, 60, 30);
    QLineEdit* SpaceInputter = new QLineEdit(this);
    SpaceInputter -> setGeometry(60, 0, 100, 30);
    QPushButton* MakeHistogram = new QPushButton("生成直方图", this);
    MakeHistogram -> setGeometry(160, 0, 240, 30);
    QPushButton* MakeNormalLine = new QPushButton("正态分布曲线", this);
    MakeNormalLine -> setGeometry(400, 0, 100, 30);
    MakeNormalLine -> setEnabled(false);
    QPushButton* Export = new QPushButton("导出", this);
    Export -> setGeometry(500, 0, 100, 30);

    connect(MakeHistogram, &QPushButton :: clicked, this, [=](){
        QString SpaceStr = SpaceInputter -> text();
        float space = 0;

        // 检查输入合法性
        QString CheakerStr = "^([0-9]\.[0-9]+|[1-9][0-9]*(\.[0-9]+)?)$";
        QRegularExpression cheaker(CheakerStr);
        if(!cheaker.match(SpaceStr).hasMatch())
        {
            QMessageBox :: critical(this, "错误", "输入不合法，请检查");
            return;
        }
        else
        {
            space = SpaceStr.toFloat();
        }

        if(MakeHistogram -> text() == "生成直方图")
        {
            HistogramVis(InputData, space, ChartHistogram);
            MakeHistogram -> setText("重新生成直方图并隐藏正态曲线");
        }
        else
        {
            HistogramVis(InputData, space, ChartHistogram, true);
        }
        MakeNormalLine -> setEnabled(true);
    });
    ChartViewHistogram -> setParent(this);
    ChartViewHistogram -> setGeometry(0, 30, 900, 600);

    // 绘制正态曲线
    connect(MakeNormalLine, &QPushButton :: clicked, [=](){
        NormalDistributionVis(InputData, ChartHistogram);
        MakeNormalLine -> setEnabled(false);
    });
    // 导出图片
    connect(Export, &QPushButton :: clicked, this, [=](){
        QString fileName = QFileDialog::getSaveFileName(this, tr("保存图片"), "", tr("图片文件 (*.png *.jpg *.bmp);;所有文件 (*)"));
        QPixmap image = ChartViewHistogram -> grab();
        if (!image.save(fileName))
        {
            QMessageBox :: warning(this, "警告", "导出失败");
        }
    });

    show();
}
