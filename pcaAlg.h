#pragma once
#ifndef PCAALG_H
#define PCAALG_H

#include "common.h"

extern Eigen::MatrixXf pca(const std::vector<std::vector<float>> &in, const int k);

#endif // PCA_HPP
