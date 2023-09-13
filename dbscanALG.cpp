#include <iostream>
#include <vector>
#include <cmath>

// 计算两个点距离
float distance(std :: vector<float> point1, std :: vector<float> point2) // 应该算相对距离
{
    int dimension = point1.size();
    float value = 0; // 维度和返回值
    for(int i = 0; i < dimension; i ++)
    {
        value += std :: sqrt((point1[i] - point2[i]) * (point1[i] - point2[i]));
    }
    return value;
}

// 递归
void recursion(std :: vector<int>& Category, const std :: vector<std :: vector<float>> Points, int id, int cluster, float eps, int MinPts)
{
    Category[id] = cluster; // 加入聚类中
    int PtsNum = Points.size();

    std :: vector<int> UnvisitedNeighbours;
    int NeighboursNum = 0;
    for(int Judged = 0; Judged < PtsNum; Judged ++)
    {
        if(Judged == id) // 邻居不算自己
        {
            continue;
        }
        if(Category[Judged] != 0 && Category[Judged] != cluster) // 是别的类的点
        {
            continue;
        }
        if(distance(Points[id], Points[Judged]) <= eps) // 二者是邻居且对方是自己类或未访问点
        {
            if(Category[Judged] == cluster) // 是自己类已访问点
            {
                NeighboursNum ++;
                continue;
            }
            UnvisitedNeighbours.push_back(Judged);
            NeighboursNum ++;
            if(NeighboursNum >= MinPts) // 核心点！
            {
                for(auto id : UnvisitedNeighbours)
                {
                    recursion(Category, Points, id, cluster, eps, MinPts); // 遍历邻居以递归
                }
            }
        }
    }
}

// 返回每个点所属聚类，噪声点返回为0
std :: vector<int> clusterDBSCAN(std :: vector<std :: vector<float>> Points, float eps, int MinPts)
{
    int PtsNum = Points.size(); // 点个数
    //std :: vector<int> Category(PtsNum, 0); // 点的种类：1为核心点，2为非核心点，0为噪声点
    std :: vector<int> Category(PtsNum, 0); // 聚类，0为噪声点
    int cluster = 0; // 当前聚类个数

    // 先找到核心点，给其挂类,遍历其邻居，挂类，若为核心点再遍历邻居
    // 最外层遍历
    for(int Chosen = 0; Chosen < PtsNum; Chosen ++)
    {
        if(Category[Chosen] == 0) // 发现有希望的点
        {
            std :: vector<int> neighbours; // 选中点的邻居们
            for(int Judged = 0; Judged < PtsNum; Judged ++) // 遍历其他所有点，给当前选中点定性
            {
                if(Judged == Chosen) // 邻居不算自己
                {
                    continue;
                }
                if(Category[Judged] != 0)
                {
                    continue;
                }
                if(distance(Points[Chosen], Points[Judged]) <= eps) // 二者是邻居
                {
                    neighbours.push_back(Judged);
                }
            }
            int NeighboursNum = neighbours.size(); // 邻居数量
            if(NeighboursNum >= MinPts) // 核心点！
            {
                Category[Chosen] = ++ cluster; // 新建聚类，加入该点
                for(auto id : neighbours)
                {
                    recursion(Category, Points, id, cluster, eps, MinPts); // 遍历邻居以递归
                }
            }
        }
    }
    return Category;
}
