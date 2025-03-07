// HW2_TODO: Implement the RBFWarper class
#pragma once

#include "warper.h"

namespace USTC_CG
{
class RBFWarper : public Warper
{
   public:
    RBFWarper() = default;
    ~RBFWarper() = default;
    
    // HW2_TODO: Implement the warp(...) function with RBF interpolation
    pair<int, int> warp(int x, int y) const override;

    // HW2_TODO: other functions or variables if you need
    void update(int n, vector<ImVec2> p, vector<ImVec2> q) override;

   private:
    vector<Eigen::Vector2d> alpha;

    void solve_alpha(void);

    float gi(int i, Eigen::Vector2d p1, Eigen::Vector2d p2) const;
};
}  // namespace USTC_CG