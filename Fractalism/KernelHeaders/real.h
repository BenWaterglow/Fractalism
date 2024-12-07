#ifndef _REAL_H_
#define _REAL_H_

#if defined(__cplusplus) || defined(__OPENCL_CPP_VERSION__)
extern "C" {
#endif

#if defined(USE_DOUBLE_MATH) || ((defined(__OPENCL_C_VERSION__) || defined(__OPENCL_CPP_VERSION__)) && defined(cl_khr_fp64))
typedef double real;
#else
typedef float real;
#endif

#if defined(__cplusplus) || defined(__OPENCL_CPP_VERSION__)
}
#endif

#endif // !_REAL_H_
