#pragma once

#include "shape.h"

namespace USTC_CG
{
class Ellipse : public Shape
{
    public:
        Ellipse() = default;

        Ellipse(
            float start_point_x,
            float start_point_y,
            float end_point_x,
            float end_point_y)
            : start_point_x_(start_point_x),
              start_point_y_(start_point_y),
              end_point_x_(end_point_x),
              end_point_y_(end_point_y)
        {
        }

        virtual ~Ellipse() = default;

        void draw(const Config& config) const override;

        void update(float x, float y) override;

    private:
        float start_point_x_, start_point_y_, end_point_x_, end_point_y_;
};
}