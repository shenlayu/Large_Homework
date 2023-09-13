#pragma once
#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "src/common.h"
#include <QTableWidget>
#include <QtCharts>
#include <QtDataVisualization/Q3DScatter>

extern QTableWidget* TableVis(const QList<QStringList>&, const QStringList* const HeadsX = nullptr, QStringList* HeadsY = nullptr);
extern QChart* HistogramVis(const std :: vector<float>&, float, QChart* ExistedChart = nullptr, bool ClearExistedChart = false);
extern QChart* NormalDistributionVis(const std :: vector<float>&, QChart* ExistedChart = nullptr);
extern std :: pair<QChart*, QScatterSeries*>ScatterVis(const std :: vector<float>&, const std :: vector<float>&,
       const QString&, const QString&, std :: vector<int>* usage = nullptr,
       QMap<float, QString>* const SeriesName = nullptr, QChart* ExistedChart = nullptr);
extern QChart* PolynomialLineVis(const std :: vector<float>&, const std :: vector<float>& , int dimension, QChart* ExistedChart = nullptr);
extern std :: pair<QtDataVisualization :: Q3DScatter*, QtDataVisualization::QScatter3DSeries*> Scatter3DVis(const Eigen :: MatrixXf&,
       std :: vector<int>* usage = nullptr, QMap<float, QString>* map = nullptr, QtDataVisualization :: Q3DScatter* ExistedChart = nullptr);

#endif // VISUALIZATION_H
