#pragma once
#ifndef EXAMPLESALG_H
#define EXAMPLESALG_H

#include "common.h"

extern std::tuple<Eigen::VectorXf, float, float>
fitLeastSquareAndPR(const std::vector<float> &inX, const std::vector<float> &inY, const int inDegree);

#endif // EXAMPLES_HPP
