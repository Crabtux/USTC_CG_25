#include <cmath>

#include <Eigen/Dense>

#include "RBF_warper.h"

namespace USTC_CG
{

float RBFWarper::gi(int i, Eigen::Vector2d p1, Eigen::Vector2d p2) const
{
    if (n == 1)
        return 1.f;

    float ri = std::numeric_limits<float>::max();
    for (int j = 0; j < n; j++)
    {
        if (i == j)
            continue;

        float d = (p[i] - p[j]).norm();
        ri = std::min(ri, d);
    }

    float d = (p1 - p2).norm();
    return sqrt(d * d + ri * ri);
}

pair<int, int> RBFWarper::warp(int x, int y) const
{
    Eigen::Vector2d result(x, y);
    for (int i = 0; i < n; i++)
    {
        float gi_ = gi(i, Eigen::Vector2d(x, y), p[i]);
        result += gi_ * alpha[i];
    }
    return make_pair(result(0), result(1));
}

void RBFWarper::solve_alpha(void)
{
    Eigen::MatrixXf M(n, n);
    Eigen::MatrixXf B(n, 2);

    for (int j = 0; j < n; ++j)
    {
        for (int i = 0; i < n; ++i)
        {
            M(j, i) = gi(i, p[j], p[i]);
        }
        B(j, 0) = q[j][0] - p[j][0];
        B(j, 1) = q[j][1] - p[j][1];
    }

    Eigen::MatrixXf sol = M.colPivHouseholderQr().solve(B);

    alpha.resize(n);
    for (int i = 0; i < n; ++i)
    {
        alpha[i] = Eigen::Vector2d(sol(i, 0), sol(i, 1));
    }
}

void RBFWarper::update(int n, vector<ImVec2> p, vector<ImVec2> q)
{
    this->n = n;
    this->p.clear();
    this->q.clear();

    for (int i = 0; i < n; i++)
    {
        this->p.push_back(Eigen::Vector2d(p[i].x, p[i].y));
        this->q.push_back(Eigen::Vector2d(q[i].x, q[i].y));
    }

    // 当只有一个控制点时，直接采用平移变换
    if (n == 1)
    {
        alpha.resize(1);
        alpha[0] = this->q[0] - this->p[0];
    }
    else if (n > 1)
    {
        solve_alpha();
    }
}
}