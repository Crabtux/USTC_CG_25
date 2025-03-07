// HW2_TODO: Please implement the abstract class Warper
// 1. The Warper class should abstract the **mathematical mapping** involved in
// the warping problem, **independent of image**.
// 2. The Warper class should have a virtual function warp(...) to be called in
// our image warping application.
//    - You should design the inputs and outputs of warp(...) according to the
//    mathematical abstraction discussed in class.
//    - Generally, the warping map should map one input point to another place.
// 3. Subclasses of Warper, IDWWarper and RBFWarper, should implement the
// warp(...) function to perform the actual warping.
#pragma once

#include <vector>

#include <Eigen/Dense>

#include "imgui.h"

using std::vector, std::pair, std::make_pair;

namespace USTC_CG
{
class Warper
{
   public:
    virtual ~Warper() = default;

    // HW2_TODO: A virtual function warp(...)
    virtual pair<int, int> warp(int x, int y) const = 0;
    
    // HW2_TODO: other functions or variables if you need
    virtual void update(int n, vector<ImVec2> p, vector<ImVec2> q) = 0;

   protected:
    int n;
    std::vector<Eigen::Vector2d> p, q;
};
}  // namespace USTC_CG