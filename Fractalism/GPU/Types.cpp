#include <Fractalism/GPU/Types.hpp>

#include <cmath>

#include <Fractalism/App.hpp>

namespace fractalism::gpu::types {
  const Coordinates Coordinates::none = { std::nan(""), std::nan("") };

  Number::Number() : cltypes::number{0.0} {}

  Number::Number(real x, real y, real z) : cltypes::number{x, y, z} {}

  Viewspace::Viewspace() : center(), zoom(0.0), mapping{ 0, 0, 0 } {}

  Viewspace::Viewspace(real x, real y, real z, real zoom, cl_char xMapping, cl_char yMapping, cl_char zMapping)
    : center(x, y, z), zoom(zoom), mapping{ xMapping, yMapping, zMapping } {}

  Viewspace::Viewspace(real x, real y, real z) :
        center(x, y, z),
        zoom(Settings::zoom1x),
        mapping{ 1, 2, 3 } {}

  void Viewspace::asKernelArg(cl::Kernel& kernel, cl_uint index) const {
    cltypes::viewspace clViewspace = {
      .center = center,
      .zoom = zoom,
      .mapping = {
        .x = mapping.x,
        .y = mapping.y,
        .z = App::get<Settings>().renderDimensions == options::Dimensions::three ? mapping.z : 0
      }
    };
    try {
      kernel.setArg(index, clViewspace);
    }
    catch (const cl::Error& e) {
      throw CLError(std::format("Could not set Viewspace as kernel parameter #{}", index), e);
    }
  }
}