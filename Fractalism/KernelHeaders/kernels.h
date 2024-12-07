#ifndef _KERNEL_BASE_H_
#define _KERNEL_BASE_H_

#if !defined(KERNEL_FUNCTION)
#error "Preprocessor macro KERNEL_FUNCTION(add, sub, mul, sqr, scale, modulus_sq) is not defined."
#endif

#if !defined(ESCAPE_VALUE)
#error "Preprocessor macro ESCAPE_VALUE is not defined."
#endif

#include "spectral_color.h"
#include "number_systems.h"
#include "cltypes.h"

#if defined(__cplusplus) || defined(__OPENCL_CPP_VERSION__)
extern "C" {
#endif

// The value of log(0.5) - log(log(2.0))
__constant float log_half_minus_log_log_2 = -0.32663425997828094f;

static inline float4 fractional_escape_color(real modulus_squared, int max_iterations, int iteration) {
  // Normalize the ratio of the current iteration minus log(log(modulus)/log(2)) and the maximum iterations
  return spectral_color(
    (((float)iteration) - log(log((float)modulus_squared)) + log_half_minus_log_log_2) / (float)max_iterations);
}

#if !defined(CL_DEVICE_MAX_MEM_ALLOC_SIZE)
#error "Preprocessor macro CL_DEVICE_MAX_MEM_ALLOC_SIZE is not defined."
#endif

__constant size_t max_work_store_buffer_size = CL_DEVICE_MAX_MEM_ALLOC_SIZE / sizeof(work_store);
typedef struct __attribute__((packed)) work_store_buffer {
  __global work_store* p;
} work_store_buffer;

typedef struct work_dimensions {
  int width;
  int height;
  int depth;
} work_dimensions;

typedef struct work_item_location {
  int x;
  int y;
  int z;
} work_item_location;

typedef struct work_item {
  work_item_location location;
  work_dimensions dimensions;
} work_item;

typedef struct work_store_item {
  work_item item;
  __global work_store* p;
} work_store_item;

static inline work_item get_work_item(__write_only image3d_t output) {
  return (work_item) {
    {
      get_global_id(0),
      get_global_id(1),
      get_global_id(2)
    },
  {
    get_image_width(output),
    get_image_height(output),
    get_image_depth(output)
  }
  };
}

static inline work_store_item get_work_store_item(__write_only image3d_t output, __global work_store_buffer* buffer) {
  work_item item = get_work_item(output);

  int index = (item.location.z * item.dimensions.height * item.dimensions.width) + (item.location.y * item.dimensions.width) + item.location.x;

  return (work_store_item) {
    item,
    &buffer[index / max_work_store_buffer_size].p[index % max_work_store_buffer_size]
  };
}

static inline float4 location_to_color(work_item item) {
  return (float4)(
    ((float)item.location.x) / ((float)item.dimensions.width),
    ((float)item.location.y) / ((float)item.dimensions.height),
    ((float)item.location.z) / ((float)item.dimensions.depth),
    0.0f);
}

#define create_kernel(name, c_value, z0_value, condition, function, finish, number_system, number_system_type) \
kernel void name##_##number_system( \
    __write_only image3d_t output, \
    __global work_store_buffer *buffer, \
    viewspace view, \
    number param, \
    unsigned int last_iteration, \
    unsigned int max_iterations) { \
  work_store_item store_item = get_work_store_item(output, buffer); \
  number_system_type c = c_value; \
  number_system_type z; \
  int i; \
  if (last_iteration == 0) { \
    z = z0_value; \
    i = 0; \
  } else { \
    work_store z_last = *store_item.p; \
    z = number_system_type##_from_raw(z_last.value.raw, 0); \
    i = z_last.i; \
  } \
  for (;(i < max_iterations) && (condition); i++) { \
    function; \
  } \
  number result = (number){{0.0}}; \
  number_system_type##_to_raw(z, result.raw, 0);\
  *store_item.p = (work_store){result, i}; \
  finish; \
}

