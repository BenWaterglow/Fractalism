#ifndef _FRACTALISM_TYPES_HPP_
#define _FRACTALISM_TYPES_HPP_

#include <cmath>
#include <glm/glm.hpp>

#include <Fractalism/KernelHeaders/interop.h>
#include <Fractalism/Exceptions.hpp>

namespace fractalism::gpu::types {

  using vec3 = glm::vec<3, real, glm::defaultp>;
  using vec4 = glm::vec<4, real, glm::defaultp>;
  using mat4 = glm::mat<4, 4, real, glm::defaultp>;

  namespace cltypes {
    #include <Fractalism/KernelHeaders/cltypes.h>
  }

  template<typename T>
  concept HasKernelArgHandler = requires(T & t, cl::Kernel & kernel, cl_uint index) {
    t.asKernelArg(kernel, index);
  };

  struct Coordinates {
    static const Coordinates none;

    real x;
    real y;

    inline operator bool() const {
      return !std::isnan(x) && !std::isnan(y);
    }

    inline Coordinates operator+(const Coordinates& other) const {
      return {
        x + other.x,
        y + other.y
      };
    }

    inline Coordinates operator-(const Coordinates& other) const {
      return {
        x - other.x,
        y - other.y
      };
    }
  };

  using ViewMapping = cltypes::view_mapping;

  /**
    * @struct Number
    * @brief Wraps a raw array of real.
    */
  struct Number : public cltypes::number {
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

    inline Number& applyMapping(ViewMapping mapping, real zoom, const Coordinates& coordinates) {
      if (mapping.x) {
        raw[abs(mapping.x) - 1] += (copysign(zoom, mapping.x) * coordinates.x);
      }
      if (mapping.y) {
        raw[abs(mapping.y) - 1] += (copysign(zoom, mapping.y) * coordinates.y);
      }
      return *this;
    }

    inline void asKernelArg(cl::Kernel& kernel, cl_uint index) const {
      const cltypes::number &internal = *this;
      try {
        kernel.setArg(index, internal);
      }
      catch (const cl::Error& e) {
        throw CLKernelArgError("Could not set Number", kernel, index, e);
      }
    }
  };

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

    /**
     * @brief Constructs a 3D viewspace with specified center, zoom, and axis
     * mappings.
     * @param x X-coordinate of center.
     * @param y Y-coordinate of center.
     * @param z Z-coordinate of center.
     */
    Viewspace(real x, real y, real z);

    inline operator Number& () { return center; }
    inline operator real& () { return zoom; }
    inline operator ViewMapping& () { return mapping; }
    inline Number& operator=(const Number& value) { return center = value; }
    inline real& operator=(const real& value) { return zoom = value; }
    inline ViewMapping& operator=(const ViewMapping& value) { return mapping = value; }
    inline Viewspace& operator+=(const Coordinates& coordinates) {
      center.applyMapping(mapping, zoom, coordinates);
      return *this;
    }
    inline Number operator+(const Coordinates& coordinates) const {
      Number result = center;
      result.applyMapping(mapping, zoom, coordinates);
      return result;
    }

    void asKernelArg(cl::Kernel& kernel, cl_uint index) const;

    Number center;          ///< Center position of the viewspace.
    real zoom;              ///< Zoom factor for view rendering.
    ViewMapping mapping;    ///< Axis mapping for rendering dimensions.
  };

  /**
    * @struct WorkStore
    * @brief Holds a computed fractal iteration value and associated iteration index.
    */
  using WorkStore = cltypes::work_store;
}

#endif
