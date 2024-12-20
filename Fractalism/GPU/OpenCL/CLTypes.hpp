#ifndef _FRACTALISM_CL_TYPES_HPP_
#define _FRACTALISM_CL_TYPES_HPP_

#include <format>

#include <Fractalism/KernelHeaders/interop.h>
#include <Fractalism/Exceptions.hpp>

namespace fractalism {
  namespace gpu {
    namespace opencl {
      namespace cltypes {

        namespace internal {
          #include <Fractalism/KernelHeaders/cltypes.h>
        }

        template<typename T>
        concept HasKernelArgHandler = requires(T & t, cl::Kernel & kernel, cl_uint index) {
          t.asKernelArg(kernel, index);
        };

        /**
         * @struct Number
         * @brief Wraps a raw array of real.
         */
        struct Number : public internal::number {
          /**
           * @brief Default constructor initializing to zeros.
           */
          Number();

          /**
           * @brief Constructs a number with the first 3 compenents set to x, y, z, and the rest 0.0
           * @param x First component.
           * @param y Second component.
           * @param z Third component.
           */
          Number(real x, real y, real z);

          inline void asKernelArg(cl::Kernel& kernel, cl_uint index) const {
            const internal::number& internal = *this;
            try {
              kernel.setArg(index, internal);
            }
            catch (const cl::Error& e) {
              throw CLKernelArgError("Could not set Number", kernel, index, e);
            }
          }
        };

        using ViewMapping = internal::view_mapping;

        /**
         * @struct Viewspace
         * @brief Defines a view configuration for rendering, including center, zoom, and axis mappings.
         */
        struct Viewspace {
          /**
           * @brief Default constructor initializing viewspace with zero zoom and default mappings.
           */
          Viewspace();

          /**
           * @brief Constructs a 3D viewspace with specified center, zoom, and axis mappings.
           * @param x X-coordinate of center.
           * @param y Y-coordinate of center.
           * @param z Z-coordinate of center.
           * @param zoom Zoom level.
           * @param xMapping X-axis mapping.
           * @param yMapping Y-axis mapping.
           * @param zMapping Z-axis mapping.
           */
          Viewspace(real x, real y, real z, real zoom, cl_char xMapping, cl_char yMapping, cl_char zMapping);

          inline operator Number& () { return center; }
          inline operator real& () { return zoom; }
          inline operator ViewMapping& () { return mapping; }
          inline Number& operator=(const Number& value) { return center = value; }
          inline real& operator=(const real& value) { return zoom = value; }
          inline ViewMapping& operator=(const ViewMapping& value) { return mapping = value; }

           void asKernelArg(cl::Kernel& kernel, cl_uint index) const;

          Number center;          ///< Center position of the viewspace.
          real zoom;              ///< Zoom factor for view rendering.
          ViewMapping mapping;    ///< Axis mapping for rendering dimensions.
        };

        /**
         * @struct WorkStore
         * @brief Holds a computed fractal iteration value and associated iteration index.
         */
        using WorkStore = internal::work_store;
      };
    }
  }
}

#endif
