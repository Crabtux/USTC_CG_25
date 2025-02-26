#pragma once

#include <vector>

#include "imgui.h"
#include "shape.h"

namespace USTC_CG
{
class Polygon : public Shape
{
    public:
        Polygon() = default;

        Polygon(
            float start_point_x,
            float start_point_y)
        {
            // Start Point
            points_.push_back(ImVec2(start_point_x, start_point_y));

            // Current Point
            points_.push_back(ImVec2(start_point_x, start_point_y));
        }

        virtual ~Polygon() = default;

        void draw(const Config& config) const override;

        void update(float x, float y) override;
        
        void add_control_point(float x, float y);

        void end_drawing();
    
    private:
        std::vector<ImVec2> points_;
};
}