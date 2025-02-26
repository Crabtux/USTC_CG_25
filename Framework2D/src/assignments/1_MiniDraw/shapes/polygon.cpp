#include "polygon.h"

#include <iostream>

namespace USTC_CG
{
void Polygon::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    for (std::size_t i1 = 0, i2 = 1; i2 < points_.size(); i1++, i2++) {
        draw_list->AddLine(
            ImVec2(
                config.bias[0] + points_[i1].x,
                config.bias[1] + points_[i1].y
            ),
            ImVec2(
                config.bias[0] + points_[i2].x,
                config.bias[1] + points_[i2].y
            ),
            IM_COL32(
                config.line_color[0],
                config.line_color[1],
                config.line_color[2],
                config.line_color[3]),
            config.line_thickness
        );
    }
}

void Polygon::update(float x, float y)
{
    points_[points_.size() - 1].x = x;
    points_[points_.size() - 1].y = y;
}

void Polygon::add_control_point(float x, float y)
{
    points_.push_back(ImVec2(x, y));
}

void Polygon::end_drawing()
{
    update(points_[0].x, points_[0].y);
}
}