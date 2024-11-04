#ifndef _CLTYPES_H_
#define _CLTYPES_H_

#include "number_systems.h"

#if defined(__cplusplus) || defined(__OPENCL_CPP_VERSION__)
extern "C" {
#endif

// TODO: find out if there is a way to calculate this autmatically with more sketchy preprocessor magic?
#if !defined(MAX_NUMBER_SYSTEM_SIZE)
#error "Preprocessor macro MAX_NUMBER_SYSTEM_SIZE is not defined."
#endif

typedef union __attribute__((packed)) number {
#define X(extension_method, number_system, element_system) number_system number_system;
  NUMBER_SYSTEMS
#undef X
  real raw[MAX_NUMBER_SYSTEM_SIZE];
} number;

typedef struct __attribute__((packed)) viewspace {
  number center;
  real zoom;
  char3 mapping;
} viewspace;

typedef struct __attribute__((packed)) work_store {
  number value;
  unsigned int i;
} work_store;

#if defined(__cplusplus) || defined(__OPENCL_CPP_VERSION__)
}
#endif

#endif // !_CLTYPES_H_