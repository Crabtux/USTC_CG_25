#include "GCore/Components/MeshOperand.h"
#include "GCore/util_openmesh_bind.h"
#include "geom_node_base.h"
#include <cmath>
#include <vector>
#include <map>
#include <time.h>
#include <Eigen/Sparse>
#include <iostream>

    /*
    ** @brief HW4_TutteParameterization
    **
    ** This file presents the basic framework of a "node", which processes inputs
    ** received from the left and outputs specific variables for downstream nodes to
    ** use.
    ** - In the first function, node_declare, you can set up the node's input and
    ** output variables.
    ** - The second function, node_exec is the execution part of the node, where we
    ** need to implement the node's functionality.
    ** Your task is to fill in the required logic at the specified locations
    ** within this template, especially in node_exec.
    */

NODE_DEF_OPEN_SCOPE

// 一些便利的定义
const int CHANNEL_NUMBER = 3;

enum WeightType {
    uniform_weight,
    cotangent_weight,
    floater_weight
};

NODE_DECLARATION_FUNCTION(min_surf)
{
    // Input-1: Original 3D mesh with boundary
    b.add_input<Geometry>("Input");
    b.add_input<int>("WeightType");

    /*
    ** NOTE: You can add more inputs or outputs if necessary. For example, in
    *some cases,
    ** additional information (e.g. other mesh geometry, other parameters) is
    *required to perform
    ** the computation.
    **
    ** Be sure that the input/outputs do not share the same name. You can add
    *one geometry as
    **
    **                b.add_input<Geometry>("Input");
    **
    ** Or maybe you need a value buffer like:
    **
    **                b.add_input<float1Buffer>("Weights");
    */

    // Output-1: Minimal surface with fixed boundary
    b.add_output<Geometry>("Output");
}

