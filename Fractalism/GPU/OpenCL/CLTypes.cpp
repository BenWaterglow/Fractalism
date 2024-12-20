#include <Fractalism/GPU/OpenCL/CLTypes.hpp>

namespace fractalism {
  namespace gpu {
    namespace opencl {
      namespace cltypes {
        Number::Number() : internal::number{ 0.0 } {}

        Number::Number(real x, real y, real z) : internal::number{ x, y, z } {}

        Viewspace::Viewspace() : center(), zoom(0.0), mapping{ 0, 0, 0 } {}

        Viewspace::Viewspace(real x, real y, real z, real zoom, cl_char xMapping, cl_char yMapping, cl_char zMapping)
          : center(x, y, z), zoom(zoom), mapping{ xMapping, yMapping, zMapping } {
        }
      }
    }
  }
}