#include "GCore/Components/MeshOperand.h"
#include "geom_node_base.h"
#include "GCore/util_openmesh_bind.h"
#include <Eigen/Sparse>
#include <vector>
#include <cmath>

    /*
    ** @brief HW4_TutteParameterization
    **
    ** This file contains two nodes whose primary function is to map the boundary of
    *a mesh to a plain
    ** convex closed curve (circle of square), setting the stage for subsequent
    *Laplacian equation
    ** solution and mesh parameterization tasks.
    **
    ** Key to this node's implementation is the adept manipulation of half-edge data
    *structures
    ** to identify and modify the boundary of the mesh.
    **
    ** Task Overview:
    ** - The two execution functions (node_map_boundary_to_square_exec,
    ** node_map_boundary_to_circle_exec) require an update to accurately map the
    *mesh boundary to a and
    ** circles. This entails identifying the boundary edges, evenly distributing
    *boundary vertices along
    ** the square's perimeter, and ensuring the internal vertices' positions remain
    *unchanged.
    ** - A focus on half-edge data structures to efficiently traverse and modify
    *mesh boundaries.
    */

NODE_DEF_OPEN_SCOPE

    /*
    ** HW4_TODO: Node to map the mesh boundary to a circle.
    */

NODE_DECLARATION_FUNCTION(circle_boundary_mapping)
{
    // Input-1: Original 3D mesh with boundary
    b.add_input<Geometry>("Input");
    // Output-1: Processed 3D mesh whose boundary is mapped to a square and the
    // interior vertices remains the same
    b.add_output<Geometry>("Output");
}

NODE_EXECUTION_FUNCTION(circle_boundary_mapping)
{
    // Get the input from params
    auto input = params.get_input<Geometry>("Input");

    // (TO BE UPDATED) Avoid processing the node when there is no input
    if (!input.get_component<MeshComponent>()) {
        throw std::runtime_error("Boundary Mapping: Need Geometry Input.");
    }

    /* ----------------------------- Preprocess -------------------------------
    ** Create a halfedge structure (using OpenMesh) for the input mesh. The
    ** half-edge data structure is a widely used data structure in geometric
    ** processing, offering convenient operations for traversing and modifying
    ** mesh elements.
    */
    auto halfedge_mesh = operand_to_openmesh(&input);

    /* ----------- [HW4_TODO] TASK 2.1: Boundary Mapping (to circle)
    *------------
    ** In this task, you are required to map the boundary of the mesh to a
    *circle
    ** shape while ensuring the internal vertices remain unaffected. This step
    *is
    ** crucial for setting up the mesh for subsequent parameterization tasks.
    **
    ** Algorithm Pseudocode for Boundary Mapping to Circle
    ** ------------------------------------------------------------------------
    ** 1. Identify the boundary loop(s) of the mesh using the half-edge
    *structure.
    **
    ** 2. Calculate the total length of the boundary loop to determine the
    *spacing
    **    between vertices when mapped to a square.
    **
    ** 3. Sequentially assign each boundary vertex a new position along the
    *square's
    **    perimeter, maintaining the calculated spacing to ensure proper
    *distribution.
    **
    ** 4. Keep the interior vertices' positions unchanged during this process.
    **
    ** Note: How to distribute the points on the circle?
    **
    ** Note: It would be better to normalize the boundary to a unit circle in
    *[0,1]x[0,1] for
    ** texture mapping.
    */

    // 我不懂数学，但思考了一下，顺时针遍历还是逆时针遍历应该不是很重要
    double distance_sum = 0.f;
    std::vector<int>   border_idx;
    std::vector<float> distance;
    for (const auto &vi : halfedge_mesh->vertices())
    {
        if (vi.is_boundary())
        {
            // vi as the loop start
            auto v = vi;
            border_idx.push_back(v.idx());
            distance.push_back(0.f);
            while (true)
            {
                for (const auto &halfedge_handle : v.outgoing_halfedges())
                {
                    const auto &vj = halfedge_handle.to();
                    if (vj.is_boundary() && vj.idx() != border_idx.back())
                    {
                        auto xx = halfedge_mesh->point(v);
                        double dis_ = (halfedge_mesh->point(v) - halfedge_mesh->point(vj)).norm();
                        distance.push_back(distance_sum);
                        border_idx.push_back(vj.idx());
                        distance_sum += dis_;
                        v = vj;
                        break;
                    }
                }
                if (border_idx.front() == border_idx.back())
                    break;
            }
            break;
        }
    }

    for (int i = 0; i < border_idx.size() - 1; i++)
    {
        float theta = 2.f * M_PI * distance[i] / distance_sum;
        auto v = halfedge_mesh->vertex_handle(border_idx[i]);
        OpenMesh::Vec3f mapped_coord(cosf32(theta), sinf32(theta), 0.f);
        halfedge_mesh->set_point(v, mapped_coord);
    }

    /* ----------------------------- Postprocess ------------------------------
    ** Convert the result mesh from the halfedge structure back to Geometry
    *format as the node's
    ** output.
    */
    auto geometry = openmesh_to_operand(halfedge_mesh.get());

    // Set the output of the nodes
    params.set_output("Output", std::move(*geometry));
	return true;
}

    /*
    ** HW4_TODO: Node to map the mesh boundary to a square.
    */