NODE_EXECUTION_FUNCTION(min_surf)
{
    // Get the input from params
    auto input       = params.get_input<Geometry>("Input");
    int  weight_type = params.get_input<int>("WeightType");

    // (TO BE UPDATED) Avoid processing the node when there is no input
    if (!input.get_component<MeshComponent>()) {
        throw std::runtime_error("Minimal Surface: Need Geometry Input.");
        return false;
    }

    /* ----------------------------- Preprocess -------------------------------
    ** Create a halfedge structure (using OpenMesh) for the input mesh. The
    ** half-edge data structure is a widely used data structure in geometric
    ** processing, offering convenient operations for traversing and modifying
    ** mesh elements.
    */
    std::shared_ptr<USTC_CG::PolyMesh> halfedge_mesh = operand_to_openmesh(&input);

    /* ---------------- [HW4_TODO] TASK 1: Minimal Surface --------------------
    ** In this task, you are required to generate a 'minimal surface' mesh with
    ** the boundary of the input mesh as its boundary.
    **
    ** Specifically, the positions of the boundary vertices of the input mesh
    ** should be fixed. By solving a global Laplace equation on the mesh,
    ** recalculate the coordinates of the vertices inside the mesh to achieve
    ** the minimal surface configuration.
    **
    ** (Recall the Poisson equation with Dirichlet Boundary Condition in HW3)
    */

    /*
    ** Algorithm Pseudocode for Minimal Surface Calculation
    ** ------------------------------------------------------------------------
    ** 1. Initialize mesh with input boundary conditions.
    **    - For each boundary vertex, fix its position.
    **    - For internal vertices, initialize with initial guess if necessary.
    **
    ** 2. Construct Laplacian matrix for the mesh.
    **    - Compute weights for each edge based on the chosen weighting scheme
    **      (e.g., uniform weights for simplicity).
    **    - Assemble the global Laplacian matrix.
    **
    ** 3. Solve the Laplace equation for interior vertices.
    **    - Apply Dirichlet boundary conditions for boundary vertices.
    **    - Solve the linear system (Laplacian * X = 0) to find new positions
    **      for internal vertices.
    **
    ** 4. Update mesh geometry with new vertex positions.
    **    - Ensure the mesh respects the minimal surface configuration.
    **
    ** Note: This pseudocode outlines the general steps for calculating a
    ** minimal surface mesh given fixed boundary conditions using the Laplace
    ** equation. The specific implementation details may vary based on the mesh
    ** representation and numerical methods used.
    **
    */

    // 找内点与 index 映射
    std::unordered_map<int, int> index_map;
    int inner_vertex_num = 0;
    for (const auto &vi : halfedge_mesh->vertices())
    {
        if (!vi.is_boundary())
        {
            index_map[vi.idx()] = inner_vertex_num;
            inner_vertex_num++;
        }
    }

    // 构造稀疏方程组
    Eigen::SparseMatrix<float> A(inner_vertex_num, inner_vertex_num);
    std::vector<Eigen::Triplet<float>> triplet_list;
    std::vector<Eigen::VectorXf> result;
    for (int i = 0; i < CHANNEL_NUMBER; i++)
        result.push_back(Eigen::VectorXf::Zero(inner_vertex_num));

    for (const auto &vi : halfedge_mesh->vertices())
    {
        if (!vi.is_boundary())
        {
            // 根据不同的权值类型，计算 weights 向量
            std::vector<float> weights;
            if (weight_type == uniform_weight)
            {
                for (const auto& halfedge_handle : vi.outgoing_halfedges())
                {
                    weights.push_back(1.f);
                }
            }
            else if (weight_type == cotangent_weight)
            {
                std::vector<int> idx_ij_vec;

                // 构建循环链表，算权值
                for (const auto& halfedge_handle : vi.outgoing_halfedges())
                {
                    const auto &vj = halfedge_handle.to();
                    idx_ij_vec.push_back(vj.idx());
                }

                int size_ = idx_ij_vec.size();
                for (int i = 0; i < size_; i++)
                {
                    int vj_idx      = idx_ij_vec.at(i);
                    int last_vj_idx = idx_ij_vec.at((i - 1 + size_) % size_);
                    int next_vj_idx = idx_ij_vec.at((i + 1 + size_) % size_);

                    auto A = halfedge_mesh->point(halfedge_mesh->vertex_handle(vi.idx()));
                    auto B = halfedge_mesh->point(halfedge_mesh->vertex_handle(vj_idx));
                    auto C = halfedge_mesh->point(halfedge_mesh->vertex_handle(last_vj_idx));
                    auto D = halfedge_mesh->point(halfedge_mesh->vertex_handle(next_vj_idx));

                    // 计算向量 AC 和 BC
                    OpenMesh::Vec3f AC = C - A;
                    OpenMesh::Vec3f BC = C - B;

                    // 计算余切 ACB = (AC . BC) / |AC x BC|
                    float cot_ACB = AC | BC / (AC % BC).length();  // (AC % BC) 是叉积，返回一个向量，.length() 得到它的模长

                    // 计算向量 AD 和 BD
                    OpenMesh::Vec3f AD = D - A;
                    OpenMesh::Vec3f BD = D - B;

                    // 计算余切 ADB = (AD . BD) / |AD x BD|
                    float cot_ADB = (AD | BD) / (AD % BD).length();  // (AD % BD) 是叉积，返回一个向量，.length() 得到它的模长

                    // 计算 cot ACB + cot ADB
                    float weight = fabs(cot_ACB + cot_ADB);

                    // 用于维持数值稳定性，不加会出事
                    if (fabs(weight) > 1e-2)
                    {
                        weights.push_back(weight);
                    }
                    else
                    {
                        weights.push_back(0.f);
                    }
                }
            }
            // 防崩溃用的
            else
            {
                for (const auto& halfedge_handle : vi.outgoing_halfedges())
                {
                    weights.push_back(1.f);
                }
            }

            // 根据 weights 向量，构建稀疏方程组
            float weight_sum = 0.f;
            int idx_i  = index_map[vi.idx()], vj_idx = 0;
            for (const auto& halfedge_handle : vi.outgoing_halfedges())
            {
                float weight = weights.at(vj_idx++);
                const auto &vj = halfedge_handle.to();
                if (!vj.is_boundary())
                {
                    int idx_j = index_map[vj.idx()];
                    triplet_list.push_back(Eigen::Triplet<float>(idx_i, idx_j, -weight));
                }
                else
                {
                    const auto &position_j = halfedge_mesh->point(vj);
                    for (int i = 0; i < CHANNEL_NUMBER; i++)
                        result[i](idx_i) += weight * position_j[i];
                }
                weight_sum += weight;
            }
            triplet_list.push_back(Eigen::Triplet<float>(idx_i, idx_i, weight_sum));
        }
    }

    A.setFromTriplets(triplet_list.begin(), triplet_list.end());

    // 求解稀疏方程组
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;
    solver.compute(A);
    for (int i = 0; i < CHANNEL_NUMBER; i++)
        result[i] = solver.solve(result[i]);

    // 把结果填回去
    for (const auto &vi : halfedge_mesh->vertices())
    {
        if (!vi.is_boundary())
        {
            int idx_i = index_map[vi.idx()];
            OpenMesh::Vec3f newPoint(result[0](idx_i), result[1](idx_i), result[2](idx_i));
            halfedge_mesh->set_point(vi, newPoint);
        }
    }

    /* ----------------------------- Postprocess ------------------------------
    ** Convert the minimal surface mesh from the halfedge structure back to
    ** Geometry format as the node's output.
    */
    auto geometry = openmesh_to_operand(halfedge_mesh.get());

    // Set the output of the nodes
    params.set_output("Output", std::move(*geometry));
    return true;
}

NODE_DECLARATION_UI(min_surf);
NODE_DEF_CLOSE_SCOPE
