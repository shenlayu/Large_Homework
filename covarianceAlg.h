#pragma once
#ifndef COVARIANCEALG_H
#define COVARIANCEALG_H

#include "common.h"
#include "rowfeatureAlg.h"

extern Eigen::MatrixXf getCovariance(const std::vector<std::vector<float>> &inMat);

extern Eigen::MatrixXf getPearsonCorr(const Eigen::MatrixXf &cov, const std::vector<float> &vars);

#endif // COVARIANCE_HPP
