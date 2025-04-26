#ifndef _FRACTALISM_FRACTALISM_KERNEL_BASE_H_
#define _FRACTALISM_FRACTALISM_KERNEL_BASE_H_

#if !defined(ESCAPE_VALUE)
#error "Preprocessor macro ESCAPE_VALUE is not defined."
#endif

#include "spectral_color.h"
#include "number_systems.h"
#include "cltypes.h"

_EXTERN_C_DECL_

static inline float4 fractional_escape_color(real modulus_squared, unsigned int max_iterations, unsigned int iteration) {
  // Normalize the ratio of the current iteration minus log(log(modulus))/log(2) and the maximum iterations
  real value = ((((real)iteration) - log(log(modulus_squared) / 2.0) + ((real)M_LN2)) / (real)max_iterations);
  return (iteration < max_iterations) ?
    spectral_color((float) value) :
    (float4)(0.0f, 0.0f, 0.0f, (float) (value * value));
}

#if !defined(CL_DEVICE_MAX_MEM_ALLOC_SIZE)
#error "Preprocessor macro CL_DEVICE_MAX_MEM_ALLOC_SIZE is not defined."
#endif

__constant size_t max_work_store_buffer_size = CL_DEVICE_MAX_MEM_ALLOC_SIZE / sizeof(work_store);
_PACK_BEGIN_ struct work_store_buffer {
  __global work_store* p;
} _PACK_END_;

typedef struct work_store_buffer work_store_buffer;

typedef struct work_dimensions {
  size_t width;
  size_t height;
  size_t depth;
} work_dimensions;

typedef struct work_item_location {
  size_t x;
  size_t y;
  size_t z;
} work_item_location;

typedef struct work_item {
  work_item_location location;
  work_dimensions dimensions;
} work_item;

typedef struct work_store_item {
  work_item item;
  __global work_store* p;
} work_store_item;

static inline work_item get_work_item() {
  return (work_item) {
    .location = {
      get_global_id(0),
      get_global_id(1),
      get_global_id(2)
    },
    .dimensions = {
      get_global_size(0),
      get_global_size(1),
      get_global_size(2)
    }
  };
}

static inline work_store_item get_work_store_item(__global work_store_buffer* buffer) {
  work_item item = get_work_item();

  size_t index = (item.location.z * item.dimensions.height * item.dimensions.width) + (item.location.y * item.dimensions.width) + item.location.x;

  return (work_store_item) {
    item,
    &buffer[index / max_work_store_buffer_size].p[index % max_work_store_buffer_size]
  };
}

static inline float4 location_to_color(work_item item) {
  return (float4)(
    ((float)item.location.x) / ((float)item.dimensions.width),
    ((float)item.location.z) / ((float)item.dimensions.depth),
    ((float)item.location.y) / ((float)item.dimensions.height),
    8.0/sqrt((real)(item.dimensions.width*item.dimensions.width +
             item.dimensions.height*item.dimensions.height +
             item.dimensions.depth*item.dimensions.depth)));
}

#define create_kernel(name, c_value, z0_value, condition, function, finish, number_system, number_system_type) \
__kernel void name##_##number_system( \
    __write_only image3d_t output, \
    __global work_store_buffer *buffer, \
    viewspace view, \
    number parameter, \
    unsigned int last_iteration, \
    unsigned int max_iterations) { \
  work_store_item store_item = get_work_store_item(buffer); \
  number_system_type c = c_value; \
  number_system_type z; \
  unsigned int i; \
  if (last_iteration == 0) { \
    z = z0_value; \
    i = 0; \
  } else { \
    work_store z_last = *store_item.p; \
    z = number_system##_from_raw(z_last.value.raw, 0); \
    i = z_last.i; \
  } \
  for (;(i < max_iterations) && (condition); i++) { \
    function; \
  } \
  number result = (number){{0.0}}; \
  number_system##_to_raw(z, result.raw, 0);\
  *store_item.p = (work_store){.i = i, .value = result}; \
  finish; \
}

