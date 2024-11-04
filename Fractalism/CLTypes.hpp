#ifndef _CL_TYPES_HPP_
#define _CL_TYPES_HPP_

#define USE_DOUBLE_MATH

#include <GL/glew.h>

#include "CLIncludeHelper.hpp"

#pragma pack(push, 1)

namespace cltypes {

  /**
   * @union Number
   * @brief Union type that can represent different types of complex numbers.
   *
   * Allows interpretation of the underlying data as different complex types (c1, c2, or c3),
   * or as raw double values.
   */
  union Number {
    /**
     * @brief Default constructor initializing c3 to zeros.
     */
    Number() : raw{ 0.0 } {}

    /**
     * @brief Constructs a number with the first 3 compenents set to x, y, z, and the rest 0.0
     * @param x First component.
     * @param y Second component.
     * @param z Third component.
     */
    Number(cl_double x, cl_double y, cl_double z) : raw{ x, y, z } {}

    cl_double raw[8];     ///< Raw array view.
  };

  /**
   * @struct Viewspace
   * @brief Defines a view configuration for rendering, including center, zoom, and axis mappings.
   */
  struct Viewspace {
    /**
     * @brief Default constructor initializing viewspace with zero zoom and default mappings.
     */
    Viewspace() : center(), zoom(0.0), mapping({ {0,0,0} }) {}

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
    Viewspace(cl_double x, cl_double y, cl_double z, cl_double zoom, cl_char xMapping, cl_char yMapping, cl_char zMapping)
      : center(x, y, z), zoom(zoom), mapping({ {xMapping, yMapping, zMapping} }) {}

    Number center;       ///< Center position of the viewspace.
    cl_double zoom;      ///< Zoom factor for view rendering.
    cl_char3 mapping;    ///< Axis mapping for rendering dimensions.
  };

  /**
   * @struct WorkStore
   * @brief Holds a computed fractal value and associated iteration index.
   */
  struct WorkStore {
    Number value;       ///< Fractal value.
    cl_uint i;          ///< Iteration index.
  };
};

#pragma pack(pop)

/**
 * @class SVMPtr
 * @brief Manages Shared Virtual Memory (SVM) pointers for OpenCL operations.
 * @tparam P Data type for the SVM.
 */
template <typename P>
class SVMPtr {
public:
  /**
   * @brief Constructs an SVM pointer with the specified parameters.
   * @param ctx OpenCL context.
   * @param flags Memory flags for allocation.
   * @param itemCount Number of elements in the SVM.
   * @param alignment Memory alignment.
   */
  SVMPtr(cl::Context& ctx, cl_svm_mem_flags flags, size_t itemCount, cl_uint alignment) :
    ctx(ctx), itemCount(itemCount), ptr(reinterpret_cast<P*>(clSVMAlloc(ctx(), flags, itemCount * sizeof(P), alignment))) {}

  /**
   * @brief Destructor that releases the SVM memory.
   */
  ~SVMPtr() {
    clSVMFree(ctx(), ptr);
  }

  /**
   * @brief Assignment operator to transfer ownership of the SVM pointer.
   * @param rhs Right-hand SVMPtr to assign from.
   * @return Reference to this SVMPtr.
   */
  SVMPtr<P>& operator=(SVMPtr<P>& rhs) {
    clSVMFree(ctx(), ptr);
    ptr = rhs.claim();
    itemCount = rhs.itemCount;
    ctx = rhs.ctx;
    return *this;
  }

  /**
   * @brief Claims ownership of the SVM pointer and nullifies the original.
   * @return Pointer to the claimed SVM memory.
   */
  P* claim() {
    P* p = ptr;
    ptr = nullptr;
    return p;
  }

  /**
   * @brief Returns the total size in bytes of the SVM allocation.
   * @return Size of allocated memory.
   */
  size_t size() {
    return itemCount * sizeof(P);
  }

  /**
   * @brief Implicit conversion to raw pointer type.
   */
  operator P* () {
    return ptr;
  }

  /**
   * @brief Checks if the SVM pointer is valid.
   * @return True if the pointer is not null, false otherwise.
   */
  operator bool() {
    return ptr != nullptr;
  }

  /**
   * @brief Access operator to retrieve elements by index.
   * @param pos Position of the element.
   * @return Reference to the element at position `pos`.
   */
  P& operator[](size_t pos) {
    return ptr[pos];
  }

  size_t itemCount;          ///< Number of items in the SVM.
  cl::Context& ctx;          ///< OpenCL context associated with this SVM pointer.

private:
  P* ptr;                    ///< Raw SVM pointer.
};

#endif