NODE_DECLARATION_FUNCTION(square_boundary_mapping)
{
    // Input-1: Original 3D mesh with boundary
    b.add_input<Geometry>("Input");

    // Output-1: Processed 3D mesh whose boundary is mapped to a square and the
    // interior vertices remains the same
    b.add_output<Geometry>("Output");
}

NODE_EXECUTION_FUNCTION(square_boundary_mapping)
{
    // Get the input from params
    auto input = params.get_input<Geometry>("Input");

    // (TO BE UPDATED) Avoid processing the node when there is no input
    if (!input.get_component<MeshComponent>()) {
        throw std::runtime_error("Input does not contain a mesh");
    }

    /* ----------------------------- Preprocess -------------------------------
    ** Create a halfedge structure (using OpenMesh) for the input mesh.
    */
    auto halfedge_mesh = operand_to_openmesh(&input);

    /* ----------- [HW4_TODO] TASK 2.2: Boundary Mapping (to square)
    *------------
    ** In this task, you are required to map the boundary of the mesh to a
    *circle
    ** shape while ensuring the internal vertices remain unaffected.
    **
    ** Algorithm Pseudocode for Boundary Mapping to Square
    ** ------------------------------------------------------------------------
    ** (omitted)
    **
    ** Note: Can you perserve the 4 corners of the square after boundary
    *mapping?
    **
    ** Note: It would be better to normalize the boundary to a unit circle in
    *[0,1]x[0,1] for
    ** texture mapping.
    */
    double distance_sum = 0.f;
    std::vector<int>   border_idx;
    std::vector<float> distance;
    for (const auto &vi : halfedge_mesh->vertices())
    {
        if (vi.is_boundary())
        {
            // vi as the loop start
            auto v = vi;
            border_idx.push_back(v.idx());
            distance.push_back(0.f);
            while (true)
            {
                for (const auto &halfedge_handle : v.outgoing_halfedges())
                {
                    const auto &vj = halfedge_handle.to();
                    if (vj.is_boundary() && vj.idx() != border_idx.back())
                    {
                        double dis_ = (halfedge_mesh->point(v) - halfedge_mesh->point(vj)).norm();
                        distance_sum += dis_;
                        distance.push_back(distance_sum);
                        border_idx.push_back(vj.idx());
                        v = vj;
                        break;
                    }
                }
                if (border_idx.front() == border_idx.back())
                    break;
            }
            break;
        }
    }

    // preserve the 4 corners of the normlized square
    int   mapped_corners[4]     = {-999, -999, -999, -999};
    float mapped_corners_dis[4] = {999.f, 999.f, 999.f, 999.f};
    std::fill_n(mapped_corners_dis, 4, std::numeric_limits<float>::infinity());
    const OpenMesh::Vec3f mapped_corners_coord[4] = 
    {
        OpenMesh::Vec3f(0.f, 0.f, 0.f),
        OpenMesh::Vec3f(0.f, 1.f, 0.f),
        OpenMesh::Vec3f(1.f, 1.f, 0.f),
        OpenMesh::Vec3f(1.f, 0.f, 0.f),
    };

    for (int i = 0; i < border_idx.size() - 1; i++)
    {
        float theta = 4.f * distance[i] / distance_sum;
        OpenMesh::Vec3f mapped_coord;

        mapped_coord[2] = 0.f;
        if (theta < 1)
        {
            mapped_coord[0] = theta;
            mapped_coord[1] = 0.f;
        }
        else if (theta < 2)
        {
            mapped_coord[0] = 1.f;
            mapped_coord[1] = theta - 1.f;
        }
        else if (theta < 3)
        {
            mapped_coord[0] = 3.f - theta;
            mapped_coord[1] = 1.f;
        }
        else
        {
            mapped_coord[0] = 0.f;
            mapped_coord[1] = 4.f - theta;
        }

        for (int j = 0; j < 4; j++)
        {
            float dis_ = (mapped_coord - mapped_corners_coord[j]).norm();
            if (dis_ < mapped_corners_dis[j])
            {
                mapped_corners_dis[j] = dis_;
                mapped_corners[j]     = i;
            }
        }

        auto v = halfedge_mesh->vertex_handle(border_idx[i]);
        halfedge_mesh->set_point(v, mapped_coord);
    }

    for (int j = 0; j < 4; j++)
    {
        auto v = halfedge_mesh->vertex_handle(border_idx[mapped_corners[j]]);
        halfedge_mesh->set_point(v, mapped_corners_coord[j]);
    }

    /* ----------------------------- Postprocess ------------------------------
    ** Convert the result mesh from the halfedge structure back to Geometry
    *format as the node's
    ** output.
    */
    auto geometry = openmesh_to_operand(halfedge_mesh.get());

    // Set the output of the nodes
    params.set_output("Output", std::move(*geometry));
    return true;
}



NODE_DECLARATION_UI(boundary_mapping);
NODE_DEF_CLOSE_SCOPE