#define write_fractional_escape(number_system) \
write_imagef( \
    output, \
    (int4)(store_item.item.location.x, store_item.item.location.y, store_item.item.location.z, 0), \
    fractional_escape_color(modulus_sq_##number_system(z), max_iterations, i))

#define write_translated_point(number_system) \
int4 translated = number_system##_to_viewspace_point(view, store_item.item, z); \
if (translated.x >= 0 && translated.x < store_item.item.dimensions.width && \
    translated.y >= 0 && translated.y < store_item.item.dimensions.height && \
    translated.z >= 0 && translated.z < store_item.item.dimensions.depth) { \
  write_imagef( \
      output, \
      translated, \
      location_to_color(store_item.item)); \
}

#define create_escape_and_translated_kernels(name, c_value, z0_value, function, escape, number_system, number_system_type) \
create_kernel( \
  name##_escape, c_value, z0_value, \
  modulus_sq_##number_system(z) < escape, \
  function, \
  write_fractional_escape(number_system), \
  number_system, \
  number_system_type) \
create_kernel( \
  name##_translated, c_value, z0_value, \
  true, \
  function, \
  write_translated_point(number_system), \
  number_system, \
  number_system_type)

#define create_phase_kernels(function, escape, number_system, number_system_type) \
create_escape_and_translated_kernels( \
    phase, \
    viewspace_point_to_##number_system(view, store_item.item), \
    zero_##number_system(), \
    function, \
    escape, \
    number_system, \
    number_system_type)

#define create_dynamical_kernels(function, escape, number_system, number_system_type) \
create_escape_and_translated_kernels( \
    dynamical, \
    number_system_type##_from_raw(param.raw, 0), \
    viewspace_point_to_##number_system(view, store_item.item), \
    function, \
    escape, \
    number_system, \
    number_system_type)

static inline void apply_view_mapping_element(real* raw, real zoom, char view_mapping, int value, int range) {
  raw[abs(view_mapping)] = ((((real)value) / ((real)range)) * 2.0 - 1.0) * copysign(zoom, (real)(view_mapping));
}

static inline int reverse_view_mapping_element(real* raw, real zoom, char view_mapping, int range) {
  return convert_int_rte((((raw[abs(view_mapping)] / copysign(zoom, (real)view_mapping)) + 1.0) / 2.0) * range);
}

#define create_viewspace_point_functions(number_system, number_system_type) \
static inline number_system_type viewspace_point_to_##number_system(viewspace view, work_item item) { \
  real raw[sizeof(((number){}).raw) + 1] = {0.0}; \
  apply_view_mapping_element(raw, view.zoom, view.mapping.x, item.location.x, item.dimensions.width); \
  apply_view_mapping_element(raw, view.zoom, view.mapping.y, item.location.y, item.dimensions.height); \
  apply_view_mapping_element(raw, view.zoom, view.mapping.z, item.location.z, item.dimensions.depth); \
  return add_##number_system(number_system_type##_from_raw(raw, 1), number_system_type##_from_raw(view.center.raw, 0)); \
} \
static inline int4 number_system##_to_viewspace_point(viewspace view, work_item item, number_system_type point) { \
  real raw[sizeof(((number){}).raw) + 1] = {0.0}; \
  number_system_type##_to_raw(sub_##number_system(point, number_system_type##_from_raw(view.center.raw, 0)), raw, 1); \
  (int4)( \
    reverse_view_mapping_element(raw, view.zoom, view.mapping.x, item.dimensions.width), \
    reverse_view_mapping_element(raw, view.zoom, view.mapping.y, item.dimensions.height), \
    reverse_view_mapping_element(raw, view.zoom, view.mapping.z, item.dimensions.depth), \
    0); \
}

#define create_kernels(function, escape, number_system, number_system_type) \
create_phase_kernels(function, escape, number_system, number_system_type) \
create_dynamical_kernels(function, escape, number_system, number_system_type)

#define X(extension_method, number_system, element_system) \
  create_viewspace_point_functions(number_system, number_system##_impl) \
  create_kernels( \
    KERNEL_FUNCTION( \
      add_##number_system, \
      sub_##number_system, \
      mul_##number_system, \
      sqr_##number_system, \
      scale_##number_system, \
      modulus_sq_##number_system), \
    ESCAPE_VALUE, \
    number_system, \
    number_system##_impl)
NUMBER_SYSTEMS
#undef X

#undef create_kernels
#undef create_viewspace_point_functions
#undef create_dynamical_kernels
#undef create_phase_kernels
#undef create_escape_and_translated_kernels
#undef write_translated_point
#undef write_fractional_escape
#undef create_kernel

#if defined(__cplusplus) || defined(__OPENCL_CPP_VERSION__)
}
#endif

#endif // !_KERNEL_BASE_H_
