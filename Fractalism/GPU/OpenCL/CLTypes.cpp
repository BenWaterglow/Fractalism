#include <Fractalism/GPU/OpenCL/CLTypes.hpp>

#include <Fractalism/App.hpp>

namespace fractalism {
  namespace gpu {
    namespace opencl {
      namespace cltypes {
        Number::Number() : internal::number{ 0.0 } {}

        Number::Number(real x, real y, real z) : internal::number{ x, y, z } {}

        Viewspace::Viewspace() : center(), zoom(0.0), mapping{ 0, 0, 0 } {}

        Viewspace::Viewspace(real x, real y, real z, real zoom, cl_char xMapping, cl_char yMapping, cl_char zMapping)
          : center(x, y, z), zoom(zoom), mapping{ xMapping, yMapping, zMapping } {}

        void Viewspace::asKernelArg(cl::Kernel& kernel, cl_uint index) const {
          internal::viewspace internal = {
            .center = center,
            .zoom = zoom,
            .mapping = {
              .x = mapping.x,
              .y = mapping.y,
              .z = App::get<Settings>().renderDimensions == options::Dimensions::three ? mapping.z : 0
            }
          };
          try {
            kernel.setArg(index, internal);
          }
          catch (const cl::Error& e) {
            throw CLError(std::format("Could not set Viewspace as kernel parameter #{}", index), e);
          }
        }
      }
    }
  }
}