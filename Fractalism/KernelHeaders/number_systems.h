#ifndef _NUMBER_SYSTEMS_H_
#define _NUMBER_SYSTEMS_H_

#include "real.h"

#if defined(__cplusplus) || defined(__OPENCL_CPP_VERSION__)
extern "C" {
#endif

#define create_number_system_functions( \
  number_system, \
  number_system_type, \
  zero, \
  add, \
  sub, \
  mul, \
  sqr, \
  scale, \
  modulus_sq) \
static inline number_system_type zero_##number_system() { \
  return number_system##_ctor zero; \
} \
static inline number_system_type add_##number_system(number_system_type x, number_system_type y) { \
  return number_system##_ctor add; \
} \
static inline number_system_type sub_##number_system(number_system_type x, number_system_type y) { \
  return number_system##_ctor sub; \
} \
static inline number_system_type mul_##number_system(number_system_type x, number_system_type y) { \
  return number_system##_ctor mul; \
} \
static inline number_system_type sqr_##number_system(number_system_type x) { \
  return number_system##_ctor sqr; \
} \
static inline number_system_type scale_##number_system(number_system_type x, real y) { \
  return number_system##_ctor scale; \
} \
static inline real modulus_sq_##number_system(number_system_type x) { \
  return modulus_sq; \
}

#define create_cartesian_product_number_system_functions( \
  number_system, \
  number_system_type, \
  element_system, \
  mul, \
  sqr, \
  modulus_sq) \
create_number_system_functions( \
  /* number_system         */ number_system, \
  /* number_system_type    */ number_system_type, \
  /* zero                  */ (zero_##element_system(), zero_##element_system()), \
  /* add                   */ (add_##element_system(x.x, y.x), add_##element_system(x.y, y.y)), \
  /* sub                   */ (sub_##element_system(x.x, y.x), sub_##element_system(x.y, y.y)), \
  /* mul                   */ mul, \
  /* sqr                   */ sqr, \
  /* scale                 */ (scale_##element_system(x.x, y), scale_##element_system(x.y, y)), \
  /* modulus_sq            */ modulus_sq)

#define implement_cartesian_product_number_system( \
  number_system, \
  number_system_type, \
  element_system, \
  element_system_type, \
  mul, \
  sqr, \
  modulus_sq) \
typedef struct __attribute__((packed)) number_system_type { \
  element_system_type x; \
  element_system_type y; \
} number_system_type; \
static inline number_system_type number_system##_ctor(element_system_type x, element_system_type y) { \
  return (number_system_type) {x, y}; \
}\
static inline number_system_type number_system_type##_from_raw(real* raw, size_t offset) { \
  return number_system##_ctor( \
    element_system_type##_from_raw(raw, offset), \
    element_system_type##_from_raw(raw, offset + (sizeof(element_system_type)/sizeof(real_impl)))); \
} \
static inline void number_system_type##_to_raw(number_system_type value, real* raw, size_t offset) { \
  element_system_type##_to_raw(value.x, raw, offset); \
  element_system_type##_to_raw(value.y, raw, offset + (sizeof(element_system_type)/sizeof(real_impl))); \
} \
create_cartesian_product_number_system_functions( \
  /* number_system         */ number_system, \
  /* number_system_type    */ number_system_type, \
  /* element_system        */ element_system, \
  /* mul                   */ mul, \
  /* sqr                   */ sqr, \
  /* modulus_sq            */ modulus_sq);

typedef struct real_impl {
  real x;
  real sq;
} real_impl;

static inline real_impl real_ctor(real val) {
  return (real_impl) { val, val * val };
}

static inline real_impl real_impl_from_raw(real* raw, size_t offset) {
  return real_ctor(raw[offset]);
}

static inline void real_impl_to_raw(real_impl value, real* raw, size_t offset) {
  raw[offset] = value.x;
}

create_number_system_functions(
  /* number_system         */ real,
  /* number_system_type    */ real_impl,
  /* zero                  */ (0.0),
  /* add                   */ (x.x + y.x),
  /* sub                   */ (x.x - y.x),
  /* mul                   */ (x.x * y.x),
  /* sqr                   */ (x.sq),
  /* scale                 */ (x.x * y),
  /* modulus_sq            */ x.sq);

#if !defined(NUMBER_SYSTEMS)
#error "Preprocessor macro NUMBER_SYSTEMS is undefined. Must use the following X macro pattern: X(number_system, element_system, mul, sqr, modulus_sq)"
#endif

#define X(number_system, element_system, mul, sqr, modulus_sq) implement_cartesian_product_number_system(\
  number_system, \
  number_system##_impl, \
  element_system, \
  element_system##_impl, \
  mul, \
  sqr, \
  modulus_sq)
NUMBER_SYSTEMS
#undef X

#undef implement_cartesian_product_number_system
#undef create_cartesian_product_number_system_functions
#undef create_number_system_functions

#if defined(__cplusplus) || defined(__OPENCL_CPP_VERSION__)
}
#endif

#endif // !_NUMBER_SYSTEMS_H