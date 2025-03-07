// FisheyeWarper.h
#pragma once

#include <cmath>

#include "warper.h"

namespace USTC_CG {
class FisheyeWarper : public Warper {
public:
    FisheyeWarper(int width, int height)
        : width_(width), height_(height) {}
    ~FisheyeWarper() = default;

    // 实现 fisheye 变形
    std::pair<int, int> warp(int x, int y) const override {
        float center_x = width_ / 2.0f;
        float center_y = height_ / 2.0f;
        float dx = x - center_x;
        float dy = y - center_y;
        float distance = std::sqrt(dx * dx + dy * dy);

        // 非线性变换： r -> r' = sqrt(r) * 10
        float new_distance = std::sqrt(distance) * 10;
        if (distance == 0)
            return { static_cast<int>(center_x), static_cast<int>(center_y) };

        float ratio = new_distance / distance;
        int new_x = static_cast<int>(center_x + dx * ratio);
        int new_y = static_cast<int>(center_y + dy * ratio);
        return { new_x, new_y };
    }

    void update(int n, vector<ImVec2> p, vector<ImVec2> q) override {
    }
private:
    int width_, height_;
};
}  // namespace USTC_CG
