// HW2_TODO: Implement the IDWWarper class
#pragma once

#include "warper.h"

using std::vector, std::pair;

namespace USTC_CG
{
class IDWWarper : public Warper
{
   public:
    IDWWarper() = default;
    ~IDWWarper() = default;

    // HW2_TODO: Implement the warp(...) function with IDW interpolation
    pair<int, int> warp(int x, int y) const override;

    // HW2_TODO: other functions or variables if you need
    void update(int n, vector<ImVec2> p, vector<ImVec2> q) override;

   private:
    vector<Eigen::Matrix2d> Ti;

    void solve_Ti(int i);

    float sigma(int i, Eigen::Vector2d p) const;
    float calc_weight_single(int i, Eigen::Vector2d p) const;
    Eigen::Vector2d calc_warp_single(int i, Eigen::Vector2d p) const;
};
}  // namespace USTC_CG