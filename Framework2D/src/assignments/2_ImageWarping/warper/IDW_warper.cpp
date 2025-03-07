#include <cmath>
#include <cstdio>
#include <iostream>

#include "IDW_warper.h"

namespace USTC_CG
{
float IDWWarper::sigma(int i, Eigen::Vector2d p) const
{
    // mu equals 2：计算距离倒数平方
    double dis = (p - this->p[i]).norm();
    const double eps = 1e-6; // 阈值保护，防止 dis 过小
    if(dis < eps)
        dis = eps;
    return 1.0 / (dis * dis);
}

void IDWWarper::solve_Ti(int i)
{
    // 当控制点数不足 3 个时，直接设局部变换为单位矩阵
    if(n < 3)
    {
        Ti[i] = Eigen::Matrix2d::Identity();
        return;
    }
    
    Eigen::Matrix2d A = Eigen::Matrix2d::Zero();
    Eigen::Matrix2d B = Eigen::Matrix2d::Zero();

    for (int j = 0; j < n; j++)
    {
        if (j == i)
            continue;

        Eigen::Vector2d diffP = p[j] - p[i];
        Eigen::Vector2d diffQ = q[j] - q[i];

        double s = sigma(i, p[j]);
        A += s * diffP * diffP.transpose();
        B += s * diffQ * diffP.transpose();
    }

    // 加正则项，防止 A 病态或奇异
    const double lambda = 1e-6;
    A += lambda * Eigen::Matrix2d::Identity();

    // 使用 LDLT 分解求解 A * X^T = B^T，再转置得到 T_i = B * A^{-1}
    Eigen::Matrix2d X_transpose = A.ldlt().solve(B.transpose());
    Ti[i] = X_transpose.transpose();
}

float IDWWarper::calc_weight_single(int i, Eigen::Vector2d p) const
{
    float denominator = 0;
    for (int j = 0; j < n; j++)
    {
        // Speical case: (x, y) is one of the p
        if (p == this->p[j])
            return 1;

        denominator += sigma(j, p);
    }
    return sigma(i, p) / denominator;
}

Eigen::Vector2d IDWWarper::calc_warp_single(int i, Eigen::Vector2d p) const
{
    return q[i] + Ti[i] * (p - this->p[i]);
}

pair<int, int> IDWWarper::warp(int x, int y) const
{
    if (n == 0)
        return make_pair(x, y);

    Eigen::Vector2d result = Eigen::Vector2d::Zero();
    for (int i = 0; i < n; i++)
    {
        Eigen::Vector2d p(x, y);
        result += calc_weight_single(i, p) * calc_warp_single(i, p);
    }
    return make_pair(result(0), result(1));
}

void IDWWarper::update(int n, vector<ImVec2> p, vector<ImVec2> q)
{
    this->n = n;
    this->p.clear();
    this->q.clear();
    this->Ti.resize(n);

    for (int i = 0; i < n; i++)
    {
        this->p.push_back(Eigen::Vector2d(p[i].x, p[i].y));
        this->q.push_back(Eigen::Vector2d(q[i].x, q[i].y));
    }

    for (int i = 0; i < n; i++)
    {
        solve_Ti(i);
    }
}
}