#ifndef _FRACTALISM_NUMBER_SYSTEMS_H_
#define _FRACTALISM_NUMBER_SYSTEMS_H_

#if defined(_MSC_VER)
// Intellisense gets confused if we use the other #include format.
#include <Fractalism/KernelHeaders/interop.h>
#else
#include "interop.h"
#endif

_EXTERN_C_DECL_

#define create_number_system_functions( \
  number_system, \
  number_system_type, \
  prep_1, \
  prep_2, \
  zero, \
  neg, \
  add, \
  sub, \
  scale, \
  conj, \
  mul, \
  sqr, \
  modulus_sq) \
static inline number_system_type zero_##number_system() { \
  return number_system##_ctor zero; \
} \
static inline number_system_type neg_##number_system(number_system_type x) { \
  prep_1; \
  return number_system##_ctor neg; \
} \
static inline number_system_type add_##number_system(number_system_type x, number_system_type y) { \
  prep_1; prep_2; \
  return number_system##_ctor add; \
} \
static inline number_system_type sub_##number_system(number_system_type x, number_system_type y) { \
  prep_1; prep_2; \
  return number_system##_ctor sub; \
} \
static inline number_system_type scale_##number_system(number_system_type x, real s) { \
  prep_1; \
  return number_system##_ctor scale; \
} \
static inline number_system_type conj_##number_system(number_system_type x) { \
  prep_1; \
  return number_system##_ctor conj;\
} \
static inline number_system_type mul_##number_system(number_system_type x, number_system_type y) { \
  prep_1; prep_2; \
  return number_system##_ctor mul; \
} \
static inline number_system_type sqr_##number_system(number_system_type x) { \
  prep_1; \
  return number_system##_ctor sqr; \
} \
static inline real modulus_sq_##number_system(number_system_type x) { \
  prep_1; \
  return modulus_sq; \
}

#define implement_cartesian_product_number_system( \
  number_system, \
  number_system_type, \
  element_system, \
  element_system_type, \
  conj, \
  mul, \
  sqr, \
  modulus_sq) \
static inline size_t number_system##_element_count() { return element_system##_element_count() * 2; } \
typedef struct number_system_type { \
  element_system_type x; \
  element_system_type y; \
} number_system_type; \
static inline number_system_type number_system##_ctor(element_system_type x, element_system_type y) { \
  return (number_system_type) {x, y}; \
}\
static inline number_system_type number_system##_from_raw(real* raw, size_t offset) { \
  return number_system##_ctor( \
    element_system##_from_raw(raw, offset), \
    element_system##_from_raw(raw, offset + element_system##_element_count())); \
} \
static inline void number_system##_to_raw(number_system_type value, real* raw, size_t offset) { \
  element_system##_to_raw(value.x, raw, offset); \
  element_system##_to_raw(value.y, raw, offset + element_system##_element_count()); \
} \
create_number_system_functions( \
  /* number_system         */ number_system, \
  /* number_system_type    */ number_system_type, \
  /* prep_1                */ element_system_type a = x.x; element_system_type b = x.y, \
  /* prep_2                */ element_system_type c = y.x; element_system_type d = y.y, \
  /* zero                  */ (zero_##element_system(), zero_##element_system()), \
  /* neg                   */ (neg_##element_system(a), neg_##element_system(b)), \
  /* add                   */ (add_##element_system(a, c), add_##element_system(b, d)), \
  /* sub                   */ (sub_##element_system(a, c), sub_##element_system(b, d)), \
  /* scale                 */ (scale_##element_system(a, s), scale_##element_system(b, s)), \
  /* conj                  */ conj, \
  /* mul                   */ mul, \
  /* sqr                   */ sqr, \
  /* modulus_sq            */ modulus_sq);

static inline size_t real_element_count() { return 1; }

typedef struct real_impl {
  real x;
  real sq;
} real_impl;

static inline real_impl real_ctor(real val) {
  return (real_impl) { val, val * val };
}

static inline real_impl real_from_raw(real* raw, size_t offset) {
  return real_ctor(raw[offset]);
}

static inline void real_to_raw(real_impl value, real* raw, size_t offset) {
  raw[offset] = value.x;
}

create_number_system_functions(
  /* number_system         */ real,
  /* number_system_type    */ real_impl,
  /* prep_1                */ real a = x.x,
  /* prep_2                */ real b = y.x,
  /* zero                  */ (0.0),
  /* neg                   */ (-a),
  /* add                   */ (a + b),
  /* sub                   */ (a - b),
  /* scale                 */ (a * s),
  /* conj                  */ (a),
  /* mul                   */ (a * b),
  /* sqr                   */ (x.sq),
  /* modulus_sq            */ (x.sq));

#define X(number_system, element_system, conj, mul, sqr, modulus_sq) implement_cartesian_product_number_system(\
  number_system, \
  number_system##_impl, \
  element_system, \
  element_system##_impl, \
  conj, \
  mul, \
  sqr, \
  modulus_sq)
NUMBER_SYSTEMS
#undef X

#undef implement_cartesian_product_number_system
#undef create_number_system_functions

_EXTERN_C_END_

#endif // !_NUMBER_SYSTEMS_H