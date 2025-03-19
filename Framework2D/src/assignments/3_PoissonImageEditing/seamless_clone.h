#define CHANNEL_NUMBER 3

#include <Eigen/Sparse>

#include "common/image.h"
#include "imgui.h"

#include <memory>

namespace USTC_CG
{
// Assertion：
// 1. masked region 为矩形
// 2. masked region 和 image 长宽均大于 4 个像素
class SeamlessCloneRect
{
   public:
    SeamlessCloneRect() = default;
    SeamlessCloneRect(std::shared_ptr<Image> src_img,
                      std::shared_ptr<Image> tar_img,
                      std::shared_ptr<Image> selected_mask,
                      ImVec2 mouse_position)
                      : src_img_(src_img), tar_img_(tar_img),
                        selected_mask_(selected_mask),
                        offset_x_(mouse_position.x), offset_y_(mouse_position.y)
    {
    }

    std::shared_ptr<Image> solve(); // 给外部调用的接口，求解 Poisson 方程组，返回一个 Seamless Clone 的结果图像（和背景图像一样大，替换了选中区域）
    void update(std::shared_ptr<Image> src_img,
                     std::shared_ptr<Image> tar_img,
                     std::shared_ptr<Image> src_selected_mask,
                     ImVec2 mouse_position,
                     bool flag_mixed_gradient,
                     bool flag_matrix_preprocessing);

   private:
    std::shared_ptr<Image> src_img_; // 源图像
    std::shared_ptr<Image> tar_img_; // 背景图像
    std::shared_ptr<Image> selected_mask_;
    int offset_x_, offset_y_;        // 矩形区域在背景图像中的位置（例如，左上角的坐标）
    bool flag_mixed_gradient_, flag_matrix_preprocessing_;

    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver_;
    ImVec2 mask_top_left_p_, mask_bottom_right_p_;
};
}