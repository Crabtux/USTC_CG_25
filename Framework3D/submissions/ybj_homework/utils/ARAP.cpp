#include "ARAP.h"

// 不加这个头文件，会在动态链接时报 cross 函数的 undefined symbol 错误
#include <Eigen/Geometry>

namespace USTC_CG
{
// 计算由 halfedge 从顶点 i 到 j 的对角（在包含该 halfedge 的三角形中，顶点 k 处角度）的 cot 值
float computeCotan(const PolyMesh &mesh, const OpenMesh::HalfedgeHandle &he)
{
    // 获取 halfedge 对应的两个顶点：i 和 j
    auto vh_i = mesh.from_vertex_handle(he);
    auto vh_j = mesh.to_vertex_handle(he);
    
    // 获取所属的面句柄（注意：边界边可能没有关联面）
    auto fh = mesh.face_handle(he);
    if (!fh.is_valid())
    {
        return 0.f;
    }
    
    // 遍历该面上的所有顶点，找到除 i 和 j 之外的顶点 k
    OpenMesh::VertexHandle vh_k;
    for (auto vh : mesh.fv_range(fh))
    {
        if (vh != vh_i && vh != vh_j)
        {
            vh_k = vh;
            break;
        }
    }
    if (!vh_k.is_valid())
        throw std::runtime_error("无法找到三角形中与边 (i,j) 对应的第三个顶点！");
    
    auto p_i = mesh.point(vh_i);
    auto p_j = mesh.point(vh_j);
    auto p_k = mesh.point(vh_k);
    
    OpenMesh::Vec3f v_ki = p_i - p_k;
    OpenMesh::Vec3f v_kj = p_j - p_k;
    
    float dot = v_ki | v_kj;
    OpenMesh::Vec3f cross = v_ki % v_kj;
    float crossNorm = cross.norm();
    
    if (crossNorm == 0.0f)
        throw std::runtime_error("退化三角形，无法计算 cot 值！");
    
    return dot / crossNorm;
}

ARAP::ARAP(std::shared_ptr<PolyMesh> mesh_3d, std::shared_ptr<PolyMesh> initial_param)
{
    _u       = initial_param;
    _mesh_3d = mesh_3d;

    // 1. 初始化 _L 矩阵向量以及 _x
    for (const auto &f_t : mesh_3d->faces())
    {
        _L.push_back(Eigen::Matrix2f::Identity());

        std::vector<OpenMesh::DefaultTraits::Point> points;
        f_t.vertices().for_each([&](auto vh){
            points.push_back(mesh_3d->point(vh));
        });

        Eigen::Vector3f p0(points[0][0], points[0][1], points[0][2]);
        Eigen::Vector3f p1(points[1][0], points[1][1], points[1][2]);
        Eigen::Vector3f p2(points[2][0], points[2][1], points[2][2]);

        Eigen::Vector3f origin = p0;
        Eigen::Vector3f xAxis  = (p1 - p0).normalized();
        Eigen::Vector3f normal = (xAxis.cross(p2 - p0)).normalized();
        Eigen::Vector3f yAxis  = normal.cross(xAxis);

        Eigen::Vector3f diff1 = p1 - origin;
        Eigen::Vector3f diff2 = p2 - origin;

        OpenMesh::Vec2f x0(0.f, 0.f);
        OpenMesh::Vec2f x1(diff1.dot(xAxis), 0.f);
        OpenMesh::Vec2f x2(diff2.dot(xAxis), diff2.dot(yAxis));

        _x.push_back({x0, x1, x2});
    }

    // 2. 构建全局稀疏线性方程组
    Eigen::SparseMatrix<float> A(mesh_3d->n_vertices(), mesh_3d->n_vertices());
    std::vector<Eigen::Triplet<float>> triplet_list;
    for (const auto &vi : mesh_3d->vertices())
    {
        for (const auto &e_ij_handle : vi.outgoing_halfedges())
        {
            const auto &vj    = e_ij_handle.to();
            auto e_ji_handle  = e_ij_handle.opp();
            float coefficient = computeCotan(*mesh_3d, e_ij_handle)
                              + computeCotan(*mesh_3d, e_ji_handle);
            triplet_list.push_back(Eigen::Triplet<float>(vi.idx(), vi.idx(), coefficient));
            triplet_list.push_back(Eigen::Triplet<float>(vi.idx(), vj.idx(), -coefficient));
        }
    }

    // 固定顶点 0 和 1 的位置
    triplet_list.push_back(Eigen::Triplet<float>(0, 0, 1e10));
    triplet_list.push_back(Eigen::Triplet<float>(1, 1, 1e10));

    A.setFromTriplets(triplet_list.begin(), triplet_list.end());

    _solver = std::make_shared<Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>>>();
    _solver->compute(A);
}

void ARAP::local_phase()
{
    for (const auto &f_t : _u->faces())
    {
        size_t t = f_t.idx();
        Eigen::Matrix2f S_t = Eigen::Matrix2f::Zero();
        for (int i = 0; i < 3; i++)
        {
            int i_plus_1 = (i + 1) % 3;
            int i_plus_2 = (i + 2) % 3;

            auto u_t_vertices = f_t.vertices().to_vector();
            auto u_diff = _u->point(u_t_vertices.at(i)) - _u->point(u_t_vertices.at(i_plus_1));
            auto x_diff = _x.at(t).at(i) - _x.at(t).at(i_plus_1);

            Eigen::Vector2f u_vec(u_diff[0], u_diff[1]);
            Eigen::Vector2f x_vec(x_diff[0], x_diff[1]);

            auto e1 = _u->point(u_t_vertices.at(i_plus_2)) - _u->point(u_t_vertices.at(i));
            auto e2 = _u->point(u_t_vertices.at(i_plus_2)) - _u->point(u_t_vertices.at(i_plus_1));
            auto cot_value = (e1 | e2) / (e1 % e2).length();

            S_t += cot_value * u_vec * x_vec.transpose();
        }

        Eigen::JacobiSVD<Eigen::Matrix2f> svd(S_t, Eigen::ComputeFullU | Eigen::ComputeFullV);
        _L[t] = svd.matrixU() * svd.matrixV().transpose();
    }
}

void ARAP::global_phase()
{
    _m.clear();

    const int CHANNEL_NUM = 2;
    std::vector<Eigen::VectorXf> result(CHANNEL_NUM, Eigen::VectorXf::Zero(_mesh_3d->n_vertices()));

    for (const auto &vi : _mesh_3d->vertices())
    {
        for (const auto &e_ij_handle : vi.outgoing_halfedges())
        {
            const auto &vj   = e_ij_handle.to();

            auto f_ij_handle = _mesh_3d->face_handle(e_ij_handle);
            if (f_ij_handle.is_valid())
            {
                auto coefficient_ij = computeCotan(*_mesh_3d, e_ij_handle);

                Eigen::Matrix2f coefficient_matrix =
                    coefficient_ij * _L[f_ij_handle.idx()];

                auto find_x_i = [&](OpenMesh::Vec3f origin, std::array<OpenMesh::Vec2f, 3> x, OpenMesh::SmartFaceHandle face)
                {
                    std::vector<OpenMesh::DefaultTraits::Point> points;
                    face.vertices().for_each([&](auto pt){
                        points.push_back(_mesh_3d->point(pt));
                    });
                    for (int i = 0; i < 3; i++)
                    {
                        if (points.at(i) == origin)
                        {
                            return x.at(i);
                        }
                    }
                };

                OpenMesh::Vec2f x_i, x_j;
                Eigen::Vector2f x_ij;

                x_i = find_x_i(_mesh_3d->point(vi), _x.at(f_ij_handle.idx()), f_ij_handle);
                x_j = find_x_i(_mesh_3d->point(vj), _x.at(f_ij_handle.idx()), f_ij_handle);
                x_ij << (x_i[0] - x_j[0]), (x_i[1] - x_j[1]);

                result[0](vi.idx()) += (coefficient_matrix * x_ij)[0];
                result[1](vi.idx()) += (coefficient_matrix * x_ij)[1];

                result[0](vj.idx()) -= (coefficient_matrix * x_ij)[0];
                result[1](vj.idx()) -= (coefficient_matrix * x_ij)[1];
            }
        }
    }

    // 把顶点 0 和 1 钉死
    auto p0 = _u->point(_u->vertex_handle(0)),
         p1 = _u->point(_u->vertex_handle(1));

    result[0](0) += p0[0];
    result[1](0) += p0[1];

    result[0](1) += p1[0];
    result[1](1) += p1[1];

    result[0] = _solver->solve(result[0]);
    result[1] = _solver->solve(result[1]);

    for (const auto &vi : _u->vertices())
    {
        OpenMesh::Vec3f new_point(result[0](vi.idx()), result[1](vi.idx()), 0);
        _u->set_point(vi, new_point);
    }
}

void ARAP::unitlize()
{
    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();

    for (auto vh : _u->vertices())
    {
        const auto& pt = _u->point(vh);
        min_x = std::min(min_x, pt[0]);
        max_x = std::max(max_x, pt[0]);
        min_y = std::min(min_y, pt[1]);
        max_y = std::max(max_y, pt[1]);
    }

    float scale_x = max_x - min_x;
    float scale_y = max_y - min_y;

    for (auto vh : _u->vertices()) {
        auto pt = _u->point(vh);
        pt[0] = (pt[0] - min_x) / scale_x;
        pt[1] = (pt[1] - min_y) / scale_y;
        _u->set_point(vh, pt);
    }
}

std::shared_ptr<PolyMesh> ARAP::get_param() const
{
    return _u;
}
} // namespace USTC_CG
