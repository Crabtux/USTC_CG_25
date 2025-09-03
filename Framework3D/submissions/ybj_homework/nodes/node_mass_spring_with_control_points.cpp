#include <pxr/base/gf/vec3f.h>
#include <pxr/base/vt/array.h>

#include <Eigen/Sparse>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <unordered_set>

#include "GCore/Components/MeshOperand.h"
#include "GCore/util_openmesh_bind.h"
#include "geom_node_base.h"
#include "mass_spring/FastMassSpring.h"
#include "mass_spring/MassSpring.h"
#include "mass_spring/utils.h"

struct MassSpringWithControlPointsStorage {
  constexpr static bool has_storage = false;
  std::shared_ptr<USTC_CG::mass_spring::MassSpring> mass_spring;
};

NODE_DEF_OPEN_SCOPE
NODE_DECLARATION_FUNCTION(mass_spring_with_control_points) {
  b.add_input<Geometry>("Simulated Mesh");
  b.add_input<Geometry>("Controller");

  // Simulation parameters
  b.add_input<float>("stiffness").default_val(1000).min(100).max(10000);
  b.add_input<float>("h").default_val(0.0333333333f).min(0.0).max(0.5);
  b.add_input<float>("damping").default_val(0.995).min(0.0).max(1.0);
  b.add_input<float>("gravity").default_val(-9.8).min(-20.).max(20.);
  // Useful switches (0 or 1). You can add more if you like.
  b.add_input<int>("time integrator type")
    .default_val(0)
    .min(0)
    .max(1);  // 0 for implicit Euler, 1 for semi-implicit Euler
  b.add_input<int>("enable time profiling").default_val(0).min(0).max(1);
  b.add_input<int>("enable damping").default_val(0).min(0).max(1);
  b.add_input<int>("enable debug output").default_val(0).min(0).max(1);

  // Optional switches
  b.add_input<int>("enable Liu13").default_val(0).min(0).max(1);
  // Output
  b.add_output<Geometry>("Output Mesh");
}

NODE_EXECUTION_FUNCTION(mass_spring_with_control_points) {
  using namespace Eigen;
  using namespace USTC_CG::mass_spring;

  auto& global_payload = params.get_global_payload<GeomPayload&>();
  auto current_time = global_payload.current_time;

  auto& storage = params.get_storage<MassSpringWithControlPointsStorage&>();
  auto& mass_spring = storage.mass_spring;

  auto controller_geom = params.get_input<Geometry>("Controller");
  auto controller_mesh = controller_geom.get_component<MeshComponent>();
  auto control_points = controller_mesh->get_control_points();

  auto geometry = params.get_input<Geometry>("Simulated Mesh");
  auto mesh = geometry.get_component<MeshComponent>();
  auto fixed_points = mesh->get_control_points();
  if (mesh->get_face_vertex_counts().size() == 0) {
    throw std::runtime_error("Read USD error.");
  }
  if (controller_mesh->get_face_vertex_counts().size() == 0)
    throw std::runtime_error("Read controller mesh USD error.");

  if (current_time == 0 || !mass_spring) {  // Reset and initialize the mass spring class
    if (mesh) {
      if (mass_spring != nullptr) mass_spring.reset();

      auto edges = get_edges(usd_faces_to_eigen(mesh->get_face_vertex_counts(),
        mesh->get_face_vertex_indices()));
      auto vertices = usd_vertices_to_eigen(mesh->get_vertices());
      const float k = params.get_input<float>("stiffness");
      const float h = params.get_input<float>("h");

      bool enable_liu13 =
        params.get_input<int>("enable Liu13") == 1 ? true : false;
      if (enable_liu13) {
        // HW Optional
        mass_spring = std::make_shared<FastMassSpring>(vertices, edges, k, h);
      }
      else
        mass_spring = std::make_shared<MassSpring>(vertices, edges);

      if (!mass_spring->set_dirichlet_bc_mask(VtIntArray_to_vector_bool(fixed_points)))
        throw std::runtime_error("Mass Spring: set fixed points error.");
      if (!mass_spring->init_dirichlet_bc_vertices_control_pair(
        usd_vertices_to_eigen(controller_mesh->get_vertices()),
        VtIntArray_to_vector_bool(control_points)))
        throw std::runtime_error(
          "Mass Spring: init control points error.");

      // simulation parameters
      mass_spring->stiffness = k;
      mass_spring->h = params.get_input<float>("h");
      mass_spring->gravity = { 0, 0, params.get_input<float>("gravity") };
      mass_spring->damping = params.get_input<float>("damping");
      // --------------------------------------------------------------------------------------------------------
      mass_spring->enable_damping =
        params.get_input<int>("enable damping") == 1 ? true : false;
      mass_spring->time_integrator =
        params.get_input<int>("time integrator type") == 0
        ? MassSpring::IMPLICIT_EULER
        : MassSpring::SEMI_IMPLICIT_EULER;
      mass_spring->enable_time_profiling =
        params.get_input<int>("enable time profiling") == 1 ? true : false;
      mass_spring->enable_debug_output =
        params.get_input<int>("enable debug output") == 1 ? true : false;
    }
    else {
      mass_spring = nullptr;
      throw std::runtime_error("Mass Spring: Need Geometry Input.");
    }
  }
  else if (mass_spring) // otherwise, step forward the simulation
  {
    mass_spring->update_dirichlet_bc_vertices(usd_vertices_to_eigen(controller_mesh->get_vertices()));
    mass_spring->step();
  }
  if (mass_spring) {
    mesh->set_vertices(eigen_to_usd_vertices(mass_spring->getX()));
  }
  params.set_output("Output Mesh", geometry);
  return true;
}

NODE_DECLARATION_UI(mass_spring_with_control_points);
NODE_DEF_CLOSE_SCOPE