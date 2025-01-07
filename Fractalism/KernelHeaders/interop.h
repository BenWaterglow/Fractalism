#ifndef _FRACTALISM_INTEROP_H_
#define _FRACTALISM_INTEROP_H_

#if defined(__cplusplus) || defined(__OPENCL_CPP_VERSION__)
  // We are in a C++ compiler, wrap c code with extern "C" {}
  #define _EXTERN_C_DECL_ extern "C" {
  #define _EXTERN_C_END_ }
#else
  // We are in a C compiler, no extern "C" {} wrapper.
  #define _EXTERN_C_DECL_ 
  #define _EXTERN_C_END_
#endif

#if defined(__OPENCL_CPP_VERSION__) || defined(__OPENCL_C_VERSION__)
  // We are in the OpenCL C/C++ compiler.
  #define _ON_GPU_ 1
  // Defines to allow using the cl_* 
  #define cl_char char
  #define cl_uint uint
  #define cl_double double
  #define cl_float float
#else
  // We are in the host machine compiler.
  #define _ON_GPU_ 0
  // Include the OpenCL headers and #define a few CL keywords.
  #include <Fractalism/GPU/OpenCL/CLCommon.hpp>
  #define __constant const
  #define __global
  #define __write_only
  #define __kernel static
  #define convert_int_rte(value) (int)(value)
#endif

// We have to have a max number system size set during build.
// TODO: make this more dynamic?
#if !defined(MAX_NUMBER_SYSTEM_SIZE)
  #error "MAX_NUMBER_SYSTEM_SIZE is not defined."
#endif

#if !defined(USE_DOUBLE_MATH)
  #if _ON_GPU_
    // If fp64 is set, use double math.
    // TODO: check if this is safe for newer hardware than my old GPU.
    #if defined(cl_khr_fp64)
      #define USE_DOUBLE_MATH 1
    #else
      #define USE_DOUBLE_MATH 0
    #endif
  #else
    // Explicitly set to 0 if it's undefined on the host.
    #define USE_DOUBLE_MATH 0
  #endif
#elif ~USE_DOUBLE_MATH + 1
  // USE_DOUBLE_MATH is either empty, or a non-zero integer. Normalize to 1.
  // No need to explicitly normalize to 0, since the only other possible
  // outcome is USE_DOUBLE_MATH == 0
  #undef USE_DOUBLE_MATH
  #define USE_DOUBLE_MATH 1
#endif

// Use typedef, NOT #define!
// "#define real x" breaks the macros that find the *_real() functions.
// We could change the *_real() functions to use token-pasting, but
// the dynamic number system code is messy enough as is.
// We use + 0 here to handle 
#if USE_DOUBLE_MATH 
  typedef cl_double real;
#else
  typedef cl_float real;
#endif

#if defined(_MSC_VER)
  // Make sure MSVC handles struct packing correctly.
  #define _PACK_BEGIN_ __pragma( pack(push, 1) )
  #define _PACK_END_ __pragma( pack(pop) )
#else
  // Not sure how universal this is, but it works for OpenCL C.
  #define _PACK_BEGIN_
  #define _PACK_END_ __attribute__((packed))
#endif

#endif // !_INTEROP_H_
