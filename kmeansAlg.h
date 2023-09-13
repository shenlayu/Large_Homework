#pragma once
#ifndef KMENASALG_H
#define KMENASALG_H

#include "common.h"

extern std::tuple<Eigen::MatrixXf, std::vector<int>>
clusterKMeans(const std::vector<std::vector<float>> &in, const int k, const int maxIter);

#endif // KMENAS_HPP
