#include "wcsph.h"
#include <iostream>
using namespace Eigen;

namespace USTC_CG::sph_fluid {

WCSPH::WCSPH(const MatrixXd& X, const Vector3d& box_min, const Vector3d& box_max)
    : SPHBase(X, box_min, box_max)
{
}

void WCSPH::compute_density()
{
	// -------------------------------------------------------------
	// (HW TODO) Implement the density computation
    // You can also compute pressure in this function 
	// -------------------------------------------------------------
    for (auto& p : ps_.particles()) {
        // necessary initialization of particle p's density here  
        p->density_ = ps_.mass() * W_zero(ps_.h());

        // Then traverse all neighbor fluid particles of p
        for (auto& q : p->neighbors()) {
            // compute the density contribution from q to p
            p->density_ += ps_.mass() * W(p->x() - q->x(), ps_.h());
        }

        // WCSPH only: compute pressure from density
        p->pressure_ = stiffness() * (pow(p->density() / ps_.density0(), exponent()) - 1.0);
        p->pressure_ = std::max(0.0, p->pressure_);
    }
}

void WCSPH::step()
{
    TIC(step)
    // -------------------------------------------------------------
    // (HW TODO) Follow the instruction in documents and PPT, 
    // implement the pipeline of fluid simulation 
    // -------------------------------------------------------------

	// 1. Assign particles to cells & search for neighbors 
    ps_.assign_particles_to_cells();
    ps_.search_neighbors();

    // 2. Compute density (actually, you can directly compute pressure here)
    compute_density();

    // 3. Compute non-pressure accelerations, e.g. viscosity force, gravity
    compute_non_pressure_acceleration();
    
    // 4. Compute pressure gradient acceleration
    compute_pressure_gradient_acceleration();

    // 5. Update velocity and positions, call advect()
    advect();

    TOC(step)
}
}  // namespace USTC_CG::node_sph_fluid