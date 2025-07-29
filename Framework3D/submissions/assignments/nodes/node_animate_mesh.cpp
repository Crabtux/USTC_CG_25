#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>

#include "GCore/Components/MaterialComponent.h"
#include "GCore/Components/MeshOperand.h"
#include "GCore/Components/SkelComponent.h"
#include "GCore/Components/XformComponent.h"
#include "GCore/util_openmesh_bind.h"
#include "geom_node_base.h"
#include "pxr/base/gf/rotation.h"

#include "character_animation/animator.h"

struct AnimationStorage {
  constexpr static bool has_storage = false;
  std::shared_ptr<USTC_CG::character_animation::Animator> animator;
};

NODE_DEF_OPEN_SCOPE
NODE_DECLARATION_FUNCTION(animate_mesh) {
  b.add_input<Geometry>("Geometry"); // contain mesh and skeleton

  b.add_output<Geometry>("Output Geometry"); // contain mesh and skeleton
}

NODE_EXECUTION_FUNCTION(animate_mesh) {
  using namespace USTC_CG::character_animation;
  auto geom = params.get_input<Geometry>("Geometry");

  auto mesh = geom.get_component<MeshComponent>();
  auto skel = geom.get_component<SkelComponent>();

  if (!mesh) {
    throw std::runtime_error("Read mesh error.");
  }
  else if (!skel)
    throw std::runtime_error("Read skeleton error.");

  auto& storage = params.get_storage<AnimationStorage&>();
  auto& animator = storage.animator;
  animator = std::make_shared<Animator>(mesh, skel);
  animator->step(skel);

  params.set_output("Output Geometry", geom);
  return true;
}

NODE_DECLARATION_UI(animate_mesh);
NODE_DEF_CLOSE_SCOPE