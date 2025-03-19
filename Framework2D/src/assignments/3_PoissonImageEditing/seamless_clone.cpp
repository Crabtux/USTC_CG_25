#include "seamless_clone.h"

#include <iostream>
#include <vector>
#include <chrono>

namespace USTC_CG
{
bool operator==(const ImVec2 &p1, const ImVec2 &p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

char bound_check(int x, int y, int width, int height)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return 'O';
    if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
        return 'B';
    return 'I';
}

// std::clamp 需要 C++17
unsigned char toUChar (double value) {
    return static_cast<unsigned char>(std::clamp(value, 0.0, 255.0));
};

const int neighbours[4][2] = {
    1, 0,
    -1, 0,
    0, 1,
    0, -1
};

std::shared_ptr<Image> SeamlessCloneRect::solve()
{
    int Ws = src_img_->width(), Hs = src_img_->height();
    int Wt = tar_img_->width(), Ht = tar_img_->height();

    // 获取 masked region 信息
    int Wo = mask_bottom_right_p_.x - mask_top_left_p_.x + 1;
    int Ho = mask_bottom_right_p_.y - mask_top_left_p_.y + 1;

    // 填写方程组右边的结果向量
    std::vector<Eigen::VectorXd> color;
    for (int i = 0; i < CHANNEL_NUMBER; i++)
        color.push_back(Eigen::VectorXd::Zero(Wo * Ho));

    for (int x = 0; x < Wo; x++)
    {
        for (int y = 0; y < Ho; y++)
        {
            int idx    = y * Wo + x;
            int src_px = x + mask_top_left_p_.x, src_py = y + mask_top_left_p_.y;
            int dst_px = x + offset_x_,          dst_py = y + offset_y_;
            std::vector<ImVec2> Np;

            // 对于 src_p，求 Np
            for (int i = 0; i < 4; i++)
            {
                int dx = neighbours[i][0], dy = neighbours[i][1];
                if (bound_check(src_px + dx, src_py + dy, Ws, Hs))
                    Np.push_back(ImVec2(src_px + dx, src_py + dy));
            }

            // 内部梯度引导
            for (auto q1 : Np)
            {
                ImVec2 q2(q1.x - mask_top_left_p_.x + offset_x_,
                          q1.y - mask_top_left_p_.y + offset_y_);
                for (int chan = 0; chan < CHANNEL_NUMBER; chan++)
                {
                    int src_dv = src_img_->get_pixel(src_px, src_py)[chan]
                               - src_img_->get_pixel(q1.x,   q1.y)[chan];
                    if (bound_check(q2.x, q2.y, Wt, Ht)     != 'O' &&
                        bound_check(dst_px, dst_py, Wt, Ht) != 'O' &&
                        flag_mixed_gradient_)
                    {
                        // 梯度融合
                        int dst_dv = tar_img_->get_pixel(dst_px, dst_py)[chan]
                                   - tar_img_->get_pixel(q2.x, q2.y)[chan];
                        color[chan](idx) += (std::abs(src_dv) < std::abs(dst_dv) ? dst_dv : src_dv);
                    }
                    else
                    {
                        color[chan][idx] += src_dv;
                    }
                }
            }

            // 加入边界条件
            int tar_x = x + offset_x_, tar_y = y + offset_y_;
            for (int i = 0; i < 4; i++)
            {
                int dx = neighbours[i][0], dy = neighbours[i][1];
                if (bound_check(x + dx + 1, y + dy + 1, Wo + 2, Ho + 2) == 'B' &&
                    bound_check(tar_x + dx, tar_y + dy, Wt, Ht) != 'O')
                {
                    for (int chan = 0; chan < CHANNEL_NUMBER; chan++)
                        color[chan](idx) += tar_img_->get_pixel(tar_x + dx, tar_y + dy)[chan];
                }
            }
        }
    }

    // 求解稀疏方程组
    for (int i = 0; i < CHANNEL_NUMBER; i++)
        color[i] = solver_.solve(color[i]);

    // 把结果填回去
    for (int x = 0; x < Wo; x++)
    {
        for (int y = 0; y < Ho; y++)
        {
            int idx   = y * Wo + x;
            int tar_x = x + offset_x_, tar_y = y + offset_y_;
            if (bound_check(tar_x, tar_y, Wt, Ht) != 'O')
            {
                std::vector<unsigned char> color_;
                for (int i = 0; i < CHANNEL_NUMBER; i++)
                    color_.push_back(toUChar(color[i](idx)));
                tar_img_->set_pixel(tar_x, tar_y, color_);
            }
        }
    }

    return tar_img_;
}

void SeamlessCloneRect::update(std::shared_ptr<Image> src_img,
                               std::shared_ptr<Image> tar_img,
                               std::shared_ptr<Image> src_selected_mask,
                               ImVec2 mouse_position,
                               bool flag_mixed_gradient,
                               bool flag_matrix_preprocessing)
{
    // 用不到的信息先更新
    tar_img_  = tar_img;
    offset_x_ = mouse_position.x;
    offset_y_ = mouse_position.y;
    flag_mixed_gradient_ = flag_mixed_gradient;
    flag_matrix_preprocessing_ = flag_matrix_preprocessing;

    // 获取 masked region 信息
    int Ws = src_img->width(), Hs = src_img->height();
    ImVec2 top_left_p(-1, -1), bottom_right_p(-1, -1);
    for (int x = 0; x < Ws; x++)
    {
        for (int y = 0; y < Hs; y++)
        {
            if (src_selected_mask->get_pixel(x, y)[0] > 0)
            {
                bottom_right_p = ImVec2(x, y);
                if (top_left_p.x == -1 && top_left_p.y == -1)
                    top_left_p = ImVec2(x, y);
            }
        }
    }
    
    // masked region 变了，重新做矩阵预分解
    if (!flag_matrix_preprocessing_ ||
        src_img_             != src_img ||
        mask_top_left_p_     != top_left_p || 
        mask_bottom_right_p_ != bottom_right_p)
    {
        src_img_             = src_img;
        mask_top_left_p_     = top_left_p;
        mask_bottom_right_p_ = bottom_right_p;

        int Wo = bottom_right_p.x - top_left_p.x + 1;
        int Ho = bottom_right_p.y - top_left_p.y + 1;

        // 系数矩阵与结果向量
        Eigen::SparseMatrix<double> A(Wo * Ho, Wo * Ho);
        std::vector<Eigen::Triplet<double>> triplet_list;

        // 构造稀疏方程组，然后做矩阵预分解
        for (int x = 0; x < Wo; x++)
        {
            for (int y = 0; y < Ho; y++)
            {
                int idx   = y * Wo + x;
                int src_x = x + top_left_p.x, src_y = y + top_left_p.y;
                std::vector<ImVec2> Np;

                for (int i = 0; i < 4; i++)
                {
                    int dx = neighbours[i][0], dy = neighbours[i][1];
                    if (bound_check(src_x + dx, src_y + dy, Ws, Hs))
                        Np.push_back(ImVec2(src_x + dx, src_y + dy));
                }

                for (auto q : Np)
                {
                    int qx_ = q.x - top_left_p.x, qy_ = q.y - top_left_p.y;
                    if (bound_check(qx_, qy_, Wo, Ho) != 'O')
                    {
                        int idx_q = qy_ * Wo + qx_;
                        triplet_list.push_back(Eigen::Triplet<double>(idx, idx_q, -1.0));
                    }
                }
                triplet_list.push_back(Eigen::Triplet<double>(idx, idx, Np.size()));
            }
        }
        A.setFromTriplets(triplet_list.begin(), triplet_list.end());
        solver_.compute(A);
    }
}
}