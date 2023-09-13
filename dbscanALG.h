#pragma once
#ifndef DBSCANALG_H
#define DBSCANALG_H

#include "common.h"
#include "rowfeatureAlg.h"
float distance(std :: vector<float> point1, std :: vector<float> point2);

void recursion(std :: vector<int>& Category, const std :: vector<std :: vector<float>> Points, int id, int cluster, float eps, int MinPts);

std :: vector<int> clusterDBSCAN(std :: vector<std :: vector<float>> Points, float eps, int MinPts);
#endif
