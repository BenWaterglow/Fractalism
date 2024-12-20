#ifndef _FRACTALISM_CLTYPES_H_
#define _FRACTALISM_CLTYPES_H_

#if defined(_MSC_VER)
// Intellisense gets confused if we use the other (relative) #include format.
#include <Fractalism/KernelHeaders/interop.h>
#else
#include "interop.h"
#endif

_EXTERN_C_DECL_

  _PACK_BEGIN_ struct number {
    real raw[MAX_NUMBER_SYSTEM_SIZE];
  } _PACK_END_;

  typedef struct number number;

  _PACK_BEGIN_ struct view_mapping {
    cl_char x;
    cl_char y;
    cl_char z;
  } _PACK_END_;

  typedef struct view_mapping view_mapping;

  _PACK_BEGIN_ struct viewspace {
    number center;
    real zoom;
    view_mapping mapping;
  } _PACK_END_;

  typedef struct viewspace viewspace;

  _PACK_BEGIN_ struct work_store {
    number value;
    cl_uint i;
  } _PACK_END_;

  typedef struct work_store work_store;

_EXTERN_C_END_

#endif // !_CLTYPES_H_