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
  return number_system_type##_ctor zero; \
} \
static inline number_system_type add_##number_system(number_system_type x, number_system_type y) { \
  return number_system_type##_ctor add; \
} \
static inline number_system_type sub_##number_system(number_system_type x, number_system_type y) { \
  return number_system_type##_ctor sub; \
} \
static inline number_system_type mul_##number_system(number_system_type x, number_system_type y) { \
  return number_system_type##_ctor mul; \
} \
static inline number_system_type sqr_##number_system(number_system_type x) { \
  return number_system_type##_ctor sqr; \
} \
static inline number_system_type scale_##number_system(number_system_type x, real y) { \
  return number_system_type##_ctor scale; \
} \
static inline real modulus_sq_##number_system(number_system_type x) { \
  return modulus_sq; \
}

#define create_multicomplex_number_system_functions( \
  number_system, \
  number_system_type, \
  zero, \
  add, \
  sub, \
  mul, \
  sqr, \
  scale, \
  modulus_sq) \
create_number_system_functions( \
  /* number_system         */ number_system, \
  /* number_system_type    */ number_system_type, \
  /* zero                  */ (zero(), zero()), \
  /* add                   */ (add(x.x, y.x), add(x.y, y.y)), \
  /* sub                   */ (sub(x.x, y.x), sub(x.y, y.y)), \
  /* mul                   */ (sub(mul(x.x, y.x), mul(x.y, y.y)), add(mul(x.x, y.y), mul(x.y, y.x))), \
  /* sqr                   */ (sub(sqr(x.x), sqr(x.y)), scale(mul(x.x, x.y), 2.0)), \
  /* scale                 */ (scale(x.x, y), scale(x.y, y)), \
  /* modulus_sq            */ modulus_sq(x.x) + modulus_sq(x.y));

#define implement_multicomplex_number_system(number_system, number_system_type, element_system, element_system_type) \
typedef struct __attribute__((packed)) number_system_type { \
  element_system_type x; \
  element_system_type y; \
} number_system_type; \
static inline number_system_type number_system_type##_ctor(element_system_type x, element_system_type y) { \
  return (number_system_type) {x, y}; \
}\
static inline number_system_type number_system_type##_from_raw(real* raw, size_t offset) { \
  return number_system_type##_ctor( \
    element_system_type##_from_raw(raw, offset), \
    element_system_type##_from_raw(raw, offset + (sizeof(element_system_type)/sizeof(real_impl)))); \
} \
static inline void number_system_type##_to_raw(number_system_type value, real* raw, size_t offset) { \
  element_system_type##_to_raw(value.x, raw, offset); \
  element_system_type##_to_raw(value.y, raw, offset + (sizeof(element_system_type)/sizeof(real_impl))); \
} \
create_multicomplex_number_system_functions( \
  /* number_system         */ number_system, \
  /* number_system_type    */ number_system_type, \
  /* zero                  */ zero_##element_system, \
  /* add                   */ add_##element_system, \
  /* sub                   */ sub_##element_system, \
  /* mul                   */ mul_##element_system, \
  /* sqr                   */ sqr_##element_system, \
  /* scale                 */ scale_##element_system, \
  /* modulus_sq            */ modulus_sq_##element_system);

typedef struct real_impl {
  real x;
  real sq;
} real_impl;

static inline real_impl real_impl_ctor(real val) {
  return (real_impl) { val, val * val };
}

static inline real_impl real_impl_from_raw(real* raw, size_t offset) {
  return real_impl_ctor(raw[offset]);
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
#error "Preprocessor macro NUMBER_SYSTEMS is undefined. Must use the following X macro pattern: X(extension_method, number_system, element_system)"
#endif

#define X(extension_method, number_system, element_system) implement_##extension_method##_number_system(\
  number_system, \
  number_system##_impl, \
  element_system, \
  element_system##_impl)
NUMBER_SYSTEMS
#undef X

#undef implement_multicomplex_number_system
#undef create_multicomplex_number_system_functions
#undef create_number_system_functions

#if defined(__cplusplus) || defined(__OPENCL_CPP_VERSION__)
}
#endif

#endif // !_NUMBER_SYSTEMS_H