#define write_fractional_escape(number_system) \
write_imagef( \
    output, \
    (int4)(store_item.item.location.x, store_item.item.location.y, store_item.item.location.z, 0), \
    fractional_escape_color(modulus_sq_##number_system(z), max_iterations, i))

#define write_translated_point(number_system) \
int4 translated = reverse_view_mapping_##number_system(view, store_item.item, z); \
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
    apply_view_mapping_##number_system(view, store_item.item), \
    zero_##number_system(), \
    function, \
    escape, \
    number_system, \
    number_system_type)

#define create_dynamical_kernels(function, escape, number_system, number_system_type) \
create_escape_and_translated_kernels( \
    dynamical, \
    number_system##_from_raw(parameter.raw, 0), \
    apply_view_mapping_##number_system(view, store_item.item), \
    function, \
    escape, \
    number_system, \
    number_system_type)

static inline void apply_view_mapping_element(real* raw, real zoom, char view_mapping, int location, int range) {
  raw[abs(view_mapping)] = ((((real)location) / ((real)range)) * 2.0 - 1.0) / copysign(zoom, (real)(view_mapping));
}

static inline int reverse_view_mapping_element(real* raw, real zoom, char view_mapping, int range) {
  return convert_int_rte((((real)range) * ((raw[abs(view_mapping)] * copysign(zoom, (real)view_mapping)) + 1.0)) / 2.0);
}

#define create_view_mapping_functions(number_system, number_system_type) \
static inline number_system_type apply_view_mapping_##number_system(viewspace view, work_item item) { \
  real raw[MAX_NUMBER_SYSTEM_SIZE + 1] = {0.0}; \
  apply_view_mapping_element(raw, view.zoom, view.mapping.x, item.location.x, item.dimensions.width); \
  apply_view_mapping_element(raw, view.zoom, view.mapping.y, item.location.y, item.dimensions.height); \
  apply_view_mapping_element(raw, view.zoom, view.mapping.z, item.location.z, item.dimensions.depth); \
  return add_##number_system(number_system##_from_raw(raw, 1), number_system##_from_raw(view.center.raw, 0)); \
} \
static inline int4 reverse_view_mapping_##number_system(viewspace view, work_item item, number_system_type point) { \
  real raw[MAX_NUMBER_SYSTEM_SIZE + 1] = {0.0}; \
  number_system##_to_raw(sub_##number_system(point, number_system##_from_raw(view.center.raw, 0)), raw, 1); \
  return (int4)( \
    reverse_view_mapping_element(raw, view.zoom, view.mapping.x, item.dimensions.width), \
    reverse_view_mapping_element(raw, view.zoom, view.mapping.y, item.dimensions.height), \
    reverse_view_mapping_element(raw, view.zoom, view.mapping.z, item.dimensions.depth), \
    0); \
}

#define create_kernels(function, escape, number_system, number_system_type) \
create_phase_kernels(function, escape, number_system, number_system_type) \
create_dynamical_kernels(function, escape, number_system, number_system_type)

#define X(number_system, element_system, conj, mul, sqr, modulus_sq) \
  create_view_mapping_functions(number_system, number_system##_impl) \
  create_kernels( \
    KERNEL_FUNCTION( \
      add_##number_system, \
      sub_##number_system, \
      conj_##number_system, \
      mul_##number_system, \
      sqr_##number_system, \
      scale_##number_system, \
      modulus_sq_##number_system), \
    ESCAPE_VALUE, \
    number_system, \
    number_system##_impl)
NUMBER_SYSTEMS;
#undef X

#undef create_kernels
#undef create_view_mapping_functions
#undef create_dynamical_kernels
#undef create_phase_kernels
#undef create_escape_and_translated_kernels
#undef write_translated_point
#undef write_fractional_escape
#undef create_kernel

_EXTERN_C_END_

#endif // !_KERNEL_BASE_H_
