#include "GCore/util_openmesh_bind.h"
#include "GCore/api.h"

#include <Eigen/Sparse>

#include <vector>
#include <array>
#include <unordered_map>

namespace USTC_CG
{
class ARAP
{
public:
    ARAP(std::shared_ptr<PolyMesh> mesh_3d, std::shared_ptr<PolyMesh> initial_param);

    std::shared_ptr<PolyMesh> get_param() const;

    void local_phase();
    void global_phase();
    void unitlize();

private:
    std::vector<std::array<OpenMesh::Vec2f, 3>> _x;
    std::shared_ptr<PolyMesh> _u;
    std::vector<Eigen::Matrix2f> _L;
    std::shared_ptr<PolyMesh> _mesh_3d;
    std::unordered_map<int, bool> _m;

    std::shared_ptr<Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>>> _solver;
};
